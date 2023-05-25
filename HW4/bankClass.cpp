//bank.cpp

#include "bank.h"

using std::cin;
using std::cout;
using std::endl;
using std::set;

int Account::getID() { return id; }
void Account::setID(int id) { this->id = id; }
int Account::getPassword() { password; }
int Account::getBalance() { Balance; }
void Account::addToBalance(int amount) { balance += amount; }

//FIXME gal - this is supposed to copy the account, so pass acount by value. check if it actually does
void Bank::addAccount(Account account) { accounts.insert(account); }
int Bank::getBalance() { return balance; }
void Bank::addToBalance(int amount) { balance += amount; }
int Bank::getNextATMID() { return nextATMID++; }

Account* Bank::getAccountByID(int id)
{
	Account tmpAccount;
	tmpAccount.setID(id);
	auto it = accounts.find(tmpAccount);
	if(it == accounts.end())
		return NULL;
	return &(*it);
}

void Bank::printAccounts()
{
	cout << "Current Bank Status" << endl;
	if (accounts.empty())
		return;

	//FIXME gal - this is supposed to print in order of account ids because the set
	//is holding them ordered. make sure
	for (auto it = accounts.begin(); i != >accounts.end(); it++)
	{
		Account* currAccount = &(*it);
		cout << "Account " << currAccount->getID() << " : Balance - " <<
		currAccount->getBalance() << " $, Account Password - " << 
		currAccount->getPassword() << endl;
	}

	cout << "The bank has " << getBalance() << " $" << endl;
}

void Bank::commision()
{
	if(accounts.empty())
		return;

	int rate = ((std::rand() % MAX_RATE) + 1) / 100; //rate is 1%-5%

	for (auto it = accounts.begin(); i != accounts.end(); it++)
	{
		Account* currAccount = &(*it);
		int commision = rate * currAccount->getBalance;
		currAccount->addToBalance(-commision);
		this->addToBalance(commision);

		//FIXME gal - print to log 
	}
}