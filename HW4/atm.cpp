//atm.cpp

#include "atm.h"

using std::cin;
using std::cout;
using std::endl;
using std::stoi;
using std::vector;
using std::string;
using std::ifstream;

/*FIXME daniel: implement string parsing here
Command::Command(string line)
{

}*/


//FIXME - this is tmp for testing, implement above
Command::Command(string line)
{
	//parse strings
	vector<string> stringParses;
	const char delimiter = ' ';
	int i = 0;
	int start = 0;
	int end = line.find(delimiter);
	while (end != -1)
	{
		stringParses[i++] = line.substr(start, end - start);
		start = end + 1;
		end = line.find(delimiter, start);
	}

	//put parses in place
	commandType = static_cast<commandTypes>(stringParses[0][0]);
	sourceID = stoi(stringParses[1]);
	password = stoi(stringParses[2]);
	if (i == FIELDS_NUM - 1) //received amount
		amount = stoi(stringParses[3]);
	else
		amount = NOT_SET;
	if (i == FIELDS_NUM)
		targetID = stoi(stringParses[4]);
	else
		targetID = NOT_SET;
}

//FIXME remove later, this is for debugging only
void Command::printCommand()
{
	cout << "printing command\n";
	cout << "command type: " << commandType << "\n";
	cout << "source id: " << sourceID << "\n";
	cout << "password: " << password << "\n";
	cout << "amount " << amount << "\n";
	cout << "target id: " << targetID << "\n";

}


ATM::ATM(string filePath, int id)
{
	/*if (!f.is_open)
		//do something FIXME*/
	//add check for correct file opening
	input.open(filePath);
	this->id = id;
}

ATM::~ATM()
{
	if (input.is_open())
		input.close();
}

int ATM::getID() { return id; }

void ATM::handleAction(Command command, Bank bank)
{
<<<<<<< Updated upstream
<<<<<<< Updated upstream
	if (command.commandType == O) //open command
=======
	//O D W B Q T 
=======
	a1.io.enterWriter(); logFile << "started writing from account " << a1.getID() << " in command: T" << endl;
	a2.io.enterWriter(); logFile << "started writing from account " << a2.getID() << " in command: T"<< endl;
}

inline void exitWriterInOrder(Account& a1, Account& a2)
{
	a1.io.exitWriter(); logFile << "finished writing from account " << a1.getID() << " in command: T" << endl;
	a2.io.exitWriter();	logFile << "finished writing from account " << a2.getID() << " in command: T" << endl;
}

void ATM::handleAction(Command command)
{
	logFile << "starting with command " << command.commandType << " for account " << command.sourceID << endl;
	ostringstream oss;
	pthread_mutex_lock(&bank.mutex);
	Account& sourceAccount = bank.getAccountByID(command.sourceID);
>>>>>>> Stashed changes

	if (command.commandType == 'O') //open command
>>>>>>> Stashed changes
	{
		Account currAccount = bank.getAccountByID(command.sourceID);
		if(currAccount.getID() == NO_ID)
		{
<<<<<<< Updated upstream
			cout << "Error <ATM " << getID() <<
			">: Your transaction failed - account with same id exists" << endl;
			return;
		}

		Account account(command);
		bank.addAccount(account); //FIXME gal - make sure we dont need to dynamically allocate account
		cout << "<ATM " << this->getID() << ">: New account id is " << account.getID() <<
			" with password " << account.getPassword() << " and initial balance " <<
			account.getBalance() << endl;
		//FIXME gal - print to log & to screen
=======
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
		pthread_mutex_unlock(&bank.mutex);
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
			oss << "Error " << this->getID() << ": Your transaction failed - password for account id "
				<< command.sourceID << " is incorrect" << endl;
			logWrite(oss.str());
		}
		else
		{
			//write an invalid id to the account so it isn't found, then delete it
			sourceAccount.io.enterWriter(); logFile << "started writing to account " << sourceAccount.getID() << " in command: " << command.commandType << endl;
			map<string, Account>::iterator it = bank.accounts.find(command.sourceID);
			sourceAccount.setID(NO_ID);
			sourceAccount.io.exitWriter(); logFile << "finished writing to account " << sourceAccount.getID() << " in command: " << command.commandType << endl;
			int tmpBalance = sourceAccount.getBalance();
			if (it != bank.accounts.end())
			{
				bank.accounts.erase(command.sourceID);
				oss << this->getID() << ": Account " << command.sourceID << " is now closed. Balance was " << tmpBalance << endl;
				logWrite(oss.str());
			}
		}
		pthread_mutex_unlock(&bank.mutex);
	}

	else if (command.commandType == 'B') //balance command
	{
		if (sourceAccount.getID() == NO_ID)
		{
			oss << "Error " << this->getID() << ": Your transaction failed - account id " << command.sourceID << " does not exist" << endl;
			logWrite(oss.str());
			pthread_mutex_unlock(&bank.mutex);
			return;
		}
		sourceAccount.io.enterReader(); logFile << "reading from account " << sourceAccount.getID() << " in command: " << command.commandType << endl;
		pthread_mutex_unlock(&bank.mutex);
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
		sourceAccount.io.exitReader(); logFile << "finished reading from acount " << sourceAccount.getID() << " in command: " << command.commandType << endl;
>>>>>>> Stashed changes
	}

	/*
	else if (command.commandType == D)
	{
<<<<<<< Updated upstream

=======
		if (sourceAccount.getID() == NO_ID)
		{
			oss << "Error " << this->getID() << ": Your transaction failed - account id " << command.sourceID << " does not exist" << endl;
			logWrite(oss.str());
			pthread_mutex_unlock(&bank.mutex);
			return;
		}
		sourceAccount.io.enterWriter(); logFile << "started writing to account " << sourceAccount.getID() << " in command: " << command.commandType << endl;
		pthread_mutex_unlock(&bank.mutex);
		if (command.password != sourceAccount.getPassword())
		{
			oss << "Error " << this->getID() << ": Your transaction failed - password for account id " << command.sourceID << " is incorrect" << endl;
			logWrite(oss.str());
		}
		else
		{
			sourceAccount.addToBalance(command.amount);
			oss << this->getID() << ": Account " << sourceAccount.getID() << " new balance is " 
				<< sourceAccount.getBalance() << " after " << command.amount << " $ was deposited" << endl;
			logWrite(oss.str());
		}
		sourceAccount.io.exitWriter(); logFile << "finished writing to account " << sourceAccount.getID() << " in command: " << command.commandType << endl;
>>>>>>> Stashed changes
	}

	else if (command.commandType == W)
	{
<<<<<<< Updated upstream

=======
		if (sourceAccount.getID() == NO_ID)
		{
			oss << "Error " << this->getID() << ": Your transaction failed - account id " << command.sourceID << " does not exist" << endl;
			logWrite(oss.str());
			pthread_mutex_unlock(&bank.mutex);
			return;
		}
		sourceAccount.io.enterWriter(); logFile << "started writing to account " << sourceAccount.getID() << " in command: " << command.commandType << endl;
		pthread_mutex_unlock(&bank.mutex);
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
			oss << this->getID() << ": Account " << sourceAccount.getID() << " new balance is " 
				<< sourceAccount.getBalance() << " after " << command.amount << " $ was withdrew" << endl;
			logWrite(oss.str());
		}
		sourceAccount.io.exitWriter(); logFile << "finished writing to account " << sourceAccount.getID() << " in command: " << command.commandType << endl;
>>>>>>> Stashed changes
	}

	else if (command.commandType == B)
	{

	}

	else if (command.commandType == Q)
	{

	}

	else if (command.commandType == T)
	{

	}

	{
		case O: //open account

			break;
<<<<<<< Updated upstream
	
		case D: //deposit in account
			
			if (command.password != command.account.password)
			{
				cout << "Error <ATM " << this->getID <<
					">: Your transaction failed – password for account id " <<
					this->getID << " is incorrect" << endl;
				return;
			}

			command.account.balance += command.amount;
			break;

		case W: //withdraw from account
			break;

		case B: //
			break;

		case Q;
			break;

		case T:
			break;
			
	}*/
=======
		std::getline(this->input, line);
		Command command(line);
		//cout << "read line: " << line << endl << "made command " << command.commandType << " " << command.sourceID << " " << command.password << " " << command.amount << endl;
		Account& sourceAccount = bank.getAccountByID(command.sourceID);
		pthread_mutex_lock(&sourceAccount.mutex);
		{
			Account& targetAccount = bank.getAccountByID(command.targetID);
			this->handleAction(command, sourceAccount, targetAccount);
		}
		else
			this->handleAction(command, sourceAccount, sourceAccount);
	}
	pthread_exit(nullptr);
>>>>>>> Stashed changes
}