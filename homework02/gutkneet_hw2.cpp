/*
 * Copyright (C) Ethan Gutknecht
 */

#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using userIDMap = std::unordered_map<int, std::string>;


/**
 * This method is used to create a map with pairs of the uID and LoginID. I made
 * this method so I didn't have to keep referencing the file, Overall it should
 * make the program more efficent.
 * 
 * @return This returns a map (userIDMap std::unordered_map<int, std::string>) 
 * that will store all the users present in teh passwd.txt file.
 */
userIDMap createUserMap() {
    userIDMap returnMap;
    std::ifstream inFile("passwd.txt");
    std::string fileLine, loginID, uID;
    
    // loops through all users and creates a map of their uID and loginID
    while (std::getline(inFile, fileLine)) {
        std::replace(std::begin(fileLine), std::end(fileLine), ':', ' ');
        std::istringstream ss(fileLine);
        ss >> loginID >> uID >> uID;
        
        // inserts pair into map {2345, gutkneet}
        returnMap.insert({std::stoi(uID), loginID});
    }
    return returnMap;
}


/**
 * This method gets a list of uIDs and returns a list of login names with
 * the uIDs in brackets.
 * 
 * @param groupUserList This is a list of the uIDs of all the users within a
 * group. This function will be called from the outputGroup method when it
 * successfully finds the group name with the gID. Example:
 * "1000 1001 1003"
 * 
 * @param userMap This is a map that stores the ID of the user and the loginID
 *
 * @return This method returns an std::string that will list the users LoginID
 * and uID together. It will look similar to this:
 * "raodm(1000) campbest(1001) kiperjd(1003)"
 */
std::string getUserList(std::string groupUserList, userIDMap userMap) {
    // If there is no users within the list, return a blank string
    if (groupUserList == "") return "";
    
    // Local Variables and formatting string.
    std::string returnString = "";
    std::vector<std::string> groupUserVec;
    boost::split(groupUserVec, groupUserList, boost::is_any_of(","));
    
    for (int i = 0; i < groupUserVec.size(); i++) {
        returnString += " " + userMap.at(std::stoi(groupUserVec[i])) +
                "(" + groupUserVec[i] + ")";; 
    }
    
    return returnString;
}


/**
 *
 * @param gID This is the gID that will be searched for within the method. If 
 * found it will call a function called getUserList() to list all the users
 * within that group.
 *
 * @param userMap This is a map that stores the ID of the user and the loginID
 */
void outputGroup(int gID, userIDMap userMap) {
    std::string fileLine, word, lineGID, lineGroupID, listOfUsers;
    std::ifstream inFile("groups.txt");
    
    while (std::getline(inFile, fileLine)) {
        // Local Variables
        listOfUsers = "";
        std::replace(std::begin(fileLine), std::end(fileLine), ':', ' ');
        std::istringstream ss(fileLine);
        
        //    GroupID,       filler     GID,       user list associated. 
        ss >> lineGroupID >> lineGID >> lineGID >> listOfUsers;
        
        // If GIDs match, print output.
        if (std::stoi(lineGID) == gID) {
            std::cout << gID
                      << " = "
                      << lineGroupID
                      << ":"
                      << getUserList(listOfUsers, userMap)
                      << std::endl;
            // End method early since this groups output is complete
            return;
        }
    }
    // if group is not found, return failure to find statment
    std::cout << gID << " = Group not found." << std::endl;
}


int main(int argc, char** argv) {
    // Check number of arguments
    if (argc <= 1) {
        std::cout << "Specify more than one argument." << std::endl;
        return 1;
    }
    
    // Create and store map of users
    userIDMap userMap = createUserMap();
    
    // Loop through all the cmd arguments to outputGroups method
    for (int i = 1; i < argc; i++) {
        outputGroup(std::stoi(argv[i]), userMap);
    }
    return 0;
}
