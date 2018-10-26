#include <iostream>
#include <TApplication.h>
#include <TCanvas.h>
#include <TRandom3.h>

#include <PMTData.hh>

#define MAXNUMFILES 10000 // MAX nb of files processed

static void show_usage(std::string name);
static void processArgs(TApplication *theApp, int *nFiles, std::vector<std::string>& sources);

int main(int argc, char *argv[]) {

  // THIS IS A SMALL EXAMPLE ON HOW TO ACCESS AND PLOT ONE HISTOGRAM FROM PMTData
  // This could go in a PMTData method for example, showing histogram

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

  TCanvas *c1 = new TCanvas("c1","c1",1200,800);
  TRandom3 *r = new TRandom3(0);

  const int nbDiv=4;
  c1->Divide(nbDiv,nbDiv);
  for(int iC=0; iC<nbDiv*nbDiv; iC++){
    c1->cd(iC+1);
    int iPlot = r->Uniform(data[0]->getNbEntries());
    data[0]->getSignalHistogram(0,iPlot)->Draw();
//    data[0]->getSignalHistogram(0,iPlot)->Fit("pol0");
  }

  /////////////////////////
  // ...

  std::cout << std::endl;
  std::cout << "Hit Ctrl+C to exit" << std::endl;
  theApp.Run(kTRUE);

  return 0;
}

static void show_usage(std::string name){
  std::cerr << "Usage: " << name << " <option(s)> SOURCES" << std::endl
            << "Options:\n"
            << "\t-h\tShow this help message\n"
            << "\t-o\tOutput path\n"
            << std::endl
            << "\tSOURCES\tSpecify input data file (.txt)\n"
            << std::endl;
}

static void processArgs(TApplication *theApp, int *nFiles, std::vector<std::string>& sources){

  // Reading user input parameters
  if (theApp->Argc() < 2) {
    show_usage(theApp->Argv(0));
    exit(0);
  }

  std::string outputPath = "0";

  for (int i = 1; i < theApp->Argc(); i++) {
    std::string arg = theApp->Argv(i);
    if ((arg == "-h") || (arg == "--help")) {
      show_usage(theApp->Argv(0));
      exit(0);
    } else if ((arg == "-o")) {
      outputPath = theApp->Argv(++i);
    } else {
      if (i + 1 > theApp->Argc() && *nFiles == 0) {
        std::cout << "NO SOURCES PROVIDED !" << std::endl;
        show_usage(theApp->Argv(0));
        exit(0);
      } else {
        std::cout << "Add " << arg << " to sources" << std::endl;
        sources.push_back(arg);
        (*nFiles)++;
      }
    }
  }


  if (nFiles == 0) {
    std::cout << "NO SOURCES DETECTED !" << std::endl;
    show_usage(theApp->Argv(0));
    exit(0);
  }

}
