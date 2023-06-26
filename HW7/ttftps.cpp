//ttftps.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <sys/socket.h>

using std::string;
using std::atoi;
using std::ofstream;

#define BYTE 8
#define PACKET_SIZE 516
#define ERROR -1
#define ARG_NUM 4

#define OP_WRQ 2
#define OP_DATA 3
#define OP_ACK 4

struct WRQ
{
	unsigned char opcode[2 * BYTE];
	string fileName;
	string transmissionMode;
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



int main(int argc, char* argv[])
{
	if (argc != ARG_NUM)
	{
		perror("TTFTP_ERROR: illegal arguments");
		exit(1);
	}

	int socketFD = socket(AF_INET, SOCK_DGRAM, 0);
	if(socketFD < 0)
	{
		perror("TTFTP_ERROR: fail to open socket");
		exit(1);
	}

	struct sockaddr_in serverAddress = { 0 };
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADRR_ANY;
	serverAddress.sin_port = htons(atoi(argv[1]));
	if (bind(socketFD, (struct sockadrr*)&serverAddress, sizeof(serverAddress)) < 0)
	{
		perror("TTFTP_ERROR: fail to bind socket");
		exit(1);
	}
	
	char buffer[PACKET_SIZE];
	
	while (1)
	{
		//stop condition FIXME
		int bytesRead = recv(socketFD, buffer, PACKET_SIZE, 0);
		if (bytesRead < 0)
		{
			//handle error FIXME
		}
		//FIXME validate input

		struct WRQ wrq = (WRQ)buffer;
		if (wrq.opcode != OP_WRQ)
		{
			//FIXME handle wrong opcode error
		}
		ofstream outputFile(wrq.fileName, std::ios::out | std::ios::binary); //
		if (!outputFile)
		{
			//handle error
		}

		struct ACK ack0;
		ack0.opcode = OP_ACK;
		ack0.blockNumber = 0;


		
		if (outputFile.is_open())
			outFile.close();
	}
	close(socketFD);

	
}
