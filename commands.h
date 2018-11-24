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
#define MAX_LINE_SIZE 80
#define MAX_ARG 20
typedef struct command {
	std::string name;
	int comm_id;
	pid_t PID;
	time_t entry_time;
	bool status; // stopped or not
	bool built_in; // Yes=true Nם=false
}job_command, *pjob_command;

int ExeComp(char* lineSize);
int BgCmd(char* lineSize, void* jobs, bool* BGFlag);
int ExeCmd(void* jobs, char* lineSize, char* cmdString, bool BGFlag);
void ExeExternal(char *args[MAX_ARG], char* cmdString, bool BGFlag);
#endif
