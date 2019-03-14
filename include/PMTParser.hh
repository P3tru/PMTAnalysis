//
// Created by zsoldos on 08/11/18.
//

#ifndef PMTANALYSIS_PMTPARSER_HH
#define PMTANALYSIS_PMTPARSER_HH

#define MAXNUMCH 32 // Max nb of channels expected from DAQ

#include <yaml-cpp/yaml.h>

#include <TApplication.h>

class PMTParser{

 protected:

  // Number of Input files
  int nFiles;

  // Array of file paths
  std::vector<std::string> pathSources;
  // String of output directory for output file
  std::string outputDir;

  // Flag if output file is created
  bool writeOutput;

  // Flag if input data is flat and not PMTTree
  bool isDataFlat;

  // Request number of max events per file
  int nbMaxEntries;

  // Params for YAML file parsing
  std::string configFilename;
  YAML::Node config;

  // Items to parse from YAML file
  unsigned int nbCh;
  unsigned long nbSignals;
  unsigned long nbSamples[MAXNUMCH];
  double tStep;

  unsigned int nbBinsGND;
  int minGND;
  int maxGND;

 public:

  ////////////////////////////////
  // CONSTRUCTOR AND DESTRUCTOR //
  ////////////////////////////////

  PMTParser();
  // Destructor
  ~PMTParser();

  ////////////////////
  // PUBLIC METHODS //
  ////////////////////

  virtual void LaunchApp(TApplication *theApp);

  virtual void ShowUsage(std::string name);

  virtual void ProcessArgs(TApplication *theApp);

  virtual void ReadYAML();

  virtual void ShowSplashScreen();

  ///////////////////////
  // GETTER AND SETTER //
  ///////////////////////

  int getNFiles() const {
    return nFiles;
  }
  void setNFiles(int nFiles) {
    PMTParser::nFiles = nFiles;
  }

  const std::vector<std::string> &getPathSources() const {
    return pathSources;
  }
  void setPathSources(const std::vector<std::string> &pathSources) {
    PMTParser::pathSources = pathSources;
  }

  const std::string &getOutputDir() const {
    return outputDir;
  }
  void setoutputDir(const std::string &outputDir) {
    PMTParser::outputDir = outputDir;
  }

  bool isWriteOutput() const {
    return writeOutput;
  }
  void setWriteOutput(bool writeOutput) {
    PMTParser::writeOutput = writeOutput;
  }

  unsigned int getNbBinsGND() const {
    return nbBinsGND;
  }
  void setNbBinsGND(unsigned int nbBinsGND) {
    PMTParser::nbBinsGND = nbBinsGND;
  }

  int getMinGND() const {
    return minGND;
  }
  void setMinGND(int minGND) {
    PMTParser::minGND = minGND;
  }

  int getMaxGND() const {
    return maxGND;
  }
  void setMaxGND(int maxGND) {
    PMTParser::maxGND = maxGND;
  }

  bool isIsDataFlat() const {
    return isDataFlat;
  }
  void setIsDataFlat(bool isDataFlat) {
    PMTParser::isDataFlat = isDataFlat;
  }

  unsigned int getNbCh() const {
    return nbCh;
  }
  void setNbCh(unsigned int nbCh) {
    PMTParser::nbCh = nbCh;
  }

  unsigned long getNbSignals() const {
    return nbSignals;
  }
  void setNbSignals(unsigned long nbSignals) {
    PMTParser::nbSignals = nbSignals;
  }

  const unsigned long *getNbSamples() const {
    return nbSamples;
  }

  double getTStep() const {
    return tStep;
  }
  void setTStep(double tStep) {
    PMTParser::tStep = tStep;
  }

  int getNbMaxEntries() const {
    return nbMaxEntries;
  }
  void setNbMaxEntries(int nbMaxEntries) {
    PMTParser::nbMaxEntries = nbMaxEntries;
  }

};

class DarkRatesParser : public PMTParser{

 protected:
  // User-defineed threshold for Q dark pulse
  unsigned int threshold;

  // Integration parameters
  unsigned int nTot;
  unsigned int CFDTime;
  unsigned int nTail;
  // Define sliding window params
  unsigned int nWindow; // 16 samples -> 64ns

 public:

  ////////////////////////////////
  // CONSTRUCTOR AND DESTRUCTOR //
  ////////////////////////////////

  DarkRatesParser();

  ///////////////////////
  // GETTER AND SETTER //
  ///////////////////////

  // User-defineed threshold for Q dark pulse
  unsigned int getThreshold() const {
    return threshold;
  }
  void setThreshold(unsigned int threshold) {
    DarkRatesParser::threshold = threshold;
  }

  // Integration parameters
  unsigned int getNTot() const {
    return nTot;
  }
  void setNTot(unsigned int nTot) {
    DarkRatesParser::nTot = nTot;
  }
  unsigned int getCFDTime() const {
    return CFDTime;
  }
  void setCFDTime(unsigned int CFDTime) {
    DarkRatesParser::CFDTime = CFDTime;
  }
  unsigned int getNTail() const {
    return nTail;
  }
  void setNTail(unsigned int nTail) {
    DarkRatesParser::nTail = nTail;
  }
  // Define sliding window params
  unsigned int getNWindow() const {
    return nWindow;
  }
  void setNWindow(unsigned int nWindow) {
    DarkRatesParser::nWindow = nWindow;
  }

  ////////////////////
  // PUBLIC METHODS //
  ////////////////////

  void ShowUsage(std::string name);

  void ShowSplashScreen();

  void ProcessArgs(TApplication *theApp);

  void ReadYAML();

};

class FlatTreeParser : public PMTParser{

 protected:

 public:

  ////////////////////////////////
  // CONSTRUCTOR AND DESTRUCTOR //
  ////////////////////////////////

  FlatTreeParser();

  ///////////////////////
  // GETTER AND SETTER //
  ///////////////////////


  ////////////////////
  // PUBLIC METHODS //
  ////////////////////

  void ShowUsage(std::string name);

  void LaunchApp(TApplication *theApp);

  void ProcessArgs(TApplication *theApp);

  void ShowSplashScreen();

};

#endif //PMTANALYSIS_PMTPARSER_HH
