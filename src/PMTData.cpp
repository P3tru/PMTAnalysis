//
// Created by zsoldos on 10/05/17.
//

#include <iostream>
#include <TRandom3.h>
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
void PMTData::drawHist(int nbPlots, int iP) {

  if(iP == 0 && nbPlots == 0){

    for(unsigned int iPlot=0;iPlot<nbEntries;iPlot++) {
      if(iPlot == 0){
        hSignal[iPlot]->Draw();
        hSignal[iPlot]->GetYaxis()->SetRangeUser(-50,100);
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
    hSignal[iP]->GetYaxis()->SetRangeUser(-50,500);

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
