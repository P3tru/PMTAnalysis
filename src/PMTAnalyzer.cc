//
// Created by zsoldos on 18/07/18.
//

#include <iostream>
#include <PMTAnalyzer.hh>

#include "PMTAnalyzer.hh"

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
    meanSignal[iCh] = new TH1F(Form("meanSignal_%s_Ch%d", data->getFileName(), iCh),
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

  double tStep = data->getTimeStep();
  hQ[iCh] = new TH1F(Form("hQ_%s_Ch%d", data->getFileName(), iCh),
                     Form("hQ"),
                     500,
                     -1000,
                     1000);
  hQ[iCh]->SetXTitle("Time (ns)");
  hQ[iCh]->SetYTitle("Volts");

  hQ[iCh]->SetMarkerSize(2);

  // Loop on all signals
  const int nQ = 32;
  const int nbDiv = (int)(data->getNbSamples(iCh) / nQ);
  for(int iEntry = 0; iEntry < data->getNbEntries(); iEntry++){
    //Get integral
    for(int iQ=0; iQ<nbDiv; iQ++){
      hQ[iCh]->Fill(data->getSignalHistogram(iCh,iEntry)->Integral(1+iQ*nQ,(1+iQ)*nQ));
    }
  }// end iEntry

}

