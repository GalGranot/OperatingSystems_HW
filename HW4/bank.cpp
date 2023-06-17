//bank.cpp

#include <vector>

#include "bankClass.h"
#include "atm.h"

using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::vector;

//global variables
Bank bank;
<<<<<<< Updated upstream
//vector<string> commands;
//vector<ATM> ATMs;
=======
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
		usleep(300);
		if (bank.accounts.empty())
			continue;

		int rate = ((std::rand() % MAX_RATE) + 1); //rate is 1%-5%
		pthread_mutex_lock(&bank.mutex);
		for (auto& it : bank.accounts)
		{
			Account& currentAccount = it.second;
			currentAccount.io.enterWriter(); logFile << "writing to account " << currentAccount.getID() << " in commision" << endl;;
		}
		for(auto& it : bank.accounts)
		{
			Account& currentAccount = it.second;
			bank.commission(currentAccount, rate);
			currentAccount.io.exitWriter(); logFile << "finished writing to account " << currentAccount.getID() << " in commision" << endl;
		}
		pthread_mutex_unlock(&bank.mutex);
	}
	return nullptr;
}

void* PrintStatusWrapper(void*)
{
	cout << "entering status wrapper" << endl;
	int i = 0;
	while (1)
	{
		cout << "entering iteration " << i++ << endl;;
		if (stopStatusPrint)
			return nullptr;
		usleep(500);
		bank.printAccounts();
	}
	return nullptr;
}

/*=============================================================================
* main function
=============================================================================*/

>>>>>>> Stashed changes

int main(int argc, char* argv[])
{
	ATM atm("/media/sf_OperatingSystems_HW/HW4", 1);
	return 0;
}