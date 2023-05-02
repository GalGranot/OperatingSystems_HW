#ifndef _JOBS_H
#define _JOBS_H

#define MAX_LINE_SIZE 80
#define MAX_ARG 20

#include <list>
#include <string>

using std::string;
string var;

class Job
{
public:
	Job(int processID, string commandName, time_t startTime);
	int getJobID();
	int getProcessID();
	int jobID;
	void printJob(time_t presentTime);
	bool isStopped = 0;
private:
	int jobID;
	string commandName;
	time_t startTime;

};
class sList
{
private:
	std::list<Job> jobList;
	//return true on success, false on failure
	bool insertJob(Job* myJob);

public:
	Job* biggest_stopped();
	Job* getJobByProcessID(int ID);
	Job* getJobByJobID(int ID);
	bool jobsCompare(Job* job1, Job* job2);
	//void sortByID();
	void printJobsList(time_t presentTime);
	void kill_list();
};

#endif
