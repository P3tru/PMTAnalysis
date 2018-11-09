#include <iostream>

#include <TApplication.h>
#include <TCanvas.h>
#include <TColor.h>

#include <PMTData.hh>
#include <PMTAnalyzer.hh>
#include <PMTParser.hh>

#include "utils.h"

#define MAXNUMFILES 10000 // MAX nb of files processed


int main(int argc, char *argv[]) {

  // Create TApp
  TApplication theApp("App", &argc, argv);

  PMTParser parserApp(&theApp);
  parserApp.LaunchApp();

  PMTData *data[MAXNUMFILES];
  PMTAnalyzer *analysis[MAXNUMFILES];

  //////////////////
  // DEFINE STUFF //
  //////////////////

  TCanvas *c1;

  TColor *Color = new TColor();
  std::vector<int> palette = createColor(Color);


  // INSERT FUNCTIONS BELOW
  /////////////////////////

  for(int iFile=0; iFile < parserApp.getNFiles();iFile++){

    std::cout << "Processing file " << parserApp.getPathSources()[iFile] << std::endl;
    data[iFile] = new PMTData(parserApp.getPathSources()[iFile]);
    analysis[iFile] = new PMTAnalyzer(data[iFile]);

    /////////////////
    // DO ANALYSIS //
    /////////////////


  } // END FOR iFILE


  /////////////////////////
  // ...

  std::cout << std::endl;
  std::cout << "Hit Ctrl+C to exit" << std::endl;
  theApp.Run(kTRUE);

  return 0;
}
