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
}

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
  maxCharge = -10000;
  minCharge = 10000;
  for(int iEntry = 0; iEntry < data->getNbEntries(); iEntry++){
    charges[iEntry] = data->getSignalHistogram(signalCh, iEntry)->Integral(time2bin(peakPos[signalCh]-20),
									   time2bin(peakPos[signalCh]+80),
									   "width");
   
    if(charges[iEntry] > maxCharge) maxCharge = charges[iEntry];
    if(charges[iEntry] < minCharge) minCharge = charges[iEntry];
    
  }
}

void PMTAnalyzer::CreatePEdistribution(){
  float mean;
  float sigma;
  ComputeIntegral();
  PEdistribution = new TH1F(Form("PEdistribution_%s", data->getFileName()),
				 Form("PE distribution"),
				 100,
				 minCharge,
				 maxCharge);
  PEdistribution->SetXTitle("Charge (V*ns)");
  PEdistribution->SetYTitle("Number of entries");
  for(int iEntry = 0; iEntry < data->getNbEntries(); iEntry++){
    PEdistribution->Fill(charges[iEntry]);
  }
  /*// Redefining histogram and functions limits
  mean = PEdistribution->GetMean();
  sigma = PEdistribution->GetStdDev();
  PEdistribution->~TH1();
  minCharge = mean - 3*sigma;
  maxCharge = mean + 4*sigma;
  PEdistribution = new TH1F(Form("PEdistribution_%s", data->getFileName()),
				 Form("PE peak"),
				 100,
				 minCharge,
				 maxCharge);
  PEdistribution->SetXTitle("Charge (V*ns)");
  PEdistribution->SetYTitle("Number of entries");
  for(int iEntry = 0; iEntry < data->getNbEntries(); iEntry++){
    PEdistribution->Fill(charges[iEntry]);
    }*/
}

void PMTAnalyzer::ComputeFit(int nbPE){
  fitFunction = new TF1("fitFunction", fit, minCharge, maxCharge, 9); // fit defined in functions.cc
  Double_t w = 0.0;
  Double_t Q0 = PEdistribution->GetMean()*0.8;
  Double_t sigma0 = PEdistribution->GetStdDev()*0.4;
  Double_t alpha = 1/(0.3*PEdistribution->GetStdDev());
  Double_t mu = 0.1;
  Double_t Q1 = PEdistribution->GetStdDev()*1.0;
  Double_t sigma1 = PEdistribution->GetStdDev()*0.7;
  
  fitFunction->SetParNames("N", "Const", "w", "Q0", "sigma0", "alpha", "mu", "Q1", "sigma1");
  fitFunction->SetParameters(nbPE, data->getNbEntries(), w, Q0, sigma0, alpha, mu, Q1, sigma1);
  // Fixing the number of PE
  fitFunction->FixParameter(0, nbPE);
  fitFunction->FixParameter(2, 0);  
  fitFunction->FixParameter(5, alpha);  
  // Avoiding negative parameters
  fitFunction->SetParLimits(1, 0, 100000);
  //fitFunction->SetParLimits(2, 0, 1);
  fitFunction->SetParLimits(3, 0, 10000);
  fitFunction->SetParLimits(4, 0, 10000);
  //fitFunction->SetParLimits(5, 0, 10000);
  fitFunction->SetParLimits(6, 0, 10000);
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

void PMTAnalyzer::ComputeDarkRates(){
  float nbPeaks;
  ComputeFit(3);
  DisplayFitParts();
  nbPeaks = PEdistribution->GetListOfFunctions()->FindObject("spe1")->Integral(minCharge, maxCharge);
  darkRates = nbPeaks/(100*data->GetNbEntries());
}
