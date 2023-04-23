#ifndef _JOBS_H
#define _JOBS_H

#define MAX_LINE_SIZE 80
#define MAX_ARG 20

#include <list>
#include <string.h>

class Job
{
public:
	Job(int processID, string commandName, time_t startTime);
private:
	string commandName;
	int jobID;
	int processID;
	bool isStopped = 0;
	time_t startTime;

	void printJob(time_t presentTime);
};
class sList
{
private:
	int nextJobID = 0;
	std::list<Job> jobList;
	//return true on success, false on failure
	bool insertJob(Job* myJob);

public:
	Job* getJobByProcessID(int ID);
	Job* getJobByJobID(int ID);
	bool jobsCompare(const Job* job1, const Job* job2);
	void sortByID();
	void printJobsList(time_t presentTime);
};

#endif