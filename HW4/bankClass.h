//bank.h

#ifndef _BANK_H
#define _BANK_H

#include <iostream>
#include <set>
#include <cstdlib>

#include "atm.h"

#define MIN_RATE 1
#define MAX_RATE 5

using std::set;


class Account
{
private:
	int id;
	int password;
	int balance;

public:
	Account(Command command); //init account from open command
	int getID();
	void setID(int id);
	int getPassword();
	int getBalance();
	void addToBalance(int amount);
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
	int balance;

public:

	Bank();
	~Bank(); //FIXME gal - kill all accounts here

	//getters & setters
	int getBalance();
	void addToBalance(int amount);
	void addAccount(Account account);
	Account* getAccountByID(int id);

	void printAccounts();
	void commision();
};

#endif 

