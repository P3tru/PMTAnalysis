#include <iostream>
#include <PMTData.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TLine.h>

int main(int argc, char *argv[]) {

  TApplication theApp("App", &argc, argv);

  PMTData data(theApp.Argv(1));
  data.createTree();
  data.createSignalHistograms();
  data.createSignal();

  // INSERT FUNCTIONS BELOW
  /////////////////////////
  TCanvas *c1;

  TLine *line = new TLine(0,0,64,0);
  line->SetLineColor(kRed-4);
  line->SetLineWidth(1.);
  line->SetLineStyle(2);

  int nWindow=1;
  int nbDiv=2;
  int nbPlots=4;
  for(int j=0;j<nWindow;j++){
    c1= new TCanvas(Form("c%d",j),Form("c%d",j),800,600);
    c1->Divide(nbDiv,nbDiv);
    for(int i=0;i<(int)(nbDiv*nbDiv);i++){
      c1->cd(i+1);
      data.drawSignal(nbPlots,0,-100,600);
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