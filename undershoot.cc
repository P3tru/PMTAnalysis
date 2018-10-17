#include <iostream>
#include <TApplication.h>

#include <PMTData.hh>
#include <PMTAnalyzer.hh>

#define MAXNUMFILES 10000 // MAX nb of files processed

static void show_usage(std::string name);
static void processArgs(TApplication *theApp,
                        int *nFiles,
                        std::vector<std::string>& sources,
                        std::string *outputPath=NULL);

int main(int argc, char *argv[]) {

  /* TODO :
   * Computing undershoot, so working with LED trigger and LED ON
   * I think we should process like this
   * 1/ Average all signal
   * 2/ Find signal tail, a few ADC channels after signal peak
   * 2/ Fit with exp
   * 4/ Recover and print tau parameter of exp
  */


  // Nb files processed
  int nFiles = 0;

  // Output directory
  std::string out = "";

  // Create TApp
  TApplication theApp("App", &argc, argv);

  // Reading user input parameters
  if (theApp.Argc() < 2) {
    show_usage(theApp.Argv(0));
    return 1;
  }
  std::vector <std::string> sources;
  processArgs(&theApp, &nFiles, sources, &out);

  PMTData *data[MAXNUMFILES];

  PMTAnalyzer *analysis[MAXNUMFILES];

  // INSERT FUNCTIONS BELOW
  /////////////////////////

  for(int iFile=0; iFile < nFiles;iFile++){

    std::cout << "Processing file " << sources[iFile] << std::endl;
    data[iFile] = new PMTData(sources[iFile]);
    data[iFile]->setOutputDir(out);

    analysis[iFile] = new PMTAnalyzer(data[iFile]);

    analysis[iFile]->ComputeUndershoot(0);
    analysis[iFile]->getMeanSignal(0)->Draw("l");

    std::cout << "undershoot = " << analysis[iFile]->getUndershoot() << " ns" << std::endl;
    
    
  }// end iFile

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

static void processArgs(TApplication *theApp,
                        int *nFiles,
                        std::vector<std::string>& sources,
                        std::string *outputPath){

  // Reading user input parameters
  if (theApp->Argc() < 2) {
    show_usage(theApp->Argv(0));
    exit(0);
  }

  for (int i = 1; i < theApp->Argc(); i++) {
    std::string arg = theApp->Argv(i);
    if ((arg == "-h") || (arg == "--help")) {
      show_usage(theApp->Argv(0));
      exit(0);
    } else if ((arg == "-o")) {
      *outputPath = theApp->Argv(++i);
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
