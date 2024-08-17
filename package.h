/******************************************
 * File    : package.h
 * Version : 2018-03-26-01-WS
 * Created : 2018-03-26-01-WS
 * Author  : oxdeca.com
 ******************************************/

#ifndef _package_h
#define _package_h

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <vector>
#include <algorithm>
#include <string>
#include <syslog.h>
#include <assert.h>
#include <sqlite3.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <archive.h>
#include <archive_entry.h>
#include <pcrecpp.h>

#include "note.h"
#include "deps.h"
#include "sqlite3.h"

// message queues parameters
#define MSGSIZE                                                     4096
#define MSGFLAGS                                                    IPC_CREAT | 0660

#define BUFFSIZE                                                    16384
#define DEFAULT_BYTES_PER_BLOCK                                     (20*512)

#define DEFAULT_PREFIX                                              "/"
#define DEFAULT_BRANCH                                              "MAIN"

#define GROUPS                                                      2
#define TMPDIR                                                      "/tmp/"
#define INSTALL_GROUP                                               "install"

#define USERADD                                                     "/usr/sbin/useradd"
#define GROUPADD                                                    "/usr/sbin/groupadd"


#define FILE_INSTALL_APPROVAL                                       ".install.approval"
#define FILE_INSTALL_AUTHOR                                         ".install.author"
#define FILE_INSTALL_BACKOUT                                        ".install.backout"
#define FILE_INSTALL_BRANCH                                         ".install.branch"
#define FILE_INSTALL_CHANGES                                        ".install.changes"
#define FILE_INSTALL_IMPACT                                         ".install.customerimpact"
#define FILE_INSTALL_DEPS                                           ".install.deps"
#define FILE_INSTALL_DESC                                           ".install.desc"
#define FILE_INSTALL_NAME                                           ".install.name"
#define FILE_INSTALL_PERMS                                          ".install.perms"
#define FILE_INSTALL_RELEASE                                        ".install.release"
#define FILE_INSTALL_TEST                                           ".install.test"
#define FILE_INSTALL_TYPE                                           ".install.type"
#define FILE_INSTALL_URL                                            ".install.url"
#define FILE_INSTALL_USER                                           ".install.user"
#define FILE_INSTALL_VERSION                                        ".install.version"
#define FILE_INSTALL_LAST                                           ".install.zzzzz"

#define PACKAGE_REGEX_PACKAGE                                       "^(" DEPS_REGEX_NAME ")(:" DEPS_REGEX_BRANCH ")?(." DEPS_REGEX_TYPE ")$"
#define PACKAGE_REGEX_SYSTEM_DIRS                                   "^(/|/bin/|/etc/|/lib/|/dev/|/root/|/home/|/tmp/|/var/|/srv/|/sbin/|/proc/|/sys/|/usr/|/usr/bin/|/usr/include/|/usr/lib/|/usr/share/|/usr/libexec/|/usr/sbin/|/usr/var/|/usr/src/|/usr/lib/pkgconfig/|/etc/init.d/|/srv/apache2/|/srv/apache2/modules/|/srv/apache2/cache/|/srv/apache2/htdocs/|/srv/apache2/error/|/etc/httpd/)$"
#define PACKAGE_REGEX_INSTALL_FILE                                  "^[/a-z0-9_]+\\.install\\.[a-z]+$"
#define PACKAGE_REGEX_TARPATH_CHECK                                 "^(./|/)[0-9a-zA-Z/().+_-]+$"
#define PACKAGE_REGEX_PATH                                          "^[0-9a-zA-Z/.@+_-]+$"
#define PACKAGE_REGEX_AUTHOR                                        "^[0-9a-zA-Z.-]+@[0-9a-zA-Z.-]+$"
#define PACKAGE_REGEX_BRANCH                                        "^" DEPS_REGEX_BRANCH "$"
#define PACKAGE_REGEX_NAME                                          "^[0-9a-z_-]+$"
#define PACKAGE_REGEX_OWNER                                         "^[0-9a-z_-]+$"
#define PACKAGE_REGEX_ADMIN                                         "^[A-Z]{2}$"
#define PACKAGE_REGEX_VERSION                                       "^[0-9a-zA-Z._+-]+$"
#define PACKAGE_REGEX_RELEASE                                       "^[0-9]+$"
#define PACKAGE_REGEX_DEPS                                          "^((name=" DEPS_REGEX_NAME "|version=" DEPS_REGEX_VERSION "|release=" DEPS_REGEX_RELEASE "|type=" DEPS_REGEX_TYPE "|branch=" DEPS_REGEX_BRANCH ")[" DEPS_DELIMITER ";]?)+$"
#define PACKAGE_REGEX_PERMS                                         "^[0-9a-zA-Z:/]+$"
#define PACKAGE_REGEX_DESC                                          "^[0-9a-zA-Z .,; +_-]+$"
#define PACKAGE_REGEX_URL                                           "^(http|https|ftp)://[0-9a-zA-Z/.+?&=~#-]+$"
#define PACKAGE_REGEX_SCRIPT                                        "^.*$"
#define PACKAGE_REGEX_TEST                                          "^((date=" NOTE_REGEX_DATE "|name=" NOTE_REGEX_NAME "|user=" NOTE_REGEX_USER "|text=" NOTE_REGEX_TEXT ")[" NOTE_DELIMITER "]?)+$"
#define PACKAGE_REGEX_APPROVAL                                      "^((date=" NOTE_REGEX_DATE "|name=" NOTE_REGEX_NAME "|user=" NOTE_REGEX_USER "|text=" NOTE_REGEX_TEXT ")[" NOTE_DELIMITER "]?)+$"
#define PACKAGE_REGEX_IMPACT                                        "^[0-9a-zA-Z._-]+$"
#define PACKAGE_REGEX_CHANGES                                       "(^((http|https|ftp)://[0-9a-zA-Z/.+?&=~#-]+)?$|" PACKAGE_REGEX_DESC ")"
#define PACKAGE_REGEX_BACKOUT                                       "^[0-9a-zA-Z._ -]+$"
#define PACKAGE_REGEX_TYPE                                          "^" DEPS_REGEX_TYPE "$"
#define PACKAGE_REGEX_PREFIX                                        "^[0-9a-zA-Z/.+_-]+$"
#define PACKAGE_REGEX_FILELIST                                      "^[0-9a-zA-Z/.+_-]+$"
#define PACKAGE_REGEX_FILES                                         "^[0-9a-zA-Z/.+_-]+$"
#define PACKAGE_REGEX_YES                                           "^(y|Y|yes|Yes|YES)$"
#define PACKAGE_REGEX_SELF_OR_TOP_DIR                               "(^.$|^..$)"

#define RED(string)                                                 "\033[1;31m" << string << "\033[0;39m"
#define GREEN(string)                                               "\033[1;32m" << string << "\033[0;39m"

#define DBFILE                                                      "/etc/pkgmgr/pkgmgr.db"
#define SQL_TRANSACTION_BEGIN                                       "BEGIN TRANSACTION;"
#define SQL_TRANSACTION_END                                         "COMMIT;"
#define SQL_TRANSACTION_ROLLBACK                                    "ROLLBACK;"
#define SQL_PKG_INSTALLED(name, type, branch)                       "SELECT name    FROM packages WHERE name = '" + name + "' and type = '" + type + "' and branch = '" + branch + "';"
#define SQL_GET_PACKAGE_ID(name, type, branch)                      "SELECT id      FROM packages WHERE name = '" + name + "' and type = '" + type + "' and branch = '" + branch + "';"
#define SQL_GET_FILES(name, type, branch)                           "SELECT files   FROM packages WHERE name = '" + name + "' and type = '" + type + "' and branch = '" + branch + "';"
#define SQL_GET_PREFIX(name, type, branch)                          "SELECT prefix  FROM packages WHERE name = '" + name + "' and type = '" + type + "' and branch = '" + branch + "';"
#define SQL_GET_AUTHOR(name, type, branch)                          "SELECT author  FROM packages WHERE name = '" + name + "' and type = '" + type + "' and branch = '" + branch + "';"
#define SQL_GET_OWNER(name, type, branch)                           "SELECT owner   FROM packages WHERE name = '" + name + "' and type = '" + type + "' and branch = '" + branch + "';"
#define SQL_GET_VERSION(name, type, branch)                         "SELECT version FROM packages WHERE name = '" + name + "' and type = '" + type + "' and branch = '" + branch + "';"
#define SQL_GET_RELEASE(name, type, branch)                         "SELECT release FROM packages WHERE name = '" + name + "' and type = '" + type + "' and branch = '" + branch + "';"

//#define SQL_GET_DEP(id)                                             "SELECT pkgid, name, version, release, type, branch FROM packages WHERE id = '" + id + "';"
// dependencies
#define SQL_GET_DEP(depid)                                          "SELECT pkgid, name, version, release, type, branch FROM deps WHERE id = '" + depid + "';"
#define SQL_CHECK_DEPS(pkgid)                                       "SELECT pkgid FROM packages_deps WHERE dep_pkgid = '" + pkgid + "';"
#define SQL_CHECK_MISSING_DEPS(name, type, branch)                  "SELECT id FROM deps WHERE name = '" + name + "' and type = '" + type + "' and branch = '" + branch + "';"
#define SQL_DELETE_PACKAGE(name, type, branch)                      "DELETE FROM packages WHERE name = '" + name + "' and type = '" + type + "' and branch = '" + branch + "';"
#define SQL_GET_PACKAGE(id)                                         "SELECT name, version, release, type, branch FROM packages WHERE id = '" + id + "';"
#define SQL_INSERT_PACKAGES_DEPS(pkgid1, pkgid2)                    "INSERT INTO packages_deps VALUES('" + pkgid1 + "', " + pkgid2 + ");"
#define SQL_INSERT_PACKAGES_DEPS2(pkgid, name, type, branch)        "INSERT INTO packages_deps SELECT '" + pkgid + "', id FROM packages WHERE name = '" + name + "' and type = '" + type + "' and branch = '" + branch + "';"


#define SQL_GET_LAST_INSERT_ROWID                                   "SELECT last_insert_rowid();"

#define SQL_INSERT_TEST(pkgid, name, user, text)                    "INSERT INTO test(pkgid, date, name, user, text)     VALUES('" + pkgid + "', DATETIME('NOW'), '" + name + "', '" + user + "', '" + text + "');"
#define SQL_INSERT_APPROVAL(pkgid, name, user, text)                "INSERT INTO approval(pkgid, date, name, user, text) VALUES('" + pkgid + "', DATETIME('NOW'), '" + name + "', '" + user + "', '" + text + "');"
#define SQL_DELETE_DEPS(pkgid)                                      "DELETE FROM deps WHERE pkgid = '" + pkgid + "';"
#define SQL_DELETE_PACKAGES_DEPS(pkgid)                             "DELETE FROM packages_deps WHERE package = '" + pkgid + "' or dependency = '" + pkgid + "';"

#define SQL_INSERT_DEP(pkgid, name, version, release, type, branch) "INSERT INTO deps(pkgid, name, version, release, type, branch) VALUES('" + pkgid + "', '" + name + "', '" + version + "', '" + release + "', '" + type + "', '" + branch + "');"
#define SQL_INSERT_DEPENDENCIES(pkgid, depid)                       "INSERT INTO packages_deps(package, dependency) SELECT '" + pkgid + "', " + depid + ";"
#define SQL_INSERT_PACKAGE(author, owner, name, admin, version, release, type, branch, prefix, desc, url, files, changes, backout, impact, preremove, postremove)   "INSERT INTO packages(author, owner, name, admin, version, release, type, branch, prefix, desc, url, files, changes, backout, impact, preremove, postremove, installed) VALUES('" + author + "', '" + owner + "', '" + name + "', '" + admin + "', '" + version + "', '" + release + "', '" + type + "', '" + branch + "', '" + prefix + "', '" +  desc + "', '" + url + "', '" + files + "', '" + changes + "', '" + backout + "', '" + impact + "', '" + preremove + "', '" + postremove + "', DATETIME('now'));"

//#define SQL_INSERT_PACKAGE(owner, name, version, release, type, desc, url, files) "INSERT INTO packages(owner, name, version, release, type, description, url, filelist, date) VALUES('" + owner + "', '" + name + "', '" + version + "', '" + release + "', '" + type + "', '" + desc + "', '" + url + "', '" + files + "', datetime('now'));"
#define SQL_UPDATE_FILES(name, type, branch, files)                 "UPDATE packages SET files = '" + files + "' WHERE name = '" + name + "' and type = '" + type + "' and branch = '" + branch + "';"

#define SQL_PKG_LIST_BY_NAME(name)                                  "SELECT name, version, release, type FROM packages WHERE name = '" + name + "';"
// TODO change filelist to files in DB-Schema and query
//#define SQL_GET_FILES(name, version, release, type)                 "SELECT filelist FROM packages WHERE name = '" + name + "' and version = '" + version + "' and release = '" + release + "' and type = '" + type + "';"
//#define SQL_GET_PREFIX(name, version, release, type)                "SELECT prefix FROM packages WHERE name = '" + name + "' and version = '" + version + "' and release = '" + release + "' and type = '" + type + "';"
//#define SQL_GET_OWNER(name, version, release, type)                 "SELECT owner FROM packages WHERE name = '" + name + "' and version = '" + version + "' and release = '" + release + "' and type = '" + type + "';"
//#define SQL_UPDATE_FILES(name, version, release, type, file)        "UPDATE packages SET filelist = '" + file + "' WHERE name = '" + name + "' and version = '" + version + "' and release = '" + release + "' and type = '" + type + "';"
//#define SQL_DELETE_PACKAGE(name, version, release, type)            "DELETE FROM packages WHERE name = '" + name + "' and version = '" + version + "' and release = '" + release + "' and type = '" + type + "';"
//"INSERT INTO packages(user, name, version, release, type, dependencies, description, url, files, date) VALUES('" + owner + "', '" + name + "', '" + version + "', '" + release + "', '" + type + "', '" + deps + "', '" + desc + "', '" + url + "', '" + files + "', datetime('now'));"

typedef struct {
   long mtype;
   char mtext[MSGSIZE];
} msg;

//typedef struct {
//   vector<string> header;
//   vector<string> data;
//   int            rows;
//   int            cols;
//   string         error;
//} sql_result;


using namespace std;
using namespace pcrecpp;

class Package {
   public:
                     Package(string name, string type, string branch = DEFAULT_BRANCH);
                     Package(string path);
                    ~Package();
      bool           setPath(string path);
      bool           setAuthor(string name);
      bool           setName(string name);
      bool           setOwner(string user);
      bool           setAdmin(string admin);
      bool           setVersion(string version);
      bool           setRelease(string release);
      bool           setDeps(string deps);
      bool           setPerms(string perms);
      bool           setDesc(string desc);
      bool           setUrl(string url);
      bool           setType(string type);
      bool           setBranch(string branch = DEFAULT_BRANCH);
      bool           setPrefix(string prefix = DEFAULT_PREFIX);
      bool           setFiles(string files);
      bool           setPreRemove(string script);
      bool           setPostRemove(string script);
      bool           setTest(string test);
      bool           setApproval(string approval);
      bool           setImpact(string impact);
      bool           setChanges(string changes);
      bool           setBackout(string backout);
      string         getPath()           { return this->path;           }
      string         getAuthor()         { return this->author;         }
      string         getName()           { return this->name;           }
      string         getOwner()          { return this->owner;          }
      string         getAdmin()          { return this->admin;          }
      string         getVersion()        { return this->version;        }
      string         getRelease()        { return this->release;        }
      string         getPerms()          { return this->perms;          }
      string         getDesc()           { return this->desc;           }
      string         getUrl()            { return this->url;            }
      string         getType()           { return this->type;           }
      string         getBranch()         { return this->branch;         }
      string         getPrefix()         { return this->prefix;         }
      vector<Deps *> getDeps()           { return this->deps;           }
      vector<string> getFiles()          { return this->files;          }
      Note          *getTest()           { return this->test;           }
      Note          *getApproval()       { return this->approval;       }
      string         getImpact()         { return this->impact;         }
      string         getChanges()        { return this->changes;        }
      string         getBackout()        { return this->backout;        }
      string         getPreRemove()      { return this->preremove;      }
      string         getPostRemove()     { return this->postremove;     }
      string         getError()          { return this->error;          }
      int            getErrorNo()        { return this->errorcode;      }

      // methods
      bool install(bool force = false);
      bool remove(bool force = false);
      bool update();
      bool list();
      void printDetails();
      

   private:
      string          path;
      string          author; 
      string          name;
      string          owner;
      string          admin;
      string          version;
      string          release;
      string          perms;
      string          desc;
      string          url;
      string          type;
      string          branch;
      string          prefix;
      vector<Deps *>  deps;
      vector<string>  files;
      string          preremove;
      string          postremove;
      //sqlite3        *db;
      int             msgid;
      Note           *test;
      Note           *approval;
      string          impact;
      string          changes;
      string          backout;
      string          error;
      int             errorcode;
      int             MSGKEY;
      Sqlite3        *sqlite;
      

      // methods
      bool           copy(string in, string out);
      bool           setUID(string user, string group);
      string         chomp(string s);
      vector<string> str2Vector(string text);
      //sql_result     sql_query(string query);
      void           setError(string error);
      void           setErrorNo(int errorcode);
};



class PackageException {
   public:
             PackageException();
             PackageException(int error);
             PackageException(int error, string msg);
             PackageException(string msg);
      string getMessage();

   private:
      string  msg;
      int     error;
};

#endif
