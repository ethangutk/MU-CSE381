#ifndef CHILD_PROCESS_CPP
#define CHILD_PROCESS_CPP

/**
 * This source file contains the implementation for the various
 * methods defined in the ChildProcess class.
 *
 * Copyright (C) 2020 raodm@miamioh.edu
 */

// All the necessary #includes are already here
#include <unistd.h>
#include <sys/wait.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include "ChildProcess.h"

// Named-constants to keep pipe code readable below
const int READ = 0, WRITE = 1;


/** NOTE: Unlike Java, C++ does not require class names and file names
 * should match.  Hence when defining methods pertaining to a specific
 * class, the class name should be prefixed with each method -- as in
 * "ChildProcess::"
 */


// This method is just a copy-paste from lecture notes. This is done
// to illustrate an example.

void
ChildProcess::myExec(StrVec argList) {
    std::vector<char*> args;  // list of pointers to args
    for (auto& s : argList) {
        args.push_back(&s[0]);  // address of 1st character
    }
    // nullptr is very important
    args.push_back(nullptr);
    // Make execvp system call to run desired process
    execvp(args[0], &args[0]);
    // In case execvp ever fails, we throw a runtime execption
    throw std::runtime_error("Call to execvp failed for: " + argList[0]);
}

// The constructor

ChildProcess::ChildProcess() : childPid(-1), childOutput(&pipeBuf) {
    // childPid is initialized and not assigned!  Hence body is empty.
}

// Implement the destructor.  The destructor is an empty method
// because this class does not have any resources to release.

ChildProcess::~ChildProcess() {
    // Maybe it is a good idea to call wait() method here to ensure
    // that the child process does not get killed? On the other hand,
    // if we call wait() here and the child process misbehaves, then
    // it will appear as if this program is buggy -- Awwww.. -- no
    // good choice.  In this situation, a good API is to leave it to
    // the user to decide what is the best course of action.
}

StrVec
ChildProcess::split(const std::string& str) {
    StrVec retVal;  // The list of words to be returned.
    std::istringstream is(str);  // Stream to split words

    // Extract words while honoring quotes
    for (std::string word; is >> std::quoted(word);) {
        retVal.push_back(word);  // add words to the vector.
    }
    return retVal;  // return the list of words
}

// Use the comments in the header to implement the forkNexec method.
// This is a relatively simple method with an if-statement to call
// myExec in the child process and just return the childPid in parent.

int
ChildProcess::forkNexec(const StrVec& argList) {
    // Fork and save the pid of the child process
    childPid = fork();
    // Call the myExec helper method in the child
    if (childPid == 0) {
        // We are in the child process
        myExec(argList);
    }
    // Control drops here only in the parent process!
    return childPid;
}

// Use the comments in the header to implement the wait method.  This
// is a relatively simple method which uses waitpid call to get
// exitCode as shown in Slide #6 of ForkAndExec.pdf

int
ChildProcess::wait() const {
    int exitCode = 0;  // Child process's exit code
    waitpid(childPid, &exitCode, 0);  // wait for child to finish
    return exitCode;
}

// Method to first redirect output of child process via a pipe. Then
// this method runs the specified program in the child process. The
// output of the cild process can be read in the parent process via
// the childProcess stream.

int
ChildProcess::forkNexecIO(const StrVec& argList) {
    // Use example(s) from video/slides to implement this method. Here is
    // a check-list of things to keep in mind:
    int fd[2];

    //    1. First create a pipe (with read and write file descriptors).
    pipe(fd);

    //    2. Use the code from forkNexec method to fork the process and
    //        2.1. In child -- close read-end of pipe, use dup2 to tie write
    //             end of pipe, and use myExec to execute command.
    //
    //        2.2. In parent -- close write-end of pipe, and initialize
    //             the pipe buffer as shown below. You will not need any
    //             istream as its already setup in the constructor.
    //
    //             pipeBuf = {pipefd[READ], std::ios::in, sizeof(char)}; 
    childPid = fork();
    if (childPid == 0) {
        close(fd[0]);
        dup2(fd[WRITE], 1);
        // std::stdio_filebuf<char> fb(fd[1], ios::out, 1);
        // std::ostream os(&fb);
        myExec(argList);
    }
    pipeBuf = {fd[READ], std::ios::in, sizeof(char)};
    close(fd[WRITE]);
    // Return the child's pid in the parent.
    return childPid;
}

#endif

