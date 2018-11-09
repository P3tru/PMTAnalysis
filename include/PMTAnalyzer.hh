//
// Created by zsoldos on 18/07/18.
//

#ifndef PMTANALYSIS_PMTANALYZER_HH
#define PMTANALYSIS_PMTANALYZER_HH

#include <fstream>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TF1.h>

#include <TGraph.h>

#include <PMTData.hh>

#define MAXNUMCH 32 // Max nb of channels expected from DAQ

class PMTAnalyzer {
 protected:
  // Associated data
  PMTData* data;

  // Histograms of mean signal
  TH1D* meanSignal[MAXNUMCH];

  // Positions of the peak and tail in the mean signal (extremum in 4ns unit)
  float peakPos;
  float tailPos;

  float undershoot;
  float undershootEr;

  // Histogram of Charge
  TH1D* hQ[MAXNUMCH];
  TGraph* gDeltaQ[MAXNUMCH];

  // Min and Max non-empty bins for Q histogram
  double minQ;
  double maxQ;
  int nbBinsQ;

 public:
  // Constructor
  PMTAnalyzer(PMTData *inputdata);
  // Destructor
  ~PMTAnalyzer();

  void CreateMeanSignal();

  void ComputeUndershoot(int iCh);

  void ComputeQ(int iCh);

  std::vector<double> ComputeQFitParams(int iCh);

  std::vector<int> GetPeaksPos(TH1 *h);

  void FitDarkCounts(int iCh, int color);

  void SearchMinMaxHistQ(TH1 *h);

  ////////////////////////////////////// //
  // Various accessors and set functions //
  ////////////////////////////////////// //

  TH1D* getMeanSignal(int iCh){return meanSignal[iCh];}
  TH1D* getChargeSignal(int iCh){return hQ[iCh];}

  TGraph* getDeltaQGraph(int iCh){return gDeltaQ[iCh];}

  float getPeakPos() const {return peakPos;}  

  float getTailPos() const {return tailPos;}

  void setUndershoot(float undershoot){PMTAnalyzer::undershoot = undershoot;};
  float getUndershoot() const { return undershoot;}
  void setUndershootEr(float undershoot){PMTAnalyzer::undershootEr = undershoot;};
  float getUndershootEr() const { return undershootEr;}

  double getMinQ() const {
    return minQ;
  }

  void setMinQ(double minQ) {
    PMTAnalyzer::minQ = minQ;
  }

  double getMaxQ() const {
    return maxQ;
  }

  void setMaxQ(double maxQ) {
    PMTAnalyzer::maxQ = maxQ;
  }

 public:
  int getNbBinsQ() const {
    return nbBinsQ;
  }
  void setNbBinsQ(int nbBinsQ) {
    PMTAnalyzer::nbBinsQ = nbBinsQ;
  }

  // Defining integration parameter in sample
  int CFDTime = 2;
  int nTot    = 16;
  int nSize   = CFDTime+nTot;
};

#endif //PMTANALYSIS_PMTANALYZER_HH
