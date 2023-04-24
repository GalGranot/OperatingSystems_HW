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

using std::string;
using std::cout;
using std::endl;

Job::Job(int processID, int jobID, string commandName, time_t startTime)
	: jobID(jobID), processID(processID), commandName(commandName), startTime(startTime) {}

int Job::getJobID() {return this->jobID;}
int Job::getProcessID() {return this->processID;}


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
	if (this->jobList.empty())
	{
		myJob->jobID = 1;
		this->jobList.push_back(*myJob);
		return true;
	}
	std::list<Job>::iterator it = this->jobList.begin();
	if(it == this->jobList.end())
		return false;
	while (it != this->jobList.end())
	{
		//it = (Job*)it; //FIXME gal - not sure if cast is necessary, or it's done automatically by std::list
		if (kill(it->getProcessID(), 0) == 0) //current process is alive
		{
			nextJobID = it->getJobID();
			it++;
		}
		else
			it = this->jobList.erase(it); //this line both deletes current element and returns ptr to next element
	}
	myJob->jobID = ++nextJobID;
	this->jobList.push_back(*myJob);
	return true;
}

Job* sList::getJobByProcessID(int ID)
{
	std::list<Job>::iterator it = this->jobList.begin();
	while (it != this->jobList.end())
	{
		//it = (Job)*it; //FIXME gal - unsure if this is necessary
		if (ID == it->getProcessID())
			return &(*it);
		else it++;
	}
	return NULL; //didn't find ID
}

Job* sList::getJobByJobID(int ID)
{
	std::list<Job>::iterator it = jobList.begin();
	while (it != this->jobList.end())
	{
		//it = (Job)*it; //FIXME gal - unsure if this is necessary
		if (ID == it->jobID)
			return &(*it);
		else it++;
	}
	return NULL; //didn't find ID
}

bool sList::jobsCompare(Job* job1, Job* job2)
{
	return job1->getProcessID() < job2->getProcessID();
}

//FIXME gal - this function is problematic for no apparent reason,
// something about passing the function ptr to the sort function,
//will check later
void sList::sortByID()
{
//	this->jobList.sort(&sList::jobsCompare);
}

void sList::printJobsList(time_t presentTime)
{
	std::list<Job>::iterator it = this->jobList.begin();
	while (it != this->jobList.end())
	{
		it->printJob(presentTime);
		it++;
	}
}
