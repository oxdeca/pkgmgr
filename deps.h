/*********************************************************************
 * File    : deps.h
 * Version : 2018-03-26-01-WS
 * Created : 2018-03-26-01-WS
 * Author  : oxdeca.com
 *********************************************************************/

#ifndef _deps_h_

#define _deps_h_

#include <iostream>
#include <syslog.h>
#include <pcrecpp.h>
#include <vector>
#include <cerrno>

#define DEPS_DELIMITER            ":"
#define DEPS_REGEX_NAME           "[a-z][0-9a-z-]*"
#define DEPS_REGEX_VERSION        "[>]?[0-9a-z.-]+"
#define DEPS_REGEX_RELEASE        "[>]?[0-9]+"
#define DEPS_REGEX_TYPE           "(all|bin|dev|doc|cfg|web)"
#define DEPS_REGEX_BRANCH         "(MAIN|[0-9a-z+_-]+)"


using namespace std;
using namespace pcrecpp;

class Deps {
   public:
             Deps(string csv);
             Deps(string name, string version, string release, string type, string branch);

      bool   setName(string name);
      bool   setVersion(string version);
      bool   setRelease(string release);
      bool   setType(string type);
      bool   setBranch(string branch);

      string getName()    { return this->name;    }
      string getVersion() { return this->version; }
      string getRelease() { return this->release; }
      string getType()    { return this->type;    }
      string getBranch()  { return this->branch;  }

      bool   checkVersion(string version);
      bool   checkRelease(string release);

   private:
      string name;
      string version;
      string release;
      string type;
      string branch;
};

#endif
