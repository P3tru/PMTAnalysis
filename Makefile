# Basic Makefile

### Compilers
CC  = gcc
CXX = g++

DEBUG_LEVEL     = -g
EXTRA_CCFLAGS   = -W -Wall -std=c++11
CPPFLAGS        = $(DEBUG_LEVEL) $(EXTRA_CCFLAGS)
CCFLAGS         = $(CPPFLAGS)

RM = rm -f

SRCDIR := src
INCDIR := include

### ROOT
ROOTCFLAGS := $(shell root-config --cflags) -DUSE_ROOT -fPIC
ROOTLIBS   := $(shell root-config --libs) -lSpectrum
#USERLIBS   := -lUtils

CPPFLAGS  += -I$(ROOTSYS)/include -I$(INCDIR) $(ROOTCFLAGS) 
EXTRALIBS  = $(ROOTLIBS)
#EXTRALIBS += $(USERLIBS)

SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(subst .cpp,.o,$(SRCS))

all: PMTAnalysis

PMTAnalysis: main.o $(OBJS)
	$(CXX) $(CPPFLAGS) -o PMTAnalysis main.cpp $(OBJS) $(ROOTLIBS)
	$(RM) main.o $(OBJS)

showPlots: showPlots.o $(OBJS)
	$(CXX) $(CPPFLAGS) -o showPlots showPlots.cpp $(OBJS) $(ROOTLIBS)
	$(RM) showPlots.o $(OBJS)

darkRates: darkRates.o $(OBJS)
	$(CXX) $(CPPFLAGS) -o darkRates darkRates.cpp $(OBJS) $(ROOTLIBS)
	$(RM) darkRates.o $(OBJS)

afterPulses: afterPulses.o $(OBJS)
	$(CXX) $(CPPFLAGS) -o afterPulses afterPulses.cpp $(OBJS) $(ROOTLIBS)
	$(RM) afterPulses.o $(OBJS)

undershoot: undershoot.o $(OBJS)
	$(CXX) $(CPPFLAGS) -o undershoot undershoot.cpp $(OBJS) $(ROOTLIBS)
	$(RM) undershoot.o $(OBJS)

showSignals: showSignals.o $(OBJS)
	$(CXX) $(CPPFLAGS) -o showSignals showSignals.cpp $(OBJS) $(ROOTLIBS)
	$(RM) showSignals.o $(OBJS)

clean:
	$(RM) $(OBJS) PMTAnalysis showPlots darkRates afterPulses undershoot showSignals
