//
// Created by zsoldos on 18/07/18.
//

#ifndef PMTANALYSIS_PMTANALYZER_HH
#define PMTANALYSIS_PMTANALYZER_HH

#include <fstream>

#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TF1.h>

#include <TGraph.h>

#include <PMTData.hh>

#define MAXNUMCH 32 // Max nb of channels expected from DAQ

class PMTAnalyzer {
 protected:
  // Associated data
  PMTData* data;

  // Histograms of mean signal
  TH1F* meanSignal[MAXNUMCH];

  // Positions of the peak and tail in the mean signal (extremum in 4ns unit)
  float peakPos;
  float tailPos;

  float undershoot = 0;

 
 public:
  // Constructor
  PMTAnalyzer(PMTData *inputdata);
  // Destructor
  ~PMTAnalyzer();

  void CreateMeanSignal();

  void ComputeUndershoot();
  ////////////////////////////////////// //
  // Various accessors and set functions //
  ////////////////////////////////////// //

  TH1F* getMeanSignal(int iCh){return meanSignal[iCh];}

  float getPeakPos() const {return peakPos;}  

  float getTailPos() const {return tailPos;}

  float getUndershoot() const { return undershoot;}
};

#endif //PMTANALYSIS_PMTANALYZER_HH
