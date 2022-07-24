/*
 * Copyright (C) Ethan Gutknecht 2021
 */


#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <functional>

// Forward declaration only used to help seperate the code logically into a
// section that should not be modified.
int countFactors(const long);


void threadMain(const std::vector<long>& numList, const size_t start, 
        const size_t end, std::vector<int>& factCounts) {
    for (size_t i = start; (i < end); i++) {
        factCounts[i] = countFactors(numList[i]);
    }
}

/** Method to count the factors for a given list of values.
 *
 * \param[in] numList The list of numbers whose factor counts are to
 * be computed.
 *
 * \param[in] thrCount The number of threads to be used. The starter
 * code ignores this parameter.
 *
 * \return The list of factor counts corresponding to each value in
 * numList.
 */
std::vector<int> 
getFactorCount(const std::vector<long>& numList, const int thrCount) {
    // First allocate the return vector
    std::vector<int> factCounts(numList.size());
    
    // Compute the range of values each thread should iterate over.
    const int count = (numList.size() + thrCount - 1) / thrCount;
    // Now spin-up threads to do the work.
    std::vector<std::thread> threadList;
    
    // This will iterate through all the threads
    for (int thr = 0; (thr < thrCount); thr++) {
        const size_t start = thr * count;
        const size_t end   = std::min(numList.size(), start + count);
        
        threadList.push_back(std::thread(threadMain, std::ref(numList),
                                      start, end, std::ref(factCounts)));
    }
    
    // Return the result back
    return factCounts;
}

//-------------------------------------------------------------
//  DO  NOT  MODIFY  CODE  BELOW  THIS  LINE
//-------------------------------------------------------------

/** Helper method to count the number of factors for a given number.
    This method is rather simple.  It iterates from 1..num and counts
    number of factors encountered.
    
    \param[in] num The number for which the factors need to be counted.

    \return The numebr of factors (including 1 and num) for the given
    number. For example, if num is 6, this method returns 4 (because
    factors of 6 are 1, 2, 3, 6).
*/
int countFactors(const long num) {
    int factorCount = 0;
    for (int long fact = 1; (fact <= num); fact++) {
        if (num % fact == 0) {
            factorCount++;  // Found a factor
        }
    }
    return factorCount;
}

/**
   Convenience method to load integers from a given file.

   \param[in] filePath The path to the file from where the numbers are
   to be loaded.

   \return The list of numbers loaded from the given data file.
*/
std::vector<long> loadData(const std::string& filePath) {
    // Load numbers from a given text file.
    std::ifstream dataFile(filePath);
    std::istream_iterator<long> readIter(dataFile), eof;
    std::vector<long> numberList(readIter, eof);
    return numberList;
}

/** A simple error message to reduce clutter in the source code. */
const std::string INSUFFICIENT_ARGS =
    "Error: Insufficient Arguments.\nThe program requires the following " \
    "arguments:\n    1. The file with numbers to be processed.\n" \
    "    2. The number of threads to be used for processing.\n" \
    "EXAMPLE: ./exercise8_part2 numbers.txt 3\n";


/**
 * The main method that fires off various threads and waits for them
 * to finish.
 *
 * @param args Optional command-line arguments. The first argument is
 * the number of threads. The second argument is the time delay.
 */
int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << INSUFFICIENT_ARGS;
        return 1;
    }
    // Load the data from the file.
    const std::vector<long> numberList = loadData(argv[1]);
    // Get factor counts for each number.
    const int numThreads = std::stoi(argv[2]);
    const std::vector<int> factCount = getFactorCount(numberList, numThreads);
    
    // Print the results
    for (size_t i = 0; (i < numberList.size()); i++) {
        std::cout << numberList[i] << ": " << factCount[i] << "\n";
    }
    return 0;
}
