//atm.h

#ifndef _ATM_H
#define _ATM_H

#include <string>
#include <fstream>

#include "bank.h"

using std::ifstream;

enum commandTypes{O, D, W, B, Q, T};

struct Command
{
	Command(string line);
	
	commandTypes commandType;
	Account account;
	int password;
	int amount;
	Account targetAccount;
};


class ATM
{
private:
	ifstream input;
	int id;

public:
	
	ATM(ifstream inputFile, int id);
	~Atm();

	int getID();
	void handleAction(Command command);
};


#endif