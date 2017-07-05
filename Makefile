# Basic Makefile

### Compilers
CC  = $(DATAHOME)/.cluster/gcc-5.4.0-install/bin/gcc
CXX = $(DATAHOME)/.cluster/gcc-5.4.0-install/bin/g++

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
USERLIBS   := -L$(HOME)/.local/lib/ -lUtils
#USERLIBS   := -lUtils

CPPFLAGS  += -I$(ROOTSYS)/include -I$(INCDIR) -I$(HOME)/.local/include $(ROOTCFLAGS) 
EXTRALIBS  = $(ROOTLIBS)
EXTRALIBS += $(USERLIBS)

SRCS = $(wildcard $(SRCDIR)/*.cc)
OBJS = $(subst .cc,.o,$(SRCS))

all: PMTAnalysis

PMTAnalysis: main.o $(OBJS)
	$(CXX) $(CPPFL\AGS) -o PMTAnalysis main.cc $(OBJS) $(EXTRALIBS)
	$(RM) main.o $(OBJS)

showPlots: showPlots.o $(OBJS)
	$(CXX) $(CPPFLAGS) -o showPlots showPlots.cc $(OBJS) $(EXTRALIBS)
	$(RM) showPlots.o $(OBJS)

darkRates: darkRates.o $(OBJS)
	$(CXX) $(CPPFLAGS) -o darkRates darkRates.cc $(OBJS) $(EXTRALIBS)
	$(RM) darkRates.o $(OBJS)

afterPulses: afterPulses.o $(OBJS)
	$(CXX) $(CPPFLAGS) -o afterPulses afterPulses.cc $(OBJS) $(EXTRALIBS)
	$(RM) afterPulses.o $(OBJS)

undershoot: undershoot.o $(OBJS)
	$(CXX) $(CPPFLAGS) -o undershoot undershoot.cc $(OBJS) $(EXTRALIBS)
	$(RM) undershoot.o $(OBJS)

showSignals: showSignals.o $(OBJS)
	$(CXX) $(CPPFLAGS) -o showSignals showSignals.cc $(OBJS) $(EXTRALIBS)
	$(RM) showSignals.o $(OBJS)

clean:
	$(RM) $(OBJS) PMTAnalysis showPlots darkRates afterPulses undershoot showSignals
