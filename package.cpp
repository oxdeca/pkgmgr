/*********************************************************************
 * File    : package.cpp
 * Version : 2018-03-26-01-WS
 * Created : 2018-03-26-01-WS
 * Author  : oxdeca.com
 *********************************************************************/

/**
* @file     package.cpp
* @version  
* @author   oxdeca.com
*/

#include "package.h"

bool compStrings(string a, string b);

// @TODO Error-Handling
// @TODO check for mandatory params (i.e. name, version, release, type, prefix)
// @TODO replace message-queue with pipe

/**
 *  @param path absolute path of the packagefile
 */
Package::Package(string path) {
   struct  archive       *archive;
   struct  archive_entry *entry;
   struct  msqid_ds       buf;
   char    buffer[BUFFSIZE];
   ssize_t len = BUFFSIZE;
   int     rc;
   string  curpath;

   syslog(LOG_INFO, "Package - Package()...");
   syslog(LOG_DEBUG, "Package - path: %s", path.c_str());

   sqlite = new Sqlite3(DBFILE);

   if(setPath(path)) {
      // read in package params
      if((archive = archive_read_new()) == NULL) {
         setError((string)"Could not instantiate archive object: " + archive_error_string(archive));
         syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
         throw PackageException(getError());
      }

      if((rc = archive_read_support_compression_all(archive)) != 0) {
         setError((string)"Could not add compression support: " + archive_error_string(archive));
         syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
         throw PackageException(getError());
      }

      if((rc = archive_read_support_format_all(archive)) != 0) {
         setError((string)"Could not add support for all tar-formats: " + archive_error_string(archive));
         syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
         throw PackageException(getError());
      }

      if((rc = archive_read_open_file(archive, getPath().c_str(), DEFAULT_BYTES_PER_BLOCK)) != 0) {
         setError((string)"Could not open tarfile: " + archive_error_string(archive));
         syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
         throw PackageException(getError());
      }

      while(archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
         curpath = (string)archive_entry_pathname(entry); 
         memset(buffer, 0, len);

         // if we read behind .install.zzzzz we are finished
         if(((string) basename(curpath.c_str())).compare(FILE_INSTALL_LAST) >  0) {
            break;
         }
         

         if(((string) basename(curpath.c_str())).compare(FILE_INSTALL_APPROVAL) == 0) {
            if(archive_read_data(archive, &buffer, len) == ARCHIVE_OK) {
               setApproval(chomp((string)buffer));
            } else {
               setError((string)"Could not read Data from tarfile: " + archive_error_string(archive));
               syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
            }
         }
 
         if(((string) basename(curpath.c_str())).compare(FILE_INSTALL_AUTHOR) == 0) {
            if((rc = archive_read_data(archive, &buffer, len)) == ARCHIVE_OK) {
               setAuthor(chomp((string)buffer));
            } else {
               setError((string)"Could not read Data from tarfile: " + archive_error_string(archive));
               syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
            }
         }
         
         if(((string) basename(curpath.c_str())).compare(FILE_INSTALL_BACKOUT) == 0) {
            if(archive_read_data(archive, &buffer, len) == ARCHIVE_OK) {
               setBackout(chomp((string)buffer));
            } else {
               setError((string)"Could not read Data from tarfile: " + archive_error_string(archive));
               syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
            }
         }
 
         if(((string) basename(curpath.c_str())).compare(FILE_INSTALL_BRANCH) == 0) {
            if((rc = archive_read_data(archive, &buffer, len)) == ARCHIVE_OK) {
               setBranch(chomp((string)buffer));
            } else {
               setError((string)"Could not read Data from tarfile: " + archive_error_string(archive));
               syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
               throw PackageException(getError());
            }
         }
         
         if(((string) basename(curpath.c_str())).compare(FILE_INSTALL_CHANGES) == 0) {
            if(archive_read_data(archive, &buffer, len) == ARCHIVE_OK) {
               setChanges(chomp((string)buffer));
            } else {
               setError((string)"Could not read Data from tarfile: " + archive_error_string(archive));
               syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
            }
         }
 
         if(((string) basename(curpath.c_str())).compare(FILE_INSTALL_IMPACT) == 0) {
            if(archive_read_data(archive, &buffer, len) == ARCHIVE_OK) {
               setImpact(chomp((string)buffer));
            } else {
               setError((string)"Could not read Data from tarfile: " + archive_error_string(archive));
               syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
            }
         }
 
         if(((string) basename(curpath.c_str())).compare(FILE_INSTALL_DEPS) == 0) {
            if(archive_read_data(archive, &buffer, len) == ARCHIVE_OK) {
               setDeps(chomp((string)buffer));
            } else {
               setError((string)"Could not read Data from tarfile: " + archive_error_string(archive));
               syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
            }
         }
 
         if(((string) basename(curpath.c_str())).compare(FILE_INSTALL_DESC) == 0) {
            if(archive_read_data(archive, &buffer, len) == ARCHIVE_OK) {
               setDesc(chomp((string)buffer));
            } else {
               setError((string)"Could not read Data from tarfile: " + archive_error_string(archive));
               syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
            }
         }
 
         if(((string) basename(curpath.c_str())).compare(FILE_INSTALL_NAME) == 0) {
            if(archive_read_data(archive, &buffer, len) == ARCHIVE_OK) {
               setName(chomp((string)buffer));
            } else {
               setError((string)"Could not read Data from tarfile: " + archive_error_string(archive));
               syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
               throw PackageException(getError());
            }
         }
 
         if(((string) basename(curpath.c_str())).compare(FILE_INSTALL_PERMS) == 0) {
            if(archive_read_data(archive, &buffer, len) == ARCHIVE_OK) {
               setPerms(chomp((string)buffer));
            } else {
               setError((string)"Could not read Data from tarfile: " + archive_error_string(archive));
               syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
            }
         }

         if(((string) basename(curpath.c_str())).compare(FILE_INSTALL_RELEASE) == 0) {
            if(archive_read_data(archive, &buffer, len) == ARCHIVE_OK) {
               setRelease(chomp((string)buffer));
            } else {
               setError((string)"Could not read Data from tarfile: " + archive_error_string(archive));
               syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
               throw PackageException(getError());
            }
         }
 
         if(((string) basename(curpath.c_str())).compare(FILE_INSTALL_TYPE) == 0) {
            if(archive_read_data(archive, &buffer, len) == ARCHIVE_OK) {
               setType(chomp((string)buffer));
            } else {
               setError((string)"Could not read Data from tarfile: " + archive_error_string(archive));
               syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
               throw PackageException(getError());
            }
         }
 
         if(((string) basename(curpath.c_str())).compare(FILE_INSTALL_URL) == 0) {
            if(archive_read_data(archive, &buffer, len) == ARCHIVE_OK) {
               setUrl(chomp((string)buffer));
            } else {
               setError((string)"Could not read Data from tarfile: " + archive_error_string(archive));
               syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
            }
         }
 
         if(((string) basename(curpath.c_str())).compare(FILE_INSTALL_USER) == 0) {
            if(archive_read_data(archive, &buffer, len) == ARCHIVE_OK) {
               setOwner(chomp((string)buffer));
            } else {
               setError((string)"Could not read Data from tarfile: " + archive_error_string(archive));
               syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
               throw PackageException(getError());
            }
         }
 
         if(((string) basename(curpath.c_str())).compare(FILE_INSTALL_VERSION) == 0) {
            if(archive_read_data(archive, &buffer, len) == ARCHIVE_OK) {
               setVersion(chomp((string)buffer));
            } else {
               setError((string)"Could not read Data from tarfile: " + archive_error_string(archive));
               syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
               throw PackageException(getError());
            }
         }
 
         if(((string) basename(curpath.c_str())).compare(FILE_INSTALL_TEST) == 0) {
            if(archive_read_data(archive, &buffer, len) == ARCHIVE_OK) {
               setTest(chomp((string)buffer));
            } else {
               setError((string)"Could not read Data from tarfile: " + archive_error_string(archive));
               syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
            }
         }
      }
      archive_read_close(archive);
      archive_read_finish(archive);
   } else {
      setError((string)"Could not read PackageFile: " + strerror(errno));
      syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
      throw PackageException(getError());
   }
   
   // @TODO check if all mandatory params have been set
   if(getOwner().empty()) {
      setError("Owner not set!");
      syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
      throw PackageException(getError());
   }

   if(getName().empty()) {
      setError("Name not set!");
      syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
      throw PackageException(getError());
   }

   if(getVersion().empty()) {
      setError("Version not set!");
      syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
      throw PackageException(getError());
   }

   if(getRelease().empty()) {
      setError("Release not set!");
      syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
      throw PackageException(getError());
   }

   if(getType().empty()) {
      setError("Type not set!");
      syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
      throw PackageException(getError());
   }

   if(getBranch().empty()) {
      setError("Branch not set!");
      syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
      setBranch("");
   }

   // create MSGKEY
   this->MSGKEY = rand() % 9000 + 1000;
   syslog(LOG_INFO, "Package - MSGKEY: %d", this->MSGKEY);

   // create msgqueue
   if((msgid  = msgget(this->MSGKEY, MSGFLAGS)) < 0) {
      setError((string)"Could not create messagequeue: " + strerror(errno));
      syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
      throw PackageException(getError());
   }
   // set permissions on the messagequeue to root.install 0660
   if(msgctl(msgid, IPC_STAT, &buf) == -1) {
      setError((string)"msgctl: " + strerror(errno));
      syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
      throw PackageException(getError());
   }
   buf.msg_perm.gid = (getgrnam(INSTALL_GROUP))->gr_gid;
   if(msgctl(msgid, IPC_SET, &buf) == -1) {
      setError((string)"msgctl: " + strerror(errno));
      syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
      throw PackageException(getError());
   }
}


/**
 *  Constructor for already installed packages. Parameters are read from the database.
 *  @param name
 *  @param type
 *  @param branch
 */
Package::Package(string name, string type, string branch) {
   struct msqid_ds buf;
   sql_result      result;

   //syslog(LOG_INFO, "-------------------- Logfile opened --------------------");
   //syslog(LOG_INFO, "$Id: package.cpp,v 1.20 2010/01/25 17:15:34 wolfi Exp $");
   syslog(LOG_INFO, "Package - Package()...");
   syslog(LOG_DEBUG, "Package - name: %s, type: %s, branch: %s",  name.c_str(), type.c_str(), branch.c_str());

   // check if name, type  and branch have been set successfully
   if(setName(name) && setType(type) && setBranch(branch)) {
      sqlite = new Sqlite3(DBFILE);

      // first thing we do is to check if the package is already installed
      result = sqlite->query(SQL_GET_PACKAGE_ID(getName(), getType(), getBranch()));
      if(result.rows == 0) {
         setError("Package is not installed!");
         syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
         throw PackageException(getError());
      }

      // read author from db
      result = sqlite->query(SQL_GET_AUTHOR(getName(), getType(), getBranch()));
      // there should only be 1 author
      if(result.rows == 1) {
         setAuthor(result.data[0]);
      } else {
         syslog(LOG_ERR, "Package - ERROR: initializing author");
         throw PackageException(75, "could not set author");
         //exit(75);
      }

      // read owner from db
      result = sqlite->query(SQL_GET_OWNER(getName(), getType(), getBranch()));
      // there should only be 1 owner
      if(result.rows == 1) {
         setOwner(result.data[0]);
      } else {
         syslog(LOG_ERR, "Package - ERROR: initializing owner");
         throw PackageException(75, "could not set owner");
         //exit(75);
      }

      // read version from db
      result = sqlite->query(SQL_GET_VERSION(getName(), getType(), getBranch()));
      // there should only be 1 version
      if(result.rows == 1) {
         setVersion(result.data[0]);
      } else {
         syslog(LOG_ERR, "Package - ERROR: initializing version");
         throw PackageException(75, "could not set version");
         //exit(75);
      }

      // read release from db
      result = sqlite->query(SQL_GET_RELEASE(getName(), getType(), getBranch()));
      // there should only be 1 release
      if(result.rows == 1) {
         setRelease(result.data[0]);
      } else {
         syslog(LOG_ERR, "Package - ERROR: initializing release");
         throw PackageException(75, "could not set release");
         //exit(75);
      }

      syslog(LOG_DEBUG, "Package - initFiles");
      // get the Files from the database
      result = sqlite->query(SQL_GET_FILES(getName(), getType(), getBranch()));
      // there should only be 1 string containing all paths delimitetd with "\n" (newline)
      if(result.rows == 1) {
         setFiles(result.data[0]);
      } else {
         syslog(LOG_ERR, "Package - ERROR: initializing files");
         throw PackageException(75, "could not set files");
         //exit(75);
      }

      syslog(LOG_DEBUG, "Package - initPrefix");
      // get the Prefix from the database
      result = sqlite->query(SQL_GET_PREFIX(getName(), getType(), getBranch()));
      // there should only be 1 prefix
      if(result.rows == 1) {
         setPrefix(result.data[0]);
      } else {
         syslog(LOG_ERR, "Package - error setting prefix - taking DEFAULT: %s", DEFAULT_PREFIX );
         setPrefix();
         //exit(75);
      }

   } else {
      setError((string)"Could not initialize Package Object.");
      syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
      throw PackageException(getError());
   }

   // @TODO check if all mandatory params have been set


   // create MSGKEY
   this->MSGKEY = rand() % 9000 + 1000;
   syslog(LOG_INFO, "Package - MSGKEY: %d", this->MSGKEY);

   // create msgqueue
   if((msgid  = msgget(this->MSGKEY, MSGFLAGS)) < 0) {
      setError((string)"Could not create messagequeue: " + strerror(errno));
      syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
      throw PackageException(getError());
   }

   // set permissions on the messagequeue to root.install 0660
   if(msgctl(msgid, IPC_STAT, &buf) == -1) {
      setError((string)"msgctl: " + strerror(errno));
      syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
      throw PackageException(getError());
   }

   // set the group to the INSTALL_GROUP
   buf.msg_perm.gid = (getgrnam(INSTALL_GROUP))->gr_gid;
   if(msgctl(msgid, IPC_SET, &buf) == -1) {
      setError((string)"msgctl: " + strerror(errno));
      syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
      throw PackageException(getError());
   }
}


/**
 *  Deconstructor
 */
Package::~Package() {
   int rc;

   // delete messagequeue
   if((rc = msgctl(msgid, IPC_RMID, 0)) == -1) {
      syslog(LOG_ERR, "Package - msgctl: %s", strerror(errno));
   }

   //sqlite3_close(db);
   //syslog(LOG_INFO, "-------------------- Logfile closed --------------------");
   syslog(LOG_INFO, "Package - ~Package()");
}


/**
 *  @param force
 */
bool Package::install(bool force) {
   int       pid, status;
   string    line;
   string    path;
   string    dirs;
   string    files;
   msg       buf;
   sql_result result;
   int       rc;
   string    testid;
   string    apprid;
   string    pkgid;
   string    instVersion;
   string    instRelease;
   bool      install = true;
   Deps      *dep;

   syslog(LOG_INFO, "Package - install()...");
   if(force) {
      syslog(LOG_DEBUG, "Package - force is set.");
   }

   result = sqlite->query(SQL_GET_PACKAGE_ID(getName(), getType(), getBranch()));

   // check if the package is already installed
   syslog(LOG_DEBUG, "Package - check if package is installed...");
   if(result.rows > 0 && force == false) {
      // getVersion from the already installed package
      result = sqlite->query(SQL_GET_VERSION(getName(), getType(), getBranch()));
      if(result.rows == 1) {
         instVersion = result.data[0];
      }

      // getRelease from the already installed package
      result = sqlite->query(SQL_GET_RELEASE(getName(), getType(), getBranch()));
      if(result.rows == 1) {
         instRelease = result.data[0];
      }

      setErrorNo(10);
      // read the installed version number of the package (don't display the version of the package to be installed)
      //if((instVersion.compare(getVersion()) ==  0)  && (instRelease.compare(getRelease()) == 0)) {
      if(!instVersion.empty() && !instRelease.empty()) {
         setError("Another Package of same type is already installed: \"" + getName() + ":" + getBranch() + "-" + instVersion + "-" + instRelease + "-" + getType() + "\" [use -f to force install]");
      } else {
         setError("Another Package of same type is already installed! [use -f to force install]");
      }
      syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());

      return false;
   } 

   if(result.rows > 0 && force == true) {
      syslog(LOG_DEBUG, "Package - package will be installed forced! => remove old package first...");
      // remove the package
      Package *pkg = new Package(getName(), getType(), getBranch());
      pkg->remove(force);
      delete pkg;
   }
   
   syslog(LOG_DEBUG, "Package - check dependencies...");
   // check Dependencies
   for(vector<Deps *>::iterator it = this->deps.begin(); it < this->deps.end(); ++it) {
      // check if dependent package is installed (Dependency-Data from the package)
      result = sqlite->query(SQL_GET_PACKAGE_ID((*it)->getName(), (*it)->getType(), (*it)->getBranch()));
      if(result.rows == 0) {
         install = false;

         setError("Package " + (*it)->getName() + ":" + (*it)->getBranch() + "-" + (*it)->getType() + "-" + (*it)->getVersion() + "-" + (*it)->getRelease() + " is not installed!");
         syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
      } 
   }

   // check if a prefix has been set
   if(getPrefix().empty()) {
      // check if the package is of type web, then the prefix has to be set on the commandline
      if(getType() == "web") {
         setError("Packages of type 'web' have to be installed with a prefix (-p <prefix>)!");
         return false;
      } else {
         // otherwise use the default-prefix
         setPrefix();
      }
   } else {
      if(RE(PACKAGE_REGEX_SYSTEM_DIRS).FullMatch(getPrefix()) == false) {
         files = getPrefix() + "\n";
         dirs  = getPrefix() + "\n";
      }
   }


   if(!install && force == false) {
      setError("Missing dependent Packages! (see Logfile)");
      return false;
   } else {
      syslog(LOG_DEBUG, "Package - extractTar");

      // fork() and do in child
      if((pid = fork()) == 0) {
         // setUID
         if(setUID(owner, INSTALL_GROUP) == false) {
            setError((string)"Could not set UID: " + strerror(errno));
            syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
            //syslog(LOG_ERR, "Could not set UID: %s", strerror(errno));
            return false;
         }

         //  extractTarfile
         struct archive       *archive;
         struct archive_entry *entry;

         if((archive = archive_read_new()) == NULL) {
            syslog(LOG_ERR, "Package - Could not create archive structure: %s.", strerror(errno));
         }

         if((rc = archive_read_support_compression_all(archive)) != 0) {
            syslog(LOG_ERR, "Package - Could not add compression support: %s.", strerror(errno));
         }

         if((rc = archive_read_support_format_all(archive)) != 0) {
            syslog(LOG_ERR, "Package - Could not add support for all tar archives: %s.", strerror(errno));
         }

         if((rc = archive_read_open_file(archive, getPath().c_str(), DEFAULT_BYTES_PER_BLOCK)) != 0) {
            syslog(LOG_ERR, "Package - Could not open tarfile: %s.", strerror(errno));
         }

         while(archive_read_next_header(archive, &entry) != ARCHIVE_EOF) {
            path = (string)archive_entry_pathname(entry);

            // check if path in tarfile starts with "./" or "/". All others paths are ignored, e.g. "../", ".../", "//", ...
            if(RE(PACKAGE_REGEX_TARPATH_CHECK).FullMatch(path)) {
               // check if the prefix ends on a "/"
               if(prefix.rfind("/") == prefix.size() - 1) {
                  // put prefix and path together to an abslolute path
                  path = (prefix + path.substr(path.find("/") + 1, path.size() - path.find("/")));
               } else {
                  // put prefix and path together to an abslolute path
                  path = (prefix + path.substr(path.find("/"), path.size() - path.find("/") + 1));
               }
            } else {
               setError("Path in Tarfile is not a valid path: " + path);
               syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
               continue;
            }

            if(!archive_entry_hardlink(entry)) {
               archive_entry_set_pathname(entry, path.c_str());
            }

            // don't extract .install.* files
            if(RE(PACKAGE_REGEX_INSTALL_FILE).FullMatch(path) == false && RE(PACKAGE_REGEX_SYSTEM_DIRS).FullMatch(path) == false) {
               // TODO extracting the time does not work! we have to find out why! permissions should not be the problem, but syslog tells us it's an permissions error ?!?
               //if(archive_read_extract(archive, entry, ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_OWNER) != ARCHIVE_OK) {
               if(archive_read_extract(archive, entry, ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_OWNER) != ARCHIVE_OK) {
                  syslog(LOG_ERR, "Package - ERROR: %s: %s archive-error:%s", path.c_str(), strerror(errno), archive_error_string(archive));
                  // optimize for terminals with size 80x24
                  cout.width(74);
                  cout << left << path << RED("ERROR") << endl;
               } else {
                  // optimize for terminals with size 80x24
                  cout.width(77);
                  cout << left << path << GREEN("OK") << endl;

                  // if file to extract is a directory store it in the var dirs that we will pass to the parent, when the extracting is finished
                  if(S_ISDIR(archive_entry_mode(entry))) {
                     dirs  += path + "\n";
                  }
                  files += path + "\n";
               }
            }
         }

         archive_read_close(archive);
         archive_read_finish(archive);

         // set message type to 1, because we only have 1 kind of message
         buf.mtype = 1;
         strncpy(buf.mtext, dirs.c_str(), MSGSIZE);
         // send the dirs to the parent, so we can change the permissions on them
         if((rc = msgsnd(msgid, &buf, sizeof(buf), 0)) == -1) {
            syslog(LOG_ERR, "Package - msgsnd: %s", strerror(errno));
         }

         // update DB
         sqlite->query(SQL_TRANSACTION_BEGIN);

         sqlite->query(SQL_INSERT_PACKAGE(getAuthor(), getOwner(), getName(), getAdmin(), getVersion(), getRelease(), getType(), getBranch(), getPrefix(), getDesc(), getUrl(), files, getChanges(), getBackout(), getImpact(), getPreRemove(), getPostRemove()));
         result = sqlite->query(SQL_GET_LAST_INSERT_ROWID);
         if(result.rows == 1) {
            pkgid = result.data[0];
         }
         sqlite->query(SQL_INSERT_TEST(pkgid, test->getName(), test->getUser(), test->getText()));
         sqlite->query(SQL_INSERT_APPROVAL(pkgid, approval->getName(), approval->getUser(), approval->getText()));

         sqlite->query(SQL_TRANSACTION_END);

         // insert dependencies (dependencies from the package-file .install.deps)
         syslog(LOG_INFO, "Package - insert dependencies into db...");
         for(vector<Deps *>::iterator it = this->deps.begin(); it < this->deps.end(); ++it) {
            sqlite->query(SQL_TRANSACTION_BEGIN);
            sqlite->query(SQL_INSERT_DEP(pkgid, (*it)->getName(), (*it)->getVersion(), (*it)->getRelease(), (*it)->getType(), (*it)->getBranch()));
            sqlite->query(SQL_INSERT_PACKAGES_DEPS2(pkgid, (*it)->getName(), (*it)->getType(), (*it)->getBranch()));
            sqlite->query(SQL_TRANSACTION_END);
         }

         sql_result result2;
         syslog(LOG_INFO, "Package - checkMissingDeps...");
         // checkMissingDeps()
         // check if the package to install is anothers (installed) dependent package (Dependency-Data from the DB)
         result = sqlite->query(SQL_CHECK_MISSING_DEPS(getName(), getType(), getBranch()));
         if(result.rows > 0) {
            for(int i = 0; i < result.rows * result.cols; i++) {
               // version and release check
               result2 = sqlite->query(SQL_GET_DEP(result.data[i]));
               if(result2.rows > 0) {
                  for(int j = 0; j < result2.rows * result2.cols; j += result2.cols) {
                     dep = new Deps(result2.data[j + 1], result2.data[j + 2], result2.data[j + 3], result2.data[j + 4], result2.data[j + 5]);
                     if(dep->checkVersion(getVersion()) && dep->checkRelease(getRelease())) {
                        // make connection in packages_deps
                        sqlite->query(SQL_TRANSACTION_BEGIN);
                        sqlite->query(SQL_INSERT_PACKAGES_DEPS(result2.data[j], pkgid));
                        sqlite->query(SQL_TRANSACTION_END);
                     } else {
                        setError("There are unresolved dependencies!");
                        syslog(LOG_ERR, "Package - ERROR: %s", getError().c_str());
                     }
                  }
               }
            }
         }
         
         exit(0);
      } else {
         waitpid(pid, &status, 0);
         if(WIFEXITED(status)) {
            if((rc = msgrcv(msgid, &buf, sizeof(buf), 0, IPC_NOWAIT)) == -1) {
               syslog(LOG_ERR, "Package - msgrcv: %s", strerror(errno));
            } else {
               dirs = buf.mtext;
               
               // for every dir in dirs set Permission to root.install and 1775
               char *tmp = (char *)dirs.c_str();
               char *token = strtok(tmp, "\n");
               while(token != NULL) {
                  if(chown(token, geteuid(), (getgrnam(INSTALL_GROUP))->gr_gid) != 0) {
                     syslog(LOG_ERR, "Package - chown %s: %s", token, strerror(errno));
                  } else {
                     syslog(LOG_DEBUG, "Package - chown root.install %s", token);
                  }

                  if(chmod(token, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH | S_ISVTX) != 0) {
                     syslog(LOG_ERR, "Package - chmod %s: %s", token, strerror(errno));
                  } else {
                     syslog(LOG_DEBUG, "Package - chmod 1775 %s", token);
                  }
                  token = strtok(NULL, "\n");
               }
            }
         }
      }
   }
   return true;
}


/**
 *
 */
bool Package::update() {
   syslog(LOG_INFO, "Package - update()...");

   // check if package is already installed

   return true;
}

/**
 *  @pre name, version, release, type and owner have to be set
 *  @param force
 */
bool Package::remove(bool force) {
   struct stat    sbuf;
   bool           emptydir = false;
   int            pid;
   string         curpath;
   string         tmp;
   sql_result     result;
   DIR           *dir;
   struct dirent *file; 
   vector<string> files;
   vector<string> isfile;
   vector<string> isdir;
   bool           remove = true;
   char           line[5];
   vector<string>::const_iterator entry;
   string         pkgid;
   sql_result     package;

   syslog(LOG_INFO, "Package - remove()...");
   if(force) {
      syslog(LOG_DEBUG, "Package - force is set.");
   }

   // if the package is not forced to be removed -> check Dependencies, otherwise, don't check them, because the package is to be removed anyways
   if(force == false) {
      // check Dependencies
      // first get the package id from the installed package
      result = sqlite->query(SQL_GET_PACKAGE_ID(getName(), getType(), getBranch()));
      if(result.rows > 0) {
         pkgid = result.data[0];
         result = sqlite->query(SQL_CHECK_DEPS(pkgid));
         if(result.rows > 0) {
            remove = false;
            for(int i = 0; i < (result.rows * result.cols); i++) {
               package = sqlite->query(SQL_GET_PACKAGE(result.data[i]));
               cout << "Package " << package.data[0] << "-" << package.data[1] << "-" << package.data[2] << "-" << package.data[3] << "-" << package.data[4];
               if((i + 1) % result.cols == 0) {
                  cout << " depends on " << getName() << "-" << getVersion() << "-" << getRelease() << "-" << getType() << endl;
               } else {
                  cout << "-";
               }
            }
         }
      }
   }

   if(remove == false) {
      if(force == false) {
         cout << "Do you really want to remove this package (" << getName() << "-" << getVersion() << "-" << getRelease() << "-" << getType() << ")?" << endl;
         while(cin.getline(line, 5)) {
            if(RE(PACKAGE_REGEX_YES).FullMatch(line)) {
               remove = true;
               break;
            } else {
               setError("remove() stopped by user!");
               return false;
            }
         }
      }
   }

   syslog(LOG_DEBUG, "Package - isDir?");
   files = getFiles();
   // for each file in Files, figure out if it's a file or dir
   for(entry = files.begin(); entry != files.end(); entry++) {
      stat((*entry).c_str(), &sbuf);

      if(!S_ISDIR(sbuf.st_mode)) {
         isfile.push_back(*entry);
      } else {
         isdir.push_back(*entry);
      }
   }

   // create child that sets UID to package install user, so we are only allowed to delete files of the package
   if((pid = fork()) == 0) {
      vector<string> failed;
      string tmp   = "";

      syslog(LOG_DEBUG, "Package - setUID");
      // setUID before we delete any file
      if(setUID(owner, INSTALL_GROUP)) {
         for(entry = isfile.begin(); entry != isfile.end(); entry++) {
            if(unlink((*entry).c_str()) != 0) {
               syslog(LOG_ERR, "Package - %s", strerror(errno));
               string tmp = *entry;
               cout.width(74);
               cout << left << *entry << RED("ERROR")  << endl;
               // if unlink was not successfull store those files in the list "failed"
               failed.push_back(*entry);
            }
         }

         // create a string with all files that were not unlinked
         for(entry = failed.begin(); entry != failed.end(); entry++) {
            tmp += (*entry) + "\n";
         }
      } else {
        // TODO
        // tell the parent that the child could not delete the files, because setUID went wrong 
      }
   
      exit(0);
   } else {
      // wait for the child to end
      waitpid(pid, NULL, 0);
 
      // sort dirs by depth, so we can delete the deepest dirs first, otherwise the dirs would not be empty
      std::sort(isdir.begin(), isdir.end(), compStrings); 

      // iterate through the dirs and check if dir is empty or not.
      for(entry = isdir.begin(); entry != isdir.end(); entry++) {
         emptydir = true;

         if((dir = opendir((*entry).c_str())) != NULL) {
            // check if dir is empty
            while((file = readdir(dir)) && (emptydir == true)) {
               if(!RE(PACKAGE_REGEX_SELF_OR_TOP_DIR).FullMatch(file->d_name)) {
                  emptydir = false;
                  break;
               }
            }
            (void) closedir(dir);
         }

         // if dir is empty delete it
         if(emptydir) {
            if(rmdir((*entry).c_str()) != 0) {
               syslog(LOG_INFO, "Package - ERROR: Could not unlink dir %s", (*entry).c_str());
               cout.width(68);
               cout << left << *entry << RED("ERROR")  << endl;
            }
         }
      }

      result = sqlite->query(SQL_GET_PACKAGE_ID(getName(), getType(), getBranch()));
      if(result.rows > 0) {
         pkgid = result.data[0];
      }

      // delete entry from Database
      sqlite->query(SQL_DELETE_PACKAGE(getName(), getType(), getBranch()));

   }

   return true;
}

/**
 *  @param path path of package
 */
bool Package::setPath(string path) {
   fstream fin;
   syslog(LOG_DEBUG, "Package - setPath()...");

   // validate path
   if(RE(PACKAGE_REGEX_PATH).FullMatch(path)) {
      fin.open(path.c_str(), ios::in);
      if(fin.is_open()) {
         this->path = path;
         syslog(LOG_DEBUG, "Package - path: %s", path.c_str());
      } else {
         syslog(LOG_INFO, "Package - setPath() - could not read PackageFile!");
         return false;
      }
      fin.close();
   } else {
      syslog(LOG_INFO, "Package - setPath() - package contains no valid path!");
      return false;
   }
 
   return true;
}


/**
 * @param author author of package
 */
bool Package::setAuthor(string author) {
   syslog(LOG_DEBUG, "Package - setAuthor()...");

   // validate author
   if(RE(PACKAGE_REGEX_AUTHOR).FullMatch(author)) {
      this->author = author;
      syslog(LOG_DEBUG, "Package - author: %s", author.c_str());
   } else {
      syslog(LOG_INFO, "Package - setAuthor() - package contains no valid author!");
      cout << RED("ERROR: ") << "package contains no valid author!" << endl;
      return false;
   }

   return true;
}


/**
 *  @param name name of package
 */
bool Package::setName(string name) {
   syslog(LOG_DEBUG, "Package - setName()...");

   // validate name
   if(RE(PACKAGE_REGEX_NAME).FullMatch(name)) {
      this->name = name;
      syslog(LOG_DEBUG, "Package - name: %s", name.c_str());
   } else {
      syslog(LOG_INFO, "Package - setName() - package contains no valid name!");
      return false;
   }

   return true;
}


/**
 *  @param owner name of user that is allowed to install the package
 */
bool Package::setOwner(string owner) {
   syslog(LOG_DEBUG, "Package - setOwner()...");

   // validate user
   if(RE(PACKAGE_REGEX_OWNER).FullMatch(owner)) {
      this->owner = owner;
      syslog(LOG_DEBUG, "Package - owner: %s", owner.c_str());
   } else {
      syslog(LOG_INFO, "Package - setOwner() - package contains no valid owner!");
      return false;
   }

   return true;
}


/**
 *  @param admin name of admin who installed the package
 */
bool Package::setAdmin(string admin) {
   syslog(LOG_DEBUG, "Package - setAdmin()...");

   // validate user
   if(RE(PACKAGE_REGEX_ADMIN).FullMatch(admin)) {
      this->admin = admin;
      syslog(LOG_DEBUG, "Package - admin: %s", admin.c_str());
   } else {
      setError("setAdmin() - no valid admin given!");
      syslog(LOG_INFO, "Package - %s", this->getError().c_str());
      return false;
   }

   return true;
}


/**
 *  @param version verion of package software
 */
bool Package::setVersion(string version) {
   syslog(LOG_DEBUG, "Package - setVersion()...");

   // validate version
   if(RE(PACKAGE_REGEX_VERSION).FullMatch(version)) {
      this->version = version;
      syslog(LOG_DEBUG, "Package - version: %s", version.c_str());
   } else {
      syslog(LOG_INFO, "Package - setVersion() - package contains no valid version!");
      return false;
   }

   return true;
}



/**
 *  @param release release of package
 */
bool Package::setRelease(string release) {
   syslog(LOG_DEBUG, "Package - setRelease()...");

   // validate release
   if(RE(PACKAGE_REGEX_RELEASE).FullMatch(release)) {
      this->release = release;
      syslog(LOG_DEBUG, "Package - release: %s", release.c_str());
   } else {
      syslog(LOG_INFO, "Package - setRelease() - package contains no valid release!");
      return false;
   }

   return true;
}


/**
 *  @param perms
 */
bool Package::setPerms(string perms) {
   syslog(LOG_DEBUG, "Package - setPerms()...");

   // validate perms
   if(RE(PACKAGE_REGEX_PERMS).FullMatch(perms)) {
      // TODO parse Perms and create for each Perm a new object of type Perm (this class/struct hat to be created as well)
      this->perms = perms;
      syslog(LOG_DEBUG, "Package - perms: %s", perms.c_str());
   } else {
      syslog(LOG_INFO, "Package - setPerms() - package contains no valid permissions!");
      return false;
   }

   return true;
}


/**
 *  @param deps dependencies of package
 */
bool Package::setDeps(string deps) {
   char *token;
   vector<string> tokens;
   Deps *dep;

   syslog(LOG_DEBUG, "Package - setDeps()...");

   // validate deps
   if(RE(PACKAGE_REGEX_DEPS).FullMatch(deps)) {
      token = strtok((char*)deps.c_str(), ";");
      while(token != NULL) {
         tokens.push_back(token);
         token = strtok(NULL, ";");
      } 

      for(vector<string>::iterator it = tokens.begin(); it < tokens.end(); ++it) {
         dep = new Deps(*it);
         if(dep->getBranch().empty()) {
            dep->setBranch(DEFAULT_BRANCH);
         }
         //this->deps.push_back(new Deps(*it));
         this->deps.push_back(dep);
      }
      
   } else {
      syslog(LOG_INFO, "Package - setDeps() - package contains no valid dependencies!");
      return false;
   }

   return true;
}


/**
 *  @param desc description of package
 */
bool Package::setDesc(string desc) {
   syslog(LOG_DEBUG, "Package - setDesc()...");

   //validate Desc
   if(RE(PACKAGE_REGEX_DESC).FullMatch(desc)) {
      this->desc = desc;
      syslog(LOG_DEBUG, "Package - desc: %s", desc.c_str());
   } else {
      syslog(LOG_INFO, "Package - setDesc() - package contains no valid description!");
      return false;
   }

   return true;
}


/**
 *  @param url url of package-source
 */
bool Package::setUrl(string url) {
   syslog(LOG_DEBUG, "Package - setUrl()...");

   //validate url
   if(RE(PACKAGE_REGEX_URL).FullMatch(url)) {
      this->url = url;
      syslog(LOG_DEBUG, "Package - url: %s", url.c_str());
   } else {
      syslog(LOG_INFO, "Package - setUrl() - package contains no valid url!");
      return false;
   }

   return true;
}


/**
 *  @param type type of package (bin, dev, doc, all)
 */
bool Package::setType(string type) {
   syslog(LOG_DEBUG, "Package - setType()...");

   // validate type
   if(RE(PACKAGE_REGEX_TYPE).FullMatch(type)) {
      this->type = type;
      syslog(LOG_DEBUG, "Package - type: %s", type.c_str());
   } else {
      syslog(LOG_INFO, "Package - setType() - package contains no valid type!");
      return false;
   }

   return true;
}


/**
 *  @param branch branch of package
 */
bool Package::setBranch(string branch) {
   syslog(LOG_DEBUG, "Package - setBranch()...");

   if(RE(PACKAGE_REGEX_BRANCH).FullMatch(branch)) {
      this->branch = branch;
      syslog(LOG_DEBUG, "Package - branch: %s", branch.c_str());

      return true;
   } else {
      this->branch = DEFAULT_BRANCH;
      syslog(LOG_INFO, "Package - setBranch() - no valid branch given - using default: %s", DEFAULT_BRANCH);

      return true;
   }
}


/**
 *  @param prefix prefix of package
 */
bool Package::setPrefix(string prefix) {
   syslog(LOG_DEBUG, "Package - setPrefix()...");

   // validate prefix and check if path exists on filesystem 
   // @TODO
   // if(RE(PACKAGE_REGEX_PREFIX).FullMatch(prefix) && fileExists(prefix)) {
   if(RE(PACKAGE_REGEX_PREFIX).FullMatch(prefix)) {
      this->prefix = prefix;
      syslog(LOG_DEBUG, "Package - prefix: %s", prefix.c_str());
   } else {
      this->prefix = DEFAULT_PREFIX;
      syslog(LOG_INFO, "Package - setPrefix():  no valid prefix given!");
      return false;
   }
 
   return true;
}


/**
 *  @param script
 */
bool Package::setPreRemove(string script) {
   syslog(LOG_DEBUG, "Package - setPreRemove()...");

   if(RE(PACKAGE_REGEX_SCRIPT).FullMatch(script)) {
      syslog(LOG_DEBUG, "Package - script: %s", script.c_str());
      return true;
   } else {
      syslog(LOG_INFO, "Package - setPreRemove(): script has invalid chars");
      return false;
   }
}


/**
 *  @param script
 */
bool Package::setPostRemove(string script) {
   syslog(LOG_DEBUG, "Package - setPostRemove()...");

   if(RE(PACKAGE_REGEX_SCRIPT).FullMatch(script)) {
      syslog(LOG_DEBUG, "Package - script: %s", script.c_str());
      return true;
   } else {
      syslog(LOG_INFO, "Package - setPostRemove(): script has invalid chars");
      return false;
   }
}


/**
 *  @param test test of package
 */
bool Package::setTest(string test) {
   syslog(LOG_DEBUG, "Package - setTest()...");

   if(RE(PACKAGE_REGEX_TEST).FullMatch(test)) {
      this->test = new Note(test);
      syslog(LOG_DEBUG, "Package - test: %s", test.c_str());
   } else {
      syslog(LOG_INFO, "Package - setTest() - no valid TestData given!");
      return false;
   }

   return true;
}

/**
 *  @param approval approval of package
 */
bool Package::setApproval(string approval) {
   syslog(LOG_DEBUG, "Package - setApproval()...");

   if(RE(PACKAGE_REGEX_APPROVAL).FullMatch(approval)) {
      this->approval = new Note(approval);
      syslog(LOG_DEBUG, "Package - approval: %s", approval.c_str());
   } else {
      syslog(LOG_INFO, "Package - setApproval() - no valid ApprovalData given!");
      return false;
   }

   return true;
}

/**
 *  @param impact impact of package
 */
bool Package::setImpact(string impact) {
   syslog(LOG_DEBUG, "Package - setImpact()...");

   if(RE(PACKAGE_REGEX_IMPACT).FullMatch(impact)) {
      this->impact = impact;
      syslog(LOG_DEBUG, "Package - impact: %s", impact.c_str());
   } else {
      syslog(LOG_INFO, "Package - setImpact() - no valid Impact given!");
      return false;
   }
 
   return true;
}

/**
 *  @param backout backout of package
 */
bool Package::setBackout(string backout) {
   syslog(LOG_DEBUG, "Package - setBackout()...");

   if(RE(PACKAGE_REGEX_BACKOUT).FullMatch(backout)) {
      this->backout = backout;
      syslog(LOG_DEBUG, "Package - backout: %s", backout.c_str());
   } else {
      syslog(LOG_INFO, "Package - setBackout() - no valid backout given!");
      return false;
   }
 
   return true;
}

/**
 *  @param changes changes of package
 */
bool Package::setChanges(string changes) {
   syslog(LOG_DEBUG, "Package - setChanges()...");

   if(RE(PACKAGE_REGEX_CHANGES).FullMatch(changes)) {
      this->changes = changes;
      syslog(LOG_DEBUG, "Package - changes: %s", changes.c_str());
   } else {
      syslog(LOG_INFO, "Package - setChanges() - no valid changes given!");
      return false;
   }
 
   return true;
}


/**
 *  @param text text hold list of pathnames
 */
bool Package::setFiles(string text) {
   string curpath;
   int    pos1 = 0;
   int    pos2 = 0;

   syslog(LOG_DEBUG, "Package - setFiles()...");

   pos2 = text.find('\n', pos1);
   while(pos2 != (int)string::npos) {
      curpath = text.substr(pos1, pos2 - pos1);

      // validate path
      if(RE(PACKAGE_REGEX_PATH).FullMatch(curpath)) {
         files.push_back(curpath);
      } else {
         syslog(LOG_INFO, "Package - setFiles() - Files contain invalid path!");
         return false;
      }

      if(pos2 == (int)string::npos && pos1 < (int)text.length()) {
         curpath = text.substr(pos1, text.length() - 1);
      }

      pos1 = pos2 + 1;
      pos2 = text.find('\n', pos1);
   }

   return true;
}

/**
 *  prints details of package
 */
void Package::printDetails() {
   int width = 10;

   syslog(LOG_INFO, "Package - printDetails()...");
   
   cout.width(width);
   cout << left << "Author"  << " : " << getAuthor()  << endl;
   cout.width(width);
   cout << left << "Owner"   << " : " << getOwner()   << endl;
   cout.width(width);
   cout << left << "Name"    << " : " << getName()    << endl;
   cout.width(width);
   cout << left << "Version" << " : " << getVersion() << endl;
   cout.width(width);
   cout << left << "Release" << " : " << getRelease() << endl;
   cout.width(width);
   cout << left << "Type"    << " : " << getType()    << endl;
   cout.width(width);
   cout << left << "Desc"    << " : " << getDesc()    << endl;
   cout.width(width);
   cout << left << "Url"     << " : " << getUrl()     << endl;
}


// ------- PRIVATE -------

/**
 *  @param s
 */
string Package::chomp(string s) {
   int pos;

   syslog(LOG_DEBUG, "Package - chomp()");

   pos = s.find_last_of("\n");
   s.erase(pos, strlen("\n"));

   return s;
}

/**
 *  @param text
 */
// split the String line by line and store the paths in vector<string> list
vector<string> Package::str2Vector(string text) {
   vector<string> list;
   string         curpath;
   int            pos1 = 0;
   int            pos2 = 0;

   pos2 = text.find('\n', pos1);
   while(pos2 != (int)string::npos) {
      curpath = text.substr(pos1, pos2 - pos1);
      list.push_back(curpath);

      if(pos2 == (int)string::npos && pos1 < (int)text.length()) {
         curpath = text.substr(pos1, text.length() - 1);
      }

      pos1 = pos2 + 1;
      pos2 = text.find('\n', pos1);
   }

   return list;
}

/**
 *  @param user
 *  @param group
 */
bool Package::setUID(string user, string group) {
   gid_t  groups[GROUPS];
   struct passwd *pwd;
   struct group  *grp;

   syslog(LOG_INFO, "Package - setUID()...");

   // check if user and group exists (/etc/passwd, /etc/groups)
   if((pwd = getpwnam(user.c_str())) == NULL) {
      syslog(LOG_ERR, "Package - User %s does not exist.", user.c_str());
   }

   if((grp = getgrnam(group.c_str())) == NULL) {
      syslog(LOG_ERR, "Package - Group %s does not exist.", group.c_str());
   }

   // user does not exist, so create one
   if(pwd == NULL) {
      int pid;
      int status;

      syslog(LOG_INFO, "Package - Creating User %s.", user.c_str());

      char *const pwd_params[] =  { USERADD, "-g", (char *)user.c_str(), "-G", INSTALL_GROUP, "-s", "/bin/false", "-d", "/usr/src", (char *)user.c_str(), NULL };
      char *const grp_params[] =  { GROUPADD, (char *)user.c_str(), NULL };

      // first create the group
      if((pid = fork()) == 0) {
          if(execv(GROUPADD, grp_params) == -1) {
             syslog(LOG_ERR, "Package - Could not create group %s: %s", group.c_str(), strerror(errno));
             exit(1);
          }
          exit(0);
      } else {
         waitpid(pid, &status, 0);
         if(WIFEXITED(status)) {
            if(WEXITSTATUS(status) == 1) {
               return false;
            }   
         }
      }

      // if the group was created successfully, also create the user
      if((pid = fork()) == 0) {
         if(execv(USERADD, pwd_params) == -1) {
            syslog(LOG_ERR, "Package - Could not create user: %s", strerror(errno));
            exit(1);
         }
         exit(0);
      } else {
         waitpid(pid, &status, 0);
         if(WIFEXITED(status)) {
            if(WEXITSTATUS(status) == 1) {
               return false;
            }   
         }
      }

      // because the new user has just been created, we have to reread the passwd-/group-entry
      if((pwd = getpwnam(user.c_str())) == NULL) {
         syslog(LOG_ERR, "Package - Could not get passwd entry: %s", strerror(errno));
      }
   
      if((grp = getgrnam(group.c_str())) == NULL) {
         syslog(LOG_ERR, "Package - Could not get group entry: %s", strerror(errno));
      }
   }

   // groups has the size 2, because we need the user to be in group install and it's own group
   groups[0] = grp->gr_gid;      // install group
   groups[1] = pwd->pw_gid;      // user's own group

   if(setgroups(GROUPS, groups) == -1) {
      syslog(LOG_ERR, "Package - Could not set groups: %s", strerror(errno));
      return false;
   }

   if(setgid(pwd->pw_gid) == -1) {
      syslog(LOG_ERR, "Package - Could not set GID: %s", strerror(errno));
      return false;
   }

   if(setuid(pwd->pw_uid) == -1) {
      syslog(LOG_ERR, "Package - Could not set UID: %s", strerror(errno));
      return false;
   }

   return true;
}

/**
 *  tests if string a has more tokens than string b delimited by delimiter
 *  @param a
 *  @param b
 */
bool compStrings(string a, string b) {
   size_t found;
   int    count_a   = 0;
   int    count_b   = 0;
   string delimiter = "/";

   found = a.find(delimiter);
   while(found != string::npos) {
      count_a++;
      found = a.find(delimiter, found + 1);
   }

   found = b.find(delimiter);
   while(found != string::npos) {
      count_b++;
      found = b.find(delimiter, found + 1);
   }

   return (count_a > count_b);
}

/**
 *  copy file "in" to file "out"
 *  @param in
 *  @param out
 */
bool Package::copy(string in, string out) {
   int  readBytes = 1;
   char buff[BUFFSIZE];

   syslog(LOG_DEBUG, "Package - copy()...");
   // check if infile exists
   ifstream *infile = new ifstream(in.c_str(), ios::in | ios::binary);
   if(! *infile) {
      syslog(LOG_INFO, "Package - ERROR: File %s not found", path.c_str());
      return false;
   }

   ofstream *outfile = new ofstream(out.c_str(), ios::out | ios::binary);
   // check if outfile exists
   if(! *outfile) {
      syslog(LOG_INFO, "Package - ERROR: File %s not found", ((string)TMPDIR  + basename(path.c_str())).c_str());
      return false;
   }

   while(readBytes != 0) {
      infile->read((char *)buff, BUFFSIZE);
      readBytes = infile->gcount();
      outfile->write((char *)buff, readBytes);
   }

   outfile->close();
   infile->close();
   delete(outfile);
   delete(infile);

   return true;
}

/**
 *  @param error
 */
void Package::setError(string error) {
   this->error = error;
}

/**
 *  @param errorcode
 */
void Package::setErrorNo(int errorcode) {
   this->errorcode = errorcode;
}



// -------------------------------------------------------------------
// PackageException
// -------------------------------------------------------------------

/**
 *  @param error
 */
PackageException::PackageException(int error) {
   this->error = error;
}

/**
 *  @param error
 *  @param msg
 */
PackageException::PackageException(int error, string msg) {
   this->error = error;
   this->msg   = msg;
}

/**
 *  @param msg
 */
PackageException::PackageException(string msg) {
   this->msg   = msg;
}

/**
 *  
 */
string PackageException::getMessage() {
   if(!msg.empty()) {
      return (msg);
   } else {
      return ((string)strerror(errno) + " " + msg);
   }
}
