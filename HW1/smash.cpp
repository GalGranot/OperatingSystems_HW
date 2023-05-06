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


//void* jobs = NULL; //This represents the list of jobs. Please change to a preferred type (e.g array of char*)
// FIXME daniel: we added the jobs already in the main function. 
//               should we add it here instead?
//              gal - i think it's not necessary, i'll try and delete this after compilation
//FIXME daniel: is that an array of chars, or a c string? gal - this is a array of chars instead of a ptr to an array, dont see why it's necessary here

//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char* argv[])
{
    string cmdString;
    sList* Jobs = new sList;

    //signal declaretions
    //NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
    /* add your code here */
    /************************************/
    //NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
    //set your signal handlers here
    /* add your code here */
    /************************************/
    /************************************/
    // Init globals 

    L_Fg_Cmd = "";
    //signal(SIGINT, ctrlCHandler);
    //signal(SIGTSTP, ctrlZHandler);
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