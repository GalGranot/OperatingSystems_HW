//bank.cpp

#include <vector>
#include <iostream>
#include <fstream>

#include "bankClass.h"
#include "atm.h"

using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::ofstream;
using std::vector;


//global variables
Bank bank;
ofstream logFile;
//vector<string> commands;
//vector<ATM> ATMs;


/*=============================================================================
* global functions
=============================================================================*/

// Open the log file for writing
void OpenLogFile(const std::string& filename) {
	logFile.open(filename);

	if (!logFile.is_open()) 
		std::cerr << "Error: Failed to open log file." << std::endl;

}

// Write the message to the log file
//FIXME daniel - i removed the command = null default arg because it can't compile with that,
// maybe define a default command with null arguments?
/*
void writeToLogFile(int ATMid = 0, bool error = 0,bool minus = 0, 
	Command command = NULL, int Balance = 0, bool commissions = 0, int percentage = 0) {
	if (commissions)
		logFile << "Bank: " << id << "commissions of " << percentage << 
		" % were charged, the bank gained " << amount << "$ from account "
		<< id << std::endl;
	else if (getAccountByID(command.sourceID) == NO_ID)
		logFile << "Error " << ATMid << ": Your transaction failed – account id " 
		<< command.sourceID <<" does not exists" << std::endl;
	else
	{
		switch (command.commandType) {
		case O: 
			if (error)
				logFile << "Error " << ATMid 
				<< ": Your transaction failed – account with the same id exists" 
				<< std::endl;
			else
				logFile << ATMid << ": New account id is " << command.sourceID 
				<< " with password " << command.password <<" and initial balance " 
				<< command.amount << std::endl;
			break;

		case D:
			if (error)
				logFile << "Error " << ATMid 
				<< ": Your transaction failed – password for account id" 
				<< command.sourceID << "is incorrect" << std::endl;
			else
				logFile << ATMid << ": Account " << command.sourceID << " new balance is " 
				<< Balance << " after " << command.amount << " $ was deposited" << std::endl;
			break;

		case W:
			if (minus)
				logFile << "Error " << ATMid << 
				": Your transaction failed – account id " << command.sourceID 
				<< " balance is lower than" << command.amount << std::endl;
			else if (error)
				logFile << "Error " << ATMid 
				<< ": Your transaction failed – password for account id " 
				<< command.sourceID << " is incorrect" << std::endl;
			else
				logFile << ATMid << ": Account " << command.sourceID << " new balance is " 
				<< Balance << " after " << command.amount << " $ was withdrew" << std::endl;
			break;

		case B:
			if (error)
				logFile << "Error " << ATMid << ": Your transaction failed – password for account id" << command.sourceID << "is incorrect" << std::endl;
			else
				logFile << ATMid << ": Account " << command.sourceID << " balance is " << Balance << std::endl;
			break;

		case Q:
			if (error)
				logFile << "Error " << ATMid << ": Your transaction failed – password for account id" << command.sourceID << "is incorrect" << std::endl;
			else
				logFile << ATMid << ": Account " << command.sourceID << " is now closed. Balance was " << Balance << std::endl;
			break;

		case T:
			if (getAccountByID(command.targetID) == NO_ID)
				logFile << "Error " << ATMid << ": Your transaction failed – account id " << command.targetID << " does not exists" << std::endl;
			else if (minus)
				logFile << "Error " << ATMid << ": Your transaction failed – account id " << command.sourceID << " balance is lower than" << command.amount << std::endl;
			else if (error)
				logFile << "Error " << ATMid << ": Your transaction failed – password for account id " << command.sourceID << " is incorrect" << std::endl;
			else
				logFile << ATMid << ": Transfer " << command.amount << " from account " << command.sourceID << " to account " << command.targetID << "  new account balance is " << Balance <<" new target account balance is "<< Balance << std::endl;
			break;     // FIXME change last balance in print

		}
	}
} */


/*=============================================================================
* main function
=============================================================================*/


int main(int argc, char* argv[])
{
	/*OpenLogFile("log.txt");
	ifstream f("media/sf_OperatingSystems_HW/HW4/test");
	if (f.is_open);
	cout << "open";
	ATM atm(f, 1);

	logFile.close();
	*/


	return 0;
}