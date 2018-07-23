//
// Created by zsoldos on 10/05/17.
//

#include <iostream>

#include <boost/filesystem.hpp>

#include <utils.h>
#include <PMTData.hh>

PMTData::PMTData(std::string inputUserArg) {

  // Open TTree inside rootFile
  inputFile = new TFile(inputUserArg.c_str());
  boost::filesystem::path p(inputUserArg);

  dataFileName = p.stem().string();

  GND = 8225;
  voltConv = 2.06/5000.0;
  nbEntries=0;

  if(OpenPMTDataTTree()){
    std::cout << "Tree open successfully" << std::endl;
    CreateWaveformsHistogram();
  }

  std::string str;
  str = p.parent_path().string() + "/" + p.stem().string() + "_FLAT.root";

  outputFile = new TFile(str.c_str(),"RECREATE");
  std::cout << "Created " << str.c_str() << std::endl;

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
      hSignal[iCh][iEntry] = new TH1F(Form("hSignal_%s_%d_Ch%d", dataFileName.c_str(), iEntry,iCh),
                                      Form("Signal"),
                                      nbSamples[iCh],
                                      -tStep/2,
                                      (nbSamples[iCh]-1)*tStep + tStep/2);
      hSignal[iCh][iEntry]->GetXaxis()->SetTitle("Time (ns)");
      hSignal[iCh][iEntry]->GetYaxis()->SetTitle("Volts");

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

