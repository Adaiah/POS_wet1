#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <string>
#include <errno.h>
#include <vector>
#include <queue>
#define MAX_LINE_SIZE 80
#define MAX_ARG 20
#define DO_PRINT 1
#define DONT_PRINT 0
typedef struct command {
	std::string name;
	int comm_id;
	pid_t PID;
	time_t entry_time;
	bool status; // stopped or not
}job_command, *pjob_command;
void addNewJob(pid_t pid , bool isStopped, std::string name);
int ExeComp(char* lineSize);
int BgCmd(char* lineSize, bool* BGFlag);
int ExeCmd(char* lineSize, char* cmdString, bool BGFlag);
void ExeExternal(char *args[MAX_ARG], char* cmdString, bool BGFlag);
int sendSignal(int signum, pid_t pid, bool print);
#endif
