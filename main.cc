#include <iostream>
#include <TApplication.h>
#include <TCanvas.h>

#include <PMTData.hh>

int main(int argc, char *argv[]) {

  // Create ROOT Application
  TApplication theApp("App", &argc, argv);

  // Create ROOT Tree from ascii data and basic histograms for the signals
  PMTData data(theApp.Argv(1));
  data.createTree();
  data.createSignalHistograms();

  // Keep the app running
  std::cout << std::endl;
  std::cout << "Hit Ctrl+C to exit" << std::endl;
  theApp.Run(kTRUE);

  return 0;
}