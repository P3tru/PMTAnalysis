#include <iostream>

#include <TApplication.h>
#include <TCanvas.h>

#include <PMTData.hh>
#include <PMTParser.hh>

#include <utils.h>

#define MAXNUMFILES 10000 // MAX nb of files processed


int main(int argc, char *argv[]) {

  // Create TApp
  TApplication theApp("App", &argc, argv);

  FlatTreeParser parserApp;
  parserApp.LaunchApp(&theApp);

  PMTData *data[MAXNUMFILES];

  // INSERT FUNCTIONS BELOW
  /////////////////////////

  for(int iFile=0; iFile < parserApp.getNFiles();iFile++){

    std::cout << "Processing file " << parserApp.getPathSources()[iFile] << std::endl;
    data[iFile] = new PMTData(parserApp,iFile);
    data[iFile]->WriteOutputFile(parserApp.getNbMaxEntries());

    //Set default signal channel recorded at Queen Mary
    const int iCh = 0;


  } // END FOR iFile


  /////////////////////////
  // ...

  std::cout << std::endl;
  std::cout << "Hit Ctrl+C to exit" << std::endl;
  theApp.Run(kTRUE);

  return 0;
}