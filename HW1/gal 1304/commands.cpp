//		commands.c

//********************************************

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





using namespace std;



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

	char* c_CommandLine;
	strcpy(c_CommandLine, CommandLine.data());

	string cmd(strtok(c_CommandLine, delimiters));

	if (cmd.empty())

		return 0;

	strcpy(args[0], cmd.data());

	for (i = 1; i < MAX_ARG; i++)

	{

		args[i] = strtok(NULL, delimiters);

		if (args[i] == NULL)

			num_arg++;



	}

	/*************************************************/

	// Built in Commands PLEASE NOTE NOT ALL REQUIRED

	// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD

	// MORE IF STATEMENTS AS REQUIRED

	/*************************************************/

	if (cmd == "cd") //implement

	{

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

	else if (cmd == "pwd") //implement

	{

		getcwd(pwd, sizeof(char*));

		cout << pwd;

	}



	/*************************************************/

	//else if (!strcmp(cmd, "mkdir")) //don't implement

	//{

 //		

	//}

	///*************************************************/



	else if (cmd == "jobs") //implement

	{

		auto it = Jobs->begin();

		time_t present_time = time(NULL); // use same timing for all jobs

		while (it != Jobs->end())

		{

			if (kill(it->processID, 0) == 0) // check if job is still running by sending a signal 0

			{

				it->printJob(present_time);

				advance(it, 1);

			}

			else Jobs->erase(it); //delete this job from list 

		}

	}



	/*************************************************/

	else if (cmd == "showpid") //implement

	{

		pid_t Pid = getpid();

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

	else if (cmd == "diff") //implement

	{



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
				//FIXME daniel: need to impliment the following: 
				//Jobs->delete_done();  
				//jobID = Jobs->get_new_id();

				Job* job = new Job(pid, jobID, cmdString, startTime)
				ExeCmd(Jobs, CommandLine, cmdString);
				exit(1);

			default:
				// parent 
			
			}

	}

	return -1;

}