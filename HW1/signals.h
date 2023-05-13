#ifndef _SIGS_H
#define _SIGS_H

#define MAX_LINE_SIZE 80
#define MAX_ARG 20

extern Job* fgJob;
extern sList* Jobs;
extern int wait_line;

void ctrlCHandler(int sigNum);
void ctrlZHandler(int sigNum);

#endif
