//
// Created by zsoldos on 18/07/18.
//

#include <iostream>
#include <PMTAnalyzer.hh>
#include <TColor.h>
#include <TSpectrum.h>

#include "PMTAnalyzer.hh"
#include "utils.h"

PMTAnalyzer::PMTAnalyzer(PMTData* inputdata){

  data = inputdata;

  if(data == NULL){std::cout << "PMTData empty" << std::endl;}
  else{
    CreateMeanSignal();
  }
  
}

PMTAnalyzer::~PMTAnalyzer(){
  // Delete any members pointers to avoid mem leaks

  for(int iCh; iCh < data->getNbCh(); iCh++){
    delete meanSignal[iCh];
  }
  delete data;
};


void PMTAnalyzer::CreateMeanSignal(){

  for(int iCh = 0; iCh < data->getNbCh(); iCh++){
    double tStep = data->getTimeStep();
    meanSignal[iCh] = new TH1D(Form("meanSignal_%s_Ch%d", data->getFileName(), iCh),
                               Form("MeanSignal"),
                               data->getNbSamples(iCh),
                               -tStep/2,
                               (data->getNbSamples(iCh)-1)*tStep + tStep/2);
    meanSignal[iCh]->SetXTitle("Time (ns)");
    meanSignal[iCh]->SetYTitle("Volts");

    meanSignal[iCh]->SetMarkerSize(2);
    
    for(int iEntry = 0; iEntry < data->getNbEntries(); iEntry++){
      meanSignal[iCh]->Add(data->getSignalHistogram(iCh, iEntry));
    }// end iEntry

    meanSignal[iCh]->Scale((float)1/data->getNbEntries());
    peakPos = static_cast<float>((meanSignal[iCh]->GetMaximumBin()-1)*tStep);
    tailPos = static_cast<float>((meanSignal[iCh]->GetMinimumBin()-1)*tStep);

  }// end iCh
}

void PMTAnalyzer::ComputeUndershoot(int iCh){
  double tStep = data->getTimeStep();
  TF1* f = new TF1("f",
                   "[0]*(1-exp(-(x-[1])/[2]))+[3]",
                   tailPos,
                   (data->getNbSamples(iCh)-1)*tStep + tStep/2);

  f->SetParNames("baseline",
                 "t0",
                 "tau",
                 "min");

  double min = meanSignal[iCh]->GetBinContent(meanSignal[iCh]->GetMinimumBin());

  f->SetParameter(0, 0);
  f->SetParameter(1, tailPos);
  f->SetParameter(2, 100);
  f->SetParameter(3, min);

  meanSignal[0]->GetYaxis()->SetRangeUser(
      meanSignal[iCh]->GetBinContent(meanSignal[iCh]->GetMinimumBin())-1,1);

  meanSignal[0]->Fit("f", "QSMER");

  undershoot = static_cast<float>(f->GetParameter(2));
  undershootEr = static_cast<float>(f->GetParError(2));

}

void PMTAnalyzer::ComputeQ(int iCh) {

  setNbBinsQ(500);
  setMinQ(-1500);
  setMaxQ(6000);

  TH1D *buf = new TH1D("buf",
                       "buf",
                       nbBinsQ,
                       minQ,
                       maxQ);

  buf->SetXTitle("Charge (ADC)");
  buf->SetYTitle("Counts");

  buf->SetMarkerSize(2);

  buf->Sumw2();

  int nbFoundDN = 0;
  int nbNOTFoundDN = 0;

  // Loop on all signals
  const int nbDiv = (int)(data->getNbSamples(iCh) / nTot);
  for(int iEntry = 0; iEntry < data->getNbEntries(); iEntry++){

    // FIRST, look if we can find some peaks
    std::vector<int> peaks = GetPeaksPos(data->getSignalHistogram(iCh,iEntry));
    if(peaks.size()>0){
      for(int iP=0; iP<peaks.size(); iP++){
        int iBinPeak = static_cast<int>(peaks[iP]/data->getTimeStep());
        buf->Fill(data->getSignalHistogram(iCh,iEntry)->Integral(iBinPeak-CFDTime,iBinPeak+nTot));
        nbFoundDN++;
      } // END FOR PEAKS

    } else {
      //Get integral
      for(int iQ=0; iQ<nbDiv; iQ++){
        buf->Fill(data->getSignalHistogram(iCh,iEntry)->Integral(1+iQ*nTot,(1+iQ)*nTot));
        nbNOTFoundDN++;
      }
    }

  }// end iEntry

  std::cout << " Found DN:" << nbFoundDN << std::endl
            << " NOT Found DN:" << nbNOTFoundDN << std::endl
            << " Proportion:" << 100.*nbFoundDN/(nbFoundDN+nbNOTFoundDN) << "%" << std::endl;

  SearchMinMaxHistQ(buf);

  hQ[iCh] = new TH1D(Form("hQ_%s_Ch%d", data->getFileName(), iCh),
                     Form("hQ"),
                     nbBinsQ,
                     minQ,
                     maxQ);

  hQ[iCh]->SetXTitle("Charge (ADC)");
  hQ[iCh]->SetYTitle("Counts");

  hQ[iCh]->SetMarkerSize(2);

  hQ[iCh]->Sumw2();

  for(int iEntry = 0; iEntry < data->getNbEntries(); iEntry++){

    // FIRST, look if we can find some peaks
    std::vector<int> peaks = GetPeaksPos(data->getSignalHistogram(iCh,iEntry));
    if(!peaks.empty()){
      for(int iP=0; iP<peaks.size(); iP++){
        int iBinPeak = static_cast<int>(peaks[iP]/data->getTimeStep());
        hQ[iCh]->Fill(data->getSignalHistogram(iCh,iEntry)->Integral(iBinPeak-CFDTime,iBinPeak+nTot));
      } // END FOR PEAKS

    } else {
      //Get integral
      for(int iQ=0; iQ<nbDiv; iQ++){
        hQ[iCh]->Fill(data->getSignalHistogram(iCh,iEntry)->Integral(1+iQ*nTot,(1+iQ)*nTot));
      }
    }

  }// end iEntry

  hQ[iCh]->Scale(1/hQ[iCh]->GetEntries());

}

std::vector<double> PMTAnalyzer::ComputeQFitParams(int iCh) {

  std::vector<double> pos0Cross;

  gDeltaQ[iCh] = new TGraph();
  gDeltaQ[iCh]->SetTitle(Form("Delta hQ Ch %d ; ADC ; Delta",iCh));

  Int_t ci = TColor::GetFreeColorIndex();
  TColor *color = new TColor(ci, 111, 159, 216);

  gDeltaQ[iCh]->SetLineStyle(1);
  gDeltaQ[iCh]->SetLineWidth(2.);
  gDeltaQ[iCh]->SetLineColor(kRed-4);
  gDeltaQ[iCh]->SetMarkerStyle(kMultiply);
  gDeltaQ[iCh]->SetMarkerSize(2);
  gDeltaQ[iCh]->SetMarkerColor(kRed-4);

  const int deltaStep = 2;

  const double norm =  (int)(data->getNbSamples(iCh) / nTot) * data->getNbEntries();
  const int maxBin = hQ[iCh]->GetMaximumBin();

  for(int iBin=0; iBin<(nbBinsQ-deltaStep); iBin++){
    double delta = hQ[iCh]->GetBinCenter(iBin+deltaStep)-hQ[iCh]->GetBinCenter(iBin);
    double deltaBin = norm*(hQ[iCh]->GetBinContent(iBin+deltaStep)-hQ[iCh]->GetBinContent(iBin));
    gDeltaQ[iCh]->SetPoint(iBin,
                           hQ[iCh]->GetBinCenter(iBin),
                           deltaBin/delta);
    // Search 0-cross
    if(iBin>maxBin){
      double previousDeltaBin = norm*(hQ[iCh]->GetBinContent(iBin)-hQ[iCh]->GetBinContent(iBin-deltaStep));
      if(previousDeltaBin < 0 && deltaBin > 0){
        // std::cout << " Cross-0 at : " << hQ[iCh]->GetBinCenter(iBin) << std::endl;
        pos0Cross.push_back(hQ[iCh]->GetBinCenter(iBin));
      } // END IF Cross-0
    } // END IF > max Bin

  } // END FOR iBIN

  return pos0Cross;
}

std::vector<int> PMTAnalyzer::GetPeaksPos(TH1 *h) {

  TSpectrum *spectrum = new TSpectrum();
  spectrum->Search(h,4,"nobackground",0.99);
  std::vector<int> peaks;
  for(int iP=0;iP<spectrum->GetNPeaks();iP++){
    peaks.push_back(spectrum->GetPositionX()[iP]);
  }

  return peaks;
}

void PMTAnalyzer::FitDarkCounts(int iCh, int color) {

  std::vector<double> pos0Cross = ComputeQFitParams(iCh);
  std::cout << " pos0Cross[0]:" << pos0Cross[0] << std::endl;

  double pedPeakAmp = hQ[iCh]->GetBinContent(hQ[iCh]->GetMaximumBin());
  double pedPeakPos = hQ[iCh]->GetMean();
  double pedPeakSigma = hQ[iCh]->GetRMS();

  TF1 *fitPMT = new TF1("fitPMT",simplePMT,getMinQ(),getMaxQ(),9);

  fitPMT->SetParameters(
      pedPeakAmp,
      pedPeakPos,
      pedPeakSigma,

      pos0Cross[0],
      pedPeakSigma,

      1/pos0Cross[0],
      pedPeakAmp/3,
      pedPeakPos,

      1/pedPeakAmp);

  fitPMT->SetParLimits(0,0,pedPeakAmp); // Ped Peak
  fitPMT->SetParLimits(1,minQ,maxQ); // Q of PED can be negative unfortunately
  fitPMT->SetParLimits(2,0,maxQ); // Sigma of dark counts is positive

  fitPMT->SetParLimits(3,
                       pos0Cross[0]-2*abs(pedPeakSigma),
                       pos0Cross[0]+2*abs(pedPeakSigma)); // Q of dark counts is positive
  fitPMT->SetParLimits(4,abs(pedPeakSigma),3*abs(pedPeakSigma)); // Sigma of dark counts is positive

  fitPMT->SetParLimits(5,
                       1/(pos0Cross[0]+abs(pedPeakSigma)),
                       1/abs(pedPeakSigma)); // tau
  fitPMT->SetParLimits(6,0,pedPeakAmp/2); // CExp
  fitPMT->SetParLimits(7,pedPeakPos,pos0Cross[0]); // delta

  fitPMT->SetParLimits(8,0,1); // lambda

  fitPMT->SetParNames("C0","Q0","sigma0","Q1","sigma1","Tau","CExp","Delta","Lambda");

  fitPMT->SetLineColor(color);
  fitPMT->SetLineStyle(1);
  fitPMT->SetLineWidth(3.);

  hQ[iCh]->Fit("fitPMT","LEMRN","SAME");
  hQ[iCh]->Fit("fitPMT","LEMR+","SAME");

}

void PMTAnalyzer::SearchMinMaxHistQ(TH1 *h) {

  bool searchMinQ = true;
  bool searchMaxQ = true;

  const int deltaStep = 2;
  const int deltaInvStep = 4;

  for(int iBin=0; iBin<(nbBinsQ-deltaInvStep); iBin++) {

    double delta = h->GetBinContent(iBin+deltaStep)-h->GetBinContent(iBin);
    double deltaInv = h->GetBinContent(iBin+deltaInvStep)-h->GetBinContent(iBin);

    // Set min and max non-empty bin
    if (searchMinQ) {

      if (delta > 0) {
        setMinQ(h->GetBinCenter(iBin) - 100);
        searchMinQ = false;
        std::cout << " minQ:" << minQ << std::endl;
      }

    }

    if (searchMaxQ && !searchMinQ) {
      if (deltaInv == 0 && h->GetBinContent(iBin) == 0) {

        bool isAllBinsNull=true;
        for(int jBin=iBin; jBin < iBin + deltaInvStep; jBin++){
          if(h->GetBinContent(jBin) == 0){
            continue;
          } else {
            isAllBinsNull=false;
            break;
          }
        }

        if(isAllBinsNull){
          setMaxQ(h->GetBinCenter(iBin + deltaInvStep) + 100);
          searchMaxQ = false;
          std::cout << " maxQ:" << maxQ << std::endl;
        }

      }
    }

  } // END FOR iBin

  setNbBinsQ(static_cast<Int_t>((maxQ-minQ)/15));
}
