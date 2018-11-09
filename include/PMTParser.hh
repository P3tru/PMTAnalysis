//
// Created by zsoldos on 08/11/18.
//

#ifndef PMTANALYSIS_PMTPARSER_HH
#define PMTANALYSIS_PMTPARSER_HH

#include <TApplication.h>

class PMTParser{

 protected:

  TApplication *theApp;

  int nFiles;

  std::vector<std::string> pathSources;
  std::string pathOutputDir;

 public:

  ////////////////////////////////
  // CONSTRUCTOR AND DESTRUCTOR //
  ////////////////////////////////

  PMTParser(TApplication *App);
  // Destructor
  ~PMTParser();

  ////////////////////
  // PUBLIC METHODS //
  ////////////////////

  void LaunchApp();

  void ShowUsage(std::string name);

  void ShowSplashScreen();

  void ProcessArgs();

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
  const std::string &getPathOutputDir() const {
    return pathOutputDir;
  }
  void setPathOutputDir(const std::string &pathOutputDir) {
    PMTParser::pathOutputDir = pathOutputDir;
  }

};

#endif //PMTANALYSIS_PMTPARSER_HH
