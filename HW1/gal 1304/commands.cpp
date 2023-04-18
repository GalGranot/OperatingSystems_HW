//		commands.c
//********************************************
#include "commands.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <iostream>
#include <string>
#include <signal.h>
#include "signals.h"
#include "jobs.h"

using namespace std;

//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(sList* jobs, string CommandLine, string cmdString)
{
	string cmd;
	string args[MAX_ARG];
	string pwd;
	string prev_pwd ; // previous PWD, in use in CD command 
	string delimiters = " \t\n";
	int i = 0, num_arg = 0;
	bool illegal_cmd = false; // illegal command
	cmd = strtok(CommandLine, delimiters);
	if (cmd == NULL)
		return 0;
	args[0] = cmd;
	for (i = 1; i < MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters);
		if (args[i] != NULL)
			num_arg++;

	}
	/*************************************************/
	// Built in Commands PLEASE NOTE NOT ALL REQUIRED
	// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
	// MORE IF STATEMENTS AS REQUIRED
	/*************************************************/
	if (!strcmp(cmd, "cd")) //implement
	{
		if (rgs[2] != NULL) cout << "too many arguments";
		else if (args[1] == "-")
			if (prev_pwd == NULL) cout << "OLDPWD not set";
			else {
				pwd = getcwd();    // first save current dir, than change to prev one
				chdir(prev_pwd)
				prev_pwd = pwd
			}
		else {
			prev_pwd = getcwd();
			chdir(arg[1]) 
		}
	}

	/*************************************************/
	else if (!strcmp(cmd, "pwd")) //implement
	{
		pwd = getcwd();
		cout << pwd;
	}

	/*************************************************/
	//else if (!strcmp(cmd, "mkdir")) //don't implement
	//{
 //		
	//}
	///*************************************************/

	else if (!strcmp(cmd, "jobs")) //implement
	{
		auto it = jobs.begin();
		time_t present_time = time(); // use same timing for all jobs
		while (!it)
		{
			if (kill(it->processID, 0) == 0) // check if job is still running by sending a signal 0
			{
				it->printJob(present_time);
				advance(it, 1);
			}
			else jobs.erase(it); //delete this job from list 
		}
	}

	/*************************************************/
	else if (!strcmp(cmd, "showpid")) //implement
	{
		pid_t Pid = getpid();
		cout << "smash pid is " << Pid << endl;
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg")) //implement
	{

	}
	/*************************************************/
	else if (!strcmp(cmd, "bg")) //implement
	{

	}
	/*************************************************/
	else if (!strcmp(cmd, "quit")) //implement
	{

	}
	/*************************************************/
	else if (!strcmp(cmd, "kill")) //implement
	{

	}
	/*************************************************/
	else if (!strcmp(cmd, "diff")) //implement
	{

	}
	/*************************************************/


	else // external command
	{
		ExeExternal(args, cmdString);
		return 0;
	}
	if (illegal_cmd == TRUE)
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
void ExeExternal(string args[MAX_ARG], string cmdString)
{
	int pID;
	switch (pID = fork())
	{
	case -1:
		// Add your code here (error)

		/*
		your code
		*/
	case 0:
		// Child Process
		setpgrp();

		// Add your code here (execute an external command)

		/*
		your code
		*/

	default:
		// Add your code here
		
		/*
		your code
		*/
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
	char* c_CommandLine = CommandLine.data();

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
int BgCmd(string CommandLine, sList* jobs)
{

	string Command;
	string delimiters = " \t\n";
	string args[MAX_ARG];
	if (CommandLine[strlen(CommandLine) - 2] == '&')
	{
		CommandLine[strlen(CommandLine) - 2] = '\0';
		// Add your code here (execute a in the background)

		//int is_dad = fork();
		//if (is_dad == 0) {  // execute sun
		//	if (built_in command) //  exc command
		//	{ }
		//	else {
		//		exec()  // no wait!!
		//	}
		//}
		//else		//int is_dad = fork();
		//if (is_dad == 0) {  // execute sun
		//	if (built_in command) //  exc command
		//	{ }
		//	else {
		//		exec()  // no wait!!
		//	}
		//}
		//else
	}
	return -1;
}
