/*********************************************************************
 * File    : report.h
 * Version : 2018-03-26-01-WS
 * Created : 2018-03-26-01-WS
 * Author  : oxdeca.com
 *********************************************************************/

#ifndef _report_h_

#define _report_h_

#include <iostream>
#include <syslog.h>
#include "package.h"
#include "deps.h"

enum Format { XML, WIKI, TEXT };

#define GET_PACKAGES_DETAILS(host)        "SELECT '" + host + "' as host, date(packages.installed), packages.name, packages.version, packages.release, packages.type, packages.branch, packages.admin, date(test.date) as testdate, test.name as testname, test.user as testuser, test.text as testtext, date(approval.date) as approvaldate, approval.name as approvalname, approval.user as approvaluser, approval.text as approvaltext, packages.changes, packages.backout, packages.impact, packages.author, packages.owner, packages.prefix, packages.desc, packages.url, packages.files, packages.preremove, packages.postremove FROM packages, test, approval WHERE packages.id = test.pkgid and packages.id = approval.pkgid order by date(packages.installed) desc;"

#define GET_PACKAGE_DETAILS(host, pkg)    "SELECT '" + host + "' as host, date(packages.installed), packages.name, packages.version, packages.release, packages.type, packages.branch, packages.admin, date(test.date) as testdate, test.name as testname, test.user as testuser, test.text as testtext, date(approval.date) as approvaldate, approval.name as approvalname, approval.user as approvaluser, approval.text as approvaltext, packages.changes, packages.backout, packages.impact, packages.author, packages.owner, packages.prefix, packages.desc, packages.url, packages.files, packages.preremove, packages.postremove FROM packages, test, approval WHERE packages.id = test.pkgid and packages.id = approval.pkgid and packages.name = '" + pkg + "' order by date(packages.installed) desc;"

#define GET_PACKAGES_HISTORY(host)        "SELECT '" + host + "' as host, date(packages.installed), packages.name, packages.version, packages.release, packages.type, packages.branch, packages.admin, date(test.date) as testdate, test.name as testname, test.user as testuser, test.text as testtext, date(approval.date) as approvaldate, approval.name as approvalname, approval.user as approvaluser, approval.text as approvaltext, packages.changes, packages.backout, packages.impact, packages.author, packages.owner, packages.prefix, packages.desc, packages.url, packages.files, packages.preremove, packages.postremove FROM packages, test, approval WHERE packages.id = test.pkgid and packages.id = approval.pkgid UNION SELECT '" + host + "' as host, date(installed), name, version, release, type, branch, admin, date(test_date) as testdate, test_name as testname, test_user as testuser, test_text as testtext, date(approval_date) as approvaldate, approval_name as approvalname, approval_user as approvaluser, approval_text as approvaltext, changes, backout, impact, author, owner, prefix, desc, url, files, preremove, postremove FROM packages_history order by date(installed) desc;"

#define GET_PACKAGE_HISTORY(host, pkg)    "SELECT '" + host + "' as host, date(packages.installed), packages.name, packages.version, packages.release, packages.type, packages.branch, packages.admin, date(test.date) as testdate, test.name as testname, test.user as testuser, test.text as testtext, date(approval.date) as approvaldate, approval.name as approvalname, approval.user as approvaluser, approval.text as approvaltext, packages.changes, packages.backout, packages.impact, packages.author, packages.owner, packages.prefix, packages.desc, packages.url, packages.files, packages.preremove, packages.postremove FROM packages, test, approval WHERE packages.id = test.pkgid and packages.id = approval.pkgid and packages.name = '" + pkg + "' UNION SELECT '" + host + "' as host, date(installed), name, version, release, type, branch, admin, date(test_date) as testdate, test_name as testname, test_user as testuser, test_text as testtext, date(approval_date) as approvaldate, approval_name as approvalname, approval_user as approvaluser, approval_text as approvaltext, changes, backout, impact, author, owner, prefix, desc, url, files, preremove, postremove FROM packages_history WHERE name = '" + pkg + "' order by date(installed) desc;"

#define BUFSIZE                      1024
#define YELLOW                       "#FFFFCC"
#define BLUE                         "#E6E6FA"

#define REPORT_REGEX_PACKAGE         "^((" DEPS_REGEX_NAME ")(:" DEPS_REGEX_BRANCH ")?(." DEPS_REGEX_TYPE ")?|all)$"
#define REPORT_REGEX_FORMAT_XML      "xml"
#define REPORT_REGEX_FORMAT_WIKI     "wiki"
#define REPORT_REGEX_FORMAT_TEXT     "txt"
#define REPORT_REGEX_FORMAT          "^(" REPORT_REGEX_FORMAT_XML "|" REPORT_REGEX_FORMAT_WIKI "|" REPORT_REGEX_FORMAT_TEXT ")$"
#define DEFAULT_FORMAT               "wiki"


using namespace std;
using namespace pcrecpp;

class Report {
   public:
             Report(string format = DEFAULT_FORMAT, string package = "all");
            ~Report();
      string getPackageDetails();
      string getPackageHistory();

   private:
      string getHostName();
      bool   setPackage(string package);
      bool   setFormat(string format);
      bool   createOutput();

      string     package;
      string     out;
      string     format;
      Sqlite3   *db;
      sql_result result;
};

#endif
