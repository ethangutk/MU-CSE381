/*
 * Ethan Gutknecht (C) Copyright 2021
 * 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   gutkneET_hw6.cpp
 * Author: Ethan Gutknecht
 *
 * Created on March 17, 2021, 8:02 PM
 */

#include <cstdlib>
#include <complex>
#include <iostream>
#include <vector>
#include <thread>
#include <string>
#include <math.h>

using namespace std;

/**
 * Given a number input, this will return a true/false value if the
 * number inputed is a prime number. (can't be factored out)
 * 
 * @param input Number to be checked if it's prime.
 * @return True/False confirming if it is or isn't prime
 */
bool checkIfPrime(unsigned long long int input) {
    for (unsigned long long int i = 2; i <= sqrt(input); i++) {
        if (input % i == 0) {
            return false;
        }
    }
    return true;
}

/**
 * Given a very large input, this method will return the largest factor
 * of the input that is given.
 * 
 * @param input A number (not prime) to find the output for.
 *              (Example: 36679527361)
 * @return The largest factor of the number input. (Example: 191519)
 */
unsigned long long int findLargestFactor(unsigned long long int input) {
    unsigned long long int sqrtValue = sqrt(input);
    
    for (unsigned long long int i = 2; i <= sqrtValue; i++) {
        if (input % i == 0) {
            return i;
        }
    }

    return 1;
}

/**
 * 
 * Given the inputs listed below, this method will take in a very large number 
 * and check if it is prime. If it is not prime it will find the largest factors
 * for that number to be divided by and say if the factors are prime. Example
 * strings of what will be put in the vector:
 * 
 * "1844674407370 = 2 (prime) * 922337203685\n"
 * "36679527361 = 191519 (prime) * 191519 (prime)\n"
 * "78167: Is already prime.\n"
 * 
 * 
 * @param input The number to do computations on (Example: 18446744073709551614)
 * @param index The index of what array position to add the result to
 *              (Example: 2)
 * @param vec   A reference to the string vector with the results
 *              stored inside of it.
 */
void performCommands(std::string input, int index, vector<std::string>& vec) {
    unsigned long long int inputint = std::stoull(input);
    std::string result = "";
    if (checkIfPrime(inputint) == true) {
        result += input + ": Is already prime.\n";
    } else {
        unsigned long long int lowestFactor  = findLargestFactor(inputint);
        unsigned long long int highestFactor = inputint / lowestFactor;
        
        result += input + " = " + std::to_string(lowestFactor) + " ";
        
        if (checkIfPrime(lowestFactor) == true) {
            result += "(prime)";
        }
        
        result +=  " * " + std::to_string(highestFactor);
        
        if (checkIfPrime(highestFactor) == true) {
            result += " (prime)";
        }
        result += "\n";
    }
    
    vec.at(index) = result;
}

/**
 * 
 * The main method will create the threads and call the helper methods
 * to perform the tasks at hand.
 * 
 * @param argc Number of inputs in the command line.
 * @param argv Array of Character Arrays for the command line arguments
 * @return Error code, if zero then no error.
 */
int main(int argc, char** argv) {    
    // Initialize variables
    vector<thread> threads;
    vector<std::string> resultsVector(argc - 1);
    
    // Create threads
    for (int i = 1; (i < argc); i++) {
        threads.push_back(std::thread(performCommands,
                argv[i], i - 1, std::ref(resultsVector)));
    }
    
    // Make sure all the threads are joined up
    for (auto&t : threads) {
        t.join();
    }
    
    // Print the results vector
    for (std::string str : resultsVector) {
        std::cout << str;
    }
    
    return 0;
}
