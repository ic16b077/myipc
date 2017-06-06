##
## @file Makefile
## Betriebsysteme myipc Makefile
## Beispiel 3
##
## @author Aleksandar Pavlovic, ic16b056
## @author Michael Käfer ic16b077
## @author Johanna Hoffmann ic16b083
## @date 2017/06/03
##
## @version $Revision: 1.0 $
##
## Last Modified: $Author: ic16b056 $
##

##
## ------------------------------------------------------------- variables --
##

CC=gcc52
#MICHI ZUM TESTEN, DA ZUHAUSE KEIN gcc52
#CC=gcc
CFLAGS=-DDEBUG -Wall -Werror -Wextra -Wstrict-prototypes -pedantic -fno-common -O3 -g -std=gnu11
CP=cp
CD=cd
MV=mv
GREP=grep
DOXYGEN=doxygen

LIB_PATH?=/usr/local/lib

OBJECTS=myipclib.o

EXCLUDE_PATTERN=footrulewidth

##
## ----------------------------------------------------------------- rules --
##

%.o : %.c
	$(CC) $(CFLAGS) -c $<

##
## --------------------------------------------------------------- targets --
##

all: sender empfaenger

sender: $(OBJECTS) sender.o
	$(CC) $(CFLAGS) -o $@ $^ -L$(LIB_PATH) -lsem182

empfaenger: $(OBJECTS) empfaenger.o
	$(CC) $(CFLAGS) -o $@ $^ -L$(LIB_PATH) -lsem182

.PHONY: clean

clean:
	$(RM) *.o *~ empfaenger sender

doc: html pdf

html:
	$(DOXYGEN) doxygen.dcf

pdf: html
	$(CD) doc/pdf; \
	$(MV) refman.tex refman_save.tex; \
	$(GREP) -v $(EXCLUDE_PATTERN) refman_save.tex > refman.tex; \
	$(RM) refman_save.tex; \
	make; \
	$(MV) refman.pdf refman.save; \
	$(RM) *.pdf *.html *.tex *.aux *.sty *.log *.eps *.out *.ind *.idx \
	      *.ilg *.toc *.tps Makefile; \
	$(MV) refman.save refman.pdf

##
## ---------------------------------------------------------- dependencies --
##

##
## =================================================================== eof ==
##
