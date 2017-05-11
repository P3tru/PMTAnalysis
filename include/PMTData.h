//
// Created by zsoldos on 10/05/17.
//

#ifndef PMTANALYSIS_PMTDATA_H
#define PMTANALYSIS_PMTDATA_H


#include <TFile.h>
#include <TTree.h>
#include <sstream>
#include <fstream>
#include <TH1I.h>

class PMTData {
 private:
  // Data file to be open in ASCII from the DAQ
  std::ifstream inputFile;
  std::stringstream filename;

  // Output file
  TFile *file;
  // Tree containing data
  TTree *tree;

  // Signal histograms for analysis purposes
  std::vector<TH1I*> hSignal;
  std::vector<TH1I*> hBckg;

  // Parameter for histogram
  int GND;

  // Dark Rates
  int darkRates;

 public:
  // Constructor
  PMTData(const char* filename=NULL);
  // Destructor
  ~PMTData();

  void createTree();
  void createSignalHistograms();
  void drawHist(int nbPlots=0, int iP=0);
  void computeDarkRates(int ampPE=0);

  // Defining integration parameter in sample
  const int nbSamples=1024;
  const int CFDTime = 5;
  const int nTot   = 60;
  const int nSize = CFDTime+nTot;
  int nbEntries=0;
};


#endif //PMTANALYSIS_PMTDATA_H
