//
// Created by zsoldos on 16/05/17.
//

#ifndef PMTANALYSIS_PMTANAL_HH
#define PMTANALYSIS_PMTANAL_HH

#include <TH1I.h>
#include <TSpectrum.h>
#include <TGraph.h>
#include <TTree.h>

class PMTAnal {
 private:

  // Data name
  std::string fName;

  // Nb of entries to be processed
  int nbEntries;

  // DAQ Ground base value;
  int GND;

  // Histograms of raw signal recorded by DAQ
  std::vector<TH1I*> hSignal;

  // Define TSpectrum for searching pulses
  std::vector<TSpectrum*> sSignal;

  // Dark Rates
  int darkRates;

  // Histograms and Graphs of individual pulses
  std::vector<TH1I*> hPulses;
  std::vector<TGraph*> gPulses;
  int nbPulses;

  // QTOTS
  TH1D *hQtots;

  // Undershoot Histogram
  TH1D *hUndershoot;

  // Define TSpectrum for searching afterpulses
  std::vector<TSpectrum*> sAfterPulses;
  TH1D *hAfterPulses;

 public:

  PMTAnal();
  PMTAnal(std::string name="", int nE=0, int nP=0, int ground=0);
  ~PMTAnal();

  ////////////////////////////////////// //
  // Various accessors and set functions //
  ////////////////////////////////////// //

  void setName(std::string name){ fName = name; };
  std::string getName(){ return fName; };

  void setNbEntries(int n){ nbEntries = n; };
  int getNbEntries(){ return nbEntries; };

  void setNbPulses(int n){ nbPulses = n; };
  int getNbPulses(){ return nbPulses; };

  void setGND(int ground){ GND = ground; };
  int getGND(){ return GND; };

  int getDarkRates(){ return darkRates; }

  ////////////////////////////////////// //
  ////////////////////////////////////// //

  void createSignalHistograms(TTree *tree);
  void drawHist(int nbPlots=0, int iP=0, double rangeMin=-100, double rangeMax=100);
  void computeDarkRates(int ampPE=0);
  void createSignal();
  void drawSignal(int nbPlots=0, int iP=0, double rangeMin=-100, double rangeMax=100);
  void computeAfterPulses();
  void drawQtots();

  // Defining integration parameter in sample
  const int nbSamples=1024;
  const int CFDTime = 5;
  const int nTot   = 60;
  const int nSize = CFDTime+nTot;

};

#endif //PMTANALYSIS_PMTANAL_HH
