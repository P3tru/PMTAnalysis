#include <iostream>
#include <PMTData.h>
#include <TApplication.h>
#include <TCanvas.h>

int main(int argc, char *argv[]) {

  TApplication theApp("App", &argc, argv);

  PMTData data(theApp.Argv(1));
  data.createTree();
  data.createSignalHistograms();

  TCanvas *c1 = new TCanvas("c1","c1",800,600);
  data.drawHist();

  std::cout << std::endl;
  std::cout << "Hit Ctrl+C to exit" << std::endl;
  theApp.Run(kTRUE);

  return 0;
}