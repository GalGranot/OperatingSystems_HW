//ttftps.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


using std::string;
using std::atoi;
using std::ofstream;
using std::cout;
using std::endl;

#define BYTE 8
#define PACKET_SIZE 516
#define ERROR -1
#define ARG_NUM 4

#define OP_WRQ 2
#define OP_DATA 3
#define OP_ACK 4
#define OP_ERROR 5


struct WRQ
{
	uint16_t opcode;
	char fileName[100];
	char transmissionMode[50];
} __attribute__((packed));

struct ACK
{
	uint16_t opcode;
	uint16_t blockNumber;
} __attribute__((packed));


struct Data
{
	uint16_t opcode;
	uint16_t blockNumber;
	char data[512];
} __attribute__((packed));

struct ErrorMessage
{
	uint16_t opcode;
	uint16_t errorCode;
	char message[100];
} __attribute__((packed));


int main(int argc, char* argv[])
{
	if (argc != ARG_NUM)
	{
		perror("TTFTP_ERROR: illegal arguments");
		exit(1);
	}

	//FIXME check valid args - first int, then if small change to short
	unsigned short port = atoi(argv[1]);
	unsigned short timeout = atoi(argv[2]);
	unsigned short maxErrorCount = atoi(argv[3]);
	struct timeval tv;
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	char buffer[PACKET_SIZE] = { 0 };

	int socketFD = socket(AF_INET, SOCK_DGRAM, 0);
	if(socketFD < 0)
	{
		perror("TTFTP_ERROR: fail to open socket");
		exit(1);
	}

	struct sockaddr_in serverAddress;
	socklen_t serverAddressLength = sizeof(sockaddr_in);
	memset(&serverAddress, 0, serverAddressLength);
	struct sockaddr clientAddress;
	socklen_t clientAddressLength = sizeof(sockaddr);
	memset(&clientAddress, 0, clientAddressLength);

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(port);
	cout << "before bind" << endl;
	if (bind(socketFD, (struct sockaddr*)&serverAddress, serverAddressLength) < 0)
	{
		perror("TTFTP_ERROR: fail to bind socket");
		exit(1);
	}
	cout << "bind finished, before while" << endl;
	while (1)
	{
		unsigned short errorCount = 0;
		struct ErrorMessage error;
		bool success = false;
		int bytesRead = 0;
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(socketFD, &readfds);


		//write request
		bytesRead = recvfrom(socketFD, buffer, sizeof(buffer), 0, /*(struct sockaddr*)*/ &clientAddress, &clientAddressLength);
		if (bytesRead < 0)
		{
			perror("TTFTP_ERROR: recvfrom fail");
			exit(1);
		}
		cout << "buffer 1   " << buffer[1]<< buffer[2] << endl;
		
		struct WRQ wrq;
		memcpy(&wrq, buffer, sizeof(WRQ)); //fix this
		cout << "got WRQ" << endl;
		if (wrq.opcode != OP_WRQ)
		{
			error.errorCode = 7;
			strcpy(error.message , "Unknown user");
			cout << "got WRQ1" << endl;
			if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, &clientAddress, clientAddressLength) < 0)
			{
				perror("TTFTP_ERROR: sendto fail");
				exit(1);
			}
			continue;
		}
		cout << "got WRQ 2" << endl;
		ofstream outputFile(wrq.fileName, std::ios::out | std::ios::binary);
		if (!outputFile.is_open())
		{
			perror("TTFTP_ERROR: file open fail");
			exit(1);
		}

		cout << "try to send ack0" << endl;
		//ack0
		struct ACK ack0;
		ack0.opcode = OP_ACK;
		ack0.blockNumber = 0;

		if (sendto(socketFD, (void*)&ack0, (size_t)sizeof(ACK), 0, &clientAddress, clientAddressLength) < 0)
		{
			perror("TTFTP_ERROR: sendto fail");
			exit(1);
		}

		cout << "try to get data1" << endl;
		//data1
		struct sockaddr receivedAddress;
		socklen_t receivedAddressLength = sizeof(sockaddr);
		memset(&receivedAddress, 0, receivedAddressLength);
		while (!success)
		{
			cout << "sellect_0" << endl;
			int result = select(socketFD+1, &readfds, NULL, NULL, &tv); //this reads from server to client
			cout << "sellect_1 " << result<< endl;
			if (result == 0)
			{
				errorCount++;
				if (errorCount > maxErrorCount)
				{
					struct ErrorMessage error;
					error.opcode = OP_ERROR;
					error.errorCode = 0;
					strcpy(error.message ,"Abandoning file transmission");
					cout << "try to send error " << result << endl;
					if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, &clientAddress, clientAddressLength) < 0)
					{
						perror("TTFTP_ERROR: sendto fail");
						exit(1);
					}
					cout << "sent error " << result << endl;
					break;
				}
				continue;
			}

			if (result < 0)
			{
				perror("TTFTP_ERROR: select fail");
				exit(1);
			}
			bytesRead = recvfrom(socketFD, buffer, sizeof(buffer), 0, &receivedAddress, &receivedAddressLength);
			if (bytesRead < 0)
			{
				perror("TTFTP_ERROR: recvfrom fail");
				exit(1);
			}
			if (strcmp(receivedAddress.sa_data , clientAddress.sa_data) !=0) //FIXME: check if compares the right fields fpr address
			{
				struct Data data1;
				memcpy(&data1, buffer, sizeof(Data));
				error.errorCode = 4;
				strcpy(error.message , "Unexpected packet");
				if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, &receivedAddress, receivedAddressLength) < 0)
				{
					perror("TTFTP_ERROR: sendto fail");
					exit(1);
				}
				continue;
			}

			success = true;
		}
		if (!success)
		{
			//handle close files errors
			continue;
		}

		struct Data data1;
		memcpy(&data1, buffer, sizeof(Data));
		if (data1.opcode != OP_DATA)
		{
			error.errorCode = 4;
			strcpy(error.message , "Unexpected packet");
			if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, &receivedAddress, receivedAddressLength) < 0)
			{
				perror("TTFTP_ERROR: sendto fail");
				exit(1);
			}
		}
		if (data1.blockNumber != 1)
		{
			error.errorCode = 0;
			strcpy(error.message, "Bad block number");
			if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, &receivedAddress, receivedAddressLength) < 0)
			{
				perror("TTFTP_ERROR: sendto fail");
				exit(1);
			}
		}

		outputFile << data1.data; //FIXME check if this works
		success = false;
		
		cout << "try to send ack1" << endl;
		//ack1
		struct ACK ack1;
		ack1.opcode = OP_ACK;
		ack1.blockNumber = 0;

		if (sendto(socketFD, (void*)&ack1, (size_t)sizeof(ACK), 0, &clientAddress, clientAddressLength) < 0)
		{
			perror("TTFTP_ERROR: sendto fail");
			exit(1);
		}

		cout << "try to get data2" << endl;
		//data2
		while (!success)
		{
			int result = select(socketFD+1, &readfds, NULL, NULL, &tv); //this reads from server to client
			if (result == 0)
			{
				errorCount++;
				if (errorCount > maxErrorCount)
				{
					struct ErrorMessage error;
					error.opcode = OP_ERROR;
					error.errorCode = 0;
					strcpy(error.message , "Abandoning file transmission");
					if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, &clientAddress, clientAddressLength) < 0)
					{
						perror("TTFTP_ERROR: sendto fail");
						exit(1);
					}
					break;
				}
			}

			if (result < 0)
			{
				perror("TTFTP_ERROR: select fail");
				exit(1);
			}
			bytesRead = recvfrom(socketFD, buffer, sizeof(buffer), 0, &receivedAddress, &receivedAddressLength);
			if (bytesRead < 0)
			{
				perror("TTFTP_ERROR: recvfrom fail");
				exit(1);
			}
			if (strcmp(receivedAddress.sa_data, clientAddress.sa_data) != 0)
			{
				struct Data data2;
				memcpy(&data2, buffer, sizeof(Data));
				error.errorCode = 4;
				strcpy(error.message , "Unexpected packet");
				if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, &receivedAddress, receivedAddressLength) < 0)
				{
					perror("TTFTP_ERROR: sendto fail");
					exit(1);
				}
				continue;
			}

			success = true;
		}
		if (!success)
		{
			//handle close files errors
			continue;
		}

		struct Data data2;
		memcpy(&data2, buffer, sizeof(Data));
		if (data2.opcode != OP_DATA)
		{
			error.errorCode = 4;
			strcpy(error.message , "Unexpected packet");
			if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, &receivedAddress, receivedAddressLength) < 0)
			{
				perror("TTFTP_ERROR: sendto fail");
				exit(1);
			}
		}
		if (data2.blockNumber != 2)
		{
			error.errorCode = 0;
			strcpy(error.message , "Bad block number");
			if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, &receivedAddress, receivedAddressLength) < 0)
			{
				perror("TTFTP_ERROR: sendto fail");
				exit(1);
			}
		}

		cout << "try to send ack2" << endl;
		//ack2
		struct ACK ack2;
		ack2.opcode = OP_ACK;
		ack2.blockNumber = 0;

		if (sendto(socketFD, (void*)&ack2, (size_t)sizeof(ACK), 0, &clientAddress, clientAddressLength) < 0)
		{
			perror("TTFTP_ERROR: sendto fail");
			exit(1);
		}
			
		if (outputFile.is_open())
			outputFile.close();
	}
	close(socketFD);
}
