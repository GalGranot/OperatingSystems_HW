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

Job::Job(int processID, string commandName, time_t startTime)
	:  processID(processID), commandName(commandName), startTime(startTime) {}

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
	int nextJobID = 0;

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
//void sList::sortByID()
//{
//	this->jobList.sort(&sList::jobsCompare);
//}

void sList::printJobsList(time_t presentTime)
{
	std::list<Job>::iterator it = this->jobList.begin();
	while (it != this->jobList.end())
	{
		it->printJob(presentTime);
		it++;
	}
}

void sList::kill_list()
{
	int timeout = 5;
	int pid;
	std::list<Job>::iterator it = this->jobList.begin();
	while (it != this->jobList.end())
	{
		pid = it->getProcessID();
		kill(pid, SIGTERM);
		sleep(timeout);
		if (kill(pid, 0) == 0) //current process is alive
		{
			kill(pid, SIGKILL);
			std::cout << it->commandName << "– Sending SIGTERM… (5 sec passed) Sending SIGKILL… Done." << endl;
		}
		else 
			std::cout << it->commandName << "– Sending SIGTERM... Done." << endl;
		it++;
	}
}

void sList::remove_job(int pid)
{
	std::list<Job>::iterator it = this->jobList.begin();
	while (it != this->jobList.end()) {
		if (it->getProcessID() == pid) {
			it = this->jobList.erase(it);
			return;
		}
	}
}

Job* sList::biggest_stopped()
{while (it != this->jobList.end())
	int biggest_jobid = 0;
	Job* job = this->jobList.begin();
	std::list<Job>::iterator it = this->jobList.begin();
	while (it != this->jobList.end()) {
		if (it->isStopped) {
			if (it->jobID > biggest_jobid) {
				biggest_jobid = it->jobID;
				job = it;
			}
		}
	}
	return job;
}