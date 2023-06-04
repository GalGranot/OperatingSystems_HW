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
		else if (command.amount > sourceAccount.getBalance())
			;//cout << "Error <ATM " << this.getID() << ">: Your transaction failed – account id " << sourceAccount.getID() << " balance is lower than " << command.amount << endl;
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
			//cout << "<ATM " this.getID() << ">: Account " << command.sourceID << " balance is " << sourceAccount.getBalance() << endl;
		}
	}

	
	else if (command.commandType == Q)
	{
		if (command.password != sourceAccount.getPassword())
			cout << "Error <ATM " << this->getID() << ">: Your transcation failed - password for account id "
			<< sourceAccount.getPassword() << "is incorrect" << endl;
		//FIXME kill account
		if (1/*killed account*/)
			;//cout << "<ATM " << this.getID() << ">: Account " << command.sourceID << "> is now closed. Balance was " << /*FIXME getbalance*/ << endl;
	}
	
	else if (command.commandType == T)
	{
		//FIXME what should we do if one account isn't found?
		if (command.password != sourceAccount.getPassword())
			;//cout << "Error <ATM " << this->getID() << ">: Your transcation failed - password for account id "
			//<< sourceAccount.getPassword() << "is incorrect" << endl;
		else if (command.amount > sourceAccount.getBalance())
			;//cout << "Error <ATM " << this.getID() << ">: Your transaction failed – account id " << sourceAccount.getID() << " balance is lower than " << command.amount << endl;
		else
		{
			Account targetAccount = bank.getAccountByID(command.targetID);
			//FIXME - what if not found?
			sourceAccount.addToBalance(-command.amount);
			targetAccount.addToBalance(command.amount);
			//cout << "<ATM " << this.getID() << ">: Transfer " << command.amount << " from account " << sourceAccount.getID() << " to account " targetAccount.getID() << " new account balance is " << sourceAccount.getBalance() << " new target account balance is " << targetAccount.getBalance() << endl;
		}
	}

}