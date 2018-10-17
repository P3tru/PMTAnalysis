//
// Created by zsoldos on 10/05/17.
//

#ifndef PMTANALYSIS_PMTDATA_H
#define PMTANALYSIS_PMTDATA_H

#include <fstream>

#include <boost/filesystem.hpp>

#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TGraph.h>

#include <convert.h>

#define MAXNUMCH 32 // Max nb of channels expected from DAQ

class PMTData {
 protected:
  // Input file
  TFile *inputFile;
  std::string dataFileName;
  boost::filesystem::path p;
  // Output file
  TFile *outputFile;
  std::string outputDir;

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

  int nbCh;

  double tStep;

  // DAQ Ground base value;
  int GND;
  // ADC Channel to Volt conversion rate
  float voltConv;

  // Histograms of raw signal recorded by DAQ
  std::vector<TH1F*> hSignal[MAXNUMCH];

  // TGraph of raw signal recorded by DAQ
  std::vector<TGraph*> gSignal[MAXNUMCH];


 public:
  // Constructor
  PMTData(std::string inputUserArg);
  // Destructor
  ~PMTData();

  bool OpenPMTDataTTree();
  void CreateWaveformsHistogram();
  void ExtractGND(int iCh, UInt_t *data);
  void WriteOutputFile();

  ////////////////////////////////////// //
  // Various accessors and set functions //
  ////////////////////////////////////// //

  void setNbEntries(int nbEntries){ PMTData::nbEntries = nbEntries; }
  long int getNbEntries() const { return nbEntries; }

  void setNbSamples(int iCh, int nbSamples){ PMTData::nbSamples[iCh] = nbSamples; }
  int getNbSamples(int iCh) const { return nbSamples[iCh]; }

  void setNbCh(int nbCh){ PMTData::nbCh = nbCh; }
  int getNbCh() const { return nbCh; }

  void setGND(int GND){ PMTData::GND = GND; }
  int getGND() const { return GND; }

  void setOutputDir(std::string outputDir){ PMTData::outputDir = outputDir; }
  std::string getOutputDir() const { return outputDir; }

  double getTimeStep() const { return tStep;}

  float adc2V(UInt_t adc){ return ((int)adc-GND)*voltConv;};

  TH1* getSignalHistogram(int iCh, int iEntry) { return hSignal[iCh][iEntry];}

  oscheader_global* getGlobalHeader(){ return hGlobal; }
  oscheader_ch* getChannelHeader(int ch){ return hCh[ch]; }

  const char* getFileName(){ return dataFileName.c_str();}

  ////////////////////////////////////// //
  ////////////////////////////////////// //

  // Defining integration parameter in sample
  int CFDTime    = 5;
  int nTot       = 60;
  int nSize      = CFDTime+nTot;
};


#endif //PMTANALYSIS_PMTDATA_H
