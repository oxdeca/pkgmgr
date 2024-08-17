/*********************************************************************
 * File    : report.cpp
 * Version : 2018-03-26-01-WS
 * Created : 2018-03-26-01-WS
 * Author  : oxdeca.com
 *********************************************************************/

#include "report.h"


/**
 *
 */
Report::Report(string format, string package) {
   syslog(LOG_INFO, "Report - Report()...");
   syslog(LOG_DEBUG, "Report - format  : %s", format.c_str());
   syslog(LOG_DEBUG, "Report - package : %s", package.c_str());
   //try {
      this->db = new Sqlite3(DBFILE);
   //} catch(Sqlite3Exception e) {
   //   cout << e.getMessage() << endl;
  // }

   if(!setPackage(package)) {
      syslog(LOG_ERR, "Report - No valid Package given!");
   }

   setFormat(format);
}


/**
 *
 */
Report::~Report() {
   syslog(LOG_DEBUG, "Report - ~Report()...");
   delete(db);
}


/**
 *
 */
string Report::getPackageDetails() {
   syslog(LOG_DEBUG, "Report - getPackageDetails()...");

   if(package.compare("all") == 0) {
      this->result = db->query(GET_PACKAGES_DETAILS(getHostName()));
   } else {
      this->result = db->query(GET_PACKAGE_DETAILS(getHostName(), package));
   }

   if(createOutput()) {
      return out;
   } else {
      return NULL;
   }
}

/**
 *
 */
string Report::getPackageHistory() {
   syslog(LOG_DEBUG, "Report - getPackageHistory()...");

   if(package.compare("all") == 0) {
      this->result = db->query(GET_PACKAGES_HISTORY(getHostName()));
   } else {
      this->result = db->query(GET_PACKAGE_HISTORY(getHostName(), package));
   }

   if(createOutput()) {
      return out;
   } else {
      return NULL;
   }
}


// ------------------------------ PRIVATE -------------------------------

/**
 *
 */
bool Report::setPackage(string package) {
   syslog(LOG_DEBUG, "Report - setPackage()...");

   if(RE(REPORT_REGEX_PACKAGE).FullMatch(package)) {
      this->package = package;
      return true;
   } else {
      return false;
   }
}

/**
 *
 */
bool Report::setFormat(string format) {
   syslog(LOG_DEBUG, "Report - setFormat()...");

   if(RE(REPORT_REGEX_FORMAT).FullMatch(format)) {
      this->format = format;
      return true;
   } else {
      syslog(LOG_ERR, "Report - Format not supported... => setting DEFAULT_FORMAT: %s", DEFAULT_FORMAT);
      this->format = DEFAULT_FORMAT;
      return false;
   }
}


/**
 *
 */
string Report::getHostName() {
   syslog(LOG_DEBUG, "Report - getHostName()...");
   
   char  buf[BUFSIZE];

   // get HostName
   if(gethostname(buf, BUFSIZE) != 0) {
      syslog(LOG_ERR, "Report - Could not get Hostname!");
      return NULL;
   } else {
      return (string)buf;
   }
}


/**
 *
 */
bool Report::createOutput() {
   syslog(LOG_DEBUG, "Report - createOutput()...");
   int count = 0;

   if(RE(REPORT_REGEX_FORMAT_XML).FullMatch(format)) {
      syslog(LOG_DEBUG, "Report - generate xml...");
      if(result.rows > 0) {
         out += "<Packages>\n";
         for(int i = 0; i < result.rows * result.cols; i += result.cols) {
            if(result.data[i + 6].compare(DEFAULT_BRANCH) == 0) {
               out += "  <Package name=\""    + result.data[i +  2] + "-" + result.data[i + 3] + "-" + result.data[i + 4] + "-" + result.data[i + 5] + "\"";
            } else {
               out += "  <Package name=\""    + result.data[i +  2] + ":" + result.data[i + 6] + "-" + result.data[i + 3] + "-" + result.data[i + 4] + "-" + result.data[i + 5] + "\"";
            }
            out += " installed=\""         + result.data[i +  1] + "\"";
            out += " admin=\""             + result.data[i +  7] + "\"";
            out += " host=\""              + result.data[i     ] + "\"";
            out += " author=\""            + result.data[i + 19] + "\"";
            out += " owner=\""             + result.data[i + 20] + "\"";
            out += " version=\""           + result.data[i +  3] + "\"";
            out += " release=\""           + result.data[i +  4] + "\"";
            out += " type=\""              + result.data[i +  5] + "\"";
            out += " branch=\""            + result.data[i +  6] + "\"";
            out += " prefix=\""            + result.data[i + 21] + "\"";
            out += " url=\""               + result.data[i + 23] + "\">\n";
            out += "    <Description>"     + result.data[i + 22] + "</Description>\n";
            out += "    <Files>"           + result.data[i + 24] + "    </Files>\n";
            out += "    <PreRemove>"       + result.data[i + 25] + "</PreRemove>\n";
            out += "    <PostRemove>"      + result.data[i + 26] + "</PostRemove>\n";
            out += "    <Test date=\""     + result.data[i +  8] + "\"";
            out += " name=\""              + result.data[i +  9] + "\"";
            out += " user=\""              + result.data[i + 10] + "\">";
            out += result.data[i + 11]     + "</Test>\n";
            out += "    <Approval date=\"" + result.data[i + 12] + "\"";
            out += " name=\""              + result.data[i + 13] + "\"";
            out += " user=\""              + result.data[i + 14] + "\">";
            out += result.data[i + 15]     + "</Approval>\n";
            out += "    <Changes>"         + result.data[i + 16] + "</Changes>\n";
            out += "    <Backout>"         + result.data[i + 17] + "</Backout>\n";
            out += "    <Impact>"          + result.data[i + 18] + "</Impact>\n";
            out += "  </Package>\n\n";
         }
         out += "</Packages>\n";
         return true;
      }
   }

   if(RE(REPORT_REGEX_FORMAT_WIKI).FullMatch(format)) {
      syslog(LOG_DEBUG, "Report - generate wiki...");
      if(result.rows > 0) {
         for(int i = 0; i < result.rows * result.cols; i += result.cols) {
            out += "{{ update\n";
            if(count % 2 == 1) {
               out += "|  colour       = " + (string)YELLOW + "\n";
            } else {
               out += "|  colour       = " + (string)BLUE   + "\n";
            }
            out += "|  host         = " + result.data[i     ] + "\n";
            if(result.data[i + 6].compare(DEFAULT_BRANCH) == 0) {
               out += "|  version      = " + result.data[i +  2] + "-" + result.data[i + 3] + "-" + result.data[i + 4] + "-" + result.data[i + 5] + "\n";
            } else {
               out += "|  version      = " + result.data[i +  2] + ":" + result.data[i + 6] + "-" + result.data[i + 3] + "-" + result.data[i + 4] + "-" + result.data[i + 5] + "\n";
            }
            out += "|  description  = " + result.data[i + 16] + "\n";
            out += "|  date1        = " + result.data[i +  1] + "\n";
            out += "|  admin        = " + result.data[i +  7] + "\n";
            out += "|  date2        = " + result.data[i +  8] + "\n";
            out += "|  tester       = " + result.data[i + 10] + "\n";
            out += "|  testdesc     = " + result.data[i + 11] + "\n";
            out += "|  date3        = " + result.data[i + 12] + "\n";
            out += "|  approver     = " + result.data[i + 14] + "\n";
            out += "|  approvaldesc = " + result.data[i + 15] + "\n";
            out += "|  backout      = " + result.data[i + 17] + "\n";
            out += "|  impact       = " + result.data[i + 18] + "\n";
            out += "}}\n\n\n";

            count++;
         }
      }
      return true;
   }

   if(RE(REPORT_REGEX_FORMAT_TEXT).FullMatch(format)) {
      syslog(LOG_DEBUG, "Report - generate text...");
      if(result.rows > 0) {
         for(int i = 0; i < result.rows * result.cols; i += result.cols) {
            out += "Host          : " + result.data[i     ] + "\n";
            if(result.data[i + 6].compare(DEFAULT_BRANCH) == 0) {
               out += "Package       : " + result.data[i +  2] + "-" + result.data[i + 3] + "-" + result.data[i + 4] + "-" + result.data[i + 5] + "\n";
            } else {
               out += "Package       : " + result.data[i +  2] + ":" + result.data[i + 6] + "-" + result.data[i + 3] + "-" + result.data[i + 4] + "-" + result.data[i + 5] + "\n";
            }
            out += "Description   : " + result.data[i + 16] + "\n";
            out += "Installed     : " + result.data[i +  1] + "\n";
            out += "Admin         : " + result.data[i +  7] + "\n";
            out += "Tested        : " + result.data[i +  8] + "\n";
            out += "Tested by     : " + result.data[i + 10] + "\n";
            out += "Tests         : " + result.data[i + 11] + "\n";
            out += "Approved      : " + result.data[i + 12] + "\n";
            out += "Approved by   : " + result.data[i + 14] + "\n";
            out += "Approval      : " + result.data[i + 15] + "\n";
            out += "Backout       : " + result.data[i + 17] + "\n";
            out += "Impact        : " + result.data[i + 18] + "\n";
            out += "\n";
         }
      }
      return true;
   }
   
   return false;
}
