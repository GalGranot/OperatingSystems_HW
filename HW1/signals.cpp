
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

#define FG_NO_JOB -1
#define KILL_FAIL -1
//#define SIGSTOP 19 


void ctrlCHandler(int sigNum)
{
	cout << endl << "smash: caught ctrl-c" << endl;
	//if (fgjob->jobid == fg_no_job)
	//{
	//	cout << "smash >";
	//	return;
	//}
	//fgjob->jobid = fg_no_job;
	if (wait_line) {
		cout << "smash >";
		fflush(stdout);
		return;
	}
	else if (kill(fgJob->processID, SIGKILL) == KILL_FAIL)
	{
		perror("smash error: kill failed");
	}
	else
		cout << "smash: process " << fgJob->processID << " was killed" << endl;
	fflush(stdout);
	return;
}

void ctrlZHandler(int sigNum)
{	cout << endl << "smash: caught ctrl-z" << endl;

	if (wait_line)
	{
		cout << "smash >";
		fflush(stdout);
		return;
	}
	

	//stop job
	//fgJob->jobID = FG_NO_JOB;
	else if (kill(fgJob->processID, SIGSTOP) == KILL_FAIL)
		perror("smash error: stop failed");
	else {
		cout << "smash: process " << fgJob->processID << " was stopped" << endl;
		////update job to be inserted into job list

		time(&(fgJob->startTime));
		fgJob->isStopped = true;
		Jobs->insertJob(*fgJob);

		//fgJob->printJob(0);
		//Jobs->getJobByProcessID(fgJob->processID)->printJob(0);
		//setpgid(fgJob->processID, fgJob->processID);
		//raise(SIGSTOP);
	}
	//signal(SIGSTOP, SIG_IGN);
	continueWaiting = false;
	fflush(stdout);
	return;
}