// Copyright (C) 2018 raodm@miamiOH.edu

#include <unistd.h>
#include <iostream>
#include <thread>
#include <vector>

/**
 * The thread method.
 * 
 * This method simply sleeps for 1 second and prints the time
 * remaining until the specified MaxTime (seconds) has elapsed.
 */
void timer(const int MaxTime, const int threadID) {
    // Use units to make magic numbers more meaningful
    using namespace std::chrono_literals;
    // Repeatedly sleep for MaxTime seconds in steps of 1-second.
    for (int i = MaxTime; (i > 0); i--) {
        if (threadID == 0) {
            std::cout << "\r" << i << " seconds left..." << std::flush;
        }
        std::this_thread::sleep_for(1s);
    }
}

/**
 * The main method that fires off various threads and waits for them
 * to finish.
 * 
 * @param args Optional command-line arguments. The first argument is
 * the number of threads. The second argument is the time delay.
 */
int main(int argc, char *argv[]) {
    // Process command-line arguments if any.
    const int thrCount = (argc > 1 ? std::stoi(argv[1]) : 5);
    const int maxTime  = (argc > 2 ? std::stoi(argv[2]) : 2400);  // seconds
    // Print the current Process ID (PID)
    std::cout << "Your process PID: " << getpid() << std::endl;
    std::cout << "Starting up " << thrCount << " threads.\n";
    // First, start (or fork) all of the threads.
    std::vector<std::thread> thrList;
    for (int i = 0; (i < thrCount); i++) {
        // Create a thread and add it to list of threads.
        thrList.push_back(std::thread(timer, maxTime, i));
    }
    // Wait for the threads to join.
    for (std::thread& t : thrList) {
        t.join();  // Block/wait for thread to finish. Join is same
                   // as waitpid, but for threads.
    }
    return 0;
}
