#include "server.h"
#include "structures.h"

void Server::createUsersTable(){
    std::string sql = 
    "CREATE TABLE IF NOT EXISTS USER("
    "USERNAME TEXT PRIMARY KEY NOT NULL, "
    "PASSWORD TEXT NOT NULL,"
    "STATUS INT NOT NULL)";
    char *messageError;
    if(sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError)!= SQLITE_OK){
        std::cerr<<"Error Create Table"<<std::endl;
        sqlite3_free(messageError);
    }
    else
        std::cout<<"Table created successfully"<<std::endl;
}

void Server::insertUserToDb(std::string username, std::string password) {
    std::lock_guard<std::mutex> lock(mutex);

    std::stringstream query;
    query << "INSERT INTO USER (USERNAME, PASSWORD, STATUS) VALUES ('"
          << username << "', '" << password << "', " << 1 << ")";

    char *messageError;
    if (sqlite3_exec(DB, query.str().c_str(), NULL, 0, &messageError) != SQLITE_OK) {
        std::cerr << "Error inserting user into database: " << messageError << std::endl;
        sqlite3_free(messageError);
    } else {
        std::cout << "User" <<username<<" inserted successfully" << std::endl;
    }
}

bool Server::checkUserInDb(std::string login){
    std::lock_guard<std::mutex> lock(mutex);
    std::stringstream query;
    query << "SELECT COUNT(*) FROM USER WHERE USERNAME='" << login << "'";

    sqlite3_stmt *statement;
    if (sqlite3_prepare_v2(DB, query.str().c_str(), -1, &statement, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(DB) << std::endl;
        return false;
    }

    if (sqlite3_step(statement) != SQLITE_ROW) {
        std::cerr << "Error executing SQL statement: " << sqlite3_errmsg(DB) << std::endl;
        sqlite3_finalize(statement);
        return false;
    }

    int userCount = sqlite3_column_int(statement, 0);

    sqlite3_finalize(statement);

    if (userCount > 0) return true;
    else return false;
}
