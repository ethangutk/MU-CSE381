/**
 * Exercise with 3 exam/interview style questions to help students
 * review questions for upcoming exam.
 *
 * Copyright (C) 2019 raodm@miamiOH.edu
 */

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include <atomic>
#include <numeric>
#include <condition_variable>

// Prototype for functions under test
void printInOrder(const int id, std::string data);
void producer(std::vector<int>& q, std::mutex& qMutex,
              std::condition_variable& qCondVar, int n);
void consumer(std::vector<int>& q, std::mutex& qMutex,
              std::condition_variable& qCondVar, int n);
std::vector<int> countDigits(const std::vector<int> values, int n);
    
/** A simple tester method to call/test the printInOrder method.
 */
void testPrintOrder(int thrCount, const std::string& info) {
    // Generate ID's and randomize them so that we start threads in
    // random order for good testing.
    std::vector<int> thrIDs(thrCount);
    std::iota(thrIDs.begin(), thrIDs.end(), 0);
    std::random_shuffle(thrIDs.begin(), thrIDs.end());

    // Now create the threads in a random order.
    std::vector<std::thread> thrList(thrCount);
    auto addId = [info](int id) { return info + std::to_string(id); };
    std::transform(thrIDs.begin(), thrIDs.end(), thrList.begin(),
                   [info](auto i) { return std::thread(printInOrder, i,
                                               info + std::to_string(i)); });
    // Wait for the threads to finish
    std::for_each(thrList.begin(), thrList.end(),
                  [](auto& thr){ thr.join(); });
}

/** A helper method to test the operation of the producer-consumer
    operations for question #2
*/
void testProdCon(int n) {
    // Create the shared variables between the 2 threads
    std::vector<int> q;
    std::mutex qMutex;
    std::condition_variable qCond;

    // Create the producer and consumer threads
    std::thread prod(producer, std::ref(q), std::ref(qMutex),
                     std::ref(qCond), n);
    std::thread cons(consumer, std::ref(q), std::ref(qMutex),
                     std::ref(qCond), n);

    // Wait for the threds to finish
    prod.join();
    cons.join();

    std::cout << "Done testing.\n";
}

/**
 * A simple method to test the operation of the countDigits
 * implementation.
 */
void testMT(const int n) {
    // Create a list of 'n' numbers.
    std::vector<int> values(n * 3000);
    std::iota(values.begin(), values.end(), 0);
    // Randomly shuffle them for testing.
    std::random_shuffle(values.begin(), values.end());
    
    // Call method under test
    std::vector<int> counts = countDigits(values, n);

    // Print the results for testing.
    for (auto i : counts) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
}

/**
 * A method for testing multithreaded operations.
 *
 * \param[in,out] i The variable whose value is to be incremented.
 */
void increment(int& i) {
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(1ms);
    i++;
}

/**
 * The main method that processes command-line arguments and calls
 * other helper methods to test the methods to be implemented.
 *
 * \param[in] argc The number of command-line arguments. This should
 * be 2.
 *
 * \param[in] argv The actual command-line arguments.
 */
int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Specify 2 arguments.\n"
                  << "  First: must be 'q1', 'q2', or 'q3'\n"
                  << "  Second: An integer n (n > 1)\n";
        return 1;  // Can't work without command-line arguments.
    }
    // Get the question to test.
    std::string qNum = argv[1];
    const int   n    = std::stoi(argv[2]);

    // Based on the question use tester method
    if (qNum == "q1") {
        testPrintOrder(n, "q1: ");
    } else if (qNum == "q2") {
        testProdCon(n);
    } else if (qNum == "q3") {
        testMT(n);
    }

    // All done.
    return 0;
}
