/* 
 * Copyright (C) 2021 Ethan Gutknecht
 */

#ifndef HW3_H
#define HW3_H

#include <unordered_map>
#include <string>
#include <vector>

// Use an unordered map to look-up the user-id's and group-id
using IntStrMap = std::unordered_map<int, std::string>;

/**
 * A class to encapsulate methods and instance variables to streamline 
 * implementation. This class has only 1 primary interface method,
 * namely the listFiles() method. This method lists files for a given
 * user while checking for user, group, other permissions for the file
 * and its parents in the directory structure.
 */
class hw3 {
public:
    /**
     * The main interface method in this class. This method coordinates
     * all of the activities in this class. 
     * 
     * @param dirFilePath The directory path from where the list of files
     * are to be read and processed.
     * 
     * @param login An optional login ID of the user whose files are to be
     * listed. If this parameter is an empty string, then all the files
     * are listed without performing any entitlements check.
     * 
     * @param permType The type of permission check to be performed. This
     * parameter can be be one of the following strings:
     *     "u": Check only user's permissions to list files.
     *     "g": Check user & group permissions to list files.
     *     "r": Check user, group, and all parent path's permissions.
     */
    void listFiles(const std::string& dirFilePath, const std::string& login,
            const std::string& permType);
    
    /**
     * Translates the files permissions contents and changes it to the
     * formatted output for our program.
     * 
     * @param permissionsNum - This is what would be the first thing in the 
     * files1.txt line. For example: an input would be 750 or 755.
     * 
     * @return This will take the input and output the translation of the
     * number so it can be outputted. For example with teh input of 750,
     * there would be an output of "rwxr-x---" since that is the translation
     */
    std::string translatePerm(std::string permissionsNum);
    
    /**
     * Prints out the information with given parameters
     * @param perms - Permissions of given file
     * 
     * @param uID - of user that has the permissions
     * 
     * @param gID - groupID that has the permissions
     * 
     * @param filePath - path of the file in question
     */
    void print(std::string perms, int uID, int gID, std::string filePath);
    
    int getuID(std::string login);
    
    std::vector<int> getusrGIDs(int uID);
    
    /**
     * This will do the logic and decide when to call the print method
     * for the user permissions method
     * 
     * @param directPath Path of the file to open
     * 
     * @param login user of the person that will be used to test perms
     */
    void listU(std::string directPath, std::string login);
    
    /**
     * This will do the logic and decide when to call the print method
     * for the group permissions method
     * 
     * @param directPath Path of the file to open
     * 
     * @param login user of the person that will be used to test perms
     */
    void listG(std::string directPath, std::string login);
    
    /**
     * This will do the logic and decide when to call the print method
     * for the group permission method but only list files that the user has
     * accesss to.
     * 
     * @param directPath Path of the file to open
     * 
     * @param login user of the person that will be used to test perms
     */
    void listR(std::string directPath, std::string login);
    
protected:
    /** Convenience method to load user-id and login-id from a given
     * password file into the uidName instance variable. This method
     * is essentially a copy of an earlier homework's solution.
     * 
     * \param[in] passFile Path to the file with user information.
     */
    void loadUserIDs(const std::string& passFile = "passwd.txt");

    /**
     * Convenience method to load the group information from a given
     * group file into the gidName and gidUsers maps to ease 
     * processing.  This method is essentially a copy of an earlier 
     * homework's solution.
     * 
     * \param[in] groupFile Path to the file with group information.
     */
    void loadGroupInfo(const std::string& groupFile = "groups.txt");

private:
    /**
     * The unordered map containing user-id and login name.
     * The entries in this map are populated using the data in the 
     * passwd.txt file. 
     * 
     * Example: {{0, "root"}, {1000, "raodm"}, ...}
     */
    IntStrMap uidName;

    /**
     * The unordered map containing gid and group-names. The entries
     * in this map are populated using the data in the groups.txt file. 
     * Example: {{2, "faculty"}, {5, "admin"}, {6, "theory"}, ...};
     */
    IntStrMap gidName;

    /**
     * The unordered map containing gid and the list of userID's
     * in the group.  The entries in this map are populated using 
     * the data in the groups.txt file. 
     * 
     * Example: {{5, "1001,1002,0"}, {6, "2001,1500,1004,2002", ...}}
     */
    IntStrMap gidUsers;
};

#endif /* HW3_H */

