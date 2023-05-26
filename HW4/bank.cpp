//bank.cpp

#include <vector>

#include "bankClass.h"
#include "atm.h"

using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::vector;

//global variables
Bank bank;
//vector<string> commands;
//vector<ATM> ATMs;

int main(int argc, char* argv[])
{
	/* Tasks for main:
	* init bank
	* init 1 atm (for now, later more)
	* parse commands from log
	* execute commands
	* delete everything when log ends
	*/

	

	/***************************************************
	* code for later - FIXME remove comments
	***************************************************
	//init bank and ATMs
	for (int i = 0; i < argc; i++)
	{
		ifstream currFile(argv[i]);
		ATMS[i] = ATM(currFile, i + 1); //init ATMs with 1-based indexing
	}
	*/

	/***************************************************
	* endof code for later - FIXME remove comments
	***************************************************/


	/***************************************************
	* use this for testing the bank/atm adt - FIXME remove all of this later
	***************************************************/
	
	//ifstream in(test.txt); //FIXME make sure the file is read correctly
	//ATM atm = ATM(in, path);
	//printf("init ATM num %d\n", atm.getID());
	//in.close();

	Account acc(123, 1995, 1000);
	cout << "accounts details: id " << acc.getID() << ", pass " << acc.getPassword() << ", balance " << acc.getBalance() << endl;
	cout << "Account balance: " << bank.getBalance() << endl;
	
	bank.addAccount(acc);
	bank.printAccounts();

	Account& tmp = bank.getAccountByID(123);
	tmp.addToBalance(6);
	bank.printAccounts();
	bank.commision();
	bank.printAccounts();


	/***************************************************cat 
	* endof use this for testing the bank/atm adt - FIXME remove all of this later
	***************************************************/
	
	/***************************************************
	* code for later - FIXME remove comments
	***************************************************/

	/*
	//close program
	for (int i = 0; i < argc; i++)
	{
		ATMs[i].inputFile.close();
	}
	*/
	/***************************************************
	* endof code for later - FIXME remove comments
	***************************************************/
	return 0;
}