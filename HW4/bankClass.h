//bankClass.h

#ifndef _BANKCLASS_H
#define _BANKCLASS_H

#include <iostream>
#include <map>
#include <cstdlib>

#define MIN_RATE 1
#define MAX_RATE 5
#define NO_ID -1

using std::map;

class Command;

class Account
{
private:
	int id;
	int password;
	int balance = 0;

public:
	Account();
	Account(int id, int password, int balance);
	Account(Command command); //init account from open command
	int getID();
	void setID(int id);
	int getPassword();
	int getBalance();
	void addToBalance(int amount);
};

class Bank
{
private:
	map<int, Account> accounts;
	int balance = 0;

public:

	Bank();
	~Bank(); //FIXME gal - kill all accounts here

	//getters & setters
	int getBalance();
	void addToBalance(int amount);
	void addAccount(Account account);
	Account& getAccountByID(int id);

	void printAccounts();
	void commision();
};

#endif 

