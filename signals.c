// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers
/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"

extern pid_t fgPid;
extern std::string fg_name;



//**************************************************************************************
// function name: ctrlZHandler
// Description: the function handles the SIGTSTP signal
// Parameters:
// Returns: void
//**************************************************************************************
void ctrlZHandler(int signum){
    int stopped_status = 0;
    if(fgPid>0){
        std::cout<<"debug1: "<<fgPid<<std::endl; //todo
        std::cout<< "smash > signal SIGTSTP was sent to pid " << fgPid <<std::endl; //todo
        kill(fgPid,SIGTSTP);
        waitpid(fgPid,&stopped_status,WUNTRACED);
        if (WIFSTOPPED(stopped_status)){ //check if the child process stopped
            addNewJob(fgPid, true , fg_name);
        }
        fgPid = 0;
        std::cout<<"debug2: "<<fgPid<<std::endl; //todo
    }
}


//**************************************************************************************
// function name: ctrlCHandler
// Description: the function handles the SIGINT signal
// Parameters: signum - number of signal
// Returns: void
//**************************************************************************************
void ctrlCHandler(int signum){
    if(fgPid>0){
        std::cout<<"debug1: "<<fgPid<<std::endl;//TODO debud print
        sendSignal(SIGINT ,fgPid,DO_PRINT);
        fgPid = 0;
        std::cout<<"debug2: "<<fgPid<<std::endl; //TODO debud print
    }
}