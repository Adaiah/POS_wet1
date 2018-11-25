/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include "commands.h"
#include "signals.h"
#define MAX_LINE_SIZE 80
#define MAXARGS 20
#define MAXJOBS 100 // is this ok??!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define MAXHISTORY 50

using namespace std;

char* L_Fg_Cmd;
vector<job_command>* jobs= new vector<job_command>; //This represents the list of jobs. Please change to a preferred type (e.g array of char*)
queue<string> history;
char lineSize[MAX_LINE_SIZE]; 	

// Init globals 
int paretPid;
string prevDir;

//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
    char cmdString[MAX_LINE_SIZE];


	/************************************/
	struct sigaction new_sigint_action , old_sigint_action;
	struct sigaction new_sigtstp_action, old_sigtstp_action;

	new_sigint_action.sa_handler = ctrlCHandler; //the new handlers function - in signals.c
	new_sigtstp_action.sa_hander = ctrlZHandler;
	sigaction(SIGINT,&new_sigint_action,&old_sigint_action );
	sigaction(SIGTSTP,&new_sigtstp_action,&old_sigtstp_action );

	/************************************/


	bool BGFlag = false;
	L_Fg_Cmd =(char*)malloc(sizeof(char)*(MAX_LINE_SIZE+1));
	if (L_Fg_Cmd == NULL) 
			exit (-1); 
	L_Fg_Cmd[0] = '\0';
	
    	while (1)
    	{
	 	printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);    	
		cmdString[strlen(lineSize)-1]='\0';
					// perform a complicated Command
		if(!ExeComp(lineSize)) continue; 
					// background command	
	 	if(!BgCmd(lineSize, jobs, &BGFlag)) continue;
					// built in commands
		ExeCmd(jobs, lineSize, cmdString, BGFlag);
		
		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
	}
    return 0;
}

