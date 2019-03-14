//
// Created by zsoldos on 08/11/18.
//

#include <iostream>
#include <fstream>

#include "PMTParser.hh"

PMTParser::PMTParser(){

  isDataFlat=false;

  configFilename.clear();

  nFiles = 0;
  outputDir.clear();
  writeOutput = false;
  nbSignals = 0;
  nbCh = MAXNUMCH;
  for(int iCh=0; iCh<nbCh; iCh++){
    nbSamples[iCh] = 0;
  }
  tStep = 0;

  nbMaxEntries = -1;

}

PMTParser::~PMTParser() {

}

void PMTParser::LaunchApp(TApplication *theApp) {

  ShowSplashScreen();

  ProcessArgs(theApp);

  if(isDataFlat){

    std::cout << "You are trying to open a flat input." << std::endl;
    std::cout << "Looking for a YAML config file..." << std::endl;

    if(!configFilename.empty()){

      ReadYAML();

      std::cout << "Read config file SUCCESS" << std::endl;

    }

  }

}

void PMTParser::ShowUsage(std::string name) {

  std::cerr << "Usage: " << name << " <option(s)> SOURCES" << std::endl
            << "Options:\n"
            << "\t-h\tShow this help message\n"
            << "\t-w\tEnable output writing\n"
            << "\t-f\tInput a flat tree\n"
            << "\t-c\tYAML config file\n"
            << "\t-o\tOutput path\n"
            << std::endl
            << "\tSOURCES\tSpecify input data file (.txt)\n"
            << std::endl;

}

void PMTParser::ProcessArgs(TApplication *theApp) {

  // Reading user input parameters
  if (theApp->Argc() < 2) {
    ShowUsage(theApp->Argv(0));
    exit(0);
  }

  for (int i = 1; i < theApp->Argc(); i++) {
    std::string arg = theApp->Argv(i);
    if ((arg == "-h") || (arg == "--help")) {
      ShowUsage(theApp->Argv(0));
      exit(0);
    } else if ((arg == "-f")) {
      isDataFlat = true;
    } else if ((arg == "-c")) {
      configFilename = theApp->Argv(++i);
    } else if ((arg == "-w")) {
      writeOutput = true;
    } else if ((arg == "-o")) {
      outputDir = theApp->Argv(++i);
    } else {
      if (i + 1 > theApp->Argc() && nFiles == 0) {
        std::cout << "NO SOURCES PROVIDED !" << std::endl;
        ShowUsage(theApp->Argv(0));
        exit(EXIT_FAILURE);
      } else {
        std::cout << "Add " << arg << " to sources" << std::endl;
        pathSources.emplace_back(arg);
        nFiles++;
      }
    }
  }


  if (nFiles == 0) {
    std::cout << "NO SOURCES DETECTED !" << std::endl;
    ShowUsage(theApp->Argv(0));
    exit(EXIT_FAILURE);
  }

}

void PMTParser::ReadYAML() {

  config = YAML::LoadFile(configFilename);

  if(config["nbSignals"]){
    nbSignals = config["nbSignals"].as<unsigned long>();
  }

  nbCh = config["nbCh"].as<unsigned int>();

  for(int iCh=0; iCh<nbCh; iCh++){
    nbSamples[iCh] = config["nbSamples"].as<unsigned long>();
  }

  tStep = config["tStep"].as<double>();

  std::cout << "nbSignals:" << nbSignals << std::endl;
  std::cout << "nbCh:" << nbCh << std::endl;
  std::cout << "nbSamples:" << nbSamples[0] << std::endl;
  std::cout << "tStep:" << tStep << std::endl;

}


void PMTParser::ShowSplashScreen() {


}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////                     /////////////////////////////////
/////////////////////////  DARK RATES PARSER  /////////////////////////////////
/////////////////////////                     /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


DarkRatesParser::DarkRatesParser() : PMTParser() {

  threshold=0;

}


void DarkRatesParser::ShowUsage(std::string name) {

  std::cerr << "Usage: " << name << " <option(s)> SOURCES" << std::endl
            << "Options:\n"
            << "\t-h\t\tShow this help message\n"
            << "\t-t\t\tThreshold\n"
            << "\t-f [INPUTFILE]\tInput a flat tree\n"
            << "\t-c [CONFIGFILE]\tYAML config file\n"
            << "\t-o\t\tOutput path\n"
            << std::endl
            << "\tSOURCES\tSpecify input data file (.txt)\n"
            << std::endl;

}

void DarkRatesParser::ProcessArgs(TApplication *theApp) {

  // Reading user input parameters
  if (theApp->Argc() < 2) {
    ShowUsage(theApp->Argv(0));
    exit(0);
  }

  for (int i = 1; i < theApp->Argc(); i++) {
    std::string arg = theApp->Argv(i);
    if ((arg == "-h") || (arg == "--help")) {
      ShowUsage(theApp->Argv(0));
      exit(0);
    } else if ((arg == "-f")) {
      isDataFlat = true;
    } else if ((arg == "-c")) {
      configFilename = theApp->Argv(++i);
    } else if ((arg == "-t")) {
      threshold = static_cast<unsigned int>(atoi(theApp->Argv(++i)));
    } else if ((arg == "-o")) {
      outputDir = theApp->Argv(++i);
    } else {
      if (i + 1 > theApp->Argc() && nFiles == 0) {
        std::cout << "NO SOURCES PROVIDED !" << std::endl;
        ShowUsage(theApp->Argv(0));
        exit(EXIT_FAILURE);
      } else {
        std::cout << "Add " << arg << " to sources" << std::endl;
        pathSources.emplace_back(arg);
        nFiles++;
      }
    }
  }


  if (nFiles == 0) {
    std::cout << "NO SOURCES DETECTED !" << std::endl;
    ShowUsage(theApp->Argv(0));
    exit(EXIT_FAILURE);
  }

}

void DarkRatesParser::ReadYAML() {

  PMTParser::ReadYAML();

  nTot = config["nTot"].as<unsigned int>();
  CFDTime = config["CFDTime"].as<unsigned int>();
  nTail = config["nTail"].as<unsigned int>();
  // Define sliding window params
  nWindow = config["nWindow"].as<unsigned int>();

  std::cout << "nTot:" << nTot << std::endl;
  std::cout << "CFDTime:" << CFDTime << std::endl;
  std::cout << "nTail:" << nTail << std::endl;
  std::cout << "nWindow:" << nWindow << std::endl;


}

void DarkRatesParser::ShowSplashScreen() {

  std::cout << std::endl;

  std::cout << "   ___           __              __         "       << std::endl;
  std::cout << "  / _ \\___ _____/ /__  _______ _/ /____ ___ "      << std::endl;
  std::cout << " / // / _ `/ __/  '_/ / __/ _ `/ __/ -_|_-< "       << std::endl;
  std::cout << "/____/\\_,_/_/ /_/\\_\\ /_/  \\_,_/\\__/\\__/___/ " << std::endl;

  std::cout << std::endl;

}


///////////////////////////////////////////////////////////////////////////////
/////////////////////////                    //////////////////////////////////
/////////////////////////  FLAT TREE PARSER  //////////////////////////////////
/////////////////////////                    //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


FlatTreeParser::FlatTreeParser() : PMTParser() {

  writeOutput = true;
  nbMaxEntries = -1;

}

void FlatTreeParser::ShowUsage(std::string name) {

  std::cerr << "Usage: " << name << " <option(s)> SOURCES" << std::endl
            << "Options:\n"
            << "\t-h\tShow this help message\n"
            << "\t-m\tSet nbMaxEntries per output file\n"
            << "\t-o\tOutput path\n"
            << std::endl
            << "\tSOURCES\tSpecify input data file (.txt)\n"
            << std::endl;

}

void FlatTreeParser::LaunchApp(TApplication *theApp) {

  ShowSplashScreen();

  ProcessArgs(theApp);

}

void FlatTreeParser::ProcessArgs(TApplication *theApp) {

  // Reading user input parameters
  if (theApp->Argc() < 2) {
    ShowUsage(theApp->Argv(0));
    exit(0);
  }

  for (int i = 1; i < theApp->Argc(); i++) {
    std::string arg = theApp->Argv(i);
    if ((arg == "-h") || (arg == "--help")) {
      ShowUsage(theApp->Argv(0));
      exit(0);
    } else if ((arg == "-o")) {
      outputDir = theApp->Argv(++i);
    } else if ((arg == "-m")) {
      nbMaxEntries = atoi(theApp->Argv(++i));
    } else {
      if (i + 1 > theApp->Argc() && nFiles == 0) {
        std::cout << "NO SOURCES PROVIDED !" << std::endl;
        ShowUsage(theApp->Argv(0));
        exit(EXIT_FAILURE);
      } else {
        std::cout << "Add " << arg << " to sources" << std::endl;
        pathSources.emplace_back(arg);
        nFiles++;
      }
    }
  }


  if (nFiles == 0) {
    std::cout << "NO SOURCES DETECTED !" << std::endl;
    ShowUsage(theApp->Argv(0));
    exit(EXIT_FAILURE);
  }

}

void FlatTreeParser::ShowSplashScreen() {

  std::cout << std::endl;

  std::cout <<"   ______                __          ________      __     ______ "                      << std::endl;
  std::cout <<"  / ____/_______  ____ _/ /____     / ____/ /___ _/ /_   /_  __/_______  ___ "          << std::endl;
  std::cout <<" / /   / ___/ _ \\/ __ `/ __/ _ \\   / /_  / / __ `/ __/    / / / ___/ _ \\/ _ \\ "     << std::endl;
  std::cout <<"/ /___/ /  /  __/ /_/ / /_/  __/  / __/ / / /_/ / /_     / / / /  /  __/  __/  "        << std::endl;
  std::cout <<"\\____/_/   \\___/\\__,_/\\__/\\___/  /_/   /_/\\__,_/\\__/    /_/ /_/   \\___/\\___/ " << std::endl;

  std::cout << std::endl;

}

