/*********************************************************************
 * File    : note.h
 * Version : 2018-03-26-01-WS
 * Created : 2018-03-26-01-WS
 * Author  : oxdeca.com
 *********************************************************************/

#ifndef _note_h_

#define _note_h_

#include <iostream>
#include <cerrno>
#include <vector>
#include <syslog.h>
#include <pcrecpp.h>

#define NOTE_REGEX_DATE      "[0-9]{4}-([1-9]|0[1-9]|1[0-2])-([1-9]|0[1-9]|[1-2][0-9]|3[0-1])"
#define NOTE_REGEX_NAME      "[a-zA-Z ]+"
#define NOTE_REGEX_USER      "[a-zA-Z]+"
#define NOTE_REGEX_TEXT      "[0-9a-zA-Z._, -]+"
 
#define NOTE_DELIMITER       ";"


using namespace std;
using namespace pcrecpp;

class Note {
   public:
      Note(string csv);
      Note(string date, string name, string user, string text);
      string getDate() { return this->date; }
      string getName() { return this->name; }
      string getUser() { return this->user; }
      string getText() { return this->text; }

   private:
      bool   setDate(string date);
      bool   setName(string name);
      bool   setUser(string user);
      bool   setText(string text);

      string date;
      string name;
      string user;
      string text;
};

#endif
