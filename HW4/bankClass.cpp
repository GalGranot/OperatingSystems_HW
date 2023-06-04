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

Command::Command(string line)
{
	//parse strings
	vector<string> stringParses;
	int args_num = 0;
	const char delimiter = ' ';
	int start = 0;
	int end = line.find(delimiter);
	while (end != (int)string::npos) //npos = no position, ie end of string
	{
		stringParses.push_back(line.substr(start, end - start));
		args_num++;
		start = end + 1;
		end = line.find(delimiter, start);
	}
	if (start < (int)line.length()) //check if missed the end
	{
		stringParses.push_back(line.substr(start));
		args_num++;
	}
	//put parses in place
	const char cmdTyp = stringParses[0][0];
	//FIXME gal - remove the enum, does more harm then good
	if (cmdTyp == 'O') commandType = O;
	else if(cmdTyp == 'D') commandType = D;
	else if (cmdTyp == 'W') commandType = W;
	else if (cmdTyp == 'B') commandType = B;
	else if (cmdTyp == 'Q') commandType = Q;
	else if (cmdTyp == 'T') commandType = T;

	sourceID = stoi(stringParses[1]);
	if (args_num > 2)
		password = stoi(stringParses[2]);
	if (args_num > 3) {
		if (commandType == T) {
			targetID = stoi(stringParses[3]);
			amount = stoi(stringParses[4]);
		}
		else
			amount = stoi(stringParses[3]);

		this->printCommand();
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

Account::Account(Command command)
{
	this->setID(command.sourceID);
	this->setPassword(command.password);
	this->balance = 0;

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

int Account::getID() { return id; }
void Account::setID(int id) { this->id = id; }
int Account::getPassword() { return password; }
void Account::setPassword(int password) { this->password = password; }
int Account::getBalance() { return balance; }
void Account::addToBalance(int amount) { balance += amount; }
//Account::Account() {} //FIXME implement this - FIXME do we need this?
Account::Account(int id, int password, int balance)
{
	this->id = id;
	this->password = password;
	this->balance = balance;
}

//FIXME gal - this is supposed to copy the account, so pass acount by value. check if it actually does
void Bank::addAccount(Account account) { accounts[account.getID()] = account; }
int Bank::getBalance() { return balance; }
void Bank::addToBalance(int amount) { balance += amount; }
Bank::Bank() {} //FIXME implement this
Bank::~Bank() {} //FIXME implement this

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
	if (accounts.empty())
		return;
	//FIXME gal - this is supposed to print in order of account ids because the map
	//is holding them ordered. make sure it does
	for(const auto& it : accounts)
	{
		Account currAccount = it.second;
		cout << "Account " << currAccount.getID() << " : Balance - " <<
		currAccount.getBalance() << " $, Account Password - " << 
		currAccount.getPassword() << endl;
	}
	cout << "The bank has " << getBalance() << " $" << endl;
}

void Bank::commision()
{
	if(accounts.empty())
		return;

	int rate = ((std::rand() % MAX_RATE) + 1); //rate is 1%-5%
	for (auto& it : accounts)
	{
		Account& currAccount = it.second;
		int commision = rate * currAccount.getBalance() / 100;
		currAccount.addToBalance(-commision);
		this->addToBalance(commision);

		//FIXME gal - print to log 
	}
}



/* FIXME these methods are implemented above, why duplicate?
int Account::getID() {
	return this->id;
}

int Account::getPassword() {
	return this->password;
}

int Account::getBalance() {
	return this->balance;
}

void Account::setID(int id) {
	this->id = id;
}

void Account::addToBalance(int amount) {
	this->balance += amount;
} */
