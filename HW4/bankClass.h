//bankClass.h

#ifndef _BANKCLASS_H
#define _BANKCLASS_H

#include <iostream>
#include <map>
#include <cstdlib>

#include "atm.h"

#define MIN_RATE 1
#define MAX_RATE 5
#define NO_ID -1

using std::map;

class Account
{
private:
	int id;
	int password;
	int balance = 0;
<<<<<<< Updated upstream
=======
	pthread_mutex_t m;
	pthread_mutex_t writers;
	int readers = 0;
	
>>>>>>> Stashed changes

public:
	Account();
	Account(int id, int password, int balance);
	Account(Command command); //init account from open command
	int getID();
	void setID(int id);
	void setPassword(int password);
	int getPassword();
	int getBalance();
	void addToBalance(int amount);
<<<<<<< Updated upstream
=======
	void enterReader();
	void enterWriter();
	void exitReader();
	void exitWriter();
>>>>>>> Stashed changes
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

