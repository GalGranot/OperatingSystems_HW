//bank.h

#ifndef _BANK_H
#define _BANK_H

#include <iostream>
#include <set>

#include "atm.h"

using std::set;


struct Account
{
	Account(Command command); //init account from open command

	int id;
	int password;
	int balance;
};

struct AccountComparator
{
	bool operator()(Account account1, Account account2)
		return account1.id < account2.id;
};

class Bank
{
private:
	set<Account, AccountComparator> accounts;
	int nextID;

public:

	Bank();
	~Bank(); //FIXME gal - kill all accounts here

	void addAccount(Account account);
	Account* getAccountByID(int id);
	void commision();
};

#endif 

