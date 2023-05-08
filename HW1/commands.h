#ifndef _COMMANDS_H
#define _COMMANDS_H

//global includes
#include <string>

//local includes
#include "jobs.h"

using std::string;
extern sList* Jobs;
extern Job fgJob;

#define MAX_LINE_SIZE 80
#define MAX_ARG 20

bool cmpFiles(string filename1, string filename2);
//int ExeComp(string CommandLine);
int BgCmd(string CommandLine,  string cmdString);
int ExeCmd( string CommandLine, string cmdString);
void ExeExternal(char* args[MAX_ARG], string cmdString);
void delete_args(char* args[MAX_ARG]);

#endif

