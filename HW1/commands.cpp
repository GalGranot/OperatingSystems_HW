//		commands.c
//********************************************

//defines, global vars, namespaces

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

#define QUIT 99

using std::cout;
using std::endl;
using std::string;

//********************************************
//general use functions
//********************************************

//returns true if different, false if identical
bool cmpFiles(string filename1, string filename2)
{
	std::ifstream file1(filename1);
	std::ifstream file2(filename2);
	if (!file1.is_open() || !file2.is_open())
	{
		 cout << "Failed to open" << endl; //FIXME daniel: why do we have it? no demand for that
		return true; //actually error
	}
	char c1, c2;
	while (file1.get(c1) && file2.get(c2))
	{
		if (c1 != c2)
		{
			file1.close(); file2.close();
			return true;
		}
	}
	//reached eof of one file, check if eof of other file too
	if (file1.eof() && file2.eof())
	{
		file1.close(); file2.close();
		return false;
	}
	else
	{
		file1.close(); file2.close();
		return true;
	}
}

//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to Jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************

int ExeCmd(sList* Jobs, string CommandLine, string cmdString)
{
	char* args[MAX_ARG];
	char* pwd;
	char* prev_pwd; // previous PWD, in use in CD command 
	const char* delimiters = " \t\n";
	int i = 0, num_arg = 0;
	bool illegal_cmd = false; // illegal command

	char* c_CommandLine = new char[MAX_LINE_SIZE + 1]; //get c representation of string for c functions
	strcpy(c_CommandLine, CommandLine.data());
	string cmd(strtok(c_CommandLine, delimiters));
	delete[] c_CommandLine;

	if (cmd.empty())
		return 0;
	strcpy(args[0], cmd.data());

	//copy 
	for (i = 1; i < MAX_ARG; i++) //FIXME - maybe this shouldn't run until maxarg, instead run until reaching null?
	{
		args[i] = strtok(NULL, delimiters);
		if (args[i] == NULL) //FIXME - this should be args[i] != null?
			num_arg++;
	}
	/*************************************************/
	//command line commands
	/*************************************************/
	if (cmd == "cd")
	{
		if (args[2] != NULL) //got an argument after the directory
			cout << "Too many arguments" << endl; 
		else if (strcmp(args[1], "-") == 0) //trying to go to old directory
		{
			if (!prev_pwd)
				cout << "OLDPWD not set";
			else //change oldpwd to current, go to oldpwd
			{
				getcwd(pwd, MAX_LINE_SIZE); //FIXME daniel: can it be longer then MAX_LINE_SIZE?
				chdir(prev_pwd);
				prev_pwd = pwd;
			}
		}
		else //go to new dir
		{
			getcwd(prev_pwd, sizeof(char*));
			chdir(args[1]);
		}


	}
	/*************************************************/
	else if (cmd == "pwd")
	{
		cout << getcwd(pwd, sizeof(char*));

	}

	///*************************************************/
	else if (cmd == "jobs")
	{
		std::list<Job>::iterator it = Jobs->begin();
		time_t presentTime = time(NULL); // use same timing for all jobs
		
		//1. check which jobs are still running
		//2. kill the ones who aren't
		//3. order the remaining ones
		//4. print them

		//gal code
	//	while (!it)
	//	{
		//	if (!kill(it->processID, 0) == 0) //job is dead and should be deleted
		//		it = Jobs->erase(it); //this line both deletes current element and returns ptr to next element
		//	it++;
		//} //list now holds only alive jobs
		//Jobs->printJobsList(presentTime);
		//endof gal code

		while (it != Jobs->end())
		{
			if (kill(it->processID, 0) == 0) // check if job is still running by sending a signal 0
			{
				it->printJob(presentTime);
				it++;
			}
			else Jobs->erase(it); //delete this job from list 
		}
	}
	/*************************************************/
	else if (cmd == "showpid")
	{
		cout << "smash pid is" << getpid() << endl; 
	}
	/*************************************************/
	else if (cmd == "fg") //implement
	{
		int status;
		if (args[2]!= NULL)
			cout << "smash error: fg: invalid arguments"<< endl;
		else if (args[1] == NULL) {
			Job* job = Jobs->jobList.end();
			int pid = job->getProcessID();
			if (job == NULL)
				cout << "smash error: fg: jobs list is empty" << endl;
			else {
				cout << job->commandName << ": " << pid << endl;
				kill(pid, SIGCONT);
				waitpid(pid, &status, 0);
				Jobs->remove_job(pid);
			}
		}
		else {
			Job* job = Jobs->getJobByJobID(args[1]);
			int pid = job->getProcessID();
			int jobid = job->getJobID();
			if (job == NULL)
				cout << "smash error: fg: job-id " << jobid << " does not exist" << endl;
			else {
				cout << job->commandName << ": " << pid << endl;
				kill(pid, SIGCONT);
				waitpid(pid, &status, 0);
				Jobs->remove_job(pid);
			}
		}
	}
	/*************************************************/
	else if (cmd == "bg") //implement
	{
		int job_id = args[1];
		if (job_id) {
			Job* job = Jobs->getJobByJobID(job_id);
			if (args[2] != NULL)
				cout << "smash error : bg: invalid arguments" << endl;
			else if (job == NULL)
				cout << "smash error : bg: job - id " << job_id << "does not exist" << endl;
			else if (job->isStopped == 0)
				cout << "smash error : bg: job - id " << job_id << "is already running in the background" << endl;
			else {
				cout << job->commandName << ": " << job->getProcessID() << endl;
				kill(job->getProcessID, SIGCONT);
				job->isStopped = 0;
			}
		}
		else {
			Job* job = Jobs->biggest_stopped();
			if (job == NULL)
				cout << "smash error : bg:there are no stopped jobs to resume" << endl;
			else {
				cout << job->commandName << ": " << job->getProcessID() << endl;
				kill(job->getProcessID, SIGCONT);
				job->isStopped = 0;
			}
		}
	}
	/*************************************************/
	else if (cmd == "quit") //implement
	{
		int pid = getpid();
		if (!strcmp(args[1], "kill"))  //  kill argument 
			Jobs->kill_list();
		return QUIT;
		//FIXME daniel: should we also delete here all allocated memory ? 

	}
	/*************************************************/
	else if (cmd == "kill") //implement
	{
		if (args[3] != NULL || args[2] == NULL || args[1] == NULL || strcmp(args[1][0], "-") )
			cout << "smash error: kill: invalid arguments" << endl;
		else {
			Job* temp_job = sList::getJobByJobID(std::atoi(args[2]));  //returns null if job not exist
			if (temp_job == NULL)    
				cout << "smash error : kill: job - id " << args[2] << " does not exist" << endl;
			else {
				int signum = std::stoi(args[1] + 1);
				kill(temp_job->processID, signum);
				out << "signal number " << signum << " was sent to pid " << args[2] <<endl;
			}
		}
	}
	/*************************************************/
	else if (cmd == "diff")
	{
		if (args[3] != NULL || args[2] == NULL || args[1] == NULL)
			cout << "smash error: diff: invalid arguments" << endl; 
		else
		{
			cout << cmpFiles(args[1], args[2]) << endl; 
		}
	}
	/*************************************************/
	else // external command
	{
		ExeExternal(args, cmdString);
		return 0;
	}
	if (illegal_cmd == true)
	{
		cout << "smash error: > " << cmdString << endl;
		return 1;
	}
	return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char* args[MAX_ARG], string cmdString)
{
	//int pID;
	//int status;
	//switch (pID = fork())
	//{
	//case -1:
	//	perror(“smash error : fork failed”);
	//case 0:
	//{
	//	// Child Process
	//	setpgrp();
		char* c_cmdString = new char[MAX_LINE_SIZE + 1];
		strcpy(c_cmdString, cmdString.c_str());
		if (execv(c_cmdString, args) < 0) {
			perror("smash error : execv failed");
			exit(1);
		}
		delete[] c_cmdString;
		exit(0);
		//waitpid(pID, &status, 0);
		// FIXME daniel: should we continue also when process has stopeed, but not finished? 
	//}
	//default:
	//{
	//	// parent 
	//}
	//}
}

//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(string CommandLine, sList* Jobs, string cmdString)
{
	string Command;
	string delimiters = " \t\n";
	string args[MAX_ARG];
	if (CommandLine[CommandLine.length() - 2] == '&')
	{
		CommandLine[CommandLine.length() - 2] = '\0';
		int pID;
			switch (pID = fork())
			{
			case -1:
				perror("smash error : fork failed");
			case 0:
				// Child Process
				setpgrp();
				time_t startTime = time(NULL);
				int pid = getpid();
				Job* job = new Job(pid, cmdString, startTime);
				Jobs->insertJob(job);
				ExeCmd(Jobs, CommandLine, cmdString);
				exit(0);
			default:
				// parent 
				break;
			}
			return 0;
	}
	return -1;
}