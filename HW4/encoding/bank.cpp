//bank.cpp

#include <vector>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <unistd.h>

#include "bankClass.h"
#include "atm.h"

using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::ofstream;
using std::vector;

//global variables
Bank bank;
ofstream logFile;
Command defaultCommand("O -1 -1 -1");
Account defaultAccount(defaultCommand);
bool stopCommision = false;
bool stopStatusPrint = false;
pthread_mutex_t logLock;

class wrapperArgs
{
public:
	wrapperArgs() = default;
	wrapperArgs(char* path, int atmID)
	{
		this->atmID = atmID;
		string s(path);
		this->path = s;
	}
	string path;
	int atmID;
};


/*=============================================================================
* global functions
=============================================================================*/
void* atmWrapper(void* arg)
{
	wrapperArgs* w = static_cast<wrapperArgs*>(arg);
	ATM atm(w->path, w->atmID);
	string line;
	while (1)
	{
		usleep(SECOND / 10);
		if (atm.input.eof())
			break;
		std::getline(atm.input, line);
		Command command(line);
		atm.handleAction(command);
	}
	return nullptr;
}

void* CommissionWrapper(void*)
{
	while (1)
	{
		if (stopCommision)
			return nullptr;
		usleep(SECOND * 3);
		if (bank.accounts.empty())
			continue;

		int rate = ((std::rand() % MAX_RATE) + 1); //rate is 1%-5%
		for (auto& it : bank.accounts)
		{
			pthread_mutex_lock(&bank.mutex);
			Account& currentAccount = it.second;
			currentAccount.io.enterWriter();
			bank.commission(currentAccount, rate);
			currentAccount.io.exitWriter();
			pthread_mutex_unlock(&bank.mutex);
		}
	}
	return nullptr;
}

void* PrintStatusWrapper(void*)
{
	int i = 0;
	while (1)
	{
		cout << "entering iteration " << i++ << endl;;
		if (stopStatusPrint)
			return nullptr;
		usleep(SECOND / 2);
		//bank.printAccounts();
	}
	return nullptr;
}

/*=============================================================================
* main function
=============================================================================*/


int main(int argc, char* argv[])
{
	openLogFile("log.txt");
	//argc + 1 threads: argc - 1 for ATMs (first one is program name), two more for commission and status prints = argc + 1
	int* threadIDs = new int[argc + 1];
	pthread_t* threads = new pthread_t[argc + 1];
	wrapperArgs* wrapperArgsArray = new wrapperArgs[argc - 1];
	pthread_mutex_init(&logLock, nullptr);
	int result;

	for (int i = 0; i < argc - 1; i++) //init ATMs
	{
		threadIDs[i] = i + 1; //ATM numbers start with 1, match IDs with ATM numbers
		wrapperArgsArray[i].atmID = i + 1;
		wrapperArgsArray[i].path = argv[i + 1];
		result = pthread_create(&threads[i], nullptr, atmWrapper, &(wrapperArgsArray[i]));
		if (result != 0)
		{
			perror("Bank error: pthread_create failed");
			delete[] threadIDs;
			delete[] wrapperArgsArray;
			delete[] threads;
			logFile.close();
			exit(1);
		}
	}

	threadIDs[argc - 1] = argc;
	result = pthread_create(&threads[argc - 1], nullptr, CommissionWrapper, nullptr);
	if (result != 0)
	{
		perror("Bank error: pthread_create failed");
		delete[] threadIDs;
		delete[] wrapperArgsArray;
		delete[] threads;
		logFile.close();
		exit(1);
	}

	threadIDs[argc] = argc + 1;
	result = pthread_create(&threads[argc], nullptr, PrintStatusWrapper, nullptr);
	if (result != 0) {
		perror("bank error: pthread_create failed");
		delete[] threadIDs;
		delete[] wrapperArgsArray;
		delete[] threads;
		logFile.close();
		exit(1);
	}

	for (int i = 0; i < argc - 1; i++)
		pthread_join(threads[i], nullptr);

	stopCommision = true;
	pthread_join(threads[argc - 1], nullptr);
	stopStatusPrint = true;
	pthread_join(threads[argc], nullptr);

	delete[] threadIDs;
	delete[] threads;
	delete[] wrapperArgsArray;
	logFile.close();
	pthread_mutex_destroy(&logLock);

	cout << "reached end of program" << endl;
	return 0;
}