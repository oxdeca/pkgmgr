/*********************************************************************
 * File    : sqlite3.cpp
 * Version : 2018-03-26-01-WS
 * Created : 2018-03-26-01-WS
 * Author  : oxdeca.com
 *********************************************************************/

#include "sqlite3.h"

/**
 * Constructor
 */
Sqlite3::Sqlite3(string dbfile) {
   int rc;
   if((rc = sqlite3_open(dbfile.c_str(), &this->db)) != SQLITE_OK) {
      syslog(LOG_ERR, "Sqlite3 - Could not open database: %s", sqlite3_errmsg(db));
      sqlite3_close(db);
   }
}

/**
 *
 */
sql_result Sqlite3::query(string query) {
   char       **res;
   char        *error;
   int          rows;
   int          cols;
   sql_result   result;

   if(sqlite3_get_table(this->db, query.c_str(), &res, &rows, &cols, &error) == SQLITE_OK) {
      result.rows = rows;
      result.cols = cols;

      for(int i = 0; i < cols; i++) {
         result.header.push_back(res[i]);
      }

      for(int i = cols; i < (rows + 1) * cols; i++) {
         // data can have NULL-Fields, so check for them
         if(res[i] == NULL) {
            result.data.push_back("NULL");
         } else {
            result.data.push_back(res[i]);
         }
      }
   } else {
      result.rows = -1;
      result.cols = -1;
      result.error = sqlite3_errmsg(this->db);
   }
  
   sqlite3_free_table(res);

   return result;
}

/**
 * Destructor
 */
Sqlite3::~Sqlite3(){
   sqlite3_close(db);
} 
