/*********************************************************************
 * File    : pkgmgr.cpp
 * Version : 2018-03-26-01-WS
 * Created : 2018-03-26-01-WS
 * Author  : oxdeca.com
 *********************************************************************/

#include "package.h"
#include "report.h"

enum Mode   { INSTALL, UPDATE, REMOVE, LIST_DETAILS, LIST_HISTORY, HELP, DO_NOTHING };

using namespace pcrecpp;

// forward declaration
void usage();

int main(int argc, char *argv[]) {
   string     line;
   string     path;
   string     pkg;
   string     prefix;
   string     name;
   string     branch;
   string     type;
   string     admin;
   string     format;
   string     null;
   int        c;
   bool       force  = false;
   Mode       mode   = DO_NOTHING;
   sql_result result;
   Package   *package;
   Report    *report;

   syslog(LOG_INFO, "-------------------- Logfile opened --------------------");
   syslog(LOG_INFO, "$Id: pkgmgr.cpp,v 1.16 2018/03/01 10:27:44 Exp $");

   // check if current user is root
   if(getuid() != 0) {
      cout << RED("ERROR: ") << "You have to be root to execute this program!" << endl;
      syslog(LOG_INFO, "-------------------- Logfile closed --------------------");
      exit(1);
   } 

   while((c = getopt(argc, argv, "a:i:r:p:fl::s::m:h")) != -1) {
      switch(c) {
         case 'a':
            admin = optarg;
            break;
         case 'i':
            path = optarg;
            mode = INSTALL;
            break;

         case 'r':
            pkg  = optarg;
            mode = REMOVE;
            break;

         case 'p':
            prefix = optarg;
            break;

         case 'f':
            force = true;
            break;

         case 'l':
            if(optarg == NULL) {
               pkg = "all";
            } else {
               pkg  = optarg;
            }
            if(mode != LIST_DETAILS) {
               mode = LIST_HISTORY;
            } else {
               usage();
            }
            break;

         case 's':
            if(optarg == NULL) {
               pkg = "all";
            } else {
               pkg  = optarg;
            }
            if(mode != LIST_HISTORY) {
               mode = LIST_DETAILS;
            } else {
               usage();
            }
            break;

         case 'm':
            if(RE(REPORT_REGEX_FORMAT).FullMatch(optarg)) {
               format = optarg;
            }
            break;

         case 'h':
            mode = HELP;
            break;

         default:
            usage();
            break;
      }
   }

   // check if admin was given
   if((mode == INSTALL || mode == REMOVE) && admin.empty()) {
      cout << RED("ERROR: ") << "No admin given!" << endl;
      syslog(LOG_INFO, "-------------------- Logfile closed --------------------");
      exit(1);
   }

   switch(mode) {
      case INSTALL:
         try{
            package = new Package(path);
            if(!prefix.empty()) {
               package->setPrefix(prefix);
            }

            if(!package->setAdmin(admin)) {
               cout << RED("ERROR: ") << package->getError() << endl;
               exit(1);
            }

            if(package->install(force)) {
               cout << GREEN("OK: ") << "Package installed successfully." << endl;
            } else {
               cout << RED("ERROR: ") << package->getError() << endl;
            }

            delete(package);
         } catch(PackageException e) {
            cout << RED("ERROR: ") << e.getMessage() << endl;
         }
         break;

      case UPDATE:
         break;

      case REMOVE:
         // check the packagename
         if(!RE(PACKAGE_REGEX_PACKAGE).FullMatch(pkg, &name, &null, &branch, &null, &type)) {
            cout << RED("ERROR: ") << "Package-Format unsupported!" << endl;
            syslog(LOG_ERR, "Package-Format unsupported!");
            break;
         }

         try {
            package = new Package(name, type, branch);

            if(!package->setAdmin(admin)) {
               cout << RED("ERROR: ") << package->getError() << endl;
               exit(1);
            }

            if(package->remove(force)) {
               cout << GREEN("OK: ") << "Package removed successfully." << endl;
            } else {
               cout << RED("ERROR: ") << package->getError() << endl;
            }
            delete(package);
         } catch(PackageException e) {
            cout << RED("ERROR: ") << e.getMessage() << endl;
         }
         break;

      case LIST_DETAILS:
         report = new Report(format, pkg);
         cout << report->getPackageDetails();
         delete(report);
         break;

      case LIST_HISTORY:
         report = new Report(format, pkg);
         cout << report->getPackageHistory();
         delete(report);
         break;

      case HELP:
         usage();
         break;

      default:
         usage();
   }

   syslog(LOG_INFO, "-------------------- Logfile closed --------------------");

   return 0;
}


void usage() {
   cout << "Usage: pkgmgr [options...] {<packagefile>|<package>}"                      << endl;
   cout << "Options:"                                                                  << endl;
   cout << "-a <admin>          sets the Admin that installs/removes the package"       << endl;
   cout << "-f                  forces install/remove"                                  << endl;
   cout << "-p <prefix>         sets the prefix, where the package should be installed" << endl;
   cout << "-i <packagefile>    installs packagefile"                                   << endl;
   cout << "-l [<package>|all]  lists history of installed package(s)"                  << endl;
   cout << "-s [<package>|all]  lists details of installed package(s)"                  << endl;
   cout << "-r <package>        removes installed package"                              << endl;
   cout << "-m <format>         sets the format of the lists (-l and -s)"               << endl;
   cout << "                    supported formats: xml, wiki, text"                     << endl;
   cout << "-h                  show help"                                              << endl;
}

