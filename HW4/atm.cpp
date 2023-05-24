//atm.cpp

#include "atm.h";

using std::cin;
using std::cout;
using std::endl;
using std::ifstream;

//FIXME daniel: implement string parsing here
Command::Command(string line)
{

}

ATM::ATM(ifstream inputFile, int id)
{
	ifstream input(inputFile);
	if (!input.is_open())
	{
		perror("File failed to open");
		return NULL;
	}

	this->input = input;
	this->id = id;

}

ATM::getID() { return this->id };

ATM::handleAction(Command command)
{
	switch (command.commandType)
	{
		case O: //open account
			if (command.account)
			{
				cout << "Error <ATM " << this->getID <<
					">: Your transaction failed - account with same id exists" << endl;
				return;
			}

			Account* account = new Account(command);
			//FIXME gal - add account to bank
			cout << "<ATM " << this->getID << ">: New account id is " << account->id <<
				" with password " << account->password << " and initial balance " <<
				account->balance << endl;
			//FIXME gal - print to log & to screen
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
	}
}