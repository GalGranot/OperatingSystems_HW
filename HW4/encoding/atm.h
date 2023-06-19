//atm.h

#ifndef _ATM_H
#define _ATM_H
#define NO_ID "-1"

#include <string>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <pthread.h>
#include <sstream>

#include "bankClass.h"

using std::string;
using std::vector;
using std::ifstream;

class ATM
{
private:
	int id;

public:
	
	ifstream input; //FIXME move this to private, this is for testing - implement a getter method
	ATM(string path, int id);
	~ATM(); //FIXME gal - close file here

	int getID();
	void handleAction(Command command);
	void operateATM();
};

extern Bank bank;
extern pthread_mutex_t logLock;

#endif