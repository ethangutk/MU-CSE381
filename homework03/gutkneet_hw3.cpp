/* 
 *  Copyright (C) 2021 Ethan Gutknecht
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <bitset>
#include <vector>
#include "gutkneet_hw3.h"

// The primary interface method that is invoked once to
// perform the necessary functionality.

std::string
hw3::translatePerm(std::string permissionsNum) {
    // creates return string
    std::string returnString = "";
    // Creates a translation guide that will be used.
    std::unordered_map<char, std::string> translationMap = {
        {'0', "---"},
        {'1', "--x"},
        {'4', "r--"},
        {'5', "r-x"},
        {'7', "rwx"}
    };

    // loops through every number and translates it.
    for (size_t i = 0; i < 3; i++) {
        returnString += translationMap.at(permissionsNum.at(i));
    }

    // returns translation
    return returnString;
}

void
hw3::print(std::string perms, int uID, int gID, std::string filePath) {
    std::cout
            << perms
            << " " << std::setw(8) << std::left << uidName.at(uID)
            << " " << std::setw(8) << std::left << gidName.at(gID)
            << " " << filePath
            << '\n';
}

int
hw3::getuID(std::string login) {
    // Gets and returns the uID from the login parameter
    for (const auto& v : uidName) {
        if (v.second == login) {
            return v.first;
        }
    }
    return 0;
}

std::vector<int>
hw3::getusrGIDs(int uID) {
    // Find gIDs where user is apart of
    // Loops through all groups, if finds uID in the member list
    // Add it to vector
    std::vector<int> gIDswUser;

    // loop through gidUsers and list all things in group
    // first -> gID
    // second -> userList
    for (const auto& v : gidUsers) {
        if (v.second.find(std::to_string(uID)) != std::string::npos) {
            gIDswUser.push_back(v.first);
        }
    }
    return gIDswUser;
}

void
hw3::listU(std::string directPath, std::string login) {
    // Variables
    std::string line, filePerms, path;
    int fileUID, fileGID;
    std::ifstream inFile(directPath);

    // Loops though every line printing what is needed
    while (std::getline(inFile, line)) {
        std::istringstream ss(line);

        ss >> filePerms >> fileUID >> fileGID >> path;

        // checks if (login is root) OR (login is uID AND they have permission)
        if (login == "root" ||
                (login == uidName.at(fileUID) &&
                filePerms.at(0) != '0')) {
            // Calls print method 
            print(translatePerm(filePerms),
                    fileUID,
                    fileGID,
                    path);
        }
    }
}

void
hw3::listG(std::string directPath, std::string login) {
    // Variables
    std::string line, filePerms, path;
    int fileUID, fileGID, uID = getuID(login);
    std::ifstream inFile(directPath);
    std::vector<int> gIDswUser = getusrGIDs(uID);

    // Loops though every line printing what is needed
    while (std::getline(inFile, line)) {
        std::istringstream(line) >> filePerms >> fileUID >> fileGID >> path;

        bool inGroup = (std::find(gIDswUser.begin(), gIDswUser.end(), fileGID)
                != gIDswUser.end());

        // checks if (group has permission)
        // checks if (if the user itself has permission)
        // checks if (others have permission)
        if (((filePerms.at(1) != '0' || filePerms.at(2) != '0') && inGroup) ||
                (login == uidName.at(fileUID) && filePerms.at(0) != '0') ||
                filePerms.at(2) != '0') {
            // Calls print method 
            print(translatePerm(filePerms),
                    fileUID,
                    fileGID,
                    path);
        }
    }
}

void
hw3::listR(std::string directPath, std::string login) {
    // Variables
    std::string line, filePerms, path;
    int fileUID, fileGID, uID = getuID(login);
    std::ifstream inFile(directPath);
    std::vector<int> gIDswUser = getusrGIDs(uID);

    // Loops though every line printing what is needed
    while (std::getline(inFile, line)) {
        std::istringstream(line) >> filePerms >> fileUID >> fileGID >> path;

        bool inGroup = (std::find(gIDswUser.begin(), gIDswUser.end(), fileGID)
                != gIDswUser.end());

        // checks if (group has permission)
        // checks if (if the user itself has permission)
        // checks if (others have permission)
        if (((filePerms.at(1) != '0' || filePerms.at(2) != '0') && inGroup) ||
                (login == uidName.at(fileUID) && filePerms.at(0) != '0') ||
                filePerms.at(2) != '0') {
            // If it is home or lib directory
            // OR it contains the user within the directory
            if (path.find("/" + login) != std::string::npos ||
                    path == "/home" || path == "/lib") {
                print(translatePerm(filePerms),
                        fileUID,
                        fileGID,
                        path);
            }
        }
    }
}

void
hw3::listFiles(const std::string& dirFilePath, const std::string& login,
        const std::string& permType) {
    // Creates needed maps
    loadUserIDs();
    loadGroupInfo();

    // Decides what to call based on input (r, g, u)
    if (permType.at(0) == 'r') {
        listR(dirFilePath, login);
    } else if (permType.at(0) == 'g') {
        listG(dirFilePath, login);
    } else if (permType.at(0) == 'u') {
        listU(dirFilePath, login);
    }
}


//---------------------------------------------------------------
//  DO  NOT   MODIFY   CODE  BELOW  THIS LINE
//---------------------------------------------------------------

// Load userIDs and login IDs from passwd.txt file into the uidName
// unordered map. This method is essentially a copy-paste from an
// earlier homework's solution.

void
hw3::loadUserIDs(const std::string& passFile) {
    std::ifstream passwd(passFile);
    // Process line-by-line from the file.
    std::string line;
    while (std::getline(passwd, line)) {
        std::string login, dummy;
        int uid;
        // Replace ':' with space because space is one of the default 
        // delimiter used by istringstream to parse strings.
        std::replace(line.begin(), line.end(), ':', ' ');
        // Now extract the values we need.  Simple.
        std::istringstream(line) >> login >> dummy >> uid;
        // Add entry to the instance variable unordered map
        uidName[uid] = login;
    }
}

// Load user info into gidName and gidUsers map (instance variables)
// This method is essentially a copy-paste from an earlier homework's solution.

void
hw3::loadGroupInfo(const std::string& groupFile) {
    // File stream to read line-by-line from groups file
    std::ifstream group(groupFile);
    // Process line-by-line from the file.
    std::string line;
    while (std::getline(group, line)) {
        // Variables to be read below.
        std::string grpName, memberList, dummy;
        int gid = 0;
        // Extract the group name, gid, and member list after replacing
        // ':' with space because space is one of the default delimiters used
        // by istringstream to parse strings.
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream(line) >> grpName >> dummy >> gid >> memberList;
        // Add a comma to ease finding userID later on.
        memberList += ',';
        // Store the group membership information in unordered map to
        // return back.
        gidName[gid] = grpName;  // Group name
        gidUsers[gid] = memberList;  // Group member user IDs
    }
}

/**
 * The main method extracts the command-line arguments and invokes
 * the listFiles primary-method to perform the necessary processing.
 * 
 * @param argc The number of command-line arguments. 
 * 
 * @param argv The actual command-line arguments. The list of 
 * command-line arguments are assumed to be in the following order:
 *     1. First one is the file containing directory listing to process
 *     2. The login ID of the user. This can be empty string.
 *     3. The permission to apply. This can be "u", "g", or "r".
 * @return This method always returns zero.
 */
int main(int argc, char *argv[]) {
    // Extract the command-line arguments supplied
    const std::string dirFile = (argc > 1 ? argv[1] : "files.txt");
    const std::string login = (argc > 2 ? argv[2] : "raodm");
    const std::string permType = (argc > 3 ? argv[3] : "u");
    // Have the class method perform the necessary operations
    hw3 ls;
    ls.listFiles(dirFile, login, permType);
    return 0;
}
