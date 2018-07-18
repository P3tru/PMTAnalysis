#include <iostream>
#include <TApplication.h>
#include <TCanvas.h>
#include <TLine.h>

#include <PMTData.hh>
#include <PMTAnal.hh>

#include <utils.h>

static void show_usage(std::string name){
  std::cerr << "Usage: " << name << " <option(s)> SOURCES" << std::endl
            << "Options:\n"
            << "\t-h\tShow this help message\n"
            << "\t-nW\tNb of windows where plots be displayed\n"
            << "\t-nDiv\tNb of division inside canvas\n"
            << "\t-nPlot\tNb of plots by divided canvas\n"
            << "\t-min\tMin range for drawing (default=-10)\n"
            << "\t-max\tMax range for drawing (default=1000)\n"
            << "\t-o\tOutput path\n"
            << std::endl
            << "\tSOURCES\tSpecify input data file (.txt)\n"
            << std::endl;
}

int main(int argc, char *argv[]) {

  // Parameters define by user
  int nWindow = 1;
  int nbDiv   = 2;
  int nbPlots = 1;
  int minRange=-10;
  int maxRange=1000;

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
  std::string outputPath = "0";
  for (int i = 1; i < theApp.Argc(); i++) {
    std::string arg = theApp.Argv(i);
    if ((arg == "-h") || (arg == "--help")) {
      show_usage(theApp.Argv(0));
      return 0;
    } else if ((arg == "-nW")) {
      nWindow = std::stoi(theApp.Argv(++i));
    } else if ((arg == "-nDiv")) {
      nbDiv = std::stoi(theApp.Argv(++i));
    } else if ((arg == "-nPlot")) {
      nbPlots = std::stoi(theApp.Argv(++i));
    } else if ((arg == "-min")) {
      minRange = std::stoi(theApp.Argv(++i));
    } else if ((arg == "-max")) {
      maxRange = std::stoi(theApp.Argv(++i));
    } else if ((arg == "-o")) {
      outputPath = theApp.Argv(++i);
    } else {
      if(i+1>theApp.Argc() && nFiles == 0){
        std::cout << "NO SOURCES PROVIDED !" << std::endl;
        show_usage(theApp.Argv(0));
        return 0;
      }else{
        std::cout << "Add " << arg << " to sources" << std::endl;
        sources.push_back(arg);
        nFiles++;
      }
    }
  }

  if(nFiles == 0){
    std::cout << "NO SOURCES DETECTED !" << std::endl;
    show_usage(theApp.Argv(0));
    return 0;
  }

  PMTData data[nFiles];
  PMTAnal anal[nFiles];

  for(int iFile=0; iFile < nFiles;iFile++){
    std::cout << "Processing file " << sources[iFile] << std::endl;
    data[iFile].setInputFilePath(sources[iFile]);
    data[iFile].setOutputFileName(outputPath);
    if(data[iFile].createTree()){
      std::cout << "Tree Created" << std::endl;

      ////////////////////////////////////////////////
      // Set minimal parameters for using anal methods
      ////////////////////////////////////////////////
      anal[iFile].setName(data[iFile].getInputFileName());
      anal[iFile].setGND(data[iFile].getGND());
      anal[iFile].setNbEntries(data[iFile].getNbEntries());

      anal[iFile].createSignalHistograms(data[iFile].getROOTTree());
//      anal[iFile].createSignal();

      ////////////////////////////////////////
      // Play with anal[iFile] methods here //
      ////////////////////////////////////////

      ////////////////////////////////////////////////
      // Save data tree in output ROOT file
      ////////////////////////////////////////////////
      data[iFile].saveTree();
      data[iFile].saveWaveform();

    } else {
      continue;
    }
  }

  // INSERT FUNCTIONS BELOW
  /////////////////////////
  TCanvas *c1;

  TLine *line = new TLine(0,0,64,0);
  line->SetLineColor(kRed-4);
  line->SetLineWidth(1.);
  line->SetLineStyle(2);

//  for(int j=0;j<nWindow;j++){
//    for(int iFile=0; iFile < nFiles;iFile++) {
//      c1 = new TCanvas(Form("cHist%d", j), Form("cHist%d", j), 1200, 900);
//      c1->Divide(nbDiv, nbDiv);
//      for (int i = 0; i < (int) (nbDiv * nbDiv); i++) {
//        c1->cd(i + 1);
//        anal[iFile].drawHist(nbPlots, 0, minRange, maxRange);
//        line->Draw("same");
//      }
//    }
//  }
//
//  for(int j=0;j<nWindow;j++){
//    for(int iFile=0; iFile < nFiles;iFile++) {
//      c1 = new TCanvas(Form("c%d", j), Form("c%d", j), 1200, 900);
//      c1->Divide(nbDiv, nbDiv);
//      for (int i = 0; i < (int) (nbDiv * nbDiv); i++) {
//        c1->cd(i + 1);
//        anal[iFile].drawSignal(nbPlots, 0, minRange, maxRange);
//        line->Draw("same");
//      }
//    }
//  }
//
//  c1 = new TCanvas("cQtots","cQtots",800,600);
//  for(int iFile=0; iFile < nFiles;iFile++) {
//    anal[iFile].drawQtots();
//  }
//
//  c1 = new TCanvas("cPeak","cPeak",800,600);
//  for(int iFile=0; iFile < nFiles;iFile++) {
//    anal[iFile].drawPeak();
//  }

  /////////////////////////
  // ...

  std::cout << std::endl;
  std::cout << "Hit Ctrl+C to exit" << std::endl;
  theApp.Run(kTRUE);

  return 0;
}