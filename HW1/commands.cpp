//		commands.c
//****************

//defines, global vars, namespaces

//public includes
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <iostream>
#include <string>
#include <string.h>
#include <time.h>
#include <fstream>
#include <signal.h>


//local includes
#include "commands.h"
#include "jobs.h"
#include "signals.h"

#define QUIT 99
#define RUN 0
//#define SIGSTOP 19 
#define FG_NO_JOB -1
#define SUCCESS 0
#define FAIL -1


using std::cout;
using std::endl;
using std::string;
bool isBG = false;


char prev_pwd[MAX_LINE_SIZE] ; // previous PWD, in use in CD command 


//****************
//general use functions
//****************

//returns true if different, false if identical
bool cmpFiles(string filename1, string filename2)
{
	std::ifstream file1(filename1);
	std::ifstream file2(filename2);
	if (!file1.is_open() || !file2.is_open())
	{
		//cout << "Failed to open" << endl;
		return true; //actually error
	}
	char c1, c2;
	int c1Num = 0; int c2Num = 0;
	while (file1.get(c1) && file2.get(c2))
	{
		c1Num++; c2Num++;
		if (file1.eof() || file2.eof())
			break;
		if (c1 != c2)
		{
			file1.close(); file2.close();
			return true;
		}
	}

	//reached eof of one file, check if eof of other file too
	if (c1Num == c2Num)
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
void delete_args(char* args[MAX_ARG]) {
	for (int i = 0; i < MAX_ARG; i++)
		delete[] args[i];
}

int copyJob(Job* source, Job* dest)
{
	if (source == NULL || dest == NULL)
		return FAIL;

	dest->commandName = source->commandName;
	dest->jobID = source->jobID;
	dest->processID = source->processID;
	dest->startTime = source->startTime;

	return SUCCESS;
}

//****************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to Jobs, command string
// Returns: 0 - success,1 - failure
//******************************

int ExeCmd(string CommandLine, string cmdString)
{
	//var declaration
	char pwd[MAX_LINE_SIZE] ;
	const char* delimiters = " \t\n";
	int num_arg = 0;
	bool illegal_cmd = false; // illegal command

	//argument parsing
	//cout << "commandline is " << CommandLine << endl << "cmdstring is " << cmdString << endl; //rmv
	char* args[MAX_ARG] = { NULL };
	char* cmdStringCopy = new char[MAX_LINE_SIZE + 1];
	strcpy(cmdStringCopy, cmdString.c_str());
	char* tok = strtok(cmdStringCopy, delimiters);
	string cmd(tok);
	if (cmd.empty())
		return 0;
	
	args[0] = new char[cmd.size() + 1];
	strcpy(args[0], cmd.c_str());
	int i = 1;
	tok = strtok(NULL, delimiters);
	char ampersand = '&';
	while (tok != NULL)
	{
		if (num_arg > MAX_ARG)
			cout << "Too many arguments" << endl; //FIXME
		args[i] = new char[strlen(tok) + 1];
		if(*args[i] == ampersand)
			isBG = true;
		strcpy(args[i], tok);
		i++;
		num_arg++;
		tok = strtok(NULL, delimiters);
	}
	delete[] cmdStringCopy;

	fgJob->commandName = args[0];
	fgJob->processID = getpid();


	/*************************************************/
	//command line commands
	/*************************************************/
	if (cmd == "cd")
	{
		if (args[2] != NULL) //got an argument after the directory
			cout << "smash error: cd:" << "Too many arguments" << endl;
		else if (strcmp(args[1], "-") == 0) //trying to go to old directory
		{
			if (!(*prev_pwd))
				cout << "smash error: cd:" << "OLDPWD not set" << endl;
			else //change oldpwd to current, go to oldpwd
			{
				getcwd(pwd, MAX_LINE_SIZE); //FIXME daniel: can it be longer then MAX_LINE_SIZE?
				chdir(prev_pwd);
				strcpy(prev_pwd, pwd);
			}
		}
		else //go to new dir
		{
			getcwd(prev_pwd, MAX_LINE_SIZE);
			chdir(args[1]);
			//cout << prev_pwd << endl;
		}
	}
	/*************************************************/
	else if (cmd == "pwd")
	{
		cout << getcwd(pwd, MAX_LINE_SIZE) <<endl;

	}

	///*************************************************/
	else if (cmd == "jobs")
	{
		int status;
		std::list<Job>::iterator it = Jobs->jobList.begin();
		time_t presentTime = time(NULL); // use same timing for all jobs
		
		//1. check which jobs are still running
		//2. kill the ones who aren't
		//3. order the remaining ones
		//4. print them

		while (it != Jobs->jobList.end())
		{
			int wait_stat = waitpid(it->processID, &status, WNOHANG);
			if (wait_stat == RUN) // check if job is still running by sending a signal 0
			{
				it->printJob(presentTime);
				it++;
			}
			else if (wait_stat == -1)
				perror("smash error: waitpid failed");
			else {
				it = Jobs->jobList.erase(it); //delete this job from list 
			}
		}
	}
	/*************************************************/
	else if (cmd == "showpid")
	{
		cout << "smash pid is " << getpid() << endl; 
	}
	/*************************************************/
	else if (cmd == "fg") //implement
	{
		int status;
		if (args[2]!= NULL)
			cout << "smash error: fg: invalid arguments"<< endl;
		else if (args[1] == NULL) { // got no argument for 'fg'
			if (Jobs->jobList.empty())
				cout << "smash error: fg: jobs list is empty" << endl;
			else
			{
				std::list<Job>::iterator it = std::prev(Jobs->jobList.end());
				Job* job = &(*it);
				int pid = job->getProcessID();
				cout << job->commandName << ": " << pid << endl;
				if (kill(pid, SIGCONT) == -1)
					perror("smash error: kill failed");
				if (waitpid(pid, &status, 0) == -1)
					perror("smash error: waitpid failed");

				if (copyJob(job, fgJob) == FAIL) //this copies to fgJob
					perror("smash error: fg move failed");
				Jobs->remove_job(pid);
			}
		}
		else
		{
			Job* job = Jobs->getJobByJobID(std::stoi(args[1]));
			if (job == NULL)
				cout << "smash error: fg: job-id " << args[1] << " does not exist" << endl;
			else
			{
				if (copyJob(job, fgJob) == FAIL) //this copies to fgJob
					perror("smash error: fg move failed");

				int pid = job->getProcessID();
				//int jobid = job->getJobID();
				cout << job->commandName << ": " << pid << endl;
				if (kill(pid, SIGCONT) == -1)
					perror("smash error: kill failed");
				if (waitpid(pid, &status, 0) == -1)
					perror("smash error: waitpid failed"); 
				Jobs->remove_job(pid);
			}
		}
	}
	/*************************************************/
	else if (cmd == "bg") //implement
	{
		
		if (args[1] != NULL) {
			int job_id = std::stoi(args[1]);
			Job* job = Jobs->getJobByJobID(job_id);
			if (args[2] != NULL)
				cout << "smash error: bg: invalid arguments" << endl;
			else if (job == NULL)
				cout << "smash error: bg: job - id " << job_id << " does not exist" << endl;
			else if (job->isStopped == 0)
				cout << "smash error: bg: job - id " << job_id << " is already running in the background" << endl;
			else {
				cout << job->commandName << ": " << job->getProcessID() << endl;
				if (kill(job->getProcessID(), SIGCONT) == -1)
					perror("smash error: kill failed");
				job->isStopped = 0;
			}
		}
		else {
			Job* job = Jobs->biggest_stopped();
			if (job == NULL)
				cout << "smash error: bg: there are no stopped jobs to resume" << endl;
			else {
				cout << job->commandName << ": " << job->getProcessID() << endl;
				if (kill(job->getProcessID(), SIGCONT) == -1)
					perror("smash error: kill failed");
				job->isStopped = 0;
			}
		}
	}
	/*************************************************/
	else if (cmd == "quit") //implement
	{
		if (args[1] != NULL) {
			if (!strcmp(args[1], "kill"))  //  kill argument 
				Jobs->kill_list();
		}
		delete_args(args);
		return QUIT;
		//FIXME daniel: should we also delete here all allocated memory ? 

	}
	/*************************************************/
	else if (cmd == "kill") //implement
	{
		if (args[3] != NULL || args[2] == NULL || args[1] == NULL || (args[1][0] != '-' ) )
			cout << "smash error: kill: invalid arguments" << endl;
		else {
			Job* temp_job = Jobs->getJobByJobID(std::atoi(args[2]));  //returns null if job not exist
			if (temp_job == NULL)    
				cout << "smash error: kill: job - id " << args[2] << " does not exist" << endl;
			else {
				int signum = std::stoi(args[1] + 1);
				if (kill(Jobs->getJobByJobID(std::atoi(args[2]))->processID, signum) == -1)
					perror("smash error: kill failed");
				cout << "signal number " << signum << " was sent to pid " << args[2] <<endl;
				if (signum == SIGTSTP)
					Jobs->getJobByJobID(std::atoi(args[2]))->isStopped = 1;
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
		delete_args(args);
		return 0;
	}
	if (illegal_cmd == true)
	{
		cout << "smash error:" << cmdString << endl;
		delete_args(args);
		return 1;
	}
	delete_args(args);
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
	int pID;
	int status;
	continueWaiting = true;
	//cout << "external " << endl;
	switch (pID = fork())
	{
	case -1:
		perror("smash error: fork failed");
	case 0:
	{
		// Child Process
		setpgrp();
		//char* c_cmdString = new char[MAX_LINE_SIZE + 1];
		//strcpy(c_cmdString, cmdString.c_str());
		//delete[] c_cmdString;
		if (execv(args[0], args) < 0) {
			perror("smash error: execv failed");
			exit(1);
		}
		cout << " exit extern" << endl;
		exit(0);
		// FIXME daniel: should we continue also when process has stopeed, but not finished? 
	}
	default:
		// Parent 
		fgJob->processID = pID;

		// Wait for the child process to finish or receive a SIGTSTP signal
		while (continueWaiting && waitpid(pID, &status, WNOHANG) != -1)
		{
			if (WIFSTOPPED(status))
			{
				// Child process has been stopped by SIGTSTP
				continueWaiting = true;
				break;
			}
		}

		if (continueWaiting)
		{
			if (waitpid(pID, &status, 0) == -1)
				perror("smash error: waitpid failed");
		}
		break;
	}
}


//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(string CommandLine,  string cmdString)
{
	string Command;
	string delimiters = " \t\n";
	string args[MAX_ARG];
	if (cmdString[cmdString.length() - 1] == '&')
	{
		cmdString[cmdString.length() - 1] = '\0';
		int pID;
			switch (pID = fork())
			{
			case -1:
				perror("smash error: fork failed");
			case 0:
				// Child Process
			{
				setpgrp();
				ExeCmd(CommandLine, cmdString);
				exit(0);
			}
			default :
				// parent
				time_t startTime = time(NULL);
				Job job(pID, CommandLine, startTime);
				Jobs->insertJob(job);

				break;
			}
			return 0;
	}
	return 1;
}