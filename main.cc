#include <iostream>
#include <TApplication.h>

#include <PMTData.hh>

#include "utils.h"

#define MAXNUMFILES 10000 // MAX nb of files processed

static void show_usage(std::string name);
static void processArgs(TApplication *theApp,
                        int *nFiles,
                        std::vector<std::string>& sources,
                        std::string *outputPath=NULL);

int main(int argc, char *argv[]) {

  // Nb files processed
  int nFiles = 0;

  // Create TApp
  TApplication theApp("App", &argc, argv);

  // Reading user input parameters
  if (theApp.Argc() < 2) {
    show_usage(theApp.Argv(0));
    return 1;
  }
  std::vector <std::string> sources;
  processArgs(&theApp, &nFiles, sources);

  PMTData *data[MAXNUMFILES];

  // INSERT FUNCTIONS BELOW
  /////////////////////////

  for(int iFile=0; iFile < nFiles;iFile++){

    std::cout << "Processing file " << sources[iFile] << std::endl;
    data[iFile] = new PMTData(sources[iFile]);

  }

  /////////////////////////
  // ...

  std::cout << std::endl;
  std::cout << "Hit Ctrl+C to exit" << std::endl;
  theApp.Run(kTRUE);

  return 0;
}
