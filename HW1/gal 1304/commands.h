#ifndef _COMMANDS_H
#define _COMMANDS_H

#include "jobs.h"
#include <unistd.h> 
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <string>


using namespace std;

#define MAX_LINE_SIZE 80
#define MAX_ARG 20

class sList;

//gal: put this as comment (probably because it's a default c file)
//typedef enum { FALSE , TRUE } bool;
int ExeComp(string CommandLine);
int BgCmd(string CommandLine, sList* Jobs, string cmdString);
int ExeCmd(sList *Jobs, string CommandLine, string cmdString);
void ExeExternal(char* args[MAX_ARG], string cmdString);
#endif

