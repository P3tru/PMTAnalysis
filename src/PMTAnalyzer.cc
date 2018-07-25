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
  }
  
}

PMTAnalyzer::~PMTAnalyzer(){
  // Delete any members pointers to avoid mem leaks

  for(int iCh; iCh < data->getNbCh(); iCh++){
    delete meanSignal[iCh];
  }
  delete PEdistribution;
  delete fitFunction;
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
    peakPos[iCh] = (meanSignal[iCh]->GetMaximumBin()-1)*tStep;
    tailPos[iCh] = (meanSignal[iCh]->GetMinimumBin()-1)*tStep;
    std::cout << "peakPos" << iCh << " = " << peakPos[iCh] << std::endl;
    
  }// end iCh
}

void PMTAnalyzer::ComputeUndershoot(){
  TF1* f = new TF1("f", "[0]*(1-exp(-(x-[1])/[2]))", tailPos[data->getSignalCh()], 2000);
  f->SetParNames("const",
		 "t0",
		 "tau");  
  f->SetParameters(2,
		   0,
		   25);

  meanSignal[data->getSignalCh()]->Fit("f", "R");

  undershoot = f->GetParameter(2);

}

void PMTAnalyzer::ComputeIntegral(){
  int signalCh = data->getSignalCh();
  maxCharge = 0;
  minCharge = 1000000;
  for(int iEntry = 0; iEntry < data->getNbEntries(); iEntry++){
    charges[iEntry] = data->getSignalHistogram(signalCh, iEntry)->Integral(peakPos[signalCh]-data->nSize/2,
									   peakPos[signalCh]+data->nSize,
									   "width");
    if(charges[iEntry] > 50){
      std::cout << "iEntry = " << iEntry << std::endl;      
      std::cout << "integral = " << charges[iEntry] << std::endl;
    }
    if(charges[iEntry] > maxCharge) maxCharge = charges[iEntry];
    if(charges[iEntry] < minCharge) minCharge = charges[iEntry];
    
  }
  std::cout << "min = " << minCharge << std::endl;
  std::cout << "max = " << maxCharge << std::endl;
}

void PMTAnalyzer::CreatePEdistribution(){
  float mean;
  float sigma;
  ComputeIntegral();
  PEdistribution = new TH1F(Form("PEdistribution_%s", data->getFileName()),
				 Form("PE peak"),
				 100,
				 minCharge,
				 maxCharge);
  for(int iEntry = 0; iEntry < data->getNbEntries(); iEntry++){
    PEdistribution->Fill(charges[iEntry]);
  }
  
  mean = PEdistribution->GetMean();
  sigma = PEdistribution->GetStdDev();
  minCharge = mean - 2*sigma;
  maxCharge = mean + 3*sigma;
  PEdistribution = new TH1F(Form("PEdistribution_%s", data->getFileName()),
				 Form("PE peak"),
				 100,
				 minCharge,
				 maxCharge);
  PEdistribution->SetXTitle("Charge (V*ns)");
  PEdistribution->SetYTitle("Number of entries");
  for(int iEntry = 0; iEntry < data->getNbEntries(); iEntry++){
    PEdistribution->Fill(charges[iEntry]);
  }
}

void PMTAnalyzer::ComputeFit(int nbPE){
  fitFunction = new TF1("fitFunction", fit, minCharge, maxCharge, 9); // fir defined in functions.cc
  Double_t w = 0.05;
  Double_t Q0 = PEdistribution->GetMean();
  Double_t sigma0 = PEdistribution->GetStdDev()/2;
  Double_t alpha = 1/(1e-2*maxCharge);
  Double_t mu = 0.01;
  Double_t Q1 = PEdistribution->GetStdDev();
  Double_t sigma1 = PEdistribution->GetStdDev()/2;
  
  fitFunction->SetParNames("N", "Const", "w", "Q0", "sigma0", "alpha", "mu", "Q1", "sigma1");
  fitFunction->SetParameters(nbPE, data->getNbEntries(), w, Q0, sigma0, alpha, mu, Q1, sigma1);
  // Fixing the number of PE
  fitFunction->FixParameter(0, nbPE);
  // Avoiding negative parameters
  fitFunction->SetParLimits(1, 0, 100000);
  fitFunction->SetParLimits(2, 0, 1);
  fitFunction->SetParLimits(3, 0, 10000);
  fitFunction->SetParLimits(4, 0, 10000);
  fitFunction->SetParLimits(5, 0, 10000);
  fitFunction->SetParLimits(6, 0, 1);
  fitFunction->SetParLimits(7, 0, 10000);
  fitFunction->SetParLimits(8, 0, 10000);
 
  PEdistribution->Fit("fitFunction", "RW");
}

void PMTAnalyzer::DisplayFitParts(){
  TF1* bg = new TF1("bg", BG, minCharge, maxCharge, 9);
  TF1* exp = new TF1("exp", EXP, minCharge, maxCharge, 9);

  int N = fitFunction->GetParameter(0);
  TF1* spe[N];
  Double_t Const = fitFunction->GetParameter(1);
  Double_t w = fitFunction->GetParameter(2);
  Double_t Q0 = fitFunction->GetParameter(3);
  Double_t sigma0 = fitFunction->GetParameter(4);
  Double_t alpha = fitFunction->GetParameter(5);
  Double_t mu = fitFunction->GetParameter(6);
  Double_t Q1 = fitFunction->GetParameter(7);
  Double_t sigma1 = fitFunction->GetParameter(8);
  
  bg->SetParameters(N, Const, w, Q0, sigma0, alpha, mu, Q1, sigma1);
  bg->SetLineColor(3); //green
  PEdistribution->GetListOfFunctions()->Add(bg);

  exp->SetParameters(N, Const, w, Q0, sigma0, alpha, mu, Q1, sigma1);
  exp->SetLineColor(4); //blue
  PEdistribution->GetListOfFunctions()->Add(exp);

  for(int n = 0; n < N; n++){
    spe[n] = new TF1(Form("spe%d", n+1), SPE, minCharge, maxCharge, 9);
    spe[n]->SetParameters(n+1, Const, w, Q0, sigma0, alpha, mu, Q1, sigma1);
    spe[n]->SetLineColor(n+5); //yellow, pink, clear blue, green, ...    
    PEdistribution->GetListOfFunctions()->Add(spe[n]);
  }
}
