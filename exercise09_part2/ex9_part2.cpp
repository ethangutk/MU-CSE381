/**
 * A simple multithreaded program to perform a simple Monte Carlo like
 * simulation.
 *
 * Copyright (C) raodm@miamiOH.edu
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>

// A shortcut to refer to an hash map
using NumMap = std::unordered_map<int, int>;

// A namespace to ensure we don't pollute due to globals 
namespace id {
    // A shared map for tracking counts.  The first entry is thread number.
    // The second one is a generic value.
    NumMap num = {{0, 0}, {1, 0}, {2, 0}};
    std::mutex numMutex;
};

/**
 * Helper method to increase the number of occurrences.
 *
 * \param[in] i The key whose value is to be updated.
 */
void inc(int i) {
    // std::lock_guard<std::mutex> g(id::numMutex);
    id::num[i]++;
}

/**
 * Helper method to decrease the number of occurrences.
 *
 * \param[in] i The key whose value is to be updated.
 */
void dec(int i) {
    // std::lock_guard<std::mutex> g(id::numMutex);
    id::num[i]--;
}

/**
 * The method that is called from multiple threads.
 *
 * \param[in] thr The ID of the thread to be used by this method.
 *
 * \param[in] numThreads The total number of threads being run in this
 * program.
 */
void threadMain(int thr, int numThreads) {
    // Do a Monte Carlo like simulation to track number of odd and
    // even numbers generated.  Ideally, in a uniform random number
    // generator, the number of odds and evens must be the same.  So
    // the net total per thread should be zero.
    unsigned seed = thr;
    
    
    /*
     * Tried this a couple different ways but no luck :( 
     * 
     * UPDATE: overcomplicated this problem...but ill leave this
     * here to laugh at in the future.
     * 
     * 
    // Waits for the map to have the same value
    // as the threads
    if (id::num.size() < numThreads) {
        std::lock_guard<std::mutex> g(id::numMutex);
        int i = 0;
        while (numThreads < id::num.size()) {
             id::num[i] = 0;
             i++;
        } 
    }
    */
    
    
    for (int i = 0; (i < 10'000); i++) {
        std::lock_guard<std::mutex> g(id::numMutex);
        if (rand_r(&seed) % 2) {
            inc(thr);  // Track evens on this thread.
        } else {
            dec(thr);  // Track odds on this threads
        }
    }
}

//-----------------------------------------------------------
//  DO  NOT   MODIFY  CODE   BELOW   THIS   LINE
//-----------------------------------------------------------

/**
 * Main method to create and manage 'n' threads to update operations
 * from multiple threads.
 *
 * \param[in] argc The number of command-line args. For this program
 * this value should be 2.
 *
 * \param[in] argv The actual command-line argument. The first
 * argument must be the number of threads to use.
 */
#ifndef CODE_TEST
#define MAIN main
#else
#define MAIN main_part2
#endif

int MAIN(int argc, char *argv[]) {
    // Check to ensure we have number of thread specified.
    if (argc != 2) {
        std::cout << "Number of threads not specified.\n"
                  << "Not reading instructions?\n";
        return 1;  // Can't proceed further.
    }
    // Get the number of threads from command-line args. if specified.    
    const int numThreads = std::stoi(argv[1]);

    // Spin-up the specified number of threads.
    std::vector<std::thread> threads;
    for (int i = 0; (i < numThreads); i++) {
        threads.push_back(std::thread(threadMain, i, numThreads));
    }
    // Wait for threads to finish.
    for (auto& thr : threads) {
        thr.join();
    }

    // Print values from the unordered map.
    for (int i = 0; (i < numThreads); i++) {
        std::cout << "key[" << i << "] = "
                  << id::num[i] << std::endl;
    }
    // All done.
    return 0;
}
