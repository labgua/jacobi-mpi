#!/usr/bin/make -f

TARGET := JacobiGen JacobiSeq JacobiPar

#############################
# Local paths 				#
#############################

INCLUDES =../include
LOCAL_LIBS =../lib
OBJECTS=obj


#############################
# Compiler flags 			#
#############################

CC=gcc
CC_PAR=mpicc

CFLAGS= \
	$(SHARED_CFLAGS) \
	-g \
	-g3 \
	-Wall \
	-fPIC \
	-Wextra \
	-Wformat \
	-std=c99 \
	-Wno-unused-parameter \
	-Werror=format-security

# Find all .h files in includes directory
_DEPS = $(wildcard *.h)
DEPS = $(patsubst %, $(INCLUDES)/%, $(_DEPS))




all: $(TARGET) 

$(OBJECTS)/ProjectUtils.o: ProjectUtils.c $(DEPS)
	$(CC) -c ProjectUtils.c -o $(OBJECTS)/ProjectUtils.o $(CFLAGS)



$(OBJECTS)/JacobiGen.o: JacobiGen.c
	$(CC) -c JacobiGen.c -o $(OBJECTS)/JacobiGen.o $(CFLAGS)

$(OBJECTS)/JacobiSeq.o: JacobiSeq.c
	$(CC) -c JacobiSeq.c -o $(OBJECTS)/JacobiSeq.o $(CFLAGS)

$(OBJECTS)/JacobiPar.o: JacobiPar.c
	$(CC_PAR) -c JacobiPar.c -o $(OBJECTS)/JacobiPar.o $(CFLAGS)



JacobiGen: $(OBJECTS)/JacobiGen.o $(OBJECTS)/ProjectUtils.o
	$(CC) -o JacobiGen $(OBJECTS)/JacobiGen.o $(OBJECTS)/ProjectUtils.o $(CFLAGS) -lm
	cp JacobiGen ../JacobiGen

JacobiSeq: $(OBJECTS)/JacobiSeq.o $(OBJECTS)/ProjectUtils.o
	$(CC) -o JacobiSeq $(OBJECTS)/JacobiSeq.o $(OBJECTS)/ProjectUtils.o $(CFLAGS) -lm
	cp JacobiSeq ../JacobiSeq

JacobiPar: $(OBJECTS)/JacobiPar.o $(OBJECTS)/ProjectUtils.o
	$(CC_PAR) -o JacobiPar $(OBJECTS)/JacobiPar.o $(OBJECTS)/ProjectUtils.o $(CFLAGS) -lm
	cp JacobiPar ../JacobiPar



# Clean up
clean:
	rm -f $(TARGET) $(OBJECTS)/*.o *~ core $(INCDIR)/*~