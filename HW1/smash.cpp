/* smash.cpp
main file. This file contains the main function of smash
*******************************************************************/

//public includes
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <iostream>
#include <string>
#include <string.h>
#include <signal.h>

//local includes
#include "jobs.h"
#include "commands.h"
#include "signals.h"

//**************************************************************************************
// defines, global vars, usings
//**************************************************************************************
using std::cout;
using std::cin;
using std::endl;
using std::string;

#define MAX_LINE_SIZE 80
#define MAX_ARG 20
#define QUIT 99
#define FG_NO_JOB -1
string L_Fg_Cmd;
char CommandLine[MAX_LINE_SIZE];

sList* Jobs;
Job* fgJob;
int wait_line;
bool continueWaiting;

//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char* argv[])
{
    string cmdString;
    Jobs = new sList();
    fgJob = new Job(-1, " ", time(NULL));
    //fgJob->jobID = FG_NO_JOB;
    wait_line = 0;
    continueWaiting= true;


    signal(SIGINT, ctrlCHandler);
    signal(SIGTSTP, ctrlZHandler);

    L_Fg_Cmd = "";
    while (1)
    {
        //fgJob->jobID = FG_NO_JOB;
        cout << "smash > ";
        wait_line = 1;
        getline(cin, cmdString);
        wait_line = 0;
        strcpy(CommandLine, cmdString.c_str());
        // background command    
        if (!BgCmd(CommandLine, cmdString)) continue;
        // built in commands
        if (ExeCmd(CommandLine, cmdString) == QUIT) 
            return 0;
        /* initialize for next line read*/
        CommandLine[0] = '\0';
        cmdString = "";
    }
    //our additions
    delete Jobs;
    delete fgJob;

    return 0;
}