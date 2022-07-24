/** 
 * A custom web-server that can process HTTP GET requests and:
 *    1. Respond with contents of a given HTML file
 *    2. Run a specified program and return the output from the process.
 *
 * Copyright (C) 2020 raodm@miamioh.edu.
 */

#include <sys/wait.h>
#include <boost/asio.hpp>
#include <vector>
#include <string>
#include <thread>
#include <future>
#include <mutex>
#include <cmath>
#include "HTTPFile.h"
#include "ChildProcess.h"
#include "HTMLFragments.h"

// Convenience namespace to streamline the code below.
using namespace boost::asio;
using namespace boost::asio::ip;

// Forward declaration for url_decode method defined below.  We need
// this just to organize the starter code into sections that students
// should not modify (to minimize problems for students).
std::string url_decode(std::string url);

// Forward declaration for serveClient that is defined further below.
void serveClient(std::istream& is, std::ostream& os, bool);

/**
 * Runs the program as a server that listens to incoming connections.
 * 
 * @param port The port number on which the server should listen.
 */
void runServer(int port) {
    // Setup a server socket to accept connections on the socket
    io_service service;
    // Create end point
    tcp::endpoint myEndpoint(tcp::v4(), port);
    // Create a socket that accepts connections
    tcp::acceptor server(service, myEndpoint);
    std::cout << "Server is listening on "
              << server.local_endpoint().port()
              << " & ready to process clients...\n";
    // Process client connections one-by-one...forever
    using TcpStreamPtr = std::shared_ptr<tcp::iostream>;
    while (true) {
        // Create garbage-collected, shared object on heap so we can
        // send it to another thread and not worry about life-time of
        // the socket connection.
        TcpStreamPtr client = std::make_shared<tcp::iostream>();
        // Wait for a client to connect    
        server.accept(*client->rdbuf());
        // Process request from client on a separate background thread
        // I am using a lambda here so that I don't have to write
        // another method (no, not lazy, just simple :-)).
        std::thread thr([client](){ serveClient(*client, *client, true); });
        thr.detach();
    }
}

/**
 * This method will create a vector to return. The vector will
 * be printed in the sendCommandOutputs method. The vector
 * should only have two items in it as there should be only two chunks of HTML 
 * data to be created. More info about how the two elements within the vector
 * are formatted based on the parameters.
 * 
 * 
 * @param resultsVec Vector of string (always 2 elements) that will be returned
 * @param genChart This will decide if a graph will be implemented in the
 *                 second element within the return vector.
 * @return 
 */
std::vector<std::string> createPrintResults(
        std::vector<std::vector<int>> resultsVec, bool genChart) {
    std::vector<std::string> returnVec;
    int counter = 1;
    std::vector<std::string> baseCaseVec;
    for (const auto& iterationVec : resultsVec) {
        // Create the line for the base case
        std::string str = 
                "       <tr><td>" + std::to_string(counter++) +  // seconds
                "</td><td>" + std::to_string(iterationVec.at(0)) +  // u time
                "</td><td>" + std::to_string(iterationVec.at(1)) +  // s time
                "</td><td>" + std::to_string(iterationVec.at(2)) +  // v size
                "</td></tr>\n"; 
        baseCaseVec.push_back(str);
    }
    
    counter = 1;
    std::vector<std::string> graphCaseVec;
    for (const auto& iterationVec : resultsVec) {
        // Create the line for the base case
        std::string str = 
                "          [" + std::to_string(counter++) + ", " +  // seconds
                // utime + stime
                std::to_string(iterationVec.at(0) + iterationVec.at(1)) 
                + ", " + std::to_string(iterationVec.at(2)) +  // vsize
                "]\n";
        graphCaseVec.push_back(str);
    }
    
    // Base Case
    // Chunk 1 = htmlStart
    // Chunk 2 = htmlMid1 + baseCaseVec + htmlMid2 + htmlEnd
    
    // Graph Case
    // Chunk 1 = htmlStart
    // Chunk 2 = htmlMid1 + baseCaseVec + htmlMid2 + graphCaseVec + htmlEnd
    returnVec.push_back(htmlStart);
    
    std::string finalChunk = htmlMid1;
    for (const auto& line : baseCaseVec) {
        finalChunk = finalChunk + line;
    }
    finalChunk = finalChunk + htmlMid2;
    if (genChart == true) {
        for (const auto& line : graphCaseVec) {
            finalChunk = finalChunk + line;
        }
    }
    returnVec.push_back(finalChunk + htmlEnd);
    return returnVec;
}

/**
 * This will read the procfile and store the data that is needed in an int 
 * vector. The vector will be returned and used to make a graph and
 * table later. More info on how the vector looks like in within the method.
 * 
 * @param file A reference to the proc stat file that will be read.
 * @return int vec with necesary data.
 */
std::vector<int> readProcFile(std::ifstream& file) {
    int counter = 0;
    std::string line, word;
    std::vector<int> returnVector;
    
    if (file.good()) {
        // Record statistics in a stringstream
        std::getline(file, line);
        std::istringstream iss(line);


        while (iss >> word) {
            if (counter == 14) {
                // utime (SECONDS)
                returnVector.push_back((std::round(
                            std::stof(word) / sysconf(_SC_CLK_TCK))));

            } else if (counter == 15) {
                // stime
                returnVector.push_back((std::round(
                            std::stof(word) / sysconf(_SC_CLK_TCK))));

            } else if (counter == 23) {
                // vsize (MB)
                returnVector.push_back((std::stof(word) / 1000000));
            }
            counter++;
        }
    }
    return returnVector;
}


/**
 * 
 * The 3D vector would look like this
 * [
 *  {utime, stime, vsize}
 *  {1, 0, 0}
 *  {2, 0, 0}
 *  {.......}
 * ]
 * 
 * 
 * @param cp Reference to the child process object
 * @param os Reference to the output stream being used
 * @param resultsVec This is where the results will be stored
 * @return 
 */
void recordStats(ChildProcess& cp, std::ostream& os, 
        std::vector<std::vector<int>>& resultsVec) {
    int exitCode = 0;
    
    // The 3D vector would look like this
    // [
    // {utime, stime, vsize}
    // {1, 0, 0}
    // {2, 0, 0}
    // ...
    // ]
    
    while (waitpid(cp.getPid(), &exitCode, WNOHANG) == 0) {
        // sleep for 1 second.
        sleep(1);  
        
        // Read file
        std::ifstream file("/proc/" + std::to_string(cp.getPid()) + "/stat");
        resultsVec.push_back(readProcFile(file));
    }
}

/**
 * This is pretty much a copy-paste from a previous exercise solution.
 * This method uses the ChildProcess class to run a program and sends
 * the program outputs as HTTP chunked-responses.
 *
 * @param cmd The command to be executed. This command should be already
 * url_decoded and is in the form "ls -la ./"
 *
 * @param os The output stream to where the output from the command is
 * to be written in chunked HTTP response format.
 * 
 * @param genChart If flag is true, then graph data is also printed.
 */
void sendCmdOutput(std::string& cmd, std::ostream& os, bool genChart) {
    // Remove the "/cgi-bin/exec?cmd=" prefix to help process the
    // command.
    // Commented out due to "cgi-bin" being deleted already.
    // cmd = url_decode(cmd.substr(18));
    
    
    // Split the command into words for processing using helper method
    // in ChildProcess.
    const StrVec args = ChildProcess::split(cmd);
    // 3D vector for the results to be stored in
    std::vector<std::vector<int>> rslt;
    // Create the child process.
    ChildProcess cp;
    cp.forkNexecIO(args);

    // As it is waiting, we need to record stats, thus we need
    // a helper method right before this one to record.
    // The helper method will be called by a thread.
    std::thread thread(recordStats, std::ref(cp), std::ref(os), std::ref(rslt));
    // Join (or wait) for the thread to finish
    thread.join();
    // Get the outputs from the child process.
    
    // Create print out results vector
    
    std::vector<std::string> printResults = createPrintResults(rslt, genChart);
    
    std::istream& is = cp.getChildOutput();
    // Send the output from child process to the client as chunked response
    os << http::DefaultHttpHeaders << "text/html\r\n\r\n";
    // Send each line as a spearate chunk.
    for (std::string line; std::getline(is, line);) {
        line += "\n";  // Add the newline that was not included by getline
        os << std::hex << line.size() << "\r\n" << line << "\r\n";
    }
    
    // Prints html start
    os << std::hex << printResults.at(0).size() << "\r\n";
    os << printResults.at(0) << "\r\n";
    
    // It is important to wait for the process to finish. Internally,
    // the method below calls waitpid system call.
    int exitCode = cp.wait();
    const std::string line = "Exit code: " + std::to_string(exitCode) + "\n";
    os << std::hex << line.size() << "\r\n" << line << "\r\n";
    
    // Prints html middle (with graph or not depending on params)
    os << std::hex << printResults.at(1).size() << "\r\n";
    os << printResults.at(1) << "\r\n";
    
    // Finally send the trailing end-of-stream chunk
    os << "0\r\n\r\n";
}

//------------------------------------------------------------------
//  DO  NOT  MODIFY  CODE  BELOW  THIS  LINE
//------------------------------------------------------------------

/**
 * Process HTTP request (from first line & headers) and
 * provide suitable HTTP response back to the client.
 * 
 * @param is The input stream to read data from client.
 *
 * @param os The output stream to send data to client.
 * 
 * @param genChart If flag is true, then graph data is also printed.
 */
void serveClient(std::istream& is, std::ostream& os, bool genChart) {
    // Read headers from client and print them. This server
    // does not really process client headers
    std::string line, path;
    // Read the "GET" word and then the path.
    is >> line >> path;
    // Skip/ignore all the HTTP request & headers for now.
    while (std::getline(is, line) && (line != "\r")) {}
    // Check and dispatch the request appropriately
    if (path.find("/cgi-bin/exec?cmd=") == 0) {
        // This is a command to be processed. So use a helper method
        // to streamline the code.
        // Remove the "/cgi-bin/exec?cmd=" prefix to help process the
        // command.
        auto cmd = url_decode(path.substr(18));
        sendCmdOutput(cmd, os, genChart);
    } else if (!path.empty()) {
        // In this case we assume the user is asking for a file.  Have
        // the helper http class do the processing.
        path = "." + path;  // make path with-respect-to pwd.
        // Use the http::file helper method to send the response back
        // to the client.
        os << http::file(path);
    }
}

/** Convenience method to decode HTML/URL encoded strings.

    This method must be used to decode query string parameters
    supplied along with GET request.  This method converts URL encoded
    entities in the from %nn (where 'n' is a hexadecimal digit) to
    corresponding ASCII characters.

    \param[in] str The string to be decoded.  If the string does not
    have any URL encoded characters then this original string is
    returned.  So it is always safe to call this method!

    \return The decoded string.
*/
std::string url_decode(std::string str) {
    // Decode entities in the from "%xx"
    size_t pos = 0;
    while ((pos = str.find_first_of("%+", pos)) != std::string::npos) {
        switch (str.at(pos)) {
            case '+': str.replace(pos, 1, " ");
            break;
            case '%': {
                std::string hex = str.substr(pos + 1, 2);
                char ascii = std::stoi(hex, nullptr, 16);
                str.replace(pos, 3, 1, ascii);
            }
        }
        pos++;
    }
    return str;
}

/**
 * The main function that serves as a test harness based on
 * command-line arguments.
 *
 * \param[in] argc The number of command-line arguments.  This test
 * harness can work with zero or one command-line argument.
 *
 * \param[in] argv The actual command-line arguments.  If this is an
 * number it is assumed to be a port number.  Otherwise it is assumed
 * to be an file name that contains inputs for testing.
 */
int main(int argc, char *argv[]) {
    // Check and use first command-line argument if any as port or file
    const std::string True = "true";  // Just a sentinel value
    std::string arg = (argc > 1 ? argv[1] : "0");
    const bool genChart = (argc > 2 ? (argv[2] == True) : false);
    // Check and use a given input data file for testing.
    if (arg.find_first_not_of("1234567890") == std::string::npos) {
        // All characters are digits. So we assume this is a port
        // number and run as a standard web-server
        runServer(std::stoi(arg));
    } else {
        // In this situation, this program processes inputs from a
        // given data file for testing.  That is, instead of a
        // web-browser we just read inputs from a data file.
        std::ifstream getReq(arg);
        if (!getReq.good()) {
            std::cerr << "Unable to open " << arg << ". Aborting.\n";
            return 2;
        }
        // Have the serveClient method process the inputs from a given
        // file for testing.
        serveClient(getReq, std::cout, genChart);
    }
    // All done.
    return 0;
}
