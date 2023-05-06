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

string L_Fg_Cmd;
char CommandLine[MAX_LINE_SIZE];
Job fgJob; //global foreground job


//**************************************************************************************
// main function
//**************************************************************************************
int main(int argc, char* argv[])
{
    string cmdString;
    sList* Jobs = new sList;
    L_Fg_Cmd = "";
    signal(SIGINT, ctrlCHandler);
    signal(SIGTSTP, ctrlZHandler);
    fgJob.jobID = JOB_DOESNT_EXIST;
    
    while (1)
    {
        cout << "smash > ";
        getline(cin, cmdString);
        strcpy(CommandLine, cmdString.c_str());
        // background command    
        if (!BgCmd(CommandLine, Jobs, cmdString)) continue;
        // built in commands
        if (ExeCmd(Jobs, CommandLine, cmdString) == QUIT)
            return 0;
        /* initialize for next line read*/
        CommandLine[0] = '\0';
        cmdString = "";
    }
    //our additions
    delete Jobs;
}