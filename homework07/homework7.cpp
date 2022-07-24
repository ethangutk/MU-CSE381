/**
 * Copyright (C) Ethan Gutknecht 2021
 * 
 * A program to use multiple threads to spell check words from a given 
 * text file.
 *
 */

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <thread>
#include <cmath>
#include <unordered_map>


// Shortcut/synonym for a vector of strings
using StrVec = std::vector<std::string>;

// Forward dec
StrVec loadWords(const std::string& filePath);

// The global dictionary of valid words
const StrVec dictionary = loadWords("dictionary.txt");

/**
 * This will load the words from the specifed file to a string vector
 * 
 * @param filePath String of what the file's path and name is to open
 * @return StrVec of where its going to be stored.
 */
StrVec loadWords(const std::string& filePath) {
    // Create variables 
    std::string word;
    std::ifstream wordList(filePath);
    StrVec loadedWords;
    
    // Create a vector of words using the file input
    while (wordList >> word) {
        loadedWords.push_back(word);
    }
    
    // Return vector
    return loadedWords;
}

/**
 * This will use an algorithm to see how far a word is off from 
 * the correct spelling of it.
 * 
 * !!!!!----- LOGIC COPIED AND TRANSLATED FROM WIKIPEDIA -----!!!!!
 * 
 * @param s Word to be checked
 * @param t Word to be compaired to
 * @return Integer
 */
int levenshteinDistance(std::string s, std::string t) {
    int m = s.size();
    int n = t.size();

    // Declare vector D
    std::vector<std::vector<int>> d(m + 1, std::vector<int>(n + 1));
    int substitutionCost;
    
    for (int i = 1 ; i <= m ; i++) {
        d[i][0] = i;
    }
    
    for (int j = 1 ; j <= n ; j++) {
        d[0][j] = j;
    }

    for (int j = 1; j <= n; j++) {
        for (int i = 1; i <= m; i++) {
            if (s[i - 1] == t[j - 1]) {
                substitutionCost = 0;
            } else {
                substitutionCost = 1;
            }

            // deletion
            d[i][j] = std::min({d[i - 1][j] + 1, 
                // insertion
                    d[i][j - 1] + 1, 
                    // substitution
                d[i - 1][j - 1] + substitutionCost}); 
        }
    }
    return d[m][n];
}

/**
 * This method will check for typos for the parameter using the 
 * levenshteinDistance method.
 * 
 * @param word = String to be checked for typos.
 * @return String to be added to the results vector in the threadMain method
 */
std::string getResult(std::string word) {
    // String result
    std::string result = word + " -- ";
    
    // Change the word to lowercase
    std::transform(word.begin(), word.end(), word.begin(), ::tolower);

    // Check if the word exists within the dictionary 
    bool valid = std::binary_search(dictionary.begin(), dictionary.end(), word);

    if (valid == true) {
        // No suggestion is needed, word is spelled right. Thus print.
        result = result + "0:" + word;

    } else {
        // Word is spelled wrong and needs a suggestion
        // Create suggestions vector
        StrVec sugg;
        
        for (size_t i = 0; i < dictionary.size(); i++) {
            // Find Levinshtien-distance using method call
            int distance = levenshteinDistance(word, dictionary.at(i));
            
            // If it is greater than four then
            if (distance < 4) {
                // Store Levinshtien-distance + “:” + suggestion
                sugg.push_back(std::to_string(distance)
                + ":" + dictionary.at(i) + ", ");
            }
            
            // If the suggestions vector has 5 elements, break the loop
            // Limit reached
            if (sugg.size() >= 5) {
                break;
            }        
            // Example: “3:correction”, where 3 is Levinshtien-distance
            // and “correction” is the suggested word)
        } 
        // Sort Vector
        std::sort(sugg.begin(), sugg.end());
        
        // Format return;
        for (size_t i = 0; i < sugg.size(); i++) {
            result = result + sugg.at(i);
        }
        
        // Removes the comma at the end of the string.
        if (sugg.size() > 0) result = result.substr(0, result.size()-2);      
    }
    
    return result;        
}

/**
 * Call all the methods for each of the threads using start
 * index and end index.
 *
 */
/**
 * This will call the getResults method. Depending on how many threads 
 * are used, this method will run that number of times. The start and end 
 * index variables help keep the parallel thing organized.
 * 
 * @param wordList REFERENCE of words stored in vector to check 
 * @param startIdx int at which the index will start at in the loop
 * @param endIdx int at which the index will end at in the loop
 * @param results REFERENCE to the results vector that will get printed later
 */
void threadMain(const StrVec& wordList, const size_t startIdx,
                const size_t endIdx, StrVec& results) {
    results.resize(wordList.size());
    for (size_t i = startIdx; (i < endIdx); i++) {
        results[i] = getResult(wordList[i]);
    }
}

/**
 * Create a thread vector with the inputs of how many threads the user
 * specified in the command line.
 * 
 * @param wordList = vector of words given from the user's input file
 * @param threadCount = how many vectors the user specified in the command line
 * @param results = where the results are stored.
 */
void createThreads(const StrVec& wordList, const int threadCount, 
        StrVec& results) {    
    // Compute the range of values each thread should iterate over.
    const int range = (wordList.size() + threadCount - 1) / threadCount;
    
    // Now spin-up threads to do the work.
    std::vector<std::thread> threadList;
    for (int threadNum = 0; (threadNum < threadCount); threadNum++) {
        const size_t start = threadNum * range;
        const size_t end   = std::min(wordList.size(), start + range);
        
        threadList.push_back(std::thread(threadMain, std::ref(wordList),
                                      start, end, std::ref(results)));
    }
    
    threadList.resize(threadCount);
    
    // Wait for all of the threads to finish.
    for (auto& t : threadList) {
        t.join();
    }
}

/** MAIN METHOD
 * 
 * @param argc Number of command line arguments, we assume that it will
 * always be three.
 * 
 * @param argv First command line argument will be the text file to spell check
 * and the second command line argument will be the number of threads used.
 * @return Exit code: non-zero if there is an error
 */
int main(int argc, char** argv) {
    // Create variables
    StrVec resultVec;
    StrVec wordList = loadWords(argv[1]); 
    int threadCount = std::stoi(argv[2]);
    
    // Call create threads method
    createThreads(wordList, threadCount, resultVec);
    
    
    // Print the results vector
    for (size_t i = 0; i < wordList.size(); i++) {
        std::cout << resultVec[i] << "\n";
    }
}
