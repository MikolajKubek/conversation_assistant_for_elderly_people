#include "interaction_db.hpp"
#include <iostream>
#include <sqlite3.h>

int test_db_connection() {
    sqlite3* DB;

    std::string create_statement =
        "CREATE TABLE IF NOT EXISTS \"INTERACTIONS\" ("
            "\"ID\"         INTEGER NOT NULL,"
            "\"TIMESTAMP\"  DATETIME DEFAULT CURRENT_TIMESTAMP,"
            "\"SENDER\"     TEXT,"
            "\"VALUE\"      TEXT,"
            "PRIMARY KEY(\"ID\" AUTOINCREMENT)"
        ")";

    int exit = 0;
    char* message_error;

    exit = sqlite3_open("interaction_history.db", &DB);
    if (exit != SQLITE_OK) {
        std::cerr << "Error opening the DB " << sqlite3_errmsg(DB) << std::endl;
        return -1;
    }
    else {
        std::cout << "Succesfully opened the database" << std::endl;
    }

    exit = sqlite3_exec(DB, create_statement.c_str(), NULL, 0, &message_error);
    if (exit != SQLITE_OK) {
        std::cerr << "Error creating table " << message_error << std::endl;
        sqlite3_free(message_error);
    }
    else {
        std::cout << "Table created successfully" << std::endl;
    }

    std::string select_query("SELECT * FROM INTERACTIONS");


    sqlite3_close(DB);
    return 0;
}
