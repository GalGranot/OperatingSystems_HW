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
Command defaultCommand("O -1 -1 -1");
//vector<string> commands;
vector<ATM> ATMs;


/*=============================================================================
* global functions
=============================================================================*/

// Open the log file for writing
void OpenLogFile(const std::string& filename) {
	logFile.open(filename);
	if (!logFile.is_open()) 
		std::cerr << "Error: Failed to open log file." << endl;

}

// Write the message to the log file
//FIXME daniel - i removed the command = null default arg because it can't compile with that,
// maybe define a default command with null arguments?

void writeToLogFile(int ATMid = 0, bool error = 0,bool minus = 0, 
	Command command = defaultCommand, int Balance = 0, bool commissions = 0, int percentage = 0, int commisionID = 0, int commisionAmount = 0)
{
	if (commissions)
		logFile << "Bank:commissions of " << percentage << 
		" % were charged, the bank gained " << commisionAmount << "$ from account "
		<< commisionID << endl;
	else if (bank.getAccountByID(command.sourceID).getID() == NO_ID)
		logFile << "Error " << ATMid << ": Your transaction failed – account id " 
		<< command.sourceID <<" does not exists" << endl;
	else
	{
		switch (command.commandType) {
		case O: 
			if (error)
				logFile << "Error " << ATMid 
				<< ": Your transaction failed – account with the same id exists" 
				<< endl;
			else
				logFile << ATMid << ": New account id is " << command.sourceID 
				<< " with password " << command.password <<" and initial balance " 
				<< command.amount << endl;
			break;

		case D:
			if (error)
				logFile << "Error " << ATMid 
				<< ": Your transaction failed – password for account id" 
				<< command.sourceID << "is incorrect" << endl;
			else
				logFile << ATMid << ": Account " << command.sourceID << " new balance is " 
				<< Balance << " after " << command.amount << " $ was deposited" << endl;
			break;

		case W:
			if (minus)
				logFile << "Error " << ATMid << 
				": Your transaction failed – account id " << command.sourceID 
				<< " balance is lower than" << command.amount << endl;
			else if (error)
				logFile << "Error " << ATMid 
				<< ": Your transaction failed – password for account id " 
				<< command.sourceID << " is incorrect" << endl;
			else
				logFile << ATMid << ": Account " << command.sourceID << " new balance is " 
				<< Balance << " after " << command.amount << " $ was withdrew" << endl;
			break;

		case B:
			if (error)
				logFile << "Error " << ATMid << ": Your transaction failed – password for account id" << command.sourceID << "is incorrect" << endl;
			else
				logFile << ATMid << ": Account " << command.sourceID << " balance is " << Balance << endl;
			break;

		case Q:
			if (error)
				logFile << "Error " << ATMid << ": Your transaction failed – password for account id" << command.sourceID << "is incorrect" << endl;
			else
				logFile << ATMid << ": Account " << command.sourceID << " is now closed. Balance was " << Balance << endl;
			break;

		case T:
			if (bank.getAccountByID(command.targetID).getID() == NO_ID)
				logFile << "Error " << ATMid << ": Your transaction failed – account id " << command.targetID << " does not exists" << endl;
			else if (minus)
				logFile << "Error " << ATMid << ": Your transaction failed – account id " << command.sourceID << " balance is lower than" << command.amount << endl;
			else if (error)
				logFile << "Error " << ATMid << ": Your transaction failed – password for account id " << command.sourceID << " is incorrect" << endl;
			else
				logFile << ATMid << ": Transfer " << command.amount << " from account " << command.sourceID << " to account " << command.targetID << "  new account balance is " << Balance <<" new target account balance is "<< Balance << endl;
			break;     // FIXME change last balance in print

		}
	}
}


/*=============================================================================
* main function
=============================================================================*/


int main(int argc, char* argv[])
{
	OpenLogFile("log.txt");
	ifstream f("media/sf_OperatingSystems_HW/HW4/test");
	if (f.is_open);
	cout << "open";
	ATM atm(f, 1);

	int* threadIDs = new int[argc + 2];
	pthread_t threads = new pthread_t[argc + 2];
	for (int i = 0; i < argc; i++)
	{
		threadIDs[i] = i + 1;
		ATM atm(argv[i + 1], i + 1);
		ATMs.push_back(atm);
		int result = pthread_create(&threads[i], nullptr, ATMfun, &threadIDs[i]);
		if (result != 0)
		{
			perror("failed to create thread %d\n", i);
			delete[] threadIDs;
			delete[] threads;
			//FIXME close files not closed yet
			return 1;
		}
	}

	int result = pthread_create(&threads[argc + 1], nullptr, bank.commision, &threadIDs[argc + 1]); //FIXME get wrapper function for commision
	//FIXME check result
	result = pthread_create(&threads[argc + 2], nullptr, bank.printAccounts, &threadIDs[argc + 2]); //FIXME get wrapper function for printaccounts
	//FIXME check result

	//pthread join for last two ones once the other threads end



	delete[] threadIDs;
	delete[] threads;
	for (auto& it : ATMs) //FIXME check if this works
		~it();
	logFile.close();

	
	return 0;
}
