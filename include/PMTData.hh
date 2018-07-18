//
// Created by zsoldos on 10/05/17.
//

#ifndef PMTANALYSIS_PMTDATA_H
#define PMTANALYSIS_PMTDATA_H

#include <fstream>

#include <TFile.h>
#include <TTree.h>
#include <TH1I.h>
#include <TGraph.h>

#include <convert.h>

#define MAXNUMCH 32 // Max nb of channels expected from DAQ

class PMTData {
 protected:
  // Input file
  TFile *inputFile;
  std::string dataFileName;
  // Output file
  TFile *outputFile;

  // Tree containing header
  TTree *treeHeader;
  // Tree containing data
  TTree *treePMTData;

  // Relevant structures
  oscheader_global *hGlobal;
  oscheader_ch *hCh[MAXNUMCH];
  UInt_t *data[MAXNUMCH];

  // Nb of entries to be processed
  long int nbEntries;

  int nbSamples[MAXNUMCH];

  // DAQ Ground base value;
  int GND;

  // Histograms of raw signal recorded by DAQ
  std::vector<TH1I*> hSignal[MAXNUMCH];

  // TGraph of raw signal recorded by DAQ
  std::vector<TGraph*> gSignal[MAXNUMCH];


 public:
  // Constructor
  PMTData(std::string inputUserArg);
  // Destructor
  ~PMTData();

  bool OpenPMTDataTTree();
  void CreateWaveformsHistogram();
  void WriteOutputFile() { outputFile->Write(); } ;

  ////////////////////////////////////// //
  // Various accessors and set functions //
  ////////////////////////////////////// //

  void setNbEntries(int nbEntries){ PMTData::nbEntries = nbEntries; }
  long int getNbEntries() const { return nbEntries; }

  void setGND(int GND){ PMTData::GND = GND; }
  int getGND() const { return GND; }

  TH1I* getSignalHistogram(int iCh, int iEntry) { return hSignal[iCh][iEntry];}

  oscheader_global* getGlobalHeader(){ return hGlobal; }
  oscheader_ch* getChannelHeader(int ch){ return hCh[ch]; }

  ////////////////////////////////////// //
  ////////////////////////////////////// //

  // Defining integration parameter in sample
  int CFDTime    = 5;
  int nTot       = 60;
  int nSize      = CFDTime+nTot;
};


#endif //PMTANALYSIS_PMTDATA_H
