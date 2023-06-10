//bankClass.cpp

#include "bankClass.h"

using std::cin;
using std::cout;
using std::endl;
using std::map;
using std::stoi;
using std::vector;
using std::string;
using std::ifstream;

/*=============================================================================
* Command
=============================================================================*/

Command::Command(string line)
{
	//parse strings
	vector<string> stringParses;
	int argsNum = 0;
	const char delimiter = ' ';
	int start = 0;
	int end = line.find(delimiter);
	while (end != (int)string::npos) //npos = no position, ie end of string
	{
		stringParses.push_back(line.substr(start, end - start));
		argsNum++;
		start = end + 1;
		end = line.find(delimiter, start);
	}
	if (start < (int)line.length()) //check if missed the end
	{
		stringParses.push_back(line.substr(start));
		argsNum++;
	}
	//put parses in place
	commandType = stringParses[0][0];
	sourceID = stoi(stringParses[1]);
	if (argsNum > 2)
		password = stoi(stringParses[2]);
	if (argsNum > 3) {
		if (commandType == 'T') {
			targetID = stoi(stringParses[3]);
			amount = stoi(stringParses[4]);
		}
		else
			amount = stoi(stringParses[3]);
	}

	//if (i == FIELDS_NUM - 1) //received amount
	//	amount = stoi(stringParses[3]);
	//else
	//	amount = NOT_SET;
	//if (i == FIELDS_NUM)
	//	targetID = stringparses[4];
	//else
	//	targetID = NOT_SET;
}

void Command::printCommand()
{
	cout << "printing command\n";
	cout << "command type: " << commandType << "\n";
	cout << "source id: " << sourceID << "\n";
	cout << "password: " << password << "\n";
	cout << "amount " << amount << "\n";
	cout << "target id: " << targetID << "\n";
}

/*=============================================================================
* Account
=============================================================================*/

Account::Account(Command command)
{
	this->setID(command.sourceID);
	this->setPassword(command.password);
	this->balance = command.amount;
	pthread_mutex_init(&this->mutex, nullptr);
}
Account::~Account()
{
	pthread_mutex_destroy(&this->mutex);
}
int Account::getID() { return id; }
void Account::setID(int id) { this->id = id; }
int Account::getPassword() { return this->password; }
void Account::setPassword(int password) { this->password = password; }
int Account::getBalance() { return balance; }
void Account::addToBalance(int amount) { balance += amount; }
Account::Account(int id, int password, int balance)
{
	this->id = id;
	this->password = password;
	this->balance = balance;
}

/*=============================================================================
* Bank
=============================================================================*/
//FIXME gal - this is supposed to copy the account, so pass acount by value. check if it actually does
Bank::Bank() {}

void Bank::addAccount(Account account) { accounts[account.getID()] = account; }
int Bank::getBalance() { return balance; }
void Bank::addToBalance(int amount) { balance += amount; }
/// <summary>
/// this returns a reference to the actual account! everything is modified in place!
/// make sure to not change the id of an account here, this will break the map's ordering
/// 
/// </summary>
/// <param name="id"></param>
/// <returns> an account if found, an account with id=-1=NO_ID if not found </returns>
Account& Bank::getAccountByID(int id)
{
	auto it = accounts.find(id);
	if (it != accounts.end())
		return it->second; //FIXME - maybe we need to use references here? ie auto& it

	//not found
	Account tmp; tmp.setID(NO_ID);
	Account& errorAccount = tmp;
	return errorAccount;
}
void Bank::printAccounts()
{
	printf("\033[2J");
	printf("\033[1;1H");

	cout << "Current Bank Status" << endl;
	if (accounts.empty()) {
		cout << "The bank has " << getBalance() << " $" << endl;
		return;
	}
	//FIXME gal - this is supposed to print in order of account ids because the map
	//is holding them ordered. make sure it does
	for(const auto& it : accounts)
	{
		Account currAccount = it.second;
		pthread_mutex_lock(&currAccount.mutex);
		cout << "Account " << currAccount.getID() << " : Balance - " <<currAccount.getBalance() << " $, Account Password - " << currAccount.getPassword() << endl;
		pthread_mutex_unlock(&currAccount.mutex);
	}
	cout << "The bank has " << getBalance() << " $" << endl;
}
void Bank::commission(Account& currAccount, int rate)
{
	int commision = rate * currAccount.getBalance() / 100;
	currAccount.addToBalance(-commision);
	this->addToBalance(commision);
	writeToLog(0, false, false, defaultCommand, 0, true, rate, currAccount.getID(), commision);
}

/*=============================================================================
* global functions
=============================================================================*/

void writeToLog(int ATMid, bool error, bool minus, Command command,
	int Balance, bool commissions, int percentage, int commisionID,
	int commisionAmount)
{
	if (commissions)
		logFile << "Bank:commissions of " << percentage <<
		" % were charged, the bank gained " << commisionAmount << "$ from account "
		<< commisionID << endl;
	else if (bank.getAccountByID(command.sourceID).getID() == NO_ID)
		logFile << "Error " << ATMid << ": Your transaction failed – account id "
		<< command.sourceID << " does not exists" << endl;
	else
	{
		switch (command.commandType) {
		case 'O':
			if (error)
				logFile << "Error " << ATMid
				<< ": Your transaction failed – account with the same id exists"
				<< endl;
			else
				logFile << ATMid << ": New account id is " << command.sourceID
				<< " with password " << command.password << " and initial balance "
				<< command.amount << endl;
			break;

		case 'D':
			if (error)
				logFile << "Error " << ATMid
				<< ": Your transaction failed – password for account id"
				<< command.sourceID << "is incorrect" << endl;
			else
				logFile << ATMid << ": Account " << command.sourceID << " new balance is "
				<< Balance << " after " << command.amount << " $ was deposited" << endl;
			break;

		case 'W':
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

		case 'B':
			if (error)
				logFile << "Error " << ATMid << ": Your transaction failed – password for account id" << command.sourceID << "is incorrect" << endl;
			else
				logFile << ATMid << ": Account " << command.sourceID << " balance is " << Balance << endl;
			break;

		case 'Q':
			if (error)
				logFile << "Error " << ATMid << ": Your transaction failed – password for account id" << command.sourceID << "is incorrect" << endl;
			else
				logFile << ATMid << ": Account " << command.sourceID << " is now closed. Balance was " << Balance << endl;
			break;

		case 'T':
			if (bank.getAccountByID(command.targetID).getID() == NO_ID)
				logFile << "Error " << ATMid << ": Your transaction failed – account id " << command.targetID << " does not exists" << endl;
			else if (minus)
				logFile << "Error " << ATMid << ": Your transaction failed – account id " << command.sourceID << " balance is lower than" << command.amount << endl;
			else if (error)
				logFile << "Error " << ATMid << ": Your transaction failed – password for account id " << command.sourceID << " is incorrect" << endl;
			else
				logFile << ATMid << ": Transfer " << command.amount << " from account " << command.sourceID << " to account " << command.targetID << "  new account balance is " << Balance << " new target account balance is " << Balance << endl;
			break;     // FIXME change last balance in print

		}
	}
}

// Open the log file for writing
void openLogFile(const string& filename)
{
	logFile.open(filename);
	if (!logFile.is_open())
		std::cerr << "Error: Failed to open log file." << endl;

}