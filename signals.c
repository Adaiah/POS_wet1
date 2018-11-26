// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers
/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"

extern pid_t fgPid;
void ctrlZHandler(int signum);





//**************************************************************************************
// function name: ctrlZHandler
// Description: the function handles the SIGNTSTP signal
// Parameters:
// Returns: void
//**************************************************************************************
void ctrlCHandler(int signum){
    int stopped_status = 0;
    if(fgPid>0){
        sendSignal(SIGTSTP ,fgPid,DO_PRINT);
        waitpid(fgPid,&stopped_status,WUNTRACED);
        if (WIFSTOPPED(stopped_status)){ //check if the child process stopped
            addNewJob(fgPid, true);
        }
        fgPid = 0;
    }
}
