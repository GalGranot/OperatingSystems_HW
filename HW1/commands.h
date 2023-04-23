#ifndef _COMMANDS_H
#define _COMMANDS_H

#include <string.h>
#include "jobs.h"


#define MAX_LINE_SIZE 80
#define MAX_ARG 20

bool cmpFiles(string filename1, string filename2);
int ExeComp(string CommandLine);
int BgCmd(string CommandLine, sList* Jobs, string cmdString);
int ExeCmd(sList *Jobs, string CommandLine, string cmdString);
void ExeExternal(char* args[MAX_ARG], string cmdString);

#endif

