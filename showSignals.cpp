#include <iostream>
#include <PMTData.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TLine.h>

static void show_usage(std::string name){
  std::cerr << "Usage: " << name << " <option(s)> SOURCES" << std::endl
            << "Options:\n"
            << "\t-h\tShow this help message\n"
            << "\t-nW\tNb of windows where plots be displayed\n"
            << "\t-nDiv\tNb of division inside canvas\n"
            << "\t-nPlot\tNb of plots by divided canvas\n"
            << std::endl
            << "\tSOURCES\tSpecify input data file (.txt)\n"
            << std::endl;
}

int main(int argc, char *argv[]) {

  // Create TAPP
  TApplication theApp("App", &argc, argv);

  // Parameters define by user
  int nWindow = 1;
  int nbDiv   = 1;
  int nbPlots = 1;

  int nFiles = 0;

  // Reading user input parameters
  if (theApp.Argc() < 2) {
    show_usage(theApp.Argv(0));
    return 1;
  }
  std::vector <std::string> sources;
  for (int i = 1; i < theApp.Argc(); i++) {
    std::string arg = theApp.Argv(i);
    if ((arg == "-h") || (arg == "--help")) {
      show_usage(theApp.Argv(0));
      return 0;
    } else if ((arg == "-nW")) {
      nWindow = std::stoi(theApp.Argv(i++));
    } else if ((arg == "-nDiv")) {
      nbDiv = std::stoi(theApp.Argv(i++));
    } else if ((arg == "-nPlot")) {
      nbPlots = std::stoi(theApp.Argv(i++));
    } else {
      std::cout << "Add " << arg << " to sources" << std::endl;
      sources.push_back(arg);
      nFiles++;
    }
  }

  PMTData data[nFiles];

  for(int iFile=0; iFile < nFiles;iFile++){
    std::cout << "Processing file " << sources[iFile] << std::endl;
    data[iFile].setInputFilePath(sources[iFile]);
    data[iFile].createTree();
    data[iFile].createSignalHistograms();
    data[iFile].createSignal();
  }

  // INSERT FUNCTIONS BELOW
  /////////////////////////
  TCanvas *c1;

  TLine *line = new TLine(0,0,64,0);
  line->SetLineColor(kRed-4);
  line->SetLineWidth(1.);
  line->SetLineStyle(2);

  for(int j=0;j<nWindow;j++){
    c1= new TCanvas(Form("c%d",j),Form("c%d",j),800,600);
    c1->Divide(nbDiv,nbDiv);
    for(int i=0;i<(int)(nbDiv*nbDiv);i++){
      c1->cd(i+1);
      data[0].drawHist(nbPlots);
      line->Draw("same");
    }
  }
  /////////////////////////
  // ...

  std::cout << std::endl;
  std::cout << "Hit Ctrl+C to exit" << std::endl;
  theApp.Run(kTRUE);

  return 0;
}