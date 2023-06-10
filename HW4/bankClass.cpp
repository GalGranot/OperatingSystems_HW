//bankClass.cpp

#include "bankClass.h"

using std::cin;
using std::cout;
using std::endl;
using std::map;

int Account::getID() { return id; }
void Account::setID(int id) { this->id = id; }
int Account::getPassword() { return password; }
void Account::setPassword(int password) { this->password = password; }
int Account::getBalance() { return balance; }
void Account::addToBalance(int amount) { balance += amount; }
Account::Account() {} //FIXME implement this
Account::Account(int id, int password, int balance) : id(id), password(password), balance(balance) {} //FIXME implement this
Account::Account(Command command)
{
	this->setID(command.sourceID);
	this->setPassword(command.password);
<<<<<<< Updated upstream
}

=======
	this->balance = command.amount;
	this->readers = 0;
	pthread_mutex_init(&this->m, nullptr);
	pthread_mutex_init(&this->writers, nullptr);
}
Account::~Account()
{
	pthread_mutex_destroy(&this->m);
	pthread_mutex_destroy(&this->writers);
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

Account::enterReader()
{
	pthread_mutex_lock(&m);
	readers++;
	if (readers == 1)
		pthread_mutex_lock(&writers);
	pthread_mutex_unlock(&m);
}

Account::enterWriter() { pthread_mutex_lock(&writers); }

Account::exitReader()
{
	pthread_mutex_lock(&m);
	readers--;
	if (readers == 0)
		pthread_mutex_unlock(&writers);
	pthread_mutex_unlock(&m);
}

Account::exitWriter() { pthread_mutex_unlock(&writers); }

/*=============================================================================
* Bank
=============================================================================*/
>>>>>>> Stashed changes
//FIXME gal - this is supposed to copy the account, so pass acount by value. check if it actually does
void Bank::addAccount(Account account) { accounts[account.getID()] = account; }
int Bank::getBalance() { return balance; }
void Bank::addToBalance(int amount) { balance += amount; }
Bank::Bank() {} //FIXME implement this
Bank::~Bank() {} //FIXME implement this

/// <summary>
/// this returns a reference to the actual account! everything is modified in place!
/// make sure to not change the id of an account here, this will break the set's ordering
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
