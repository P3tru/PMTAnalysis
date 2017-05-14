#include <iostream>
#include <PMTData.h>
#include <TApplication.h>
#include <TCanvas.h>

int main(int argc, char *argv[]) {

  TApplication theApp("App", &argc, argv);

  PMTData data(theApp.Argv(1));
  data.createTree();
  data.createSignalHistograms();

  // INSERT FUNCTIONS BELOW
  /////////////////////////
  TCanvas *c1;

  int nWindow=1;
  int nbDiv=2;
  int nbPlots=10;
  for(int j=0;j<nWindow;j++){
    c1= new TCanvas(Form("c%d",j),Form("c%d",j),1000,1000);
    c1->Divide(nbDiv,nbDiv);
    for(int i=0;i<(int)(nbPlots*nbPlots);i++){
      c1->cd(i+1);
      data.drawHist(nbPlots,0,-100,1000);
    }
  }
  /////////////////////////
  // ...

  std::cout << std::endl;
  std::cout << "Hit Ctrl+C to exit" << std::endl;
  theApp.Run(kTRUE);

  return 0;
}