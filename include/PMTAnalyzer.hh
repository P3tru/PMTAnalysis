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

  float undershoot;
  float undershootEr;

  // Histogram of Charge
  TH1F* hQ[MAXNUMCH];

 public:
  // Constructor
  PMTAnalyzer(PMTData *inputdata);
  // Destructor
  ~PMTAnalyzer();

  void CreateMeanSignal();

  void ComputeUndershoot(int iCh);

  void ComputeQ(int iCh);
  ////////////////////////////////////// //
  // Various accessors and set functions //
  ////////////////////////////////////// //

  TH1F* getMeanSignal(int iCh){return meanSignal[iCh];}
  TH1F* getChargeSignal(int iCh){return hQ[iCh];}

  float getPeakPos() const {return peakPos;}  

  float getTailPos() const {return tailPos;}

  void setUndershoot(float undershoot){PMTAnalyzer::undershoot = undershoot;};
  float getUndershoot() const { return undershoot;}
  void setUndershootEr(float undershoot){PMTAnalyzer::undershootEr = undershoot;};
  float getUndershootEr() const { return undershootEr;}

};

#endif //PMTANALYSIS_PMTANALYZER_HH
