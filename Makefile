# ****************************************************************
# * File    : Makefile
# * Version : 2018-03-26-01-WS
# * Created : 2018-03-26-01-WS
# * Author  : oxdeca.com
# ****************************************************************

#CFLAGS=-DNDEBUG -static-libgcc -O2 
#CFLAGS=-static-libgcc -Wall -O2 -L.
CFLAGS=-Wall -O2 -L.
DEBUG=-static-libgcc -Wall -O2 -ggdb -L.
#LDFLAGS=/usr/lib/libsqlite3.a /usr/lib/libarchive.a /usr/lib/libbz2.a /usr/lib/libz.a /usr/lib/libpcrecpp.a /usr/lib/libpcre.a
#LDFLAGS=/usr/lib/libsqlite3.a /usr/lib/libarchive.a /lib/libbz2.a /usr/lib/libz.a /usr/lib/libpcrecpp.a /usr/lib/libpcre.a /usr/lib/libpthread.so /usr/lib/libdl.so
LDFLAGS=-larchive -lsqlite3 -lpcre -lpcrecpp
DESTDIR=

pkgmgr: package.o note.o deps.o sqlite3.o report.o
	g++ $(CFLAGS) -o pkgmgr pkgmgr.cpp package.o note.o deps.o sqlite3.o report.o $(LDFLAGS)

report.o: report.cpp
	g++ $(CFLAGS) -c report.cpp

sqlite3.o: sqlite3.cpp
	g++ $(CFLAGS) -c sqlite3.cpp

deps.o: deps.cpp
	g++ $(CFLAGS) -c deps.cpp

note.o: note.cpp
	g++ $(CFLAGS) -c note.cpp

package.o: package.cpp
	g++ $(CFLAGS) -c package.cpp

clean:
	rm -f *.o pkgmgr

install:
	install -D -m 0750 -o pkgmgr -g install --strip pkgmgr      $(DESTDIR)/usr/bin/pkgmgr
	install -D -m 0640 -o pkgmgr -g install         functions   $(DESTDIR)/etc/pkgmgr/functions
