//bank.cpp

#include <vector>
#include <iostream>
#include <fstream>
#include <pthread.h>

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
//vector<string> commands;

class wrapperArgs
{
public:
	wrapperArgs(int atmID, char* path)
	{
		string s(path);
		this->path = s;
		this->atmID = atmID;
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
	atm.operateATM();
	return nullptr;
}

void* CommissionWrapper()
{
	if (bank.accounts.empty())
		return nullptr;

	int rate = ((std::rand() % MAX_RATE) + 1); //rate is 1%-5%

	for (auto& it : bank.accounts) {
		Account& currAccount = it.second;
		pthread_mutex_lock(&currAccount.mutex);
		bank.commission(currAccount, rate);
		pthread_mutex_unlock(&currAccount.mutex);
	}
	return nullptr;
}

void* PrintStatusWrapper()
{
	bank.printAccounts();
	return nullptr;
}

/*=============================================================================
* main function
=============================================================================*/


int main(int argc, char* argv[])
{
	openLogFile("log.txt");

	int* threadIDs = new int[argc + 2];
	pthread_t* threads = new pthread_t[argc + 2];
	for (int i = 0; i < argc; i++)
	{
		threadIDs[i] = i + 1;
		ATM atm(argv[i + 1], i + 1);
		wrapperArgs w(i + 1, argv[i + 1]);
		int result = pthread_create(&threads[i], nullptr, atmWrapper, &w);
		if (result != 0)
		{
			perror("Failed to create thread\n");
			delete[] threadIDs;
			delete[] threads;
			//FIXME close files not closed yet
			return 1;
		}
	}

	//added comments so it would compile, daniel - implement wrappers
	//int result = pthread_create(&threads[argc + 1], nullptr, CommissionWrapper, &threadIDs[argc + 1]);
	//FIXME check result
	//result = pthread_create(&threads[argc + 2], nullptr,PrintStatusWrapper(), &threadIDs[argc + 2]); //FIXME get wrapper function for printaccounts
	//FIXME check result

	//pthread join for last two ones once the other threads end

	for (int i = 0; i < argc + 2; i++)
		pthread_join(threads[i], nullptr);

	delete[] threadIDs;
	delete[] threads;
	logFile.close();

	
	return 0;
}
