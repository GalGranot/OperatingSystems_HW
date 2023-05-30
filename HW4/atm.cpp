//atm.cpp

#include "atm.h"

using std::cin;
using std::cout;
using std::endl;
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
	while (end != -1)
	{
		vector[args_num++] = line.substr(start, end - start);
		start = end + 1;
		end = line.find(delimiter, start);
	}
	//put parses in place
	commandType = stringParses[0][0];
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

//FIXME remove later, this is for debugging only
Command::printCommand()
{
	cout << "printing command\n";
	cout << "command type: " << commandType << "\n";
	cout << "source id: " << sourceID << "\n";
	cout << "password: " << password << "\n";
	cout << "amount " << amount << "\n";
	cout << "target id: " << targetID << "\n";

}


ATM::ATM(ifstream inputFile, int id)
{
	ifstream input(inputFile); //FIXME assumes file is open, check elsewhere
	this->input = input;
	this->id = id;
}

int ATM::getID() { return id; }

void ATM::handleAction(Command command, Bank bank)
{
	if (command.commandType == O) //open command
	{
		if (command.account)
		{
			cout << "Error <ATM " << getID <<
			">: Your transaction failed - account with same id exists" << endl;
			return;
		}

		Account account(command);
		bank.addAccount(account); //FIXME gal - make sure we dont need to dynamically allocate account
		cout << "<ATM " << this->getID() << ">: New account id is " << account.getID() <<
			" with password " << account.password << " and initial balance " <<
			account->balance << endl;
		//FIXME gal - print to log & to screen
	}

	/*
	else if (command.commandType == D)
	{

	}

	else if (command.commandType == W)
	{

	}

	else if (command.commandType == B)
	{

	}

	else if (command.commandType == Q)
	{

	}

	else if (command.commandType == T)
	{

	}

	{
		case O: //open account

			break;
	
		case D: //deposit in account
			
			if (command.password != command.account.password)
			{
				cout << "Error <ATM " << this->getID <<
					">: Your transaction failed – password for account id " <<
					this->getID << " is incorrect" << endl;
				return;
			}

			command.account.balance += command.amount;
			break;

		case W: //withdraw from account
			break;

		case B: //
			break;

		case Q;
			break;

		case T:
			break;
			
	}*/
}