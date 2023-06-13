//atm.cpp

#include "atm.h"

using std::cin;
using std::cout;
using std::endl;
using std::stoi;
using std::vector;
using std::string;
using std::ifstream;
using std::ostringstream;

ATM::ATM(string path, int id)
{
	this->input.open(path);
	//fixme check if doesn't open
	this->id = id;
}

ATM::~ATM()
{
	if (input.is_open())
		input.close();
}

inline void logWrite(const string message)
{
	pthread_mutex_lock(&logLock);
	logFile << message;
	pthread_mutex_unlock(&logLock);
}

int ATM::getID() { return id; }

void ATM::handleAction(Command command, Account& sourceAccount, Account& targetAccount)
{
	ostringstream oss;
	logFile << "DEBUG " << endl << "soruce account id " << sourceAccount.getID() << endl;
	if (command.commandType == 'O') //open command
	{
		if(sourceAccount.getID() != NO_ID)
		{
			oss << "Error " << getID() <<
				": Your transaction failed - account with same id exists" << endl;
			logWrite(oss.str());
		}
		else
		{
			pthread_mutex_lock(&bank.mutex);
			if (bank.accounts.count(command.sourceID) != 0)
			{
				pthread_mutex_unlock(&bank.mutex);
				oss << "Error " << getID() <<
					": Your transaction failed - account with same id exists" << endl;
				logWrite(oss.str());
				return;
			}
		Account account(command);
		pthread_mutex_lock(&account.mutex);
		oss << this->getID() << ": New account id is " << account.getID() 
		<< " with password " << account.getPassword() << " and initial balance " 
		<< account.getBalance() << endl;
		logWrite(oss.str());
		bank.addAccount(account);
		pthread_mutex_unlock(&account.mutex);
		pthread_mutex_unlock(&bank.mutex);
		}
	}

	else if (command.commandType == 'D')
	{
		if (sourceAccount.getID() == NO_ID)
		{
			oss << "Error " << this->getID() << ": Your transaction failed - account id " << command.sourceID << " does not exist" << endl;
			logWrite(oss.str());
		}

		else if (command.password != sourceAccount.getPassword())
		{
			oss << "Error " << this->getID() << ": Your transaction failed - password for account id "
				<< command.sourceID << " is incorrect" << endl;
			logWrite(oss.str());
		}
			
		else
		{
			sourceAccount.addToBalance(command.amount);
			oss << this->getID() << ": Account " << sourceAccount.getID()
				<< " new balance is " << sourceAccount.getBalance() << " after "
				<< command.amount << " $ was deposited" << endl;;
			logWrite(oss.str());
		}
	}

	else if (command.commandType == 'W')
	{
		if (sourceAccount.getID() == NO_ID)
		{
			oss << "Error " << this->getID() << ": Your transaction failed - account id " << command.sourceID << " does not exist" << endl;
			logWrite(oss.str());

		}

		else if (command.password != sourceAccount.getPassword())
		{
			oss << "Error " << this->getID() << ": Your transaction failed - password for account id "
				<< command.sourceID << " is incorrect" << endl;
			logWrite(oss.str());
		}
		else if (command.amount > sourceAccount.getBalance())
		{
			oss << "Error " << this->getID() << ": Your transaction failed – account id " << sourceAccount.getID() << " balance is lower than " << command.amount << endl;
			logWrite(oss.str());
		}
		else
		{
			sourceAccount.addToBalance(-command.amount);
			oss << this->getID() << ": Account " << sourceAccount.getID()
				<< " new balance is " << sourceAccount.getBalance() << " after "
				<< command.amount << " $ was withdrew" << endl;
			logWrite(oss.str());
		}
	}

	else if (command.commandType == 'B')
	{
		if (sourceAccount.getID() == NO_ID)
		{
			oss << "Error " << this->getID() << ": Your transaction failed - account id " << command.sourceID << " does not exist" << endl;
			logWrite(oss.str());
		}
		else if (command.password != sourceAccount.getPassword())
		{
			oss << "Error " << this->getID() << ": Your transaction failed - password for account id "
				<< command.sourceID << " is incorrect" << endl;
			logWrite(oss.str());
		}
		else
		{
			oss << this->getID() << ": Account " << command.sourceID << " balance is " << sourceAccount.getBalance() << endl;
			logWrite(oss.str());
		}
	}

	
	else if (command.commandType == 'Q')
	{
		if (sourceAccount.getID() == NO_ID)
		{
			oss << "Error " << this->getID() << ": Your transaction failed - account id " << command.sourceID << " does not exist" << endl;
			logWrite(oss.str());
		}

		else if (command.password != sourceAccount.getPassword())
		{
			oss << "Error " << this->getID() << ": Your transaction failed - password for account id "
				<< command.sourceID << " is incorrect" << endl;
			logWrite(oss.str());
		}
		else
		{
			int tmpBalance = sourceAccount.getBalance();
			map<int, Account>::iterator it = bank.accounts.find(command.sourceID);
			if (it != bank.accounts.end())
			{
				bank.accounts.erase(command.sourceID);
				oss << this->getID() << ": Account " << command.sourceID << " is now closed. Balance was " << tmpBalance << endl;
				logWrite(oss.str());
			}
			else
				;
		}
	}
	
	else if (command.commandType == 'T')
	{
		bool flag = (sourceAccount.getID() == NO_ID);
		if (flag)
		{
			oss << "Error " << this->getID() << ": Your transaction failed - account id " << command.sourceID << " does not exist" << endl;
			logWrite(oss.str());
			return;
		}
		flag = (targetAccount.getID() == NO_ID);
		if (flag)
		{
			oss << "Error " << this->getID() << ": Your transaction failed - account id " << command.targetID << " does not exist" << endl;
			logWrite(oss.str());
			return;
		}

		if (command.password != sourceAccount.getPassword())
		{
			oss << "Error " << this->getID() << ": Your transaction failed - password for account id "
				<< command.sourceID << " is incorrect" << endl;
			logWrite(oss.str());
		}
		else if (command.amount > sourceAccount.getBalance())
		{
			oss << "Error " << this->getID() << ": Your transaction failed – account id " << sourceAccount.getID() << " balance is lower than " << command.amount << endl;
			logWrite(oss.str());
		}
		else
		{

			sourceAccount.addToBalance(-command.amount);
			targetAccount.addToBalance(command.amount);
			oss << this->getID() << ": Transfer " << command.amount << " from account " 
			<< sourceAccount.getID() << " to account " << targetAccount.getID() << endl
			<< "new account balance is " << sourceAccount.getBalance() 
			<< "new target account balance is " << targetAccount.getBalance() << endl;
			logWrite(oss.str());
		}
	}
}


void ATM::operateATM()
{
	string line;
	while (1)
	{
		if (this->input.eof())
			break;
		std::getline(this->input, line);
		Command command(line);
		Account& sourceAccount = bank.getAccountByID(command.sourceID);
		if (sourceAccount.getID() == NO_ID)
		{
			logFile << "DEBUG" << endl << "here" << endl;
			this->handleAction(command, sourceAccount, sourceAccount);
			continue;
		}

		pthread_mutex_lock(&sourceAccount.mutex);
		if (command.targetID != NO_ID)
		{
			Account& targetAccount = bank.getAccountByID(command.targetID);
			pthread_mutex_lock(&targetAccount.mutex);
			this->handleAction(command, sourceAccount, targetAccount);
			pthread_mutex_unlock(&targetAccount.mutex);
		}
		else
			this->handleAction(command, sourceAccount, sourceAccount);
		
		pthread_mutex_unlock(&sourceAccount.mutex);
	}
	pthread_exit(nullptr);
}