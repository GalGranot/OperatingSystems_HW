//atm.cpp

#include "atm.h"

using std::cin;
using std::cout;
using std::endl;
using std::stoi;
using std::vector;
using std::string;
using std::ifstream;

ATM::ATM(string path, int id)
{
	this->input.open(path);
	//FIXME check if doesn't open
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
	Account sourceAccount = bank.getAccountByID(command.sourceID);

	if (command.commandType == O) //open command
	{
		if(sourceAccount.getID() != NO_ID)
		{
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
	}

	
	else if (command.commandType == D)
	{
		//FIXME - ask what we should do if account isn't found?
		if (sourceAccount.getID() == NO_ID) //FIXME - ask what we should do if account isn't found?
			;

		else if (command.password != sourceAccount.getPassword())
			cout << "Error <ATM " << this->getID() << ">: Your transcation failed - password for account id " 
			<< sourceAccount.getPassword() << "is incorrect" << endl;
		else
		{
			sourceAccount.addToBalance(command.amount); //FIXME MAKE SURE THIS IS DONE IN PLACE!
			cout << "<ATM " << this->getID() << "> Account " << sourceAccount.getID() 
			<< " new balanace is " << sourceAccount.getBalance() << " after " 
			<< command.amount << " $ was deposited";
		}
	}

	else if (command.commandType == W)
	{
		if (sourceAccount.getID() == NO_ID) //FIXME - ask what we should do if account isn't found?
		{

		}

		else if (command.password != sourceAccount.getPassword())
			cout << "Error <ATM " << this->getID() << ">: Your transcation failed - password for account id "
			<< sourceAccount.getPassword() << "is incorrect" << endl;
		else
		{
			sourceAccount.addToBalance(-command.amount); //FIXME MAKE SURE THIS IS DONE IN PLACE!
			cout << "<ATM " << this->getID() << "> Account " << sourceAccount.getID()
				<< " new balanace is " << sourceAccount.getBalance() << " after "
				<< command.amount << " $ was withdrew" << endl;
		}
	}

	
	else if (command.commandType == B)
	{
		if (command.password != sourceAccount.getPassword())
			cout << "Error <ATM " << this->getID() << ">: Your transcation failed - password for account id "
		<< sourceAccount.getPassword() << "is incorrect" << endl;
		else
		{
			cout << "<ATM " this.getID() << ">: Account " << command.sourceID << " balance is " << sourceAccount.getBalance() << endl;
		}
	}

	
	else if (command.commandType == Q)
	{

	}
	/*
	else if (command.commandType == T)
	{

	}

	{
		case O: //open account

			break;
	
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
}