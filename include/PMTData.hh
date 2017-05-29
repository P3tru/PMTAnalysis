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
  std::string inputFilePath;
  std::string inputFileName;
  std::ifstream inputFile;

  // Output file
  TFile *file;
  // Tree containing data
  TTree *tree;

  // Number of DAQ
  int nbEntries;

  // DAQ Value for GND baseline
  int GND;

 public:
  // Constructor
  PMTData();
  PMTData(PMTData const& copy);
  PMTData(std::string fname);
  // Destructor
  ~PMTData();

  ////////////////////////////////////// //
  // Various accessors and set functions //
  ////////////////////////////////////// //

  void setInputFilePath(std::string f){ inputFilePath=f; }
  std::string getInputFilePath() const { return inputFilePath; }

  void setInputFileName(std::string f){ inputFileName=f; }
  std::string getInputFileName() const { return inputFileName; }

  void openDataFile(std::string fName){ inputFile.open(fName.c_str()); };
  void closeDataFile(){ inputFile.close(); };

  void setROOTFile(TFile *f){ file=f; };
  TFile *getROOTFile() const { return file; };

  void setROOTTree(TTree *f){ tree=f; };
  TTree *getROOTTree() const { return tree; };

  ////////////////////////////////////// //
  ////////////////////////////////////// //

  bool createTree();
  void saveTree(std::string outputPath="0");

};


#endif //PMTANALYSIS_PMTDATA_H
