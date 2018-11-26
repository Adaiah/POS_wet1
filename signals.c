// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers
/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"

extern pid_t fgPid;
extern std::string fg_name;
void ctrlCHandler(int signum);





//**************************************************************************************
// function name: ctrlZHandler
// Description: the function handles the SIGNTSTP signal
// Parameters:
// Returns: void
//**************************************************************************************
void ctrlZHandler(int signum){
    int stopped_status = 0;
    if(fgPid>0){
        sendSignal(SIGTSTP ,fgPid,DO_PRINT);
        waitpid(fgPid,&stopped_status,WUNTRACED);
        if (WIFSTOPPED(stopped_status)){ //check if the child process stopped
            addNewJob(fgPid, true , fg_name);
        }
        fgPid = 0;
    }
}
