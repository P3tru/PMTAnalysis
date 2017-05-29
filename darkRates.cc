#include <iostream>
#include <PMTData.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <csignal>
#include <TLine.h>

void sig_handler(int signo){
  if (signo == SIGINT)
    printf("received SIGINT\n");
}


int main(int argc, char *argv[]) {

  // Create ROOT Application
  TApplication theApp("App", &argc, argv);

  // Create ROOT Tree from ascii data and basic histograms for the signals
  PMTData data(theApp.Argv(1));
  data.createTree();
  data.createSignalHistograms();

  // INSERT FUNCTIONS BELOW
  /////////////////////////
  int ampPE=600;
  data.computeDarkRates(ampPE);

  TCanvas *c1;
  TLine *line = new TLine(0,ampPE,1024,ampPE);
  line->SetLineColor(kRed-4);
  line->SetLineWidth(1.);
  line->SetLineStyle(2);

  int nWindow=5;
  int nbDiv=5;
  int nbPlots=10;
  for(int j=0;j<nWindow;j++){
    c1= new TCanvas(Form("c%d",j),Form("c%d",j),1200,1200);
    c1->Divide(nbDiv,nbDiv);
    for(int i=0;i<(int)(nbPlots*nbPlots);i++){
      c1->cd(i+1);
      data.drawHist(nbPlots);
      line->Draw("same");
    }
  }
  /////////////////////////
  // ...

  // Keep the app running
  std::cout << std::endl;
  std::cout << "Hit Ctrl+C to exit" << std::endl;
  theApp.Run(kTRUE);

  return 0;
}