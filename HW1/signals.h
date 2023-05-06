#ifndef _SIGS_H
#define _SIGS_H

#define MAX_LINE_SIZE 80
#define MAX_ARG 20
#define SUCCESS 0
#define FAIL -1

void ctrlCHandler(int sigNum);
void ctrlZHandler(int sigNum);

#endif
