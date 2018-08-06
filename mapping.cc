#include <iostream>
#include <TApplication.h>

#include <PMTData.hh>
#include <PMTAnalyzer.hh>

#define MAXNUMFILES 10000 // MAX nb of files processed

static void show_usage(std::string name);
static void processArgs(TApplication *theApp, int *nFiles, std::vector<std::string>& sources);

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
  PMTAnalyzer *analysis[MAXNUMFILES];
  float step = 0.5; // in cm
  Double_t meanCharge[MAXNUMFILES];
  Double_t x[MAXNUMFILES], y[MAXNUMFILES];
  float xlow = 0, xup = 0;
  float ylow = 0, yup = 0;
  int nx, ny;

  TH2F* mapping;
  
  // INSERT FUNCTIONS BELOW
  /////////////////////////

  for(int iFile=0; iFile < nFiles;iFile++){

    std::cout << "Processing file " << sources[iFile] << std::endl;
    data[iFile] = new PMTData(sources[iFile]);
    analysis[iFile] = new PMTAnalyzer(data[iFile]);
    analysis[iFile]->ComputeIntegralMean();
    meanCharge[iFile] = analysis[iFile]->getMeanCharge();
    x[iFile] = data[iFile]->getPosition(0)*step;
    y[iFile] = data[iFile]->getPosition(1)*step;
    
    if(x[iFile] > xup) xup = x[iFile];
    if(x[iFile] < xlow) xlow = x[iFile];
    if(y[iFile] > yup) yup = y[iFile];
    if(y[iFile] < ylow) xlow = y[iFile];
    nx = (xup - xlow)/step;
    ny = (yup - ylow)/step;
  }
  mapping = new TH2F("Mapping", "Relative collected charge against position", nx, xlow - step/2, xup + step/2, ny, ylow - step/2, yup + step/2);
  mapping->FillN(nFiles, x, y, meanCharge);
  mapping->SetTitle("Relative collected charge against position");
  mapping->SetXTitle("x (cm)");
  mapping->SetXTitle("y (cm)");
  mapping->Draw();
  

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
