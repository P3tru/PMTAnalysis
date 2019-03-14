//
// Created by zsoldos on 10/05/17.
//

#ifndef PMTANALYSIS_PMTDATA_H
#define PMTANALYSIS_PMTDATA_H

#include <fstream>

#include <boost/filesystem.hpp>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TGraph.h>

#include "PMTParser.hh"

#include <convert.h>

#define MAXNUMCH 32 // Max nb of channels expected from DAQ

class PMTData {
 protected:
  // Input file
  int iFile;
  TFile *inputFile;
  std::string dataFileName;
  boost::filesystem::path p;
  bool isInputFileFlat;
  // Output file
  std::vector<TFile *> outputFile;
  std::string outputDir;
  bool writeOutput;

  // Tree containing header
  TTree *treeHeader;
  // Tree containing data
  TTree *treePMTData;

  // Relevant structures
  oscheader_global *hGlobal;
  oscheader_ch *hCh[MAXNUMCH];
  UInt_t *data[MAXNUMCH];

  // Nb of entries to be processed
  unsigned long nbEntries;
  unsigned long nbSamples[MAXNUMCH];
  unsigned  int nbCh;
  double tStep;

  // DAQ Ground base value;
  int GND;
  TH1D *hGND;
  // ADC Channel to Volt conversion rate
  double voltConv;

  // Histograms of raw signal recorded by DAQ
  TH1I *hRAWSignal;
  std::vector< std::vector<TH1D*> > hSignal;

  // TGraph of raw signal recorded by DAQ
  std::vector< std::vector<TGraph*> > gSignal;


 public:
  // Constructor
  PMTData(PMTParser const& parser, int indexFile);
  // Destructor
  ~PMTData();

  void SetGNDHist(unsigned int nbBinsGND=100, int minGND=8150, int maxGND=8250);
  void SetDummyRawHistogram(int iCh);

  bool OpenPMTDataTTree();
  bool OpenPMTDataFlatTree();

  void CreateWaveformsHistogram();

  void ExtractGND(int iCh, UInt_t *data);
  void SetTGraphSignals(int iCh, int iEntry);

  void SetOutputFile(int iFile = 0);
  void WriteOutputFile(int nbMaxEntriesPerFile = -1);


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

  double getTimeStep() const { return tStep;} // IN NANOSEC !!!
  double getTimeStepSEC() const { return tStep*1e-9;} // IN NANOSEC !!!

  double adc2V(UInt_t adc){ return ((int)adc-GND)*voltConv;};

  TGraph* getSignalGraph(int iCh, int iEntry) { return gSignal[iCh][iEntry];}

  TH1D* getMeanGND(){return hGND;}

  // const std::vector<TH1D *> *getHSignal() const {
  //   return hSignal;
  // }
  const std::vector<std::vector<TH1D *>> &getHSignal() const {
    return hSignal;
  }

  oscheader_global* getGlobalHeader(){ return hGlobal; }
  oscheader_ch* getChannelHeader(int ch){ return hCh[ch]; }

  const char* getFileName(){ return dataFileName.c_str();}

  bool isWriteOutput() const {
    return writeOutput;
  }
  void setWriteOutput(bool writeOutput) {
    PMTData::writeOutput = writeOutput;
  }
  int getIFile() const {
    return iFile;
  }
  void setIFile(int iFile) {
    PMTData::iFile = iFile;
  }

  bool isIsInputFileFlat() const {
    return isInputFileFlat;
  }
  void setIsInputFileFlat(bool isInputFileFlat) {
    PMTData::isInputFileFlat = isInputFileFlat;
  }

  ////////////////////////////////////// //
  ////////////////////////////////////// //

};


#endif //PMTANALYSIS_PMTDATA_H
