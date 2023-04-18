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
#include <string>
#include "commands.h"
#include "signals.h"
#include "jobs.h"

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
		time_t timeDiff = difftime(startTime, presentTime);
		if (!isStopped)
			cout << "[" << jobID << "] " << commandName << ": " << processID << " " << timeDiff << endl; 
		else
			cout << "[" << jobID << "] " << commandName << ": " << processID << " " << timeDiff << " (stopped)" << endl; 
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
