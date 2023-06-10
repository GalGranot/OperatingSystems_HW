//bankClass.h

#ifndef _BANKCLASS_H
#define _BANKCLASS_H

#include <iostream>
#include <fstream>
#include <map>
#include <cstdlib>
#include <vector>
#include <pthread.h>

#define MIN_RATE 1
#define MAX_RATE 5
#define NO_ID -1
#define NOT_SET -1

using std::map;
using std::string;
using std::ofstream;

extern ofstream logFile;

class ioHandler
{
	pthread_mutex_t mutex;
};

class Command
{
public:
	Command(string line);
	char commandType;
	int sourceID = NO_ID;
	int password = NOT_SET;
	int amount = NOT_SET;
	int targetID = NOT_SET;
	void printCommand();
};

class Account
{
private:
	int id;
	int password;
	int balance = 0;
	

public:
	Account(int id = NO_ID, int password = NOT_SET, int balance = 0);
	Account(Command command); //init account from open command
	~Account();
	int getID();
	void setID(int id);
	int getPassword();
	void setPassword(int password);
	int getBalance();
	void addToBalance(int amount);
	pthread_mutex_t mutex; //fixme check if needs to be private
};

class Bank
{
private:
	int balance = 0;

public:
	Bank();
	~Bank();

	map<int, Account> accounts;
	pthread_mutex_t mutex;

	//getters & setters
	int getBalance();
	void addToBalance(int amount);
	void addAccount(Account account);
	Account& getAccountByID(int id);

	void printAccounts();
	void commission(Account& currAccount, int rate);

};

extern Command defaultCommand;

void writeToLog(int ATMid = 0, bool error = 0, bool minus = 0,
	Command command = defaultCommand, int Balance = 0, bool commissions = 0, 
	int percentage = 0, int commisionID = 0, int commisionAmount = 0);

void openLogFile(const string& filename);
extern Bank bank;

#endif 

