//
// Created by zsoldos on 10/05/17.
//

#include <iostream>
#include <TRandom3.h>
#include <TSpectrum.h>
#include <TCanvas.h>

#include "utils.h"
#include "PMTData.h"

PMTData::PMTData(const char* fname) {
  inputFile.open(fname);

  filename << getFileName(fname) << ".root";

  // Check if root file already exist
  //file = new TFile(filename.str().c_str(),"RECREATE");

  tree = new TTree("PMTData","PMTData");

  GND=0;
}

void PMTData::createTree() {

  std::string line;

  TH1I hGND("hGND","hGND",100,8200,8300);

  if(inputFile.is_open()){
    int data[1024];
    tree->Branch("Data",data,"Data[1024]/i");

    int iLineRead=0;
    while(getline(inputFile,line)){
      std::stringstream test;
      std::string var;
      test << line;

      int count=0;
      while(test >> var){
        if(var == "Channel") break;
        data[count] = std::stoi(var);
        if(iLineRead % 2 == 1 && count == 0) hGND.Fill(std::stoi(var));
        count++;
      }
      if(iLineRead % 2 == 1) tree->Fill();
      if(iLineRead % 1000 == 0) std::cout << "Line read : " << iLineRead << std::endl;
      iLineRead++;
    }

    inputFile.close();
    nbEntries = tree->GetEntries();
    std::cout << "Nb of valid entries : " << nbEntries << std::endl;
    std::cout << "GND = " << hGND.GetMean() << std::endl;
    GND = hGND.GetMean();

  } else {
    std::cout << "Unable to open file" << std::endl;
  }

}
PMTData::~PMTData() {

//  tree->Write();
//  file->Close();
//  std::cout << "File " << filename.str() << " created" << std::endl;

  delete file;
  delete tree;

  for(unsigned int i=0;i<hSignal.size();i++){
    delete hSignal[i];
    delete hBckg[i];
  }

}
void PMTData::createSignalHistograms() {

  UInt_t data[nbSamples];
  tree->SetBranchAddress("Data",data);

  hSignal.reserve(nbEntries);

  for (int iEntry = 0; iEntry < nbEntries; iEntry++) {
    // Point to data
    tree->GetEntry(iEntry);

    // Create histogram and graph of signal
    hSignal[iEntry] = new TH1I(Form("hSignal%d",iEntry),Form("Signal"),nbSamples,0,nbSamples);
    hSignal[iEntry]->GetXaxis()->SetTitle("Time (4ns)");
    hSignal[iEntry]->GetYaxis()->SetTitle("ADC Channel");

    // Fill hist and graph
    for(int i=0;i<nbSamples;i++){
      int y = - (int)data[i] + GND;
      hSignal[iEntry]->SetBinContent(i+1,y);
    }

    // Give status
    if(iEntry % 1000 == 0) std::cout << iEntry << " Processed (" << (int)((double)(iEntry)*100/(double)(nbEntries)) << "%)" << std::endl;

  }

}
void PMTData::drawHist(int nbPlots, int iP, double rangeMin, double rangeMax) {

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

  }else{

    TRandom3 *random3 = new TRandom3();
    random3->SetSeed();

    while(iP == 0 || iP < nbPlots) iP = random3->Uniform(1,nbEntries-nbPlots);

    hSignal[iP]->Draw();
    hSignal[iP]->GetYaxis()->SetRangeUser(rangeMin,rangeMax);

    int iColor=1;
    for(int iPlot=iP+1;iPlot<iP+nbPlots;iPlot++) {
      hSignal[iPlot]->SetLineColor(iColor);
      iColor++;
      hSignal[iPlot]->Draw("same");
    }

  }

}
void PMTData::computeDarkRates(int ampPE) {

  darkRates = 0;

  for (int iEntry=0;iEntry<nbEntries;iEntry++) {
    for (int i = 0; i < nbSamples; i++) {
      if ((int)hSignal[iEntry]->GetBinContent(i) > ampPE) darkRates++;
    }
  }

  std::cout << "DARK COUNT : " << darkRates << " (" << (double)(darkRates)/(double)(nbEntries*nbSamples*1e-9) << " +- "
            << err(darkRates,nbEntries,nbSamples*1e-9) << " Hz) " << std::endl;

}
void PMTData::createSignal() {

  hQtots = new TH1D("hQtots","Pulse charge distribution",1000,-1000,40000);
  hQtots->GetXaxis()->SetTitle("ADC Channels");
  hQtots->Sumw2();

  sSignal.reserve(nbEntries); // Looks for signal in hSignal histograms

  gPulses.reserve(nbEntries*5); // Approx 4-5 signals by DAQ window
  hPulses.reserve(nbEntries*5); // Approx 4-5 signals by DAQ window
  int iP=0;

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

      hPulses[iP] = new TH1I(Form("hPulses%d %d",iEntry, iPeak),Form("Pulse"),nTot,0,nTot);
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


  } // END IENTRY

  nbPulses = iP;

}
void PMTData::drawSignal(int nbPlots, int iP, double rangeMin, double rangeMax) {

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

  }else{

    TRandom3 *random3 = new TRandom3();
    random3->SetSeed();

    while(iP == 0 || iP < nbPlots) iP = random3->Uniform(1,nbEntries-nbPlots);

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
void PMTData::computeAfterPulses() {

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

//  cc = new TCanvas("ccc","ccc",800,600);
//  hPulses[0]->Draw("");
//  hPulses[0]->GetYaxis()->SetRangeUser(-100,600);
//  for(unsigned int i=0;i<listPlot.size();i++) hPulses[listPlot[i]]->Draw("same");

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

