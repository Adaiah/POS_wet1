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
void printJobEntry(job_command job_entry){
	//the function prints a single job command entry
	cout<<"["<<job_entry.comm_id<<"] "<<job_entry.name<<" "
			 <<job_entry.PID<<" "<< difftime(time(NULL),job_entry.entry_time)<<" secs ";
	if (job_entry.status)
		cout<<"(stopped) "<<endl;
	else
	cout<<endl;
}


//********************************************
// function name: removeFinishedJobs
// Description: remove all the zombies from the jobs list
// Parameters: pointer to jobs
// Returns: void
//**************************************************************************************
void removeFinishedJobs(){
	//remove all the zombies from jobs list
	if(jobs.size() > 1){
		for(std::vector<job_command>::iterator it = jobs.begin()+1; it != jobs.end(); ++it) {
			if(waitpid(it->PID, NULL, WNOHANG) > 0 ) {
				if(it == (jobs.end()-1)) {
					if(!jobs.empty())
						jobs.pop_back();
				}
				else {
					jobs.erase(it);
				}
				break;
			}
		}
		int i = 1;
		if(jobs.size() > 1){
			for(std::vector<job_command>::iterator it = jobs.begin()+1; it != jobs.end(); ++it) {
				it->comm_id = i;
				i++;
			}
		}
	}

}

//********************************************
// function name: getPidFromJobs
// Description: get the pid number from the jobs list
// Parameters: pointer to jobs, the number in the jobs list
// Returns: the pid or -1 if job id is not found
//**************************************************************************************
pid_t getPidFromJobs(unsigned int id_comm){
	if((id_comm < 1) || (id_comm >= jobs.size()))
		return -1;
	std::vector<job_command>::iterator it = jobs.begin()+id_comm;
	return it->PID;
}

//********************************************
// function name: isJobStopped
// Description: get if the job stopped by Ctrl- Z
// Parameters: pointer to jobs, the number in the jobs list
// Returns: 1 - if stopped, 0-if not
//**************************************************************************************
bool isJobStopped(unsigned int id_comm) {
	if((id_comm < 1) || (id_comm >= jobs.size()))
		return NULL;
	std::vector<job_command>::iterator it = jobs.begin() + id_comm;
	return it->status;
}

//********************************************
// function name: getNameFromJobs
// Description: get the pid name from the jobs list
// Parameters: pointer to jobs, the number in the jobs list
// Returns: the name of the process
//**************************************************************************************
string getNameFromJobs(unsigned int id_comm){

	if((id_comm < 1) || (id_comm >= jobs.size()))
		return NULL;
	std::vector<job_command>::iterator it = jobs.begin() + id_comm;
	return it->name;
}


//********************************************
// function name: getNumOfJobs
// Description: return the number of BG process in the jobs list
// Parameters: pointer to jobs
// Returns: the number of BG process
//**************************************************************************************
int getNumOfJobs(){
	return jobs.size()-1;
}

//********************************************
// function name: removeJob
// Description: remove a job from the jobs list
// Parameters: pointer to jobs, pid of process to remove
// Returns: N/A
//**************************************************************************************
void removeJob(pid_t pid){
	if(jobs.size() > 1){
		for(std::vector<job_command>::iterator it = jobs.begin()+1; it != jobs.end(); ++it) {
			if (it->PID == pid) {
				if(it == (jobs.end()-1))
					jobs.pop_back();
				else
					jobs.erase(it);
			break;
			}
		}
		int i = 1;
		for(std::vector<job_command>::iterator it = jobs.begin()+1; it != jobs.end(); ++it) {
			it->comm_id = i;
			i++;
		}
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
	if(jobs.size()>1){
		for (vector<job_command>::iterator it = jobs.end();it != jobs.begin();it--)
		{
			if(it->status == true) //if curr location is stopped
			{
				return it->PID;
			}
		}
	}
	return -1;
}

//********************************************
// function name: getLastBGFromJobs
// Description: returns the last command that runs in the background
// Parameters: pointer to jobs
// Returns: pid of command to run in fg
//**************************************************************************************
pid_t getLastBGFromJobs()
{
	vector<job_command>::iterator it = jobs.end() -1;
	return it->PID;
}

//********************************************
// function name: getCommandID
// Description: returns the number of command in the job vector
// Parameters: jobs vector, curr_pid pid of the command of interest
// Returns: number of command in vector
//**************************************************************************************
int getCommandID(pid_t curr_pid)
{
	for(vector<job_command>::iterator it = jobs.begin()+1; it != jobs.end(); ++it) {
		if (it->PID == curr_pid)
			return it->comm_id;
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
	curr_job.comm_id =  jobs.size();
	curr_job.PID = new_pid;
	time(&curr_job.entry_time);
	curr_job.status = isStopped;
	jobs.push_back(curr_job);
}

//********************************************
// function name: sendSignal
// Description: sends a signal to a pid was confirmed to exist in a previous function
// Parameters: signum- the number of signal to send, pid of the target process,
// 				print - indicates if printing the sent signal is needed
// Returns: the value of the kill function to indicate success- 0 or failure - (-1)
//**************************************************************************************
int sendSignal(int signum, pid_t pid, bool print){//strsignal(int sig) is a posix function not sure this will run but giving it a go
	if(print)
	{
		cout<< "smash > signal " << strsignal(signum) << " was sent to pid " << pid <<endl;;
	}
	if(signum==SIGTSTP) {
	    int i = getCommandID(pid);
		if(jobs[i].status == false)
		{
			jobs[i].status = true; //change BG command status to stopped
		}
		else return 0; // don't send signal if job is stopped already
	}
	return kill(pid, signum);
}


//********************************************
// function name: printJobs
// Description: print the jobs list
// Parameters: none
// Returns: void
//**************************************************************************************
void printJobs(){
    if(!jobs.empty()){
		for(vector<job_command>::iterator it = jobs.begin()+1; it != jobs.end(); ++it) {
			printJobEntry(*it);
		}
	}
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
	static char prevDir[PATH_MAX] = ".";
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	if (!strcmp(cmd, "cd") ) //changes the current directory
		{
		char currDir[PATH_MAX] = "/0";
		getcwd(currDir, sizeof(currDir));
	    // changes the Dir of the current folder
		if((num_arg > 1) || (num_arg == 0)) { // if the args are more than 1 (<path>) or none
			printf("smash error: > not enough or too many arguments for \"cd\" command \n");
		}
		else if (!strcmp(args[1], "-")){
			if(chdir(prevDir) == -1)
				perror("smash error: > ");
			else{
				std::cout<<prevDir<<std::endl;
				strcpy(prevDir,currDir);
				}
			}
			else{
				if(chdir(args[1]) == -1)
					if (errno == ENOENT)
						printf("smash error: > \"%s\" - path not found\n", args[1]);
					else
						perror("smash error: > ");
				else{
					strcpy(prevDir,currDir);
				}
			}
	} 
	
	/*************************************************/
	else if (!strcmp(cmd, "pwd")) //returns the path of the current directory
	{

		if (num_arg >= 1) //too many arguments
		{
			printf("smash error: > too many arguments \n");
			return 0;
		}
		char cwd[256];
		if (getcwd(cwd, sizeof(cwd))== NULL)
			perror("smash error: > ");
		cout <<cwd <<endl;
	}
	
	/*************************************************/
	else if (!strcmp(cmd, "history")) // prints the history of all commands and their args entered by the user and
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
	
	else if (!strcmp(cmd, "jobs")) //holds the list of commands running in the bg and prints them
	{
		removeFinishedJobs();
		printJobs();
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) //prints the PID of the smash program
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
			int waited_status;
			if (num_arg == 1) { // brings specific process to the foreground
				waited_pid = getPidFromJobs(atoi(args[1]));
				fg_name = getNameFromJobs(getCommandID(waited_pid));
				if(waited_pid <= 0) { //id not found
					printf("smash error: > job id was not found in the jobs list \n");
					return 1;
				}
				if (isJobStopped(atoi(args[1]))) // check if the process was stopped by SIGTSTP
					sendSignal(SIGCONT, waited_pid, DO_PRINT);
			}
			else { //bring the last process in the BG to the FG
				waited_pid = getLastBGFromJobs();
				if(waited_pid <= 0) {
					printf("smash error: > There are no jobs in the BG \n");
					return 1;
				}
				fg_name = getNameFromJobs(jobs.size()-1);
				cout << fg_name << endl;
				if (isJobStopped(jobs.size()-1)) // check if the process was stopped by SIGTSTP
					sendSignal(SIGCONT, waited_pid, DO_PRINT);
			}
			fgPid = waited_pid;
			removeJob(waited_pid);
			pid_t done = waitpid(waited_pid, &waited_status , WUNTRACED);
			if (done < 0)
				perror("smash error: > ");
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "bg")) //resumes a process that was stopped
	{
		if(num_arg>1)
		{
			cout<< "wrong number of arguments"<<endl;
			return 0;
		}
		else {
			pid_t resume_pid;
			if(num_arg==0)// no parameters resume last stopped job
			{
				resume_pid = findLastStopped();
				cout << "bg debug1: the last stopped, the pid :"<<resume_pid<<endl;
				if(resume_pid < 0)// no stopped jobs
				{
					cout<< "There are no stopped commands" <<endl;
					return 0;
				}
				else
				{
				    printJobs();
					int cmd_num=getCommandID(resume_pid);
					cout << "bg debug2: the cmd id :"<<cmd_num<<endl;
					jobs[cmd_num].status=false; //change from stopped
					sendSignal(SIGCONT, resume_pid, DO_PRINT);
				}
			}
			else if (num_arg==1) // pid of command to run
			{
				resume_pid=getPidFromJobs(atoi(args[1]));
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
	{ // TODO: check if return value of sendSignal function needs to be evaluated.
		//quit(exit) the main process +[kill] kill all sub process
   		if (num_arg > 1) //too many arguments
			printf("smash error: > too many arguments \n");
   		else if (num_arg == 1){
   			removeFinishedJobs();
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
						perror("smash error: 7> ");
					else if (waited_pid == 0) { //if the process didn't end in 5sec
						cout << "(5 sec passed) Sending SIGKILL... ";
						sendSignal(SIGKILL, current_pid, DONT_PRINT);
						if (waitpid(current_pid, NULL, 0) > 0)
							cout << "Done." << endl;
						else
							perror("smash error: 8> ");
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
	else if (!strcmp(cmd, "mv"))//changes the name of a file to the new name entered by the user
	{

		if(num_arg!=2)
		{
			cout<< "wrong number of arguments"<<endl;
			return 0;
		}
		if(rename(args[1],args[2])<0)
		{
			perror("smash error: > ");
		}
		else
			cout<< args[1] << " has been renamed to " << args[2]<<endl;

	}
	/*************************************************/
	else if (!strcmp(cmd, "kill"))//sends signal according to the arguments entered
	{  //todo: if kill sends SIGTSTP to BG command needs to flag it as stopped.
	// this command always refers to the commands running in the BG

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
			if(cmd_id<=0 || cmd_id>getNumOfJobs())
			{
				cout<< "smash error:> kill" << cmd_id << " - job does not exist"<< endl;
			}
			else {
				pid_t sendSig2PID=getPidFromJobs(cmd_id);
				if (sendSignal(signum, sendSig2PID, DO_PRINT)) // if error in signal sending
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
				removeJob(done);
				if (done <0){
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
    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
    {
		return 0; // do nothing if the command ins complicated
	} 
	return -1;
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs data structure
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, bool* BGFlag)
{
	if(jobs.empty()){
		addNewJob(0,false,"init");
	}
	char* delimiters = " \t\n";
	if (lineSize[strlen(lineSize)-2] == '&')
	{
		lineSize[strlen(lineSize)-2] = '\0';
		addNewJob(0,false,strtok(lineSize, delimiters));
		*BGFlag=true;
		return 0;
	}
	return -1;
}




