//		commands.c
//********************************************

//defines, global vars, namespaces

#include "jobs.h"
#include "commands.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <iostream>
#include <string>
#include <signal.h>
#include "signals.h"
#include <time.h>
#include <fstream>

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
	if (!file1.is_open() || file2.is_open())
	{
		cout << "Failed to open" << endl;
		return true;; //actually error
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

	char* c_CommandLine; //get c representation of string for c functions
	strcpy(c_CommandLine, CommandLine.data());
	string cmd(strtok(c_CommandLine, delimiters));

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

		//gal code
		if (args[2] != NULL) //got an argument after the directory
			cout << "Too many arguments\n"; //FIXME gal - do we need line drops?
		else if (strcmp(args[1]), "-" == 0) //trying to go to old directory
		{
			if (!prev_PWD)
				cout << "OLDPWD not set";
			else //change oldpwd to current, go to oldpwd
			{
				getcwd(pwd, sizeof(char*));
				chdir(prev_pwd);
				prev_pwd = pwd;
			}
		}
		else //go to new dir
		{
			getcwd(prev_pwd, sizeof(char*));
			chdir(args[1]);
		}
		//endof gal code

		if (args[2] == NULL) cout << "too many arguments";
		else if (strcmp(args[1], "-") == 0)
			if (prev_pwd == NULL) cout << "OLDPWD not set";
			else {
				getcwd(pwd, sizeof(char*));    // first save current dir, than change to prev one
				chdir(prev_pwd);
				prev_pwd = pwd;
			}
		else {
			getcwd(prev_pwd, sizeof(char*));
			chdir(args[1]);
		}
	}
	/*************************************************/
	else if (cmd == "pwd")
	{
		//gal code
		cout << getcwd(pwd, sizeof(char*));
		//endof gal code

		getcwd(pwd, sizeof(char*)); //FIXME gal: why do you need to update pwd here?
		cout << pwd;
	}

	///*************************************************/
	else if (cmd == "jobs")
	{
		//auto it = Jobs->begin(); //FIXME gal: read online using auto isn't safe and we should use the whole declaration instead
		std::list<Job>::iterator it = Jobs->begin();
		time_t presentTime = time(NULL); // use same timing for all jobs
		
		//FIXME gal: we are not printing the jobs in order, need to:
		//1. check which jobs are still running
		//2. kill the ones who aren't
		//3. order the remaining ones
		//4. print them

		//gal code
		while (!it)
		{
			if (!kill(it->processID, 0) == 0) //job is dead and should be deleted
				it = Jobs->erase(it); //this line both deletes current element and returns ptr to next element
			it++;
		} //list now holds only alive jobs
		Jobs->sortByID; 
		Jobs->printJobsList(presentTime);
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
		pid_t Pid = getpid(); //FIXME gal - do we need to set this?
		//gal code
		cout << "smash pid is" << getpid() << endl; //FIXME gal - do we need new lines?
		//endof gal code

		cout << "smash pid is " << Pid << endl;
	}
	/*************************************************/
	else if (cmd == "fg") //implement
	{
	}
	/*************************************************/
	else if (cmd == "bg") //implement
	{
	}
	/*************************************************/
	else if (cmd == "quit") //implement
	{
	}
	/*************************************************/
	else if (cmd == "kill") //implement
	{
	}
	/*************************************************/
	else if (cmd == "diff")
	{
		if (args[3] != NULL) //FIXME gal - args[1] and args[2] are the file names? so args[3] should be null?
			cout << "too many arguments" << endl; //FIXME gal - do we need new lines?
		else
		{
			cout << cmpFiles(args[1], args[2]) << endl; //FIXME gal - new line
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
	int pID;
	int stat
	switch (pID = fork())
	{
	case -1:
		perror(“smash error : fork failed”);
	case 0:
		// Child Process
		setpgrp();
		char* c_cmdString;
		strcpy(c_cmdString, cmdString.data());
		execv(c_cmdString, args);
		exit(1);
	default:
		 // parent 
		waitpid(pID);
		//waitpid(pID, &stat, WUNTRACED); FIXME daniel: should we continue also when process has stopeed, but not finished? 
		// FIXME daniel: should we add status check and send perror if neede? 
		}
}
//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(string CommandLine)
{
	// char ExtCmd[MAX_LINE_SIZE + 2]; // FIXME 
	string args[MAX_ARG];
	char* c_CommandLine;
	strcpy(c_CommandLine, CommandLine.data());
	if ((strstr(c_CommandLine, "|")) || (strstr(c_CommandLine, "<")) || (strstr(c_CommandLine, ">")) || (strstr(c_CommandLine, "*")) || (strstr(c_CommandLine, "?")) || (strstr(c_CommandLine, ">>")) || (strstr(c_CommandLine, "|&")))
	{
		// Add your code here (execute a complicated command)
		/*
		your code
		*/
	}
	return -1;
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
		// Add your code here (execute a in the background)
		int pID;
			switch (pID = fork())
			{
			case -1:
				perror(“smash error : fork failed”);
			case 0:
				// Child Process
				setpgrp();
				time_t startTime = time(NULL);
				int pid = getpid();
				Job* job = new Job(pid, cmdString, startTime)
				ExeCmd(Jobs, CommandLine, cmdString);
				exit(1);
			default:
				// parent 
			}
	}
	return -1;
}