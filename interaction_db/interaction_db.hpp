#ifndef INTERACTION_DB_HPP
#define INTERACTION_DB_HPP

#include <cstdlib>
#include <iostream>
#include <sqlite3.h>
#include <string>

class InteractionDb {
public:
  InteractionDb() {
    int exit = 0;

    exit = sqlite3_open("interaction_history.db", &DB);
    if (exit != SQLITE_OK) {
      std::cerr << "Error opening the DB " << sqlite3_errmsg(DB) << std::endl;
      throw 678;
    } else {
      std::cout << "Succesfully opened the database" << std::endl;
    }

    char *message_error;
    std::string create_statement =
        "CREATE TABLE IF NOT EXISTS \"INTERACTIONS\" ("
        "\"ID\"         INTEGER NOT NULL,"
        "\"TIMESTAMP\"  DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "\"SENDER\"     TEXT,"
        "\"VALUE\"      TEXT,"
        "PRIMARY KEY(\"ID\" AUTOINCREMENT)"
        ")";
    exit = sqlite3_exec(DB, create_statement.c_str(), NULL, 0, &message_error);
    if (exit != SQLITE_OK) {
      std::cerr << "Error creating table " << message_error << std::endl;
      sqlite3_free(message_error);
      throw 890;
    } else {
      std::cout << "Table created succesfully" << std::endl;
    }
  };
  ~InteractionDb() { sqlite3_close(DB); };

  void print_current_state() {
    std::string select_query("SELECT * FROM INTERACTIONS");
    sqlite3_exec(DB, select_query.c_str(), callback, NULL, NULL);
  }

  int insert(std::string sender, std::string value) {
    int exit;
    char *message_error;
    std::string sql("INSERT INTO INTERACTIONS(SENDER, VALUE) VALUES(\"" +
                    sender + "\", \"" + value + "\");");
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &message_error);

    if (exit != SQLITE_OK) {
      std::cerr << "Problem encoundered on insert " << message_error
                << std::endl;
      sqlite3_free(message_error);

      return EXIT_FAILURE;
    } else {
      std::cout << "Succesfully inserted data into the table" << std::endl;
    }

    return EXIT_SUCCESS;
  }

private:
  sqlite3 *DB;
  static int callback(void *data, int argc, char **argv, char **azColName) {
    int i;
    fprintf(stderr, "%s: ", (const char *)data);

    for (i = 0; i < argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    printf("\n");

    return EXIT_SUCCESS;
  }
};
#endif // INTERACTION_DB_HPP
