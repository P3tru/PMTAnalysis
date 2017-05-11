# Basic Makefile

### Compilers
CC = $(DATAHOME).local/gcc-5.4.0-install/bin/gcc
CXX = $(DATAHOME).local/gcc-5.4.0-install/bin/g++

DEBUG_LEVEL     = -g
EXTRA_CCFLAGS   = -W -Wall -std=c++11
CPPFLAGS        = $(DEBUG_LEVEL) $(EXTRA_CCFLAGS)
CCFLAGS         = $(CPPFLAGS)

RM = rm -f

SRCDIR := src
INCDIR := include

### ROOT
ROOTCFLAGS := $(shell root-config --cflags) -DUSE_ROOT -fPIC
ROOTLIBS   := $(shell root-config --libs)

CPPFLAGS  += -I$(ROOTSYS)/include -I$(INCDIR) $(ROOTCFLAGS) 
EXTRALIBS = $(ROOTLIBS)

SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(subst .cpp,.o,$(SRCS))

all: PMTAnalysis showPlots

PMTAnalysis: main.o $(OBJS)
	$(CXX) $(CPPFLAGS) -o PMTAnalysis main.cpp $(OBJS) $(ROOTLIBS)
	$(RM) main.o

showPlots: showPlots.o $(OBJS)
	$(CXX) $(CPPFLAGS) -o showPlots showPlots.cpp $(OBJS) $(ROOTLIBS)
	$(RM) showPlots.o

darkRates: darkRates.o $(OBJS)
	$(CXX) $(CPPFLAGS) -o darkRates darkRates.cpp $(OBJS) $(ROOTLIBS)
	$(RM) darkRates.o

clean:
	$(RM) $(OBJS) PMTAnalysis showPlots darkRates
