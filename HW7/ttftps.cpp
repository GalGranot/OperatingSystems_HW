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

#define BYTE 8
#define PACKET_SIZE 516
#define ERROR -1
#define ARG_NUM 4

#define OP_WRQ '2'
#define OP_DATA '3'
#define OP_ACK '4'
#define OP_ERROR '5'


struct WRQ
{
	unsigned char opcode[2 * BYTE];
	char* fileName;
	char* transmissionMode;
} __attribute__((packed));

struct ACK
{
	unsigned char opcode[2 * BYTE];
	unsigned char blockNumber[2 * BYTE];
} __attribute__((packed));


struct Data
{
	unsigned char opcode[2 * BYTE];
	unsigned char blockNumber[2 * BYTE];
	unsigned char data[512 * BYTE];
} __attribute__((packed));

struct ErrorMessage
{
	unsigned char opcode[2 * BYTE];
	unsigned char errorCode[2 * BYTE];
	char* message;
} __attribute__((packed));


int main(int argc, char* argv[])
{
	if (argc != ARG_NUM)
	{
		perror("TTFTP_ERROR: illegal arguments");
		exit(1);
	}

	//FIXME check valid args
	unsigned short port = atoi(argv[1]);
	unsigned short timeout = atoi(argv[2]);
	unsigned short maxErrorCount = atoi(argv[3]);
	struct timeval tv;
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	char buffer[PACKET_SIZE];

	int socketFD = socket(AF_INET, SOCK_DGRAM, 0);
	if(socketFD < 0)
	{
		perror("TTFTP_ERROR: fail to open socket");
		exit(1);
	}

	struct sockaddr_in serverAddress;
	socklen_t serverAddressLength = sizeof(sockaddr_in);
	struct sockaddr_in clientAddress;
	socklen_t clientAddressLength = sizeof(sockaddr_in);

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(port);
	if (bind(socketFD, (struct sockaddr*)&serverAddress, serverAddressLength) < 0)
	{
		perror("TTFTP_ERROR: fail to bind socket");
		exit(1);
	}
	
	while (1)
	{
		unsigned short errorCount = 0;
		struct ErrorMessage error;
		bool success = false;
		int bytesRead = 0;
		//write request
		bytesRead = recvfrom(socketFD, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddress, &clientAddressLength);
		if (bytesRead < 0)
		{
			perror("TTFTP_ERROR: recvfrom fail");
			exit(1);
		}

		struct WRQ wrq;
		memcpy(&wrq, buffer, sizeof(WRQ)); //fix this


		if (strcmp((char*)wrq.opcode, OP_WRQ) != 0)
		{
			error.errorCode = 7;
			error.message = "Unknown user";
			if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, &clientAddress, clientAddressLength) < 0)
			{
				perror("TTFTP_ERROR: sendto fail");
				exit(1);
			}
		}
		ofstream outputFile(wrq.fileName, std::ios::out | std::ios::binary);
		if (!outputFile)
		{
			perror("TTFTP_ERROR: file open fail");
			exit(1);
		}

		//ack0
		struct ACK ack0;
		ack0.opcode = OP_ACK;
		ack0.blockNumber = 0;

		if (sendto(socketFD, (void*)&ack0, (size_t)sizeof(ACK), 0, &clientAddress, clientAddressLength) < 0)
		{
			perror("TTFTP_ERROR: sendto fail");
			exit(1);
		}

		//data1
		struct sockaddr_in receivedAddress;
		socklen_t receivedAddressLength = sizeof(sockaddr_in);
		while (!success)
		{
			int result = select(1, &socketFD, NULL, NULL, , tv); //this reads from server to client
			if (result == 0)
			{
				errorCount++;
				if (errorCount > maxErrorCount)
				{
					struct ErrorMessage error;
					error.opcode = OP_ERROR;
					error.errorCode = 0;
					error.message = "Abandoning file transmission";
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
			bytesRead = recvfrom(socketFD, buffer, sizeof(buffer), 0, (struct sockaddr*)&receivedAddress, &receivedAddress);
			if (bytesRead < 0)
			{
				perror("TTFTP_ERROR: recvfrom fail");
				exit(1);
			}
			if (receivedAddress.sin_addr != clientAddress.sin_addr)
			{
				struct Data data1;
				memcpy(&data1, buffer, sizeof(Data));
				error.errorCode = 4;
				error.message = "Unexpected packet";
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
			error.message = "Unexpected packet";
			if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, &receivedAddress, receivedAddressLength) < 0)
			{
				perror("TTFTP_ERROR: sendto fail");
				exit(1);
			}
		}
		if (data1.blockNumber != 1)
		{
			error.errorCode = 0;
			error.message = "Bad block number";
			if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, &receivedAddress, receivedAddressLength) < 0)
			{
				perror("TTFTP_ERROR: sendto fail");
				exit(1);
			}
		}

		outputFile << data1.data; //FIXME check if this works
		success = false;
		
		//ack1
		struct ACK ack1;
		ack1.opcode = OP_ACK;
		ack1.blockNumber = 0;

		if (sendto(socketFD, (void*)&ack1, (size_t)sizeof(ACK), 0, &clientAddress, clientAddressLength) < 0)
		{
			perror("TTFTP_ERROR: sendto fail");
			exit(1);
		}

		//data2
		struct sockaddr_in receivedAddress;
		socklen_t receivedAddressLength = sizeof(sockaddr_in);
		while (!success)
		{
			int result = select(1, &socketFD, NULL, NULL, , tv); //this reads from server to client
			if (result == 0)
			{
				errorCount++;
				if (errorCount > maxErrorCount)
				{
					struct ErrorMessage error;
					error.opcode = OP_ERROR;
					error.errorCode = 0;
					error.message = "Abandoning file transmission";
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
			bytesRead = recvfrom(socketFD, buffer, sizeof(buffer), 0, (struct sockaddr*)&receivedAddress, &receivedAddress);
			if (bytesRead < 0)
			{
				perror("TTFTP_ERROR: recvfrom fail");
				exit(1);
			}
			if (receivedAddress.sin_addr != clientAddress.sin_addr)
			{
				struct Data data2;
				memcpy(&data2, buffer, sizeof(Data));
				error.errorCode = 4;
				error.message = "Unexpected packet";
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
			error.message = "Unexpected packet";
			if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, &receivedAddress, receivedAddressLength) < 0)
			{
				perror("TTFTP_ERROR: sendto fail");
				exit(1);
			}
		}
		if (data2.blockNumber != 2)
		{
			error.errorCode = 0;
			error.message = "Bad block number";
			if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, &receivedAddress, receivedAddressLength) < 0)
			{
				perror("TTFTP_ERROR: sendto fail");
				exit(1);
			}
		}

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
			outFile.close();
	}
	close(socketFD);
}
