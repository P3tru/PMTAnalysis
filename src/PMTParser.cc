//
// Created by zsoldos on 08/11/18.
//

#include <iostream>

#include "PMTParser.hh"

PMTParser::PMTParser(TApplication *App){

  theApp = App;
  nFiles = 0;
  pathOutputDir = "";

}

PMTParser::~PMTParser() {

  delete theApp;

}

void PMTParser::LaunchApp() {

  ProcessArgs();

}

void PMTParser::ShowUsage(std::string name) {

  std::cerr << "Usage: " << name << " <option(s)> SOURCES" << std::endl
            << "Options:\n"
            << "\t-h\tShow this help message\n"
            << "\t-o\tOutput path\n"
            << std::endl
            << "\tSOURCES\tSpecify input data file (.txt)\n"
            << std::endl;

}

void PMTParser::ProcessArgs() {

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
      pathOutputDir = theApp->Argv(++i);
    } else {
      if (i + 1 > theApp->Argc() && nFiles == 0) {
        std::cout << "NO SOURCES PROVIDED !" << std::endl;
        ShowUsage(theApp->Argv(0));
        exit(0);
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
    exit(0);
  }

}

void PMTParser::ShowSplashScreen() {

//  std::cout << std::endl;
//
//  std::cout << "   ___           __              __         " << std::endl;
//  std::cout << "  / _ \___ _____/ /__  _______ _/ /____ ___ " << std::endl;
//  std::cout << " / // / _ `/ __/  '_/ / __/ _ `/ __/ -_|_-< " << std::endl;
//  std::cout << "/____/\_,_/_/ /_/\_\ /_/  \_,_/\__/\__/___/ " << std::endl;
//
//  std::cout << std::endl;

}


