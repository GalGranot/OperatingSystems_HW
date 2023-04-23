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
#include <time.h>

//local includes
#include "jobs.h"


using namespace std;

void Job::Job(int processID, string commandName, time_t startTime)
	: processID(processID), jobID(jobID), commandName(commandName), startTime(startTime) {};

void Job::printJob(time_t presentTime)
{
	time_t timeDiff = difftime(startTime, presentTime);
	if (!isStopped)
		cout << "[" << jobID << "] " << commandName << ": " << processID << " " << timeDiff << endl;
	else
		cout << "[" << jobID << "] " << commandName << ": " << processID << " " << timeDiff << " (stopped)" << endl;
}

bool sList::insertJob(Job* myJob)
{
	std::list<Job>::iterator it = this->begin();
	if (!it)
	{
		myJob->jobID = 1;
		this->push_back(myJob);
		return true;
	}
	while (it)
	{
		it = (Job*)it; //FIXME gal - not sure if cast is necessary, or it's done automatically by std::list
		if (kill(it->processID, 0) == 0) //current process is alive
		{
			nextJobID = it->jobID;
			it++;
		}
		else
			it = this->erase(it); //this line both deletes current element and returns ptr to next element
	}
	myJob->jobID = ++nextJobID;
	this->push_back(myJob);
}

Job* sList::getJobByProcessID(int ID)
{
	std::list<Job>::iterator it = this->begin();
	while (!it)
	{
		it = (Job)*it; //FIXME gal - unsure if this is necessary
		if (ID == it->processID)
			return it;
		else it++;
	}
	return NULL; //didn't find ID
}

Job* sList::getJobByJobID(int ID)
{
	std::list<Job>::iterator it = jobList.begin();
	while (!it)
	{
		it = (Job)*it; //FIXME gal - unsure if this is necessary
		if (ID == it->jobID)
			return it;
		else it++;
	}
	return NULL; //didn't find ID
}

bool sList::jobsCompare(const Job* job1, const Job* job2)
{
	return job1->processID < job2->processID;
}

void sList::sortByID()
{
	this->sort(jobsCompare);
}

void sList::printJobsList(time_t presentTime)
{
	std::list<Job>::iterator it = this->jobList.begin();
	while (!it)
	{
		it->printJob(presentTime);
		it++;
	}
}