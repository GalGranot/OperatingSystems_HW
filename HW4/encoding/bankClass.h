//bankClass.h

#ifndef _BANKCLASS_H
#define _BANKCLASS_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <cstdlib>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <locale>


#define MIN_RATE 1
#define MAX_RATE 5
#define NO_ID "-1"
#define NOT_SET "-1"
#define NO_AMOUNT -1
#define FIELDS_NUM 5
#define SECOND 1 //FIXME return to 1000000


using std::map;
using std::string;
using std::ofstream;

extern ofstream logFile;

class ioHandler
{
private:
	pthread_mutex_t readerLock;
	pthread_mutex_t writerLock;
	int readers;
public:
	void init();
	void killHandler();
	void enterReader();
	void enterWriter();
	void exitReader();
	void exitWriter();
};

class Command
{
public:
	Command(string line);
	char commandType;
	string sourceID = NO_ID;
	string password = NO_ID;
	int amount = NO_AMOUNT;
	string targetID = NOT_SET;
	void printCommand();
};

class Account
{
private:
	string id;
	string password;
	int balance = 0;

public:
	Account(string id = NO_ID, string password = NO_ID, int balance = 0);
	Account(Command command);
	~Account();
	string getID();
	void setID(string id);
	string getPassword();
	void setPassword(string password);
	int getBalance();
	void addToBalance(int amount);

	ioHandler io;
};

class Bank
{
private:
	int balance = 0;

public:
	Bank();
	~Bank();

	map<string, Account> accounts;

	//getters & setters
	int getBalance();
	void addToBalance(int amount);
	void addAccount(Account account);
	Account& getAccountByID(string id);

	void printAccounts();
	void commission(Account& currAccount, int rate);

	pthread_mutex_t mutex;
};

extern Command defaultCommand;

void writeToLog(int ATMid = 0, bool error = 0, bool minus = 0,
	Command command = defaultCommand, int Balance = 0, bool commissions = 0, 
	int percentage = 0, string commisionID = 0, int commisionAmount = 0);

void openLogFile(const string& filename);
extern Bank bank;
extern Account defaultAccount;

#endif 

