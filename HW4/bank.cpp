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
//vector<string> commands;

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
	cout << "atm " << w->atmID << " operating" << endl;
	atm.operateATM();
	return nullptr;
}

void* CommissionWrapper(void*)
{
	while (1) {
		usleep(3000);
		if (bank.accounts.empty())
			return nullptr;

		int rate = ((std::rand() % MAX_RATE) + 1); //rate is 1%-5%

		for (auto& it : bank.accounts) {
			Account& currAccount = it.second;
			pthread_mutex_lock(&currAccount.mutex);
			bank.commission(currAccount, rate);
			pthread_mutex_unlock(&currAccount.mutex);
		}
	}
	return nullptr;
}

void* PrintStatusWrapper(void*)
{
	while (1) {
		usleep(500);
		bank.printAccounts();
		return nullptr;
	}
}

/*=============================================================================
* main function
=============================================================================*/


int main(int argc, char* argv[])
{
	/* thread count:
	* argc - 1 threads for ATMs
	* 2 threads for bank - commision and print
	* total - argc + 1 threads
	*/
	openLogFile("log.txt");
	int* threadIDs = new int[argc + 1];
	wrapperArgs* wrapperArgsArray = new wrapperArgs[argc - 1];
	pthread_t* threads = new pthread_t[argc + 1];

	for(int i = 0; i < argc - 1; i++) //init ATMs
	{
		threadIDs[i] = i + 1;
		wrapperArgsArray[i].atmID = i + 1;
		wrapperArgsArray[i].path = argv[i + 1];
		int result = pthread_create(&threads[i], nullptr, atmWrapper, &(wrapperArgsArray[i]));
		if (result != 0)
		{
			delete[] threadIDs;
			delete[] wrapperArgsArray;
			delete[] threads;
			logFile.close();
			exit(1);
		}
	}

	//added comments so it would compile, daniel - implement wrappers
	int result = pthread_create(&threads[argc + 1], nullptr, CommissionWrapper, nullptr);
	if (result != 0) {
		perror("bank error: pthread_create failed" );
		return 1;
	}
	result = pthread_create(&threads[argc + 2], nullptr,PrintStatusWrapper, nullptr); //FIXME get wrapper function for printaccounts
	if (result != 0) {
		perror("bank error: pthread_create failed");
		return 1;
	}

	//pthread join for last two ones once the other threads end

	//FIXME - i < argc - 1 should be argc + 1 after daniel adds more threads
	for (int i = 0; i < argc - 1; i++)
	{
		printf("trying to join %d\n", i);
		pthread_join(threads[i], nullptr);
	}

	delete[] threadIDs;
	delete[] wrapperArgsArray;
	delete[] threads;
	logFile.close();

	cout << "reached end of program" << endl;
	return 0;
}
