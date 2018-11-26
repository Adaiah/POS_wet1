//		commands.c
//********************************************
#include "commands.h"
#include <linux/limits.h>

using namespace std ;
extern pid_t fgPid;
extern string fg_name;
extern vector<job_command> jobs;
extern queue<string> history;

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
void removeFinishedJobs(){
	//remove all the zombies from jobs list
	for(std::vector<job_command>::iterator it = jobs.begin(); it != jobs.end(); ++it) {
		if(waitpid(it->PID, NULL, WNOHANG) > 0 )
			jobs.erase(it);
	}
	int i = 1;
	for(std::vector<job_command>::iterator it = jobs.begin(); it != jobs.end(); ++it) {
		it->comm_id = i;
	}
}

//********************************************
// function name: getPidFromJobs
// Description: get the pid number from the jobs list
// Parameters: pointer to jobs, the number in the jobs list
// Returns: the pid
//**************************************************************************************
pid_t getPidFromJobs(int id_comm){
	if((id_comm<1) || (id_comm > jobs.size()))
		return -1;
	std::vector<job_command>::iterator it = jobs.begin()+id_comm-1;
	return it->PID;
}

//********************************************
// function name: isJobStopped
// Description: get if the job stopped by Ctrl- Z
// Parameters: pointer to jobs, the number in the jobs list
// Returns: 1 - if stopped, 0-if not
//**************************************************************************************
bool isJobStopped(int id_comm) {
	std::vector<job_command>::iterator it = jobs.begin() + id_comm - 1;
	return it->status;
}

//********************************************
// function name: getNameFromJobs
// Description: get the pid name from the jobs list
// Parameters: pointer to jobs, the number in the jobs list
// Returns: the name of the process
//**************************************************************************************
std::string getNameFromJobs(int id_comm){
	if((id_comm<1) || (id_comm > jobs.size()))
		return NULL;
	std::vector<job_command>::iterator it = jobs.begin() + id_comm - 1;
	return it->name;
}


//********************************************
// function name: getNumOfJobs
// Description: return the number of BG process in the jobs list
// Parameters: pointer to jobs
// Returns: the number of BG process
//**************************************************************************************
int getNumOfJobs(){
	return jobs.size();
}

//********************************************
// function name: removeJob
// Description: !!!!!!!!!!!!!!!!!!!!!!!!!!
// Parameters: pointer to jobs, pid of process to remove
// Returns: N/A
//**************************************************************************************
void removeJob(pid_t pid){
	for(std::vector<job_command>::iterator it = jobs.begin(); it != jobs.end(); ++it) {
		if (it->PID == pid)
			v.erase(it);
	}
	int i = 1;
	for(std::vector<job_command>::iterator it = jobs.begin(); it != jobs.end(); ++it) {
		it->comm_id = i;
	}
}


//********************************************
// function name: findLastStopped
// Description: finds the last command that was stopped
// Parameters: pointer to jobs
// Returns: returns the pid of the command last stopped
//**************************************************************************************
pid_t findLastStopped()
{
	for (vector<job_command>::iterator it = jobs.end();it!=(jobs.begin()--);it--)
	{
		if(*it.status=true) //if curr location is stopped
		{
			return *it.PID;
		}
	}
	return NULL;
}

//********************************************
// function name: getLastBGFromJobs
// Description: returns the last command that runs in the background
// Parameters: pointer to jobs
// Returns: pid of command to run in fg
//**************************************************************************************
pid_t getLastBGFromJobs()
{
	vector<job_command>::iterator it = jobs.end();
	it--;
	return *it.PID;
}

//********************************************
// function name: getCommandID
// Description: returns the number of command in the job vector
// Parameters: jobs vector, curr_pid pid of the command of interest
// Returns: number of command in vector
//**************************************************************************************
int getCommandID(pid_t curr_pid)
{
	for(vector<job_command>::iterator it = jobs.begin(); it != jobs.end(); ++it) {
		if (it->PID == curr_pid)
			return *it.comm_id;
	}
	return 0;
}
//********************************************
// function name: addNewJob
// Description: adding new job to the BG job list
// Parameters: pid of the new job, and if it was stopped
// Returns: void
//**************************************************************************************
void addNewJob(pid_t pid , bool isStopped, string name ){
	pid_t new_pid = 0;
	job_command curr_job;
	if(pid > 0)
		new_pid = pid;
	curr_job.name= name;
	curr_job.comm_id =  jobs.size()+1;
	curr_job.PID = new_pid;
	time(&curr_job.entry_time);
	curr_job.status = isStopped;
	jobs.push_back(curr_job);
}

//********************************************
// function name: addNewJob
// Description: adding new job to the BG job list
// Parameters: pid of the new job, and if it was stopped
// Returns: void
//**************************************************************************************
int sendSignal(int signum, pid_t pid, bool print){
	//TODO: print the signal
	//TODO: check if SIGTSTP wwas sent to a BG command and turn on the status flag.
	//TODO: return (kill function)
}

//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(char* lineSize, char* cmdString ,bool BGFlag)
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
		if (num_arg > 1) //too many arguments
		{
			printf("smash error: > too many arguments \n");
			return 0;
		}
		cout << getcwd();

	}
	
	/*************************************************/
	else if (!strcmp(cmd, "history"))
	{
 		int k=0;
		string temp;
		if (num_arg > 1) //too many arguments
		{
			printf("smash error: > too many arguments \n");
			return 0;
		}
		queue<string> printQueue(history);
		k=history.size();
		for(int i=0; i<k; i++)
		{
			cout<< history.front()<< endl;
			temp=history.front();
			history.pop();
			history.push(temp);
		}

	}
	/*************************************************/
	
	else if (!strcmp(cmd, "jobs")) 
	{
 		removeFinishedJobs();//TODO: add the jobs printing
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
	{	//TODO: add when the id_comm is not valid
		// brings the wanted or the last stopped command to the foreground
		// make the parent process wait to his specific child
		if (num_arg > 1) // if there are too many arguments
			printf("smash error: > too many arguments \n");
		else {
			pid_t waited_pid;
			int waited_status;
			if (num_arg == 1) { // brings specific process to the foreground
				waited_pid = getPidFromJobs(atoi(args[1]));
				if (isJobStopped(atoi(args[1]))) // check if the process was stopped by SIGTSTP
					sendSignal(SIGCONT, waited_pid, DO_PRINT);
			}
			else { //bring the last process in the BG to the FG
				waited_pid = getLastBGFromJobs();
				std::cout << getNameFromJobs(atoi(args[1])) << std::endl;
			}
			fgPid = waited_pid;
			fg_name = cmdString;
			pid_t done = waitpid(waited_pid, &waited_status , WUNTRACED);
			if ((done > 0) && WIFEXITED(waited_status))
				removeJob(jobs, done);
			else if (done < 0)
				perror("smash error: > ");
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
		if(num_arg>1)
		{
			cout<< "wrong number of arguments"<<endl;
			return 0;
		}
		else {
			pid_t resume_pid
			if(num_arg==0)// no parameters resume last stopped job
			{
				resume_pid=findLastStopped()//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				if(resume_pid==NULL)// no stopped jobs
				{
					cout<< "There are no stopped commands" <<endl;
					return 0;
				}
				else
				{
					int cmd_num=getCommandID(resume_pid);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
					vector<job_command>job[cmd_num].status=false; //change from stopped
					sendSignal(SIGCONT, resume_pid, DO_PRINT);
				}
			}
			else if (num_arg==1) // pid of command to run
			{
				resume_pid=getPidFromJobs(atoi(args[1]))
				if(resume_pid < 0) //command not found
				{
					cout<< "command not found" <<endl;
					return 0;
				}
				else
				{
					jobs[atoi(args[1])].status=false; //change from stopped
					sendSignal(SIGCONT, resume_pid, DO_PRINT);
				}
			}
		}

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
				int num_of_jobs = getNumOfJobs();
				for (i = 1; i <= num_of_jobs; i++) {
					time_t start_time, now;
					pid_t waited_pid;
					double elapsed;

					pid_t current_pid = getPidFromJobs(i);
					string job_name = getNameFromJobs(i);

					sendSignal(SIGTERM, current_pid, DONT_PRINT);
					cout << "[" << i << "] " << job_name << " - Sending SIGTERM... ";

					time(&start_time);
					do {   //wait 5 seconds for the process to end
						time(&now);
						elapsed = difftime(now, start_time);
						waited_pid = waitpid(current_pid, NULL, WNOHANG);
					} while ((elapsed < 5) && (waited_pid == 0));

					if (waited_pid < 0) //if the wait got an error
						perror("smash error: > ");
					else if (waited_pid == 0) { //if the process didn't end in 5sec
						cout << "(5 sec passed) Sending SIGKILL... ";
						sendSignal(SIGKILL, current_pid, DONT_PRINT);
						if (waitpid(current_pid, NULL, 0) > 0)
							cout << "Done." << endl;
						else
							perror("smash error: > ");
					} else { //if the process did end
						cout << "Done." << endl;
					}
				}
			}
			exit(0);
   		}
   		else{ //quit without kill
   			exit(0);
   		}
	}
		/*************************************************/
	else if (!strcmp(cmd, "mv"))
	{
		if(num_arg!=1)
		{
			cout<< "wrong number of arguments"<<endl;
			return 0;
		}
		if(rename(args[1],args[2])<0)
		{
			perror("smash error: > ");
		}
		else
			cout<< arg[1] << " has been renamed to " << arg[2]<<endl;

	}
	/*************************************************/
	else if (!strcmp(cmd, "kill"))//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	{  //todo: if kill sends SIGTSTP to BG command needs to flag it as stopped.
		if(num_arg<2)
		{
			cout<< "wrong number of arguments"<<endl;
			return 0;
		}
		else if (atoi(args[1])>0)
		{
			cout<< "bad signum parameter"<<endl;
		}
		else{
			int signum=-1*atoi(args[1]);
			int cmd_id=atoi(args[2]);
			if(cmd_id<0 || cmd_id>getNumOfJobs())
			{
				cout<< "smash error:> kill" << cmd_id << " - job does not exist"<< endl;
			}
			else {
				pid_t sendSig2PID=getPidFromJobs(cmd_id);
				if (!kill(sendSig2PID,signum)) // if error in signal sending
				{
					cout<< "smash error:> kill" << cmd_id << " cannot send signal"<< endl;
				}
			}
		}


	}
	/*************************************************/
	else // external command
	{
 		ExeExternal(args, cmdString, BGFlag);
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
void ExeExternal(char *args[MAX_ARG], char* cmdString, bool BGFlag) {
	int pID;
	switch (pID = fork()) {
		case -1:
			perror("smash error: > ");
		case 0 :
			// Child Process
			setpgrp();
			execve(cmdString, args, NULL);
			perror("smash error: > ");
		default:
			if (BGFlag == 0) {//if it's in FG
				int status_child;
				fgPid = pID;
				pid_t done = waitpid(pID, &status_child,
						WUNTRACED); //WUNTRACED - if the child process finished, killed or stopped via SIGTSTP
				if((done > 0 ) && WIFEXITED(status_child))
					removeJob(jobs,done);
				else if (done <0){
					perror("smash error: > ");
				}

			}
			else{ //if it is indeed in the BG
				//need to update the pid in the jobs list
				std::vector<job_command>::iterator it = jobs.end()-1; //the last in the list
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
		return 0;
	} 
	return -1;
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, bool* BGFlag)
{
	char* Command;
	char* delimiters = " \t\n";
	char *args[MAX_ARG];
	if (lineSize[strlen(lineSize)-2] == '&')
	{
		lineSize[strlen(lineSize)-2] = '\0';
		addNewJob(0,false,strtok(lineSize, delimiters))
		BGFlag=true;
	}
	return -1;
}




