//bank.cpp

#include "bank.h"

Bank::addAccount(Account* account) { this->accounts.insert(account); }

Account* Bank::getAccountByID(int id)
{
	Account tmpAccount;
	tmpAccount.id = id;
	auto it = this->accounts.find(tmpAccount);
	if(it == this->accounts.end()
		return NULL;
	return &(*it);
}