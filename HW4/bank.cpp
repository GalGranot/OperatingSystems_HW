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
		sleep(0.1);
		if (atm.input.eof())
			break;
		std::getline(atm.input, line);
		Command command(line);
		atm.handleAction(command);
	}
	return nullptr;
}

void* CommissionWrapper(void* arg)
{
	while (1)
	{
		if (stopCommision)
			return nullptr;
		usleep(300);
		if (bank.accounts.empty())
			continue;

		int rate = ((std::rand() % MAX_RATE) + 1); //rate is 1%-5%
		logFile << "trying to lock bank in commision" << endl;
		pthread_mutex_lock(&bank.mutex);
		logFile << "locked bank in commision" << endl;
		for (auto& it : bank.accounts)
		{
			Account& currentAccount = it.second;
			currentAccount.io.enterWriter(); logFile << "started writing to account " << currentAccount.getID() << " in commision" << endl;
		}
		for(auto& it : bank.accounts)
		{
			Account& currentAccount = it.second;
			bank.commission(currentAccount, rate);
			currentAccount.io.exitWriter(); logFile << "finished writing to account " << currentAccount.getID() << " in commision" << endl;
		}
		logFile << "trying to unlock bank in commision" << endl;
		pthread_mutex_unlock(&bank.mutex);
		logFile << "unlocked bank in commision" << endl;
	}
	return nullptr;
}

void* PrintStatusWrapper(void*)
{
	cout << "entering status wrapper" << endl;
	int i = 0;
	while (1)
	{
		usleep(500);
		cout << "entering iteration " << i++ << endl;;
		if (stopStatusPrint)
			return nullptr;
		bank.printAccounts();
		if (i > 10)
			break;
	}
	return nullptr;
}

/*=============================================================================
* main function
=============================================================================*/

int main3(int argc, char* argv[])
{
	openLogFile("log.txt");
	//int* threadIDs = new int[argc - 1];
	//pthread_t* threads = new pthread_t[argc - 1];
	wrapperArgs* wrapperArgsArray = new wrapperArgs[argc - 1];
	pthread_mutex_init(&logLock, nullptr);
	//int result;

	/*
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

	for (int i = 0; i < argc - 1; i++)
		pthread_join(threads[i], nullptr);
	*/
	wrapperArgsArray[0].atmID = 1;
	wrapperArgsArray[0].path = argv[1];
	atmWrapper(&wrapperArgsArray[0]);
	PrintStatusWrapper(0);
	//delete[] threadIDs;
	//delete[] wrapperArgsArray; //FIXME - this fails for some reason. need to valgrind for mem leaks
	//delete[] threads;	//FIXME - this fails for some reason. need to valgrind for mem leaks
	logFile.close();
	pthread_mutex_destroy(&logLock);

	cout << "reached end of program" << endl;
	return 0;
}
int main2(int argc, char* argv[])
{
	openLogFile("log.txt");
	int* threadIDs = new int[argc - 1];
	pthread_t* threads = new pthread_t[argc - 1];
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

	for (int i = 0; i < argc - 1; i++)
		pthread_join(threads[i], nullptr);

	//delete[] threadIDs;
	//delete[] wrapperArgsArray; //FIXME - this fails for some reason. need to valgrind for mem leaks
	//delete[] threads;	//FIXME - this fails for some reason. need to valgrind for mem leaks
	logFile.close();
	pthread_mutex_destroy(&logLock);

	cout << "reached end of program" << endl;
	return 0;
}

int main(int argc, char* argv[])
{
	openLogFile("log.txt");
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

	/**/
	threadIDs[argc - 1] = argc;
	result = pthread_create(&threads[argc], nullptr, CommissionWrapper, nullptr);
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
	result = pthread_create(&threads[argc + 1], nullptr, PrintStatusWrapper, nullptr);
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
	stopStatusPrint = true;
	pthread_join(threads[argc - 1], nullptr);
	pthread_join(threads[argc], nullptr);

	//delete[] threadIDs;
	//delete[] wrapperArgsArray; //FIXME - this fails for some reason. need to valgrind for mem leaks
	//delete[] threads;	//FIXME - this fails for some reason. need to valgrind for mem leaks
	logFile.close();
	pthread_mutex_destroy(&logLock);

	cout << "reached end of program" << endl;
	return 0;
}