/* 
 * Copyright (C) Ethan Gutknecht 2021
 * 
 * A very lightweight (light as air) implementation of a simple CSV-based 
 * database system that uses SQL-like syntax for querying and updating the
 * CSV files.
 * 
 */

#include <string>
#include <fstream>
#include <tuple>
#include <algorithm>
#include "SQLAir.h"
#include "HTTPFile.h"

using namespace boost::asio;
using namespace boost::asio::ip;

/**
 * A fixed HTTP response header that is used by the runServer method below.
 * Note that this a constant (and not a global variable)
 */
const std::string HTTPRespHeader = "HTTP/1.1 200 OK\r\n"
        "Server: localhost\r\n"
        "Connection: Close\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: ";

// API method to perform operations associated with a "select" statement
// to print columns that match an optional condition.

void SQLAir::selectQuery(CSV& csv, bool mustWait, StrVec colNames,
        const int whereColIdx, const std::string& cond,
        const std::string& value, std::ostream& os) {
    int count = 0;
    // Convert any "*" to suitable column names. See CSV::getColumnNames()
    if (colNames.at(0) == "*") {
        colNames = csv.getColumnNames();
    }
    // First print the column names.
    os << colNames << std::endl;

    // Print each row that matches an optional condition.
    for (const auto& row : csv) {
        // Determine if this row matches "where" clause condition, if any
        // see SQLAirBase::matches() helper method.
        if (whereColIdx == -1 ||
                SQLAirBase::matches(row.at(whereColIdx), cond, value) == true) {
            std::string delim = "";
            for (const auto& colName : colNames) {
                os << delim << row.at(csv.getColumnIndex(colName));
                delim = "\t";
            }
            os << std::endl;
            count++;
        }
    }
    os << count << " row(s) selected.\n";
}

void
SQLAir::updateQuery(CSV& csv, bool mustWait, StrVec colNames, StrVec values,
        const int whereColIdx, const std::string& cond,
        const std::string& value, std::ostream& os) {
    // Update each row that matches an optional condition.
    throw Exp("update is not yet implemented.");
}

void
SQLAir::insertQuery(CSV& csv, bool mustWait, StrVec colNames,
        StrVec values, std::ostream& os) {
    throw Exp("insert is not yet implemented.");
}

void
SQLAir::deleteQuery(CSV& csv, bool mustWait, const int whereColIdx,
        const std::string& cond, const std::string& value, std::ostream& os) {
    throw Exp("delete is not yet implemented.");
}

void
SQLAir::serveClient(std::istream& is, std::ostream& os) {
    // Variables
    std::string line, path;
    std::ostringstream sqlOut;
    
    // get the urlpath
    is >> line >> path;

    // Skip over response headers
    while (std::getline(is, line) && (line) != "\r") {}
    
    std::string sql;
    
    if (path.find("/sql-air?query=") == 0) {
        sql = Helper::url_decode(path.substr(15));
    } else if (!path.empty()) {
        sql = Helper::url_decode(path.substr(1));
        try {
            // Process Method
            // This is in the try catch because 
            // it may throw an error.
            process(sql, sqlOut);
        }
        catch (const std::exception &exp) {
            // Processing data failed
            sqlOut << "Error: " << exp.what() << std::endl;
        }
    }
    // Print the headers << length << tab and new line << the output;
    os << HTTPRespHeader << sqlOut.str().length() <<
            "\r\n\r\n" << sqlOut.str();
    
}


// The method to have this class run as a web-server. 

void
SQLAir::runServer(boost::asio::ip::tcp::acceptor& server, const int maxThr) {
    using TcpStreamPtr = std::shared_ptr<tcp::iostream>;

    while (true) {
        // Create garbage-collected, shared object on heap so we can
        // send it to another thread and not worry about life-time of        
        // the socket connection.        
        TcpStreamPtr client = std::make_shared<tcp::iostream>();
        // Wait for a client to connect            
        server.accept(*client->rdbuf());
        // 3.
        // Include this capture clause in case decide to use a lambda for
        // creating
        // threads
        std::thread thr([this, client]() {
            serveClient(*client, *client); });
        thr.detach();
    }
    throw Exp("runServer method is not yet implemented.");
}

// Helper method to check exceptions

void
SQLAir::checkURLConnection(boost::asio::ip::tcp::iostream& data,
        std::string host,
        std::string port,
        std::string path) {
    // Variables
    std::string line;
    
    // This will check if they were even able to connect to the host
    if (!data.good()) {
        throw Exp("Unable to connect to " + host + " at port " + port);
    }
    
    
    data << "GET " << path << " HTTP/1.1\r\n"
            << "Host: " << host << "\r\n"
            << "Connection: Close\r\n\r\n";


    // Get first line
    std::getline(data, line);

    // Check if data exists and is good
    if (line.find("200 OK") == std::string::npos) {
        throw Exp("Error (HTTP/1.1 404 Not Found) getting " + path +
                " from " + host + " at port " + port);
    }

    // Skip over HTTP Headers
    for (std::string hdr; std::getline(data, hdr) && !hdr.empty()
            && hdr != "\r";) {
    }
}

CSV& SQLAir::loadAndGet(std::string fileOrURL) {
    // Check if the specified fileOrURL is already loaded in a thread-safe
    // manner to avoid race conditions on the unordered_map
    {
        std::lock_guard<std::mutex> guard(recentCSVMutex);
        // Use recent CSV if parameter was empty string.
        fileOrURL = (fileOrURL.empty() ? recentCSV : fileOrURL);
        // Update the most recently used CSV for the next round
        recentCSV = fileOrURL;
        if (inMemoryCSV.find(fileOrURL) != inMemoryCSV.end()) {
            // Requested CSV is already in memory. Just return it.
            return inMemoryCSV.at(fileOrURL);
        }
    }
    // When control drops here, we need to load the CSV into memory.
    // Loading or I/O is being done outside critical sections
    CSV csv;  // Load data into this csv
    if (fileOrURL.find("http://") == 0) {
        // This is an URL. We have to get the stream from a web-server
        // Implement this feature.
        std::string host, port, path, line;
        std::tie(host, port, path) = Helper::breakDownURL(fileOrURL);

        // Create data stream using URL
        boost::asio::ip::tcp::iostream data(host, port);

        // Call helper method to check connection
        SQLAir::checkURLConnection(data, host, port, path);

        csv.load(data);
    } else {
        // We assume it is a local file on the server. Load that file.
        std::ifstream data(fileOrURL);
        // This method may throw exceptions on errors.
        csv.load(data);
    }

    // We get to this line of code only if the above if-else to load the
    // CSV did not throw any exceptions. In this case we have a valid CSV
    // to add to our inMemoryCSV list. We need to do that in a thread-safe
    // manner.
    std::lock_guard<std::mutex> guard(recentCSVMutex);
    // Move (instead of copy) the CSV data into our in-memory CSVs
    inMemoryCSV[fileOrURL].move(csv);
    // Return a reference to the in-memory CSV (not temporary one)
    return inMemoryCSV.at(fileOrURL);
}

// Save the currently loaded CSV file to a local file.

void
SQLAir::saveQuery(std::ostream& os) {
    if (recentCSV.empty() || recentCSV.find("http://") == 0) {
        throw Exp("Saving CSV to an URL using POST is not implemented");
    }
    // Create a local file and have the CSV write itself.
    std::ofstream csvData(recentCSV);
    inMemoryCSV.at(recentCSV).save(csvData);
    os << recentCSV << " saved.\n";
}
