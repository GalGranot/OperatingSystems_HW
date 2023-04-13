/* smash.cpp
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <iostream>
#include <string>
#include <signal.h>
#include "commands.h"
#include "signals.h"
#include "jobs.h"

using namespace std;

const int MAX_LINE_SIZE = 80;
const int MAXARGS = 20;

string L_Fg_Cmd;


//void* jobs = NULL; //This represents the list of jobs. Please change to a preferred type (e.g array of char*)
// FIXME daniel: we added the jobs already in the main function. 
//               should we add it here instead?

//FIXME daniel: is that an array of chars, or a c string?
char lineSize[MAX_LINE_SIZE];

//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char* argv[])
{
    string cmdString;

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

    //our additions

    sList* Jobs = new sList;

    L_Fg_Cmd = "";
    while (1)
    {
        cout << "smash > ";
        getline(cin, cmdString);
        strcpy(lineSize, cmdString.c_str());
        // perform a complicated Command
        if (!ExeComp(lineSize)) continue;
        // background command    
        if (!BgCmd(lineSize, jobs)) continue;
        // built in commands
        ExeCmd(jobs, lineSize, cmdString);

        /* initialize for next line read*/
        lineSize[0] = '\0';
        cmdString = "";
    }

    //our additions
    delete Jobs;
    return 0;
}
