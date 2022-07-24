/**
 * Copyright (C) 2021 Ethan Gutknecht
 * 
 * A simple source file to keep parts of the exercise nicely organized.
 * 
 */

#include "ChildProcess.h"

/** A simple method that must be modified to run 3 processes in
    parallel. The starter code for this method is included in the
    exercise document.  You can copy-paste the starter code from the
    document (ensure formatting is good) and then modify it.
    
    \note This method is already called from main()
*/
void runProcesses() {
    // The command to be run -- this just sleeps for 5 seconds.
    const StrVec cmd = {"sleep", "5"};
    // Run the same process 3 times in serial fashion.
    ChildProcess process1, process2, process3;

    process1.forkNexec(cmd);
    process2.forkNexec(cmd);
    process3.forkNexec(cmd);
    process1.wait();
    process2.wait();
    process3.wait();
}

