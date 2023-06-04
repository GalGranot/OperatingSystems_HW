//atm.h

#ifndef _ATM_H
#define _ATM_H
#define NO_ID -1
#define NOT_SET -1
#define FIELDS_NUM 5

#include <string>
#include <fstream>
#include <vector>

#include "bankClass.h"

using std::string;
using std::ifstream;

enum commandTypes{O, D, W, B, Q, T};


typedef struct Command
{
	Command(string line);
	
	commandTypes commandType;
	int sourceID;
	int password;
	int amount = NOT_SET;
	int targetID = NOT_SET;

	void printCommand();

} Command;




class ATM
{
private:
	ifstream input;
	int id;

public:
	
	ATM(string filePath, int id);
	~ATM(); //FIXME gal - close file here

	int getID();
	void handleAction(Command command, Bank bank);
};


#endif