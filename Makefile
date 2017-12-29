# Makefile for "CreateBlastDB" C++ shell program
#
# Author: Matt Preston (website: matthewpreston.github.io)
# Created On: Oct 14, 2016
# Revised On: Oct 25, 2016 - Added boost::filesystem for USAGE printout

DEBUG = -g
CXX = g++
CXXFLAGS = -Wall $(DEBUG)
LDFLAGS = -L/usr/lib/x86_64-linux-gnu \
		  -lboost_filesystem \
		  -lboost_program_options \
		  -lboost_system
OBJECTS = CreateBlastDB.o HelperFunctions.o Taxonomy.o

all: CreateBlastDB

CreateBlastDB: $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)
CreateBlastDB.o: HelperFunctions.hpp HelperFunctions.tpp Taxonomy.hpp Taxonomy.tpp
HelperFunctions.o: HelperFunctions.hpp HelperFunctions.tpp
Taxonomy.o: Taxonomy.hpp Taxonomy.tpp HelperFunctions.hpp HelperFunctions.tpp

.PHONY: all clean
clean:
	$(RM) CreateBlastDB $(OBJECTS)
