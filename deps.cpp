/*********************************************************************
 * File    : deps.cpp
 * Version : 2018-03-26-01-WS
 * Created : 2018-03-26-01-WS
 * Author  : oxdeca.com
 *********************************************************************/

#include "deps.h"

/**
 *  das Format ist: name=$NAME:version=$VERSION:release=$RELEASE:type=$TYPE
 *  @param cvs
 */
Deps::Deps(string csv) {
   char          *token;
   char          *param;
   vector<string> tokens;

   syslog(LOG_INFO, "Deps - Deps()");

   token = strtok((char *)csv.c_str(), DEPS_DELIMITER);
   while(token != NULL) {
      tokens.push_back(token);
      token = strtok(NULL, DEPS_DELIMITER);
   }

   for(vector<string>::iterator it = tokens.begin(); it < tokens.end(); ++it) {
      param = strtok((char *)(*it).c_str(), "=");

      if(strncmp(param, "name", sizeof("name")) == 0) {
         if(!setName(strtok(NULL, "="))) {
            cout << "Could not set name: " << strerror(errno) << endl;
            syslog(LOG_ERR, "Deps - Could not set name: %s", strerror(errno));
         }
      }

      if(strncmp(param, "version", sizeof("version")) == 0) {
         if(!setVersion(strtok(NULL, "="))) {
            cout << "Could not set version: " << strerror(errno) << endl;
            syslog(LOG_ERR, "Deps - Could not set version: %s", strerror(errno));
         }
      }

      if(strncmp(param, "release", sizeof("release")) == 0) {
         if(!setRelease(strtok(NULL, "="))) {
            cout << "Could not set release: " << strerror(errno) << endl;
            syslog(LOG_ERR, "Deps - Could not set release: %s", strerror(errno));
         }
      }

      if(strncmp(param, "type", sizeof("type")) == 0) {
         if(!setType(strtok(NULL, "="))) {
            cout << "Could not set type: " << strerror(errno) << endl;
            syslog(LOG_ERR, "Deps - Could not set type: %s", strerror(errno));
         }
      }

      if(strncmp(param, "branch", sizeof("branch")) == 0) {
         if(!setBranch(strtok(NULL, "="))) {
            cout << "Could not set branch: " << strerror(errno) << endl;
            syslog(LOG_ERR, "Deps - Could not set branch: %s", strerror(errno));
         }
      }
   }
}

/**
 *  @param name
 *  @param version
 *  @param release
 *  @param type
 *  @param branch
 */
Deps::Deps(string name, string version, string release, string type, string branch) {
   if(!(setName(name) && setVersion(version) && setRelease(release) && setType(type) && setBranch(branch))) {
      cout << "Could not initialize Deps" << endl;
      syslog(LOG_ERR, "Deps - Could not initialize Deps");
   }
}

/**
 *  @param version
 *  @return true on success
 *  @return false on error
 */
bool Deps::checkVersion(string version) {
   switch(this->version.at(0)) {
      case '>':
         if(!(version >= this->version.substr(1, this->version.length() - 2))) {
            return false;
         }
         break;
      default:
         if(!(version == this->version)) {
            return false;
         } 
   } 
 
   return true;
}

/**
 *  @param release
 *  @return true on success
 *  @return false on error
 */
bool Deps::checkRelease(string release) {
   switch(this->release.at(0)) {
      case '>':
         if(!(release >= this->release.substr(1, this->release.length() - 2))) {
            return false;
         }
         break;
      default:
         if(!(release == this->release)) {
            return false;
         } 
   } 
 
   return true;
}


/**
 *  @param name
 *  @return true on success
 *  @return false on error
 */
bool Deps::setName(string name) {
 if(RE(DEPS_REGEX_NAME).FullMatch(name)) {
      this->name = name;
   } else {
      syslog(LOG_ERR, "Deps - name not correct!");
      return false;
   }

   return true;
}


/**
 *  @param version
 *  @return true on success
 *  @return false on error
 */
bool Deps::setVersion(string version) {
 if(RE(DEPS_REGEX_VERSION).FullMatch(version)) {
      this->version = version;
   } else {
      syslog(LOG_ERR, "Deps - version not correct!");
      return false;
   }

   return true;
}


/**
 *  @param release
 *  @return true on success
 *  @return false on error
 */
bool Deps::setRelease(string release) {
 if(RE(DEPS_REGEX_RELEASE).FullMatch(release)) {
      this->release = release;
   } else {
      syslog(LOG_ERR, "Deps - release not correct!");
      return false;
   }

   return true;
}


/**
 *  @param type
 *  @return true on success
 *  @return false on error
 */
bool Deps::setType(string type) {
 if(RE(DEPS_REGEX_TYPE).FullMatch(type)) {
      this->type = type;
   } else {
      syslog(LOG_ERR, "Deps - type not correct!");
      return false;
   }

   return true;
}


/**
 *  @param branch
 *  @return true on success
 *  @return false on error
 */
bool Deps::setBranch(string branch) {
 if(RE(DEPS_REGEX_BRANCH).FullMatch(branch)) {
      this->branch = branch;
   } else {
      syslog(LOG_ERR, "Deps - branch not correct!");
      return false;
   }

   return true;
}
