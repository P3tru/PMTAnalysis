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

### BOOST
BOOSTCFLAGS := -I/usr/include/boost/
BOOSTLIBS   := -lboost_system -lboost_filesystem

CPPFLAGS  += -I$(ROOTSYS)/include -I$(INCDIR) -I$(HOME)/.local/include $(ROOTCFLAGS)
CPPFLAGS  +=  $(BOOSTCFLAGS)
EXTRALIBS  = $(ROOTLIBS)
EXTRALIBS += $(BOOSTLIBS)

SRCS = $(wildcard $(SRCDIR)/*.cc)
OBJS = $(subst .cc,.o,$(SRCS))

all: PMTAnalysis

PMTAnalysis: main.o $(OBJS)
	$(CXX) $(CPPFLAGS) -o PMTAnalysis main.cc $(OBJS) $(EXTRALIBS)
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

timing: timing.o $(OBJS)
	$(CXX) $(CPPFLAGS) -o timing timing.cc $(OBJS) $(EXTRALIBS)
	$(RM) timing.o $(OBJS)

fit: fit.o $(OBJS)
	$(CXX) $(CPPFLAGS) -o fit fit.cc $(OBJS) $(EXTRALIBS)
	$(RM) fit.o $(OBJS)
clean:
	$(RM) $(OBJS) PMTAnalysis showPlots darkRates afterPulses undershoot timing
