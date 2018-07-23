//
// Created by zsoldos on 18/07/18.
//

#ifndef PMTANALYSIS_PMTANALYZER_HH
#define PMTANALYSIS_PMTANALYZER_HH

#include <fstream>

#include <TFile.h>
#include <TTree.h>
#include <TH1I.h>
#include <TGraph.h>

#include <PMTData.hh>

#define MAXNUMCH 32 // Max nb of channels expected from DAQ

class PMTAnalyzer {
 protected:
  // Associated data
  PMTData* data;

  // Histograms of mean signal
  TH1F* meanSignal[MAXNUMCH];

  // Position of the peak in the mean signal (extremum in 4ns unit)
  int peakPos;

 
 public:
  // Constructor
  PMTAnalyzer(PMTData *inputdata);
  // Destructor
  ~PMTAnalyzer();

  void CreateMeanSignal();
  //void findPeak();
  
  ////////////////////////////////////// //
  // Various accessors and set functions //
  ////////////////////////////////////// //

  TH1F* getMeanSignal(int iCh){return meanSignal[iCh];}

  int getPeakPos() const {return peakPos;}  
};

#endif //PMTANALYSIS_PMTANALYZER_HH
