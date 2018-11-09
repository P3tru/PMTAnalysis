//
// Created by zsoldos on 10/05/17.
//

#include <iostream>

#include <TF1.h>

#include <PMTData.hh>

PMTData::PMTData(std::string inputUserArg) {

  // Open TTree inside rootFile
  inputFile = new TFile(inputUserArg.c_str());
  p = inputUserArg;

  dataFileName = p.stem().string();

  GND = 0;
  const int nbBinsGND=100;
  hGND = new TH1D("hGND",
                  "Extraded baseline values",
                  nbBinsGND,
                  8210,
                  8220);
  hGND->SetXTitle("ADC Channels");

  hGND->SetMarkerSize(2);

  voltConv = 2.06/5000.;
  voltConv = 1;
  nbEntries=0;

  if(OpenPMTDataTTree()){
    std::cout << "Tree open successfully" << std::endl;
    CreateWaveformsHistogram();
  }

}

PMTData::~PMTData() {

  inputFile->Close();
  outputFile->Close();
  delete inputFile;
  delete outputFile;

  delete treeHeader;
  delete treePMTData;

  for (unsigned int iCh = 0; iCh < hGlobal->NumCh; iCh++){
    delete data[iCh];
    delete hCh[iCh];

    const unsigned long int nbEvt = hSignal->size();

    for(unsigned long int iEvt = 0; iEvt < nbEvt; iEvt++){
      delete hSignal[iCh][iEvt];
    }

  }

  delete hGlobal;
}

bool PMTData::OpenPMTDataTTree(){

  treeHeader = (TTree*)inputFile->Get("PMTDataHeader");
  treePMTData = (TTree*)inputFile->Get("PMTData");

  if(!treeHeader->IsZombie() && !treePMTData->IsZombie()){

    hGlobal = new oscheader_global();

    // Assign structure to header branch
    treeHeader->SetBranchAddress("GlobalHeader",hGlobal);
    treeHeader->GetEntry(0);

    nbCh = hGlobal->NumCh;
    tStep = hGlobal->TimeStep*1e9; // in ns

    for(int iCh = 0; iCh < nbCh; iCh++){

      hCh[iCh] = new oscheader_ch();
      treeHeader->SetBranchAddress(Form("Ch%dHeader",iCh),hCh[iCh]);
      treeHeader->GetEntry(0);

      nbSamples[iCh] = hCh[iCh]->NumSamp;

      data[iCh] = new UInt_t[hCh[iCh]->NumSamp];
      treePMTData->SetBranchAddress(Form("DataCh%d",iCh),data[iCh]);

      hSignal[iCh].reserve(static_cast<unsigned long>(treePMTData->GetEntries()));
    }

    nbEntries = treePMTData->GetEntries();
    return nbEntries > 0;

  } else {

    std::cout << "Can't read TTree from file " << std::endl;
    return false;

  }
}

void PMTData::CreateWaveformsHistogram() {

  ////////////////////////////////////////
  // Creating Histograms from ROOT tree //
  ////////////////////////////////////////

  for (int iEntry = 0; iEntry < nbEntries; iEntry++) {

    // Point to data
    treePMTData->GetEntry(iEntry);

    for(unsigned int iCh = 0; iCh < hGlobal->NumCh; iCh++){

      // Create histogram and graph of signal
      hSignal[iCh][iEntry] = new TH1D(Form("hSignal_%s_%d_Ch%d", dataFileName.c_str(), iEntry,iCh),
                                      Form("Signal"),
                                      nbSamples[iCh],
                                      -tStep/2,
                                      (nbSamples[iCh]-1)*tStep + tStep/2);
      hSignal[iCh][iEntry]->GetXaxis()->SetTitle("Time (ns)");
      hSignal[iCh][iEntry]->GetYaxis()->SetTitle("Volts");

      // Extract baseline
      ExtractGND(iCh, data[iCh]);

      // Fill hist and graph
      for (int i = 0; i < nbSamples[iCh]; i++) {
        hSignal[iCh][iEntry]->SetBinContent(i + 1, -adc2V(data[iCh][i]));
      }

    } // END for iCh

    // Give status
    if (iEntry % 1000 == 0)
      std::cout << iEntry << " Processed ("
                << (int) ((double) (iEntry) * 100 / (double) (nbEntries))
                << "%)" << std::endl;

  } // END for iEntry

}
void PMTData::ExtractGND(int iCh, UInt_t *data) {

  TH1I *hRAWSignal = new TH1I("hRaw","hRaw",
                              nbSamples[iCh],
                              -tStep/2,
                              (nbSamples[iCh]-1)*tStep + tStep/2);

  // Extract baseline
  for (int i = 0; i < nbSamples[iCh]; i++) {
    hRAWSignal->SetBinContent(i + 1, data[i]);
  }
  hRAWSignal->Fit("pol0","QSME0+");
  TF1 *fit = hRAWSignal->GetFunction("pol0");
  if(fit){
    GND = (int)fit->GetParameter(0);
    hGND->Fill(GND);
    delete fit;
  }
  delete hRAWSignal;

}
void PMTData::WriteOutputFile() {

  std::string str;
  if(outputDir == "")
    str = p.parent_path().string() + "/" + p.stem().string() + "_FLAT.root";
  else
    str = outputDir + "/" + p.stem().string() + "_FLAT.root";

  outputFile = new TFile(str.c_str(),"RECREATE");
  std::cout << "Created " << str.c_str() << std::endl;

  outputFile->Write();
}

