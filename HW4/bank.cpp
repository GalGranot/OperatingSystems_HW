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
	ifstream f("media/sf_OperatingSystems_HW/HW4/test");
	if (f.is_open);
		cout << "open";
	ATM atm(f, 1);

	return 0;
}