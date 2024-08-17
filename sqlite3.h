/*********************************************************************
 * File    : sqlite3.h
 * Version : 2018-03-26-01-WS
 * Created : 2018-03-26-01-WS
 * Author  : oxdeca.com
 *********************************************************************/

#ifndef _sqlite3_h
#define _sqlite3_h

#include <iostream>
#include <vector>
#include <syslog.h>
#include <sqlite3.h>

using namespace std;

typedef struct {
   vector<string> header;
   vector<string> data;
   int            rows;
   int            cols;
   string         error;
} sql_result;



class Sqlite3 {
   public:
      Sqlite3(string dbfile);
      ~Sqlite3();

      sql_result query(string query);
   private:
      sqlite3 *db;
};

#endif
