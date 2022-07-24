/*
 * Copyright (C) 2021 Ethan Gutknecht
 * 
 * Created on February 23, 2021, 8:14 PM
 */

#include <iostream>
#include <vector>
#include <string>
// #include <iomanip>
#include <boost/asio.hpp>
#include <iomanip>
#include "ChildProcess.h"

// Using lines
using namespace std;
using StrVec = std::vector<std::string>;

/**
 * This is a helper method that will take a exe command given a a string
 * written in the variable line. (for example: "sleep 5" or 
 * "echo "Hello \t world!") It will split that line up, print a running
 * statement, perform the command, and print the exit code. Depending
 * if the user wants parallel or serial (parallel bool parameter), it will
 * do all the processes at once or do them one at a time.
 * 
 * @param line This is the raw input line the user will input within
        the console. This should be an exe command to execute
 * @param parallel This is true if all the execommand inputs need to be
 *      run at once and will skip over the wait time.
 * @return This will be used with the parallel command only. The returned
 *      ChildProcess will be put into a vector and to later perform the
 *      wait() function.
 */
ChildProcess runOther(std::string line, bool parallel = false) {
    // StrVec commandArguments;
    std::istringstream ss(line);

    StrVec v;

    // Creates argument vector
    while (ss >> std::quoted(line)) {
        v.push_back(line);
    }

    // Prints output
    std::cout << "Running:";
    for (size_t i = 0; i < v.size(); i++) {
        std::cout << " " << v[i];
    }
    std::cout << endl;

    // Calls child process method
    ChildProcess cp;
    cp.forkNexec(v);

    if (parallel == false) {
        std::cout << "Exit code: " << cp.wait() << std::endl;
    }
    return cp;
}

/**
 * Method is similar to the base case. It makes sure the lines from the website
 * does not start with a '#' or the line is not empty. If the website line is
 * "exit" it will break the loop. Otherwise, it will call the runOther line.
 * If the user wanted to do the commands Parallel, it will use the vector of
 * ChildProcesses created and call the wait() all at once after every exe
 * command is given. (This part is skipped in the runOther())
 * 
 * @param websiteData Data taken from the URL and read line by line in method.
 * @param parallel If true, exe commands from lines will run all at once.
 */
void runSerialOrParallel(std::istream& websiteData, bool parallel = false) {
    std::string line;
    std::vector<ChildProcess> prcVec;
    for (std::string hdr;
            std::getline(websiteData, hdr)
            && !hdr.empty() && hdr != "\r";) {
    }

    while (std::getline(websiteData, line)) {
        if (line.size() == 0 || line.at(0) == '#') {
            continue;

        } else if (line.substr(0, 4) == "exit") {
            break;

        } else {
            prcVec.push_back(runOther(line, parallel));
        }
    }
    if (parallel == true) {
        for (auto i : prcVec) {
            std::cout << "Exit code: " << i.wait() << std::endl;
        }
    }
}

/**
 * METHOD COPIED FROM HOMEWORK 1
 * 
 * This method takes in a URL as a string and breaks it down into
 * host, port, and path.
 * 
 * @param url String of URL
 * @return tuple<host name, port number, file path>
 */
std::tuple<std::string, std::string, std::string>
breakDownURL(const std::string& url) {
    // The values to be returned.
    std::string hostName, port = "80", path = "/";

    // Extract the substrings from the given url into the above
    // variables.  This is very simple 174-level logic problem.
    // Implement your logic here to appropriately set the values for
    // hostName, port, and path variables (defined above)

    std::string fString = url.substr(url.find("//") + 2, std::string::npos);

    if (fString.find(":") != std::string::npos) {
        hostName = fString.substr(1, fString.find(":"));
        fString = fString.substr(fString.find(":") + 1, std::string::npos);
        port = fString.substr(0, fString.find("/"));
    } else {
        hostName = fString.substr(0, fString.find("/"));
    }
    path = path + fString.substr(fString.find("/") + 1, std::string::npos);

    // Return 3-values encapsulated into 1-tuple.
    return {hostName, port, path};
}

/**
 * 
 * METHOD COPIED FROM HOMEWORK 1
 * 
 * This method will get data from a URL and get the info that is
 * important to us in executing commands. (file data)
 * 
 * @param url URL from where to get the data from.
 * @param choice A String that decides if exe commands will be run parallel
 *        or serial.
 */
void processData(std::string url, std::string choice) {
    // Next extract download URL components. That is, given a URL
    // "http://www.users.miamioh.edu/raodm/ones.txt", 
    std::string hostname, port, path;
    std::tie(hostname, port, path) = breakDownURL(url);

    // Start the download of the file (that the user wants to be
    // processed) at the specified URL.  We use a BOOST tcp::iostream.
    boost::asio::ip::tcp::iostream data(hostname, port);
    data << "GET " << path << " HTTP/1.1\r\n"
            << "Host: " << hostname << "\r\n"
            << "Connection: Close\r\n\r\n";
    // Have the helper method process the file's data and print/send
    // results (in HTTP/HTML format) to a given output stream.
    if (choice == "PARALLEL") {
        runSerialOrParallel(data, true);
    } else if (choice == "SERIAL") {
        runSerialOrParallel(data);
    }
}

/**
 * Method called by main method to get input from the user. After
 * the input is received, it will decide what helper method to
 * call using an if statement. This loop will run until "quit"
 * is typed within the console.
 * 
 * @param is Input Stream, defaulted to cin.
 * @param prompt What the user will be promt with.
 * @param parallel True if exe commands are to be run parallel.
 */
void process(std::istream& is = std::cin,
        const std::string& prompt = "> ", bool parallel = false) {
    // Variables
    std::string line;

    // Prompt the user with input until user types in "quit"
    while (std::cout << prompt, std::getline(std::cin, line)) {
        // If starts with # ignore or line is empty
        if (line.empty() == true || line.at(0) == '#') {
            continue;

            // if exit, terminate
        } else if (line == "exit") {
            break;

            // If SERIAL - processData method called with PARALLEL param
        } else if (line.substr(0, 8) == "PARALLEL") {
            processData(line.substr(10), "PARALLEL");

            // If SERIAL - processData method called with SERIAL param
        } else if (line.substr(0, 6) == "SERIAL") {
            processData(line.substr(10), "SERIAL");

            // run first word as command, rest as arguments, parallel
            // param defaulted to false.
        } else {
            runOther(line);
        }
    }
}

int main(int argc, char** argv) {
    process();
    return 0;
}

