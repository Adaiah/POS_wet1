//		commands.c
//********************************************
#include "commands.h"
#include <linux/limits.h>
#define DO_PRINT 1
#define DONT_PRINT 0



//********************************************
// function name: printJobEntry
// Description: prints a single job command entry
// Parameters: pointer to job_command
// Returns: void
//**************************************************************************************
void printJobEntry(pjob_command job_entry){
	//the function prints a single job command entry
	std::cout<<"["<<job_entry->comm_id<<"} "<<job_entry->name<<" "
			 <<job_entry->PID<<" "<< difftime(time(NULL),job_entry->entry_time)<<" secs ";
	if (job_entry->status)
		std::cout<<"(stopped) "<<std::endl;
	else
	std::cout<<std::endl;
}


//********************************************
// function name: removeFinishedJobs
// Description: remove all the zombies from the jobs list
// Parameters: pointer to jobs
// Returns: void
//**************************************************************************************
void removeFinishedJobs(void* jobs){
	//remove all the zombies from jobs list
	std::vector<job_command>& v = *static_cast<std::vector<job_command>*>(jobs);
	for(std::vector<job_command>::iterator it = v.begin(); it != v.end(); ++it) {
		if(waitpid(it->PID, NULL, WNOHANG) > 0 )
			v.erase(it);
	}
	int i = 1;
	for(std::vector<job_command>::iterator it = v.begin(); it != v.end(); ++it) {
		it->comm_id = i;
	}
}

//********************************************
// function name: getPidFromJobs
// Description: get the pid number from the jobs list
// Parameters: pointer to jobs, the number in the jobs list
// Returns: the pid
//**************************************************************************************
pid_t getPidFromJobs(void* jobs, int id_comm){
	std::vector<job_command>& v = *static_cast<std::vector<job_command>*>(jobs);
	std::vector<job_command>::iterator it = v.begin()+id_comm-1;
	return it->PID;
}

//********************************************
// function name: isJobStopped
// Description: get if the job stopped by Ctrl- Z
// Parameters: pointer to jobs, the number in the jobs list
// Returns: 1 - if stopped, 0-if not
//**************************************************************************************
bool isJobStopped(void* jobs ,int id_comm) {
	std::vector <job_command> &v = *static_cast<std::vector <job_command> *>(jobs);
	std::vector<job_command>::iterator it = v.begin() + id_comm - 1;
	return it->status;
}

//********************************************
// function name: getNameFromJobs
// Description: get the pid name from the jobs list
// Parameters: pointer to jobs, the number in the jobs list
// Returns: the name of the process
//**************************************************************************************
std::string getNameFromJobs(void* jobs, int id_comm){
	std::vector <job_command> &v = *static_cast<std::vector <job_command> *>(jobs);
	std::vector<job_command>::iterator it = v.begin() + id_comm - 1;
	return it->name;
}


//********************************************
// function name: getNumOfJobs
// Description: return the number of BG process in the jobs list
// Parameters: pointer to jobs
// Returns: the number of BG process
//**************************************************************************************
int getNumOfJobs(void* jobs){
	std::vector <job_command> &v = *static_cast<std::vector <job_command> *>(jobs);
	return v.size();
}

//********************************************
// function name: getNumOfJobs
// Description: return the number of BG process in the jobs list
// Parameters: pointer to jobs
// Returns: the number of BG process
//**************************************************************************************
void removeJob(void* jobs, pid_t pid){
	std::vector<job_command>& v = *static_cast<std::vector<job_command>*>(jobs);
	for(std::vector<job_command>::iterator it = v.begin(); it != v.end(); ++it) {
		if (it->PID == pid)
			v.erase(it);
	}
	int i = 1;
	for(std::vector<job_command>::iterator it = v.begin(); it != v.end(); ++it) {
		it->comm_id = i;
	}
}

//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(void* jobs, char* lineSize, char* cmdString ,bool BGFlag)
{
	char* cmd; 
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE];
	char* delimiters = " \t\n";
	int i = 0, num_arg = 0;
	bool illegal_cmd = false; // illegal command
    	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0; 
   	args[0] = cmd;
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters); 
		if (args[i] != NULL) 
			num_arg++; 
 
	}
	static char prevDir[PATH_MAX] = "/0";
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	if (!strcmp(cmd, "cd") ) 
	{

	    // changes the Dir of the current folder
		if(num_arg > 1 ) { //if the args are more than 1 (<path>)
			printf("smash error: > too many arguments \n");
		}
		else if (!strcmp(args[1], "-")){
				if(chdir(prevDir) == -1)
						perror("smash error: > ");
				else{
					std::cout<<prevDir<<std::endl;
					getcwd(prevDir , sizeof(prevDir));
				}
			}
			else{
				if(chdir(args[1]) == -1)
					if (errno == ENOENT)
						printf("smash error: > \"%s\" - path not found\n", args[1]);
					else
						perror("smash error: > ");
				else{
					getcwd(prevDir , sizeof(prevDir));
				}
			}
	} 
	
	/*************************************************/
	else if (!strcmp(cmd, "pwd")) 
	{
		
	}
	
	/*************************************************/
	else if (!strcmp(cmd, "mkdir"))
	{
 		
	}
	/*************************************************/
	
	else if (!strcmp(cmd, "jobs")) 
	{
 		removeFinishedJobs(jobs);
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{
		if (num_arg > 0 )
			printf("smash error: > too many arguments \n");
		else
			std::cout<<"smash pid is "<<getpid()<<std::endl;
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg")) 
	{
		// brings the wanted or the last stopped command to the foreground
		// make the parent process wait to his specific child
		if (num_arg > 1) // if there are too many arguments
			printf("smash error: > too many arguments \n");
		else {
			pid_t waited_pid;
			if (num_arg == 1){ // brings specific process to the foreground
				waited_pid = getPidFromJobs(jobs, atoi(args[1]));
				if(isJobStopped(jobs,atoi(args[1]))) // check if the process was stopped by SIGTSTP
					sendSignal(SIGCONT, waited_pid, DO_PRINT);
			}
			else{ //bring the last process in the BG to the FG
				waited_pid = getLastBGFromJobs(jobs);
				std::cout<<getNameFromJobs(jobs, atoi(args[1]))<<std::endl;
				}
			pid_t done = waitpid(waited_pid, NULL , WUNTRACED);
			if (done > 0)
				removeJob(jobs, done);
			else if (done < 0) //TODO : check about the c-Z/C
				perror("smash error: > ");
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
  		
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
		//quit(exit) the main process +[kill] kill all sub process
   		if (num_arg > 1) //too many arguments
			printf("smash error: > too many arguments \n");
   		else if (num_arg == 1){
   			if (strcmp(args[1], "kill")){
				printf("smash error: > unknown argument in quit command \n");
   			}
   			else { //quit with kill
				int num_of_jobs = getNumOfJobs(jobs);
				for (i = 1; i <= num_of_jobs; i++) {
					time_t start_time, now;
					pid_t waited_pid;
					double elapsed;

					pid_t current_pid = getPidFromJobs(jobs, i);
					std::string job_name = getNameFromJobs(jobs, i);

					sendSignal(SIGTERM, current_pid, DONT_PRINT);
					std::cout<<"["<<i<<"] "<<job_name<<" - Sending SIGTERM... ";

					time(&start_time);
					do {   //wait 5 seconds for the process to end
						time(&now);
						elapsed = difftime(now,start_time);
						waited_pid = waitpid(current_pid ,NULL , WNOHANG);
					}while ((elapsed < 5) && (waited_pid == 0) );

					if (waited_pid < 0 ) //if the wait got an error
						perror("smash error: > ");
					else if (waited_pid == 0){ //if the process didn't end in 5sec
						std::cout<<"(5 sec passed) Sending SIGKILL... ";
						sendSignal(SIGKILL, current_pid, DONT_PRINT);
						if (waitpid(current_pid, NULL, 0) > 0)
							std::cout<<"Done."<<std::endl;
						else
							perror("smash error: > ");
					}
					else{ //if the process did end
						std::cout<<"Done."<<std::endl;
					}
				}
			}
			delete static_cast<std::vector<job_command>*>(jobs);
			exit(0);
   		}
   		else{ //quit without kill
   			delete static_cast<std::vector<job_command>*>(jobs);
   			exit(0);
   		}
	}
	/*************************************************/
	else // external command
	{
 		ExeExternal(args, cmdString, BGFlag, jobs);
	 	return 0;
	}
	if (illegal_cmd == true)
	{
		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString, bool BGFlag, void* jobs) {
	int pID;
	std::vector<job_command>& v = *static_cast<std::vector<job_command>*>(jobs);
	switch (pID = fork()) {
		case -1:
			perror("smash error: > ");
		case 0 :
			// Child Process
			setpgrp();
			execve(cmdString, args, NULL);
			perror("smash error: > ");
		default:
			if (BGFlag == 0) {//if it is not in BG
				waitpid(pID, NULL,
						WUNTRACED); //WUNTRACED - if the child process finished, killed or stopped via SIGTSTP
			}
			else{ //if it is indeed in the BG
				//need to update the pid in the jobs list
				std::vector<job_command>::iterator it = v.end()-1; //the last in the list
				it->PID = pID;
			}
	}
}

//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char* lineSize)
{
	char ExtCmd[MAX_LINE_SIZE+2];
	char *args[MAX_ARG];
    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
    {
		// Add your code here (execute a complicated command)
					
		/* 
		your code
		*/
	} 
	return -1;
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, void* jobs, bool* BGFlag)
{

	char* Command;
	char* delimiters = " \t\n";
	char *args[MAX_ARG];
	if (lineSize[strlen(lineSize)-2] == '&')
	{
		lineSize[strlen(lineSize)-2] = '\0';
		// Add your code here (execute a in the background)
					
		/* 
		your code
		*/
		
	}
	return -1;
}




