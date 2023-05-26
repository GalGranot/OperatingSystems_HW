//atm.h

#ifndef _ATM_H
#define _ATM_H
#define NO_ID -1

#include <string>
#include <fstream>

#include "bankClass.h"

using std::string;
using std::ifstream;

enum commandTypes{O, D, W, B, Q, T};

struct Command
{
	Command(string line);
	
	commandTypes commandType;
	int sourceID;
	int password;
	int amount;
	int targetID = NO_ID;
};


class ATM
{
private:
	ifstream input;
	int id;

public:
	
	ATM(ifstream inputFile, int id);
	~ATM(); //FIXME gal - close file here

	int getID();
	void handleAction(Command command, Bank bank);
};


#endif