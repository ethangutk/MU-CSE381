#ifndef MAIN_CPP
#define MAIN_CPP

/**
 * This source file serves as a tester for validating correct
 * operation of the ChildProcess class and writing data in a chunked
 * format.
 *
 * Copyright (C) 2020 raodm@miamiOH.edu
 */

#include <iostream>
#include <string>
#include <vector>
#include "ChildProcess.h"

/** The HTTP response header to be printed at the beginning of the
    response */
const std::string HTTPHeaders =
    "HTTP/1.1 200 OK\r\n"
    "Transfer-Encoding: chunked\r\n"
    "Connection: Close\r\n"
    "Content-Type: text/plain\r\n"
    "\r\n";

/**
 * The method that runs a command and prints its output in HTTP
 * Chunked format.
 *
 * \param[out] os The output stream to where the chunked HTTP response
 * is to be written.
 *
 * \param[in] cmd The command to be run by this method using methods
 * in ChildProcess class.
 */
void sendCmdOutput(std::ostream& os, const std::string& cmd) {
    // Split the command into individual words
    const StrVec args = ChildProcess::split(cmd);
    // Create a child process to run the command
    ChildProcess cp;
    cp.forkNexecIO(args);
    // Now print the output of the program line-by-line to the given
    // output stream.
    os << HTTPHeaders;
    std::istream& progOutStream = cp.getChildOutput();
    
    
    for (std::string line; std::getline(progOutStream, line);) {     
        line += '\n';
        os << std::hex << line.size() << "\r\n" << line << "\r\n";
    }
    
    // Must call wait, which calls waitpid to finish the process.
    cp.wait();
    os << "0\r\n\r\n";
}

//--------------------------------------------------------------------
//  DO  NOT  MODIFY  CODE  BELOW  THIS  LINE
//--------------------------------------------------------------------

/**
 * A simple main program that serves as a test harness for validating
 * the operations of the program.
 *
 * \param[in] argc The number of command-line arguments.  This value
 * should always be 1 for this program.
 *
 * \param[in] argv The actual command-line arguments.  This program
 * uses only the first command-line argument.
 */
int main(int argc, char *argv[]) {
    // Assume the first command-line argument has multiple words
    // containing the command to be run -- eg: "ls -l '/'"
    const std::string cmd = argv[1];
    // Run the command using helper method.
    sendCmdOutput(std::cout, cmd);
    // All done.
    return 0;
}

#endif
