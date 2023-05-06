// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
//public includes
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <iostream>
#include <string>
#include <time.h>
#include <fstream>
#include <signal.h>

//local includes
#include "commands.h"
#include "jobs.h"
#include "signals.h"

//using
using std::cout;
using std::endl;
using std::string;


//FIXME gal - the ctrlc and ctrlz are sent to the smash parent process, how should we catch it to address the current smash child process?
void ctrlCHandler()
{
	cout << "smash: caught ctrl-C" << endl;
	if(//process is running in foreground)
		{
			//FIXME gal - how do i get a process from foreground? it's not saved anywhere
			kill(processID, SIGKILL); //FIXME gal - we need to make sure this goes to the next command immediately, maybe by flags?
			cout << "smash: process " << processID << " was killed" << endl;
		}
	else return;

}

void ctrlZHandler()
{
	cout << "smash: caught ctrl-z" << endl;
	if(//process is running in foreground)
		{
			kill(processID< SIGTSTOP);
			cout << "smash: process " << processID << " was stopped" << endl;
		}
}

// TODO for gal : when stopping job, add the command line to the fiels job->commandName