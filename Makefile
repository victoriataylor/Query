
# Filename: Makefile
# Description: The make file is to build up the crawler.
# Warning:  See how to use CFLAGS1 to build file.c
CC=gcc
CFLAGS= -Wall -pedantic -std=c11

UTILDIR=../util/
UTILFLAG=-ltseutil
UTILLIB=$(UTILDIR)libtseutil.a
UTILC=$(UTILDIR)file.c $(UTILDIR)web.c $(UTILDIR)hashtable.c $(UTILDIR)list.c
UTILH=$(UTILC:.c=.h)


query: query.c $(UTILLIB) queryengine_test
	$(CC) $(CFLAGS) -o query query.c -L$(UTILDIR) $(UTILFLAG)

queryengine_test: queryengine_test.c $(UTILLIB)
	$(CC) $(CFLAGS) -Wno-sign-compare -g -ggdb -o queryengine_test queryengine_test.c -L$(UTILDIR) $(UTILFLAG)

$(UTILLIB): $(UTILC) $(UTILH)
	cd $(UTILDIR); make;

clean: 
	rm -f query
