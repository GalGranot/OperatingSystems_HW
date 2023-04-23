/*	jobs.c
*******************************************************************/

//public includes
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
#include <algorithm>

//local includes
#include "commands.h"
#include "signals.h"
#include "jobs.h"

using namespace std;

class Job
{
public:
	Job(int processID,  string commandName, time_t startTime)
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

	// delete from list finished jobs, and set next empty biggest JobID for new Job
	int insertJob(Job* myJob)
	{
		auto it = jobList.begin();
		if (!it) {
			myJob->jobID = 1;
			jobList.push_back(myJob);
		}
		else while(!it)
		{
			it = (Job)*it;
			if (kill(it->processID, 0) == 0) { // check if job is still running by sending a signal 0
				nextJobID = it->jobID; 
				advance(it, 1);
			}
			else
				erase(it);
		}
		myJob->jobID = nextJobID + 1;
		jobList.push_back(myJob);
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
	//gal code
	bool jobsCompare(const Job* job1, const Job* job2)
	{
		return job1->processID < job2->processID;
	}
	//endof gal code

	void sortByID()
	{
		this->sort(jobsCompare);
	}

	void printJobsList(time_t presentTime)
	{
		std::list<Job>::iterator it = this->jobList.begin();
		while (!it)
		{
			it->printJob(presentTime);
			it++;
		}
	}
};