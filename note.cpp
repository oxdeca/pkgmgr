/*********************************************************************
 * File    : note.cpp
 * Version : 2018-03-26-01-WS
 * Created : 2018-03-26-01-WS
 * Author  : oxdeca.com
 *********************************************************************/

#include "note.h"

/**
 *
 * Takes a string in the format of a comma seperated value (delimiter = ';')
 * The format is: date=$DATE;name=$NAME;user=$USER;text=$TEXT
 * e.g. date=2009-06-04;name=Tested by;user=wolfi;text=-
 *
 * @param csv
 */
Note::Note(string csv) {
   char           *token;
   char           *param;
   vector<string>  tokens;

   syslog(LOG_INFO, "Note - Note()");

   token = strtok((char *)csv.c_str(), NOTE_DELIMITER);
   while(token != NULL) {
      tokens.push_back(token);
      token = strtok(NULL, NOTE_DELIMITER);
   }

   for(vector<string>::iterator it = tokens.begin(); it < tokens.end(); ++it) {
      param = strtok((char *)(*it).c_str(), "=");

      if(strncmp(param, "date", sizeof("date")) == 0) {
         if (!setDate(strtok(NULL, "="))) {
            cout << "Could not set date: " << strerror(errno) << endl;
            syslog(LOG_ERR, "Note - Could not set date: %s", strerror(errno));
         }
      }
   
      if(strncmp(param, "name", sizeof("name")) == 0) {
         if(!setName(strtok(NULL, "="))) {
            cout << "Could not set name: " << strerror(errno) << endl;
            syslog(LOG_ERR, "Note - Could not set name: %s", strerror(errno));
         }
      }
 
      if(strncmp(param, "user", sizeof("user")) == 0) {
         if(!setUser(strtok(NULL,  "="))) {
            cout << "Could not set user: " << strerror(errno) << endl;
            syslog(LOG_ERR, "Note - Could not set user: %s", strerror(errno));
         }
      }
   
      if(strncmp(param, "text", sizeof("text")) == 0) {
         if(!setText(strtok(NULL, "="))) {
            cout << "Could not set text: " << strerror(errno) << endl;
            syslog(LOG_ERR, "Note - Could not set text: %s", strerror(errno));
         }
      }
   }
}


/**
 * @param date
 * @param name
 * @param user
 * @param text
 */
Note::Note(string date, string name, string user, string text) {
   if(!(setDate(date) && setName(name) && setUser(user) && setText(text))) {
      syslog(LOG_ERR, "Note - Could not instantiate Note Object");
      throw 20;
   }
}


/**
 * @param date 
 * @return true on success
 * @return false on error
 */
bool Note::setDate(string date) {
   if(RE(NOTE_REGEX_DATE).FullMatch(date)) {
      this->date = date;
   } else {
      syslog(LOG_ERR, "Note - date not correct!");
      return false;
   }
   
   return true;
}


/**
 * @param name 
 * @return true on success
 * @return false on error
 */
bool Note::setName(string name) {
   if(RE(NOTE_REGEX_NAME).FullMatch(name)) {
      this->name = name;
   } else {
      syslog(LOG_ERR, "Note - name not correct!");
      return false;
   }

   return true;
}


/**
 * @param user 
 * @return true on success
 * @return false on error
 */
bool Note::setUser(string user) {
   if(RE(NOTE_REGEX_USER).FullMatch(user)) {
      this->user = user;
   } else {
      syslog(LOG_ERR, "Note - user not correct!");
      return false;
   }

   return true;
}


/**
 * @param text 
 * @return true on success
 * @return false on error
 */
bool Note::setText(string text) {
   if(RE(NOTE_REGEX_TEXT).FullMatch(text)) {
      this->text = text;
   } else {
      syslog(LOG_ERR, "Note - text not correct!");
      return false;
   }

   return true;
}
