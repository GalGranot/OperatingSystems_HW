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
void ctrlCHandler(int sigNum)
{
	cout << "smash: caught ctrl-C" << endl;
	if (fgJob.jobID == JOB_DOESNT_EXIST)
		cout << "smash > ";
	else //trying to kill existing job
	{
		int flagKill = kill(fgJob.processID, SIGKILL) == SUCCESS;
		if (flagKill == SUCCESS)
		{
			fgJob.jobID = JOB_DOESNT_EXIST;
			cout << "smash: process " << processID << " was killed" << endl;
		}
		else if (flagKill = FAIL)
		{
			perror("smash error: kill failed") //FIXME gal - not sure we need this
		}
	}

}

void ctrlZHandler(int sigNum)
{
	cout << "smash: caught ctrl-z" << endl;
	if (fgJob.jobID == JOB_DOESNT_EXIST)
		cout << "smash > ";
	else //trying to stop existing job
	{
		int flagKill = kill(fgJob.processID, SIGSTOP);
		if (flagKill == SUCCESS)
		{
			fgJob.jobID = JOB_DOESNT_EXIST;
			cout << "smash: process " << processID << " was stopped" << endl;
		}
		else if (flagKill == FAIL)
		{
			perror("smash error: kill failed") //FIXME gal - not sure we need this
		}
	}
}

// TODO for gal : when stopping job, add the command line to the fiels job->commandName
//FIXME gal - not sure what you meant here