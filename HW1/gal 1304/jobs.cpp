/*	jobs.c
*******************************************************************/

#include <iostream>
#include <list>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <iostream>
#include "commands.h"
#include "signals.h"
#include <string>

using namespace std;

class Job
{
public:
	Job(int processID, int jobID, string commandName, time_t startTime)
	: processID(processID), jobID(jobID), commandName(commandName), startTime(startTime){}

private:
	string commandName;
	int jobID;
	int processID;
	bool isStopped = 0;
	time_t startTime;

	void printJob(time_t presentTime)
	{
		time_t timeDiff = difftime(startTime, presentTime)
		cout << "[%s] %s : %d %d", jobID, commandName, processID, timeDiff; //check s-format for time_t
		if (isStopped)
			cout << "(stopped)";
		cout << "\n";
	}
};

class sList
{
private:
	int nextJobID = 0;
	list<Job> jobList;

	int insertJob(Job* myJob)
	{
		auto it = jobList.begin();
		if (!it)
			jobList.push_back(myJob);
		else while(!it)
		{
			it = (Job)*it;
			if (myJob->jobID > it->jobID || it->jobID == (Job)*jobList.end->jobID)
			{
				advance(it, 1);
				continue;
			}
			else
			{
				jobList.insert(it, myJob);
				nextJobID++;
			}
		}
	}

	Job* getJobByProcessID(int ID)
	{
		auto it = jobList.begin();
		while (!it)
		{
			it = (Job)*it;
			if (ID == it->processID)
				return it;
			else advance(it, 1);
		}
		return NULL; //didn't find ID
	}

	Job* getJobByJobID(int ID)
	{
		auto it = jobList.begin();
		while (!it)
		{
			it = (Job)*it;
			if (ID == it->jobID)
				return it;
			else advance(it, 1);
		}
		return NULL; //didn't find ID
	}
};
