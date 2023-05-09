
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


void ctrlCHandler(int sigNum)
{
	cout << endl << "smash: caught ctrl-c" << endl;

	if (fgJob.jobID == FG_NO_JOB)
	{
		//cout << "smash >";
		return;
	}
	fgJob.jobID = FG_NO_JOB;

	if (kill(fgJob.processID, SIGKILL) == KILL_FAIL)
		perror("smash error: kill failed");
	else
		cout << "smash: process " << fgJob.processID << " was killed" << endl;
	cout << "here";
	fflush(stdout);
	return;
}

void ctrlZHandler(int sigNum)
{
	cout << endl << "smash: caught ctrl-z" << endl;
	if (fgJob.jobID == FG_NO_JOB)
	{
		cout << "smash: ";
		return;
	}

	//update job to be inserted into job list
	fgJob.isStopped = true;
	time_t currentTime; time(&currentTime);
	fgJob.startTime = currentTime;
	Jobs->jobList.push_back(fgJob);

	//kill job
	fgJob.jobID = FG_NO_JOB;
	if (kill(fgJob.processID, SIGKILL) == KILL_FAIL)
		perror("smash error: kill failed");
	else
		cout << "smash: process " << fgJob.processID << " was killed" << endl;
	fflush(stdout);
	return;
}