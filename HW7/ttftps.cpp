//ttftps.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstdlib>
#include <stdio.h>
#include <fcntl.h>



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
	struct sockaddr_in clientAddress;
	socklen_t clientAddressLength = sizeof(sockaddr_in);
	memset(&clientAddress, 0, clientAddressLength);

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
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
		error.opcode = htons(OP_ERROR);
		bool success = false;
		int bytesRead = 0;
		fd_set readfds;

		FD_ZERO(&readfds);
		FD_SET(socketFD, &readfds);
		tv.tv_sec = timeout;
		tv.tv_usec = 0;

		memset(buffer, 0, sizeof(buffer));

		//write request
		bytesRead = recvfrom(socketFD, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddress, &clientAddressLength);

		if (bytesRead < 0)
		{
			perror("TTFTP_ERROR: recvfrom fail");
			exit(1);
		}
		struct WRQ wrq;
		memcpy(&(wrq.opcode), buffer, 2); 
		wrq.opcode = ntohs(wrq.opcode);
		strcpy(wrq.fileName, buffer + 2);
		strcpy(wrq.transmissionMode, buffer + 2+ strlen(buffer + 2) + 1); // copy after opcode + filename + /0
		if (wrq.opcode != OP_WRQ)
		{
			error.errorCode = htons(7);
			strcpy(error.message , "Unknown user");
			if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, (struct sockaddr*)&clientAddress, clientAddressLength) < 0)
			{
				perror("TTFTP_ERROR: sendto fail");
				exit(1);
			}
			continue;
		}

		//FIXME make sure file doesn't already exist
		//ofstream outputFile(wrq.fileName, std::ios::out | std::ios::binary);
		char* tmpFileName = buffer + 2;
		//FILE* tmpFilePtr = fopen(tmpFileName, "r");
		//if (tmpFilePtr)
		FILE* tmpFilePtr = fopen(tmpFileName, "r");
		if(tmpFilePtr)
		{
			fclose(tmpFilePtr);
			error.errorCode = htons(6);
			strcpy(error.message, "File already exists");
			if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, (struct sockaddr*)&clientAddress, clientAddressLength) < 0)
			{
				perror("TTFTP_ERROR: sendto fail");
				exit(1);
			}
			continue;
		}
		ofstream outputFile;
		outputFile.open(wrq.fileName);
		if (!outputFile.is_open())
		{
			perror("TTFTP_ERROR: file open fail");
			exit(1);
		}

		//ack0
		struct ACK ack0;
		ack0.opcode = htons(OP_ACK);
		ack0.blockNumber = htons(0); 
		if (sendto(socketFD, (void*)&ack0, (size_t)sizeof(ACK), 0, (struct sockaddr*)&clientAddress, clientAddressLength) < 0)
		{
			if (outputFile.is_open())
				outputFile.close();
			perror("TTFTP_ERROR: sendto fail");
			exit(1);

		}
		//data1
		memset(buffer, '\0', sizeof(buffer)); // clean buffer before reciving data to it
		struct sockaddr_in receivedAddress;
		socklen_t receivedAddressLength = sizeof(sockaddr_in);
		memset(&receivedAddress, 0, receivedAddressLength);
		while (!success)
		{
			FD_ZERO(&readfds);
			FD_SET(socketFD, &readfds);
			tv.tv_sec = timeout;
			tv.tv_usec = 0;
			int result = select(socketFD + 1, &readfds, NULL, NULL, &tv); //this reads from server to client
			if (result == 0)
			{
				errorCount++;
				if (errorCount > maxErrorCount)
				{
					error.errorCode = htons(0);
					strcpy(error.message ,"Abandoning file transmission");
					if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, (struct sockaddr*)&clientAddress, clientAddressLength) < 0)
					{
						if (outputFile.is_open())
							outputFile.close();
						perror("TTFTP_ERROR: sendto fail");
						exit(1);
					}
					break;
				}
				if (outputFile.is_open())
					outputFile.close();
				continue;
			}

			if (result < 0)
			{
				if (outputFile.is_open())
					outputFile.close();
				perror("TTFTP_ERROR: select fail");
				exit(1);
			}
			bytesRead = recvfrom(socketFD, buffer, sizeof(buffer), 0, (struct sockaddr*)&receivedAddress, &receivedAddressLength);
			if (bytesRead < 0)
			{
				if (outputFile.is_open())
					outputFile.close();
				perror("TTFTP_ERROR: recvfrom fail");
				exit(1);
			}
			//if (strcmp(receivedAddress.sin_addr, clientAddress.sin_addr) !=0) //FIXME: check if compares the right fields fpr address
			if (receivedAddress.sin_addr.s_addr != clientAddress.sin_addr.s_addr)
			{
				//struct Data data1;
				//memcpy(&data1, buffer, sizeof(Data));  // FIXME why are we doing it?
				error.errorCode = htons(4);
				strcpy(error.message , "Unexpected packet");
				if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, (struct sockaddr*)&receivedAddress, receivedAddressLength) < 0)
				{
					if (outputFile.is_open())
						outputFile.close();
					perror("TTFTP_ERROR: sendto fail");
					exit(1);
				}
				if (outputFile.is_open())
					outputFile.close();
				continue;
			}

			success = true;
		}
		if (!success)
		{
			if(outputFile.is_open())
				outputFile.close();
			continue;
		}

		struct Data data1;
		memcpy(&(data1.opcode), buffer, 2);
		memcpy(&(data1.blockNumber), buffer + 2, 2);
		memcpy(&(data1.data), buffer + 4, PACKET_SIZE - 4);
		data1.opcode = ntohs(data1.opcode);
		data1.blockNumber = ntohs(data1.blockNumber);
		if (data1.opcode != OP_DATA)
		{
			error.errorCode = htons(4);
			strcpy(error.message , "Unexpected packet");
			if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, (struct sockaddr*)&receivedAddress, receivedAddressLength) < 0)
			{
				if (outputFile.is_open())
					outputFile.close();
				perror("TTFTP_ERROR: sendto fail");
				exit(1);
			}
			if (outputFile.is_open())
				outputFile.close();
			continue;
		}
		if (data1.blockNumber !=1)
		{
			error.errorCode = htons(0);
			strcpy(error.message, "Bad block number");
			if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, (struct sockaddr*)&receivedAddress, receivedAddressLength) < 0)
			{
				if (outputFile.is_open())
					outputFile.close();
				perror("TTFTP_ERROR: sendto fail");
				exit(1);
			}
			if (outputFile.is_open())
				outputFile.close();
			continue;
		}
		outputFile << data1.data; //FIXME check if this works
		success = false;
		//ack1
		struct ACK ack1;
		ack1.opcode = htons(OP_ACK);
		ack1.blockNumber = htons(1);
		//int bytesSent = sendto(socketFD, (void*)&ack1, (size_t)sizeof(ACK), 0, (struct sockaddr*)&clientAddress, clientAddressLength);				
		if (sendto(socketFD, (void*)&ack1, (size_t)sizeof(ACK), 0, (struct sockaddr*)&clientAddress, clientAddressLength) < 0)
		{
			if (outputFile.is_open())
				outputFile.close();
			perror("TTFTP_ERROR: sendto fail");
			exit(1);
		}
		//data2
		memset(buffer, '\0', sizeof(buffer)); // clean buffer before reciving data to it
		memset(&receivedAddress, 0, receivedAddressLength);
		while (!success)
		{
			FD_ZERO(&readfds);
			FD_SET(socketFD, &readfds);
			tv.tv_sec = timeout;
			tv.tv_usec = 0;
			int result2 = select(socketFD + 1, &readfds, NULL, NULL, &tv); //this reads from server to client
			if (result2 == 0)
			{
				errorCount++;
				if (errorCount > maxErrorCount)
				{

					error.errorCode = htons(0);
					strcpy(error.message , "Abandoning file transmission");
					if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, (struct sockaddr*)&clientAddress, clientAddressLength) < 0)
					{
						if (outputFile.is_open())
							outputFile.close();
						perror("TTFTP_ERROR: sendto fail");
						exit(1);
					}
					break;
				}
				if (outputFile.is_open())
					outputFile.close();
				continue;
			}

			if (result2 < 0)
			{
				if (outputFile.is_open())
					outputFile.close();
				perror("TTFTP_ERROR: select fail");
				exit(1);
			}
			bytesRead = recvfrom(socketFD, buffer, sizeof(buffer), 0, (struct sockaddr*)&receivedAddress, &receivedAddressLength);
			if (bytesRead < 0)
			{
				if (outputFile.is_open())
					outputFile.close();
				perror("TTFTP_ERROR: recvfrom fail");
				exit(1);
			}
			//if (strcmp(receivedAddress.sin_addr, clientAddress.sin_addr) != 0)
			if (receivedAddress.sin_addr.s_addr != clientAddress.sin_addr.s_addr)
			{
				//struct Data data2;
				//memcpy(&data2, buffer, sizeof(Data));  // FIXME why are we doing it?
				error.errorCode = htons(4);
				strcpy(error.message , "Unexpected packet");
				if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, (struct sockaddr*)&receivedAddress, receivedAddressLength) < 0)
				{
					if (outputFile.is_open())
						outputFile.close();
					perror("TTFTP_ERROR: sendto fail");
					exit(1);
				}
				if (outputFile.is_open())
					outputFile.close();
				continue;
			}

			success = true;
		}
		if (!success)
		{
			//handle close files errors
			if (outputFile.is_open())
				outputFile.close();
			continue;
		}

		struct Data data2;
		memcpy(&(data2.opcode), buffer, 2);
		memcpy(&(data2.blockNumber), buffer + 2, 2);
		memcpy(&(data2.data), buffer + 4, PACKET_SIZE - 4);
		data2.opcode = ntohs(data2.opcode);
		data2.blockNumber = ntohs(data2.blockNumber);
		//data2.data = ntohs(data2.data); //FIXME
		if (data2.opcode != OP_DATA)
		{
			error.errorCode = htons(4);
			strcpy(error.message , "Unexpected packet");
			if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, (struct sockaddr*)&receivedAddress, receivedAddressLength) < 0)
			{
				if (outputFile.is_open())
					outputFile.close();
				perror("TTFTP_ERROR: sendto fail");
				exit(1);
			}
			if (outputFile.is_open())
				outputFile.close();
			continue;
		}
		if (data2.blockNumber != 2)
		{
			error.errorCode = htons(0);
			strcpy(error.message , "Bad block number");
			if (sendto(socketFD, (void*)&error, (size_t)sizeof(ErrorMessage), 0, (struct sockaddr*)&receivedAddress, receivedAddressLength) < 0)
			{
				if (outputFile.is_open())
					outputFile.close();
				perror("TTFTP_ERROR: sendto fail");
				exit(1);
			}
			if (outputFile.is_open())
				outputFile.close();
			continue;
		}

		outputFile << data2.data; //FIXME check if this works
		success = false;
		//ack2
		struct ACK ack2;
		ack2.opcode = htons(OP_ACK);
		ack2.blockNumber = htons(2);

		if (sendto(socketFD, (void*)&ack2, (size_t)sizeof(ACK), 0, (struct sockaddr*)&clientAddress, clientAddressLength) < 0)
		{
			if (outputFile.is_open())
				outputFile.close();
			perror("TTFTP_ERROR: sendto fail");
			exit(1);
		}
			
		if (outputFile.is_open())
			outputFile.close();
	}
	close(socketFD);
}
