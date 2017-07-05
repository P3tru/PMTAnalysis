//
// Created by zsoldos on 16/05/17.
//
#include "PMTAnal.hh"

#include <TCanvas.h>
#include <TRandom3.h>
#include <iostream>

#include "utils.h"

PMTAnal::PMTAnal() {

}

PMTAnal::PMTAnal(std::string name, int nE, int nP, int ground) {
  fName = name;
  nbEntries = nE;
  nbPulses = nP;
  GND = ground;
}

PMTAnal::~PMTAnal() {

  ///////////////////////////
  // Deleting all pointers //
  ///////////////////////////

  for(unsigned int i=0;i<hSignal.size();i++){
    delete hSignal[i];
    delete sSignal[i];
  }

  for(unsigned int i=0;i<hPulses.size();i++){
    delete hPulses[i];
    delete gPulses[i];
  }

  for(unsigned int i=0;i<sAfterPulses.size();i++){
    delete sAfterPulses[i];
  }

  delete hUndershoot;
  delete hAfterPulses;
  delete hQtots;
}


void PMTAnal::createSignalHistograms(TTree *tree) {

  UInt_t data[nbSamples];
  tree->SetBranchAddress("Data",data);

  const int nbEntries = tree->GetEntries();

  hSignal.reserve(nbEntries);

  std::cout << "Create histograms from raw signals" << std::endl;

  for (int iEntry = 0; iEntry < nbEntries; iEntry++) {
    // Point to data
    tree->GetEntry(iEntry);

    // Create histogram and graph of signal
    hSignal[iEntry] = new TH1I(Form("hSignal%s%d",fName.c_str(),iEntry),Form("Signal"),nbSamples,0,nbSamples);
    hSignal[iEntry]->GetXaxis()->SetTitle("Time (4ns)");
    hSignal[iEntry]->GetYaxis()->SetTitle("ADC Channel");

    // Fill hist and graph
    for(int i=0;i<nbSamples;i++){
      hSignal[iEntry]->SetBinContent(i+1,-(int)data[i] + GND);
    }

    // Give status
    if(iEntry % 1000 == 0) std::cout << iEntry << " Processed (" << (int)((double)(iEntry)*100/(double)(nbEntries)) << "%)" << std::endl;

  }

}

void PMTAnal::drawHist(int nbPlots, int iP, double rangeMin, double rangeMax) {

  if(iP == 0 && nbPlots == 0){

    for(int iPlot=0;iPlot<nbEntries;iPlot++) {
      if(iPlot == 0){
        hSignal[iPlot]->Draw();
        hSignal[iPlot]->GetYaxis()->SetRangeUser(rangeMin,rangeMax);
      }
      hSignal[iPlot]->Draw("same");

      // Give status
      if(iPlot % 1000 == 0) std::cout << iPlot << " Drawn (" << (int)((double)(iPlot)*100/(double)(nbEntries)) << "%)" << std::endl;

    }

  }else if(nbPlots != 0){

    TRandom3 *random3 = new TRandom3();
    random3->SetSeed();

    if(iP == 0){
      while(iP == 0 || iP < nbPlots) iP = random3->Uniform(1,nbEntries-nbPlots);
    }else{
      if(iP > nbEntries-nbPlots) iP = nbEntries-nbPlots;
    }

    hSignal[iP]->Draw();
    hSignal[iP]->GetYaxis()->SetRangeUser(rangeMin,rangeMax);

    int iColor=1;
    for(int iPlot=iP+1;iPlot<iP+nbPlots;iPlot++) {
      hSignal[iPlot]->SetLineColor(iColor);
      iColor++;
      hSignal[iPlot]->Draw("same");
    }

  }else{
    hSignal[iP]->Draw();
    hSignal[iP]->GetYaxis()->SetRangeUser(rangeMin,rangeMax);

    if(nbPlots == 0) nbPlots=1;

    int iColor=1;
    for(int iPlot=iP+1;iPlot<iP+nbPlots;iPlot++) {
      hSignal[iPlot]->SetLineColor(iColor);
      iColor++;
      hSignal[iPlot]->Draw("same");
    }

  }

}

void PMTAnal::computeDarkRates(int ampPE) {

  darkRates = 0;

  for (int iEntry=0;iEntry<nbEntries;iEntry++) {
    for (int i = 0; i < nbSamples; i++) {
      if ((int)hSignal[iEntry]->GetBinContent(i) > ampPE) darkRates++;
    }
  }

  std::cout << "DARK COUNT : " << darkRates << " (" << (double)(darkRates)/(double)(nbEntries*nbSamples*1e-9) << " +- "
            << err(darkRates,nbEntries,nbSamples*1e-9) << " Hz) " << std::endl;

}

void PMTAnal::createSignal() {

  hQtots = new TH1D(Form("hQtots%s",fName.c_str()),"Pulse charge distribution",1000,-1000,40000);
  hQtots->GetXaxis()->SetTitle("ADC Channels");
  hQtots->Sumw2();

  sSignal.reserve(nbEntries); // Looks for signal in hSignal histograms

  gPulses.reserve(nbEntries*5); // Approx 4-5 signals by DAQ window
  hPulses.reserve(nbEntries*5); // Approx 4-5 signals by DAQ window
  int iP=0;

  std::cout << "Create signal histogram" << std::endl;

  for (int iEntry=0;iEntry<nbEntries;iEntry++) {
    // Histograms and graphs are defined
    // Can play to find qTot, afterpulses, aftershoot...
    sSignal[iEntry] = new TSpectrum();
    sSignal[iEntry]->Search(hSignal[iEntry],4,"nodraw",0.5);
    const int nbPeaks = sSignal[iEntry]->GetNPeaks();

    // Compute charge for each peak
    for(int iPeak=0;iPeak<nbPeaks;iPeak++){
      gPulses[iP] = new TGraph();
      gPulses[iP]->SetTitle(Form("Pulse %d %d",iEntry,iPeak));
      gPulses[iP]->GetXaxis()->SetTitle("Time (4ns)");
      gPulses[iP]->GetYaxis()->SetTitle("ADC Channel");
      gPulses[iP]->SetMarkerStyle(kMultiply);
      gPulses[iP]->SetMarkerSize(1.);

      hPulses[iP] = new TH1I(Form("hPulses%s%d%d",fName.c_str(),iEntry,iPeak),Form("Pulse"),nTot,0,nTot);
      hPulses[iP]->GetXaxis()->SetTitle("Time (4ns)");
      hPulses[iP]->GetYaxis()->SetTitle("ADC Channel");


      double xPeak = sSignal[iEntry]->GetPositionX()[iPeak];
      double yPeak = sSignal[iEntry]->GetPositionY()[iPeak];
      int qTotPeak = 0;
      if((int)(xPeak) < nbSamples - nTot){
        int jBin=0;
        for(int iBin=(int)(xPeak)-CFDTime;iBin<(int)(xPeak+nTot);iBin++){
          qTotPeak+=hSignal[iEntry]->GetBinContent(iBin);
          gPulses[iP]->SetPoint(jBin,jBin,hSignal[iEntry]->GetBinContent(iBin));
          hPulses[iP]->SetBinContent(jBin,hSignal[iEntry]->GetBinContent(iBin));
          jBin++;
        } // END FOR BINS

        // CHARGE
        hQtots->Fill(qTotPeak);
        iP++;
      } // END IF
    } // END FOR PEAKS

    // Give status
    if(iEntry % 1000 == 0) std::cout << iEntry << " Processed (" << (int)((double)(iEntry)*100/(double)(nbEntries)) << "%)" << std::endl;

  } // END IENTRY

  nbPulses = iP;

}

void PMTAnal::drawQtots() {
  hQtots->Draw("E");
}


void PMTAnal::drawSignal(int nbPlots, int iP, double rangeMin, double rangeMax) {

  if(iP == 0 && nbPlots == 0){

    for(int iPlot=0;iPlot<nbPulses;iPlot++) {
      if(iPlot == 0){
        gPulses[iPlot]->Draw("APC");
        gPulses[iPlot]->GetYaxis()->SetRangeUser(rangeMin,rangeMax);
      }
      gPulses[iPlot]->Draw("PCsame");

      // Give status
      if(iPlot % 1000 == 0) std::cout << iPlot << " Drawn (" << (int)((double)(iPlot)*100/(double)(nbPulses)) << "%)" << std::endl;

    }

  }else if(nbPlots != 0){

    TRandom3 *random3 = new TRandom3();
    random3->SetSeed();

    if(iP == 0){
      while(iP == 0 || iP < nbPlots) iP = random3->Uniform(1,nbEntries-nbPlots);
    }else{
      if(iP > nbEntries-nbPlots) iP = nbEntries-nbPlots;
    }

    gPulses[iP]->Draw();
    gPulses[iP]->GetYaxis()->SetRangeUser(rangeMin,rangeMax);

    int iColor=1;
    for(int iPlot=iP+1;iPlot<iP+nbPlots;iPlot++) {
      gPulses[iPlot]->SetLineColor(iColor);
      iColor++;
      gPulses[iPlot]->Draw("same");
    }

  }else{

    if(nbPlots == 0) nbPlots=1;

    gPulses[iP]->Draw();
    gPulses[iP]->GetYaxis()->SetRangeUser(rangeMin,rangeMax);

    int iColor=1;
    for(int iPlot=iP+1;iPlot<iP+nbPlots;iPlot++) {
      gPulses[iPlot]->SetLineColor(iColor);
      iColor++;
      gPulses[iPlot]->Draw("same");
    }
  }

}
void PMTAnal::computeAfterPulses() {

  sAfterPulses.reserve(nbPulses);

  hAfterPulses = new TH1D("hAfterPulses","After pulses",nTot,0,nTot);
  hAfterPulses->Sumw2();

  TCanvas *cc = new TCanvas("cc","cc",800,600);
  gPulses[0]->Draw("APC");
  gPulses[0]->GetYaxis()->SetRangeUser(-100,600);

  TRandom3 *random3 = new TRandom3();
  random3->SetSeed();

  std::vector<int> listPlot;

  for (int iEntry=0;iEntry<nbPulses;iEntry++) {

    sAfterPulses[iEntry] = new TSpectrum();
    sAfterPulses[iEntry]->Search(hPulses[iEntry], 3, "nodraw", 0.10);
    const int nbPeaks = sAfterPulses[iEntry]->GetNPeaks();

    if(nbPeaks > 1){
      for(int iPeak=0;iPeak<nbPeaks;iPeak++) {
        double xPeak = sAfterPulses[iEntry]->GetPositionX()[iPeak];
        double yPeak = sAfterPulses[iEntry]->GetPositionY()[iPeak];
        if(xPeak > 10) hAfterPulses->Fill(xPeak);
      }
      listPlot.push_back(iEntry);
      gPulses[iEntry]->SetLineColor((int)random3->Uniform(1,10));
      gPulses[iEntry]->Draw("PCsame");
    }
  }

  int iPP=0;
  int nWindow=10;
  int nbDiv=4;
  for(int j=0;j<nWindow;j++){
    if(iPP > listPlot.size()-nbDiv*nbDiv) break;
    cc= new TCanvas(Form("cc%d",j),Form("cc%d",j),800,600);
    cc->Divide(nbDiv,nbDiv);
    for(int i=0;i<(int)(nbDiv*nbDiv);i++){
      cc->cd(i+1);
      if(iPP > listPlot.size()-2) break;
      hPulses[listPlot[iPP]]->Draw();
      iPP++;
      hPulses[listPlot[iPP]]->SetLineColor(kRed-4);
      hPulses[listPlot[iPP]]->Draw("same");
      iPP++;
    }
  }

  cc= new TCanvas("ccc","ccc",800,600);
  hAfterPulses->Draw();

}