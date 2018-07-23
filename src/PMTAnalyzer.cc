//
// Created by zsoldos on 18/07/18.
//

#include <iostream>

#include "PMTAnalyzer.hh"

PMTAnalyzer::PMTAnalyzer(PMTData* inputdata){

  data = inputdata;

  if(data == NULL){std::cout << "PMTData empty" << std::endl;}
  else{
    CreateMeanSignal();
    Undershoot();
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
    
    for(int iEntry = 0; iEntry < data->getNbEntries(); iEntry++){
      meanSignal[iCh]->Add(data->getSignalHistogram(iCh, iEntry));
    }// end iEntry
    meanSignal[iCh]->Scale((float)1/data->getNbEntries());
    peakPos = (meanSignal[iCh]->GetMaximumBin()-1)*tStep;
    tailPos = (meanSignal[iCh]->GetMinimumBin()-1)*tStep;
    
  }// end iCh
}

void PMTAnalyzer::Undershoot(){
  TF1* f = new TF1("f", "[0]*(1-exp(-(x-[1])/[2]))", tailPos, 2000);
  f->SetParNames("const",
		 "t0",
		 "tau");  
  f->SetParameters(2,
		   0,
		   25);

  meanSignal[0]->Fit("f", "R");

  undershoot = f->GetParameter(2);

}

