//atm.cpp

#include "atm.h"

using std::cin;
using std::cout;
using std::endl;
using std::ifstream;

//FIXME daniel: implement string parsing here
Command::Command(string line)
{

}

ATM::ATM(ifstream inputFile, int id)
{/*
	ifstream input(inputFile); //FIXME assumes file is open, check elsewhere
	this->input = input;
	this->id = id;
	*/
}

int ATM::getID() { return id; }

void ATM::handleAction(Command command, Bank bank)
{/*
	if (command.commandType == O) //open command
	{
		if (command.account)
		{
			cout << "Error <ATM " << getID <<
			">: Your transaction failed - account with same id exists" << endl;
			return;
		}

		Account account(command);
		bank.addAccount(account); //FIXME gal - make sure we dont need to dynamically allocate account
		cout << "<ATM " << this->getID() << ">: New account id is " << account.getID() <<
			" with password " << account.password << " and initial balance " <<
			account->balance << endl;
		//FIXME gal - print to log & to screen
	}

	else if (command.commandType == D)
	{

	}

	else if (command.commandType == W)
	{

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