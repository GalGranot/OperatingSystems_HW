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
using std::vector;


//global variables
Bank bank;
std::ofstream logFile;
//vector<string> commands;
//vector<ATM> ATMs;

int main(int argc, char* argv[])
{
	OpenLogFile("log.txt");

	ifstream f("media/sf_OperatingSystems_HW/HW4/test");
	if (f.is_open);
		cout << "open";
	ATM atm(f, 1);

	logFile.close();

	return 0;
}

// Open the log file for writing
void OpenLogFile(const std::string& filename) {
	logFile.open(filename);

	if (!logFile.is_open()) 
		std::cerr << "Error: Failed to open log file." << std::endl;

}

// Write the message to the log file
void writeToLogFile(const std::string& message = NULL, int id, bool error = 0, bool commissions = 0, int presentage = 0, int money = 0) {
	if (commissions)
		logFile << "Bank: " << id << "commissions of " << presentage << " % were charged, the bank gained "<< money <<"$ from account " << id << std::endl;
	else if (error)
		logFile << "Error " <<id << ": "<< message << std::endl;
	else
		logFile << id << ": " << message << std::endl;
}


