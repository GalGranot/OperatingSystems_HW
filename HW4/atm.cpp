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

void logWrite(const string message)
{
	pthread_mutex_lock(&logLock);
	logFile << message;
	pthread_mutex_unlock(&logLock);
}

int ATM::getID() { return id; }

string getSmallerString(string s1, string s2) { return ((stoi(s1) <= stoi(s2)) ? s1 : s2); }

inline void enterWriterInOrder(Account& a1, Account& a2)
{
	a1.io.enterWriter();
	a2.io.enterWriter();
}

inline void exitWriterInOrder(Account& a1, Account& a2)
{
	a1.io.exitWriter();
	a2.io.exitWriter();
}

void ATM::handleAction(Command command)
{
	logFile << "command: type " << command.commandType << ", sourceID " << command.sourceID << endl;
	ostringstream oss;
	logFile << "trying to lock bank in command" << endl;
	pthread_mutex_lock(&bank.mutex);
	logFile << "locked bank in command" << endl;
	Account& sourceAccount = bank.getAccountByID(command.sourceID);

	if (command.commandType == 'O') //open command
	{
		if (sourceAccount.getID() != NO_ID) //account already exists
		{
			oss << "Error " << getID() << ": Your transaction failed - account with same id exists" << endl;
			logWrite(oss.str());
		}
		else //account doesn't exist, init it
		{
			Account account(command);
			bank.addAccount(account);
			oss << this->getID() << ": New account id is " << account.getID() << " with password " << account.getPassword() << " and initial balance " << account.getBalance() << endl;
			logWrite(oss.str());
		}
		logFile << "trying to unlock bank in command" << endl; pthread_mutex_unlock(&bank.mutex); logFile << "unlocked bank in command" << endl;
	}

	else if (command.commandType == 'Q') //close command
	{
		if (sourceAccount.getID() == NO_ID)
		{
			oss << "Error " << this->getID() << ": Your transaction failed - account id " << command.sourceID << " does not exist" << endl;
			logWrite(oss.str());
		}

		else if (command.password != sourceAccount.getPassword())
		{
			oss << "Error " << this->getID() << ": Your transaction failed - password for account id " << command.sourceID << " is incorrect" << endl;
			logWrite(oss.str());
		}
		else
		{
			//write an invalid id to the account so it isn't found, then delete it
			sourceAccount.io.enterWriter(); logFile << "command: started writing to account " << sourceAccount.getID() << endl;
			map<string, Account>::iterator it = bank.accounts.find(command.sourceID);
			sourceAccount.setID(NO_ID);
			sourceAccount.io.exitWriter(); logFile << "finished writing to account " << sourceAccount.getID() << endl;
			int tmpBalance = sourceAccount.getBalance();
			if (it != bank.accounts.end())
			{
				bank.accounts.erase(command.sourceID);
				oss << this->getID() << ": Account " << command.sourceID << " is now closed. Balance was " << tmpBalance << endl;
				logWrite(oss.str());
			}
		}
		logFile << "trying to unlock bank in command" << endl; pthread_mutex_unlock(&bank.mutex); logFile << "unlocked bank in command" << endl;
	}

	else if (command.commandType == 'B') //balance command
	{
		if (sourceAccount.getID() == NO_ID)
		{
			oss << "Error " << this->getID() << ": Your transaction failed - account id " << command.sourceID << " does not exist" << endl;
			logWrite(oss.str());
			logFile << "trying to unlock bank in command" << endl; pthread_mutex_unlock(&bank.mutex); logFile << "unlocked bank in command" << endl;
			return;
		}
		sourceAccount.io.enterReader(); logFile << "command: started reading from account " << sourceAccount.getID() << endl;
		logFile << "trying to unlock bank in command" << endl; pthread_mutex_unlock(&bank.mutex); logFile << "unlocked bank in command" << endl;
		if (command.password != sourceAccount.getPassword())
		{
			oss << "Error " << this->getID() << ": Your transaction failed - password for account id " << command.sourceID << " is incorrect" << endl;
			logWrite(oss.str());
		}
		else
		{
			oss << this->getID() << ": Account " << command.sourceID << " balance is " << sourceAccount.getBalance() << endl;
			logWrite(oss.str());
		}
		sourceAccount.io.exitReader(); logFile << "finished reading from account " << sourceAccount.getID() << endl;
	}

	else if (command.commandType == 'D') //deposit command
	{
		if (sourceAccount.getID() == NO_ID)
		{
			oss << "Error " << this->getID() << ": Your transaction failed - account id " << command.sourceID << " does not exist" << endl;
			logWrite(oss.str());
			logFile << "trying to unlock bank in command" << endl; pthread_mutex_unlock(&bank.mutex); logFile << "unlocked bank in command" << endl;
			return;
		}
		sourceAccount.io.enterWriter(); logFile << "command: started writing to account " << sourceAccount.getID() << endl;
		logFile << "trying to unlock bank in command" << endl; pthread_mutex_unlock(&bank.mutex); logFile << "unlocked bank in command" << endl;
		if (command.password != sourceAccount.getPassword())
		{
			oss << "Error " << this->getID() << ": Your transaction failed - password for account id " << command.sourceID << " is incorrect" << endl;
			logWrite(oss.str());
		}
		else
		{
			sourceAccount.addToBalance(command.amount);
			oss << this->getID() << ": Account " << sourceAccount.getID() << " new balance is " << sourceAccount.getBalance() << " after " << command.amount << " $ was deposited" << endl;
			logWrite(oss.str());
		}
		sourceAccount.io.exitWriter(); logFile << "finished writing to account " << sourceAccount.getID() << endl;
	}

	else if (command.commandType == 'W')
	{
		if (sourceAccount.getID() == NO_ID)
		{
			oss << "Error " << this->getID() << ": Your transaction failed - account id " << command.sourceID << " does not exist" << endl;
			logWrite(oss.str());
			logFile << "trying to unlock bank in command" << endl; pthread_mutex_unlock(&bank.mutex); logFile << "unlocked bank in command" << endl;
			return;
		}
		sourceAccount.io.enterWriter(); logFile << "command: started writing to account " << sourceAccount.getID() << endl;
		logFile << "trying to unlock bank in command" << endl; pthread_mutex_unlock(&bank.mutex); logFile << "unlocked bank in command" << endl;
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
			oss << this->getID() << ": Account " << sourceAccount.getID() << " new balance is " << sourceAccount.getBalance() << " after " << command.amount << " $ was withdrew" << endl;
			logWrite(oss.str());
		}
		sourceAccount.io.exitWriter(); logFile << "finished writing to account " << sourceAccount.getID() << endl;
	}

	else if (command.commandType == 'T') //transfer command
	{
		if (sourceAccount.getID() == NO_ID)
		{
			oss << "Error " << this->getID() << ": Your transaction failed - account id " << command.sourceID << " does not exist" << endl;
			logWrite(oss.str());
			logFile << "trying to unlock bank in command" << endl; pthread_mutex_unlock(&bank.mutex); logFile << "unlocked bank in command" << endl;
			return;
		}
		Account& targetAccount = bank.getAccountByID(command.targetID);
		if (targetAccount.getID() == NO_ID)
		{
			oss << "Error " << this->getID() << ": Your transaction failed - account id " << command.targetID << " does not exist" << endl;
			logWrite(oss.str());
			logFile << "trying to unlock bank in command" << endl; pthread_mutex_unlock(&bank.mutex); logFile << "unlocked bank in command" << endl;
			return;
		}
		//if reached here - both accounts exist. lock accounts by order - smallest id first
		string smallerID = getSmallerString(sourceAccount.getID(), targetAccount.getID());
		bool sourceSmallerID = (smallerID == sourceAccount.getID());
		if (sourceSmallerID)
			enterWriterInOrder(sourceAccount, targetAccount);
		else
			enterWriterInOrder(targetAccount, sourceAccount);
		logFile << "trying to unlock bank in command" << endl; pthread_mutex_unlock(&bank.mutex); logFile << "unlocked bank in command" << endl;

		//now both accounts are locked, bank is unlocked
		if (command.password != sourceAccount.getPassword())
		{
			oss << "Error " << this->getID() << ": Your transaction failed - password for account id " << command.sourceID << " is incorrect" << endl;
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
			oss << this->getID() << ": Transfer " << command.amount << " from account " << sourceAccount.getID() << " to account " << targetAccount.getID() << endl << "new account balance is " << sourceAccount.getBalance() << "new target account balance is " << targetAccount.getBalance() << endl;
			logWrite(oss.str());
		}

		//account is finished, exit accounts in reverse order if entry
		if (sourceSmallerID)
			exitWriterInOrder(targetAccount, sourceAccount);
		else
			exitWriterInOrder(sourceAccount, targetAccount);
	}
}

//old functions - don't use
/*
void ATM::handleActionOLD(Command command)
{
	ostringstream oss;
	Account& sourceAccount;
	Account& targetAccount;

	if (command.commandType == 'O') //open command
	{
		pthread_mutex_lock(&bank.mutex);
		sourceAccount = bank.getAccountByID(command.sourceID);
		if(sourceAccount.getID() != NO_ID) //account already exists
		{
			oss << "Error " << getID() << ": Your transaction failed - account with same id exists" << endl;
			logWrite(oss.str());
		}

		else //account doesn't exist, init it
		{
			Account account(command);
			pthread_mutex_lock(&account.mutex);
			bank.addAccount(account);
			oss << this->getID() << ": New account id is " << account.getID() 
			<< " with password " << account.getPassword() << " and initial balance " 
			<< account.getBalance() << endl;
			logWrite(oss.str());
			logFile << "trying to unlock bank in command" << endl; pthread_mutex_unlock(&bank.mutex); logFile << "unlocked bank in command" << endl;
		}
		logFile << "trying to unlock bank in command" << endl; pthread_mutex_unlock(&bank.mutex); logFile << "unlocked bank in command" << endl;
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
	Account& sourceAccount = bank.getAccountByID(command.sourceID);
	if (sourceAccount.getID() == NO_ID)
	{
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
	pthread_exit(nullptr);
}
*/
