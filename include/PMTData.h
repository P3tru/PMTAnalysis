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
#include <TGraph.h>
#include <TSpectrum.h>

class PMTData {
 private:
  // Data file to be open in ASCII from the DAQ
  std::ifstream inputFile;
  std::string inputFilePath;
  std::string inputFileName;

  // Output file
  TFile *file;
  // Tree containing data
  TTree *tree;

  // Signal histograms for analysis purposes
  std::vector<TH1I*> hSignal;
  std::vector<TH1I*> hBckg;

  // Ground Parameter for histograms
  int GND;

  // Dark Rates
  int darkRates;

  // Graphs of individual signals
  std::vector<TH1I*> hPulses;
  std::vector<TGraph*> gPulses;
  int nbPulses;

  // Undershoot Histogram
  TH1D *hUndershoot;

  // Define TSpectrum for searching signals
  std::vector<TSpectrum*> sSignal;

  // Define TSpectrum for searching afterpulses
  std::vector<TSpectrum*> sAfterPulses;
  TH1D *hAfterPulses;

  // QTOTS
  TH1D *hQtots;

 public:
  // Constructor
  PMTData();
  PMTData(PMTData const& copy);
  PMTData(std::string fname);
  // Destructor
  ~PMTData();

  void setInputFilePath(std::string f){ inputFilePath=f; }
  std::string getInputFilePath() const { return inputFilePath; }

  void setInputFileName(std::string f){ inputFileName=f; }
  std::string getInputFileName() const { return inputFileName; }

  void openDataFile(std::string fName){ inputFile.open(fName.c_str()); };

  void setROOTFile(TFile *f){ file=f; };
  TFile *getROOTFile() const { return file; };

  void setROOTTree(TTree *f){ tree=f; };
  TTree *getROOTTree() const { return tree; };

  void createTree();
  void createSignalHistograms();
  void drawHist(int nbPlots=0, int iP=0, double rangeMin=-100, double rangeMax=100);
  void computeDarkRates(int ampPE=0);
  void createSignal();
  void drawSignal(int nbPlots=0, int iP=0, double rangeMin=-100, double rangeMax=100);
  void computeAfterPulses();

  // Defining integration parameter in sample
  const int nbSamples=1024;
  const int CFDTime = 5;
  const int nTot   = 60;
  const int nSize = CFDTime+nTot;
  int nbEntries=0;
};


#endif //PMTANALYSIS_PMTDATA_H
