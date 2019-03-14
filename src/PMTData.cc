//
// Created by zsoldos on 10/05/17.
//

#include <iostream>

#include <boost/algorithm/string/replace.hpp>

#include <TF1.h>

#include <PMTData.hh>

#include "ProgressBar.hpp"

PMTData::PMTData(PMTParser const& parser, int indexFile) {

  // 2 cases, normal or flat input data file.
  // IF normal, then read all the relevant infro directly from PMTTreeHeader
  // IF flat, set it up thank to PMT parser

  // Open TTree inside rootFile
  // Common to both situation
  p = parser.getPathSources()[indexFile];
  inputFile = new TFile(p.c_str());
  dataFileName = p.stem().string();

  if(inputFile->IsZombie()){
    std::cout << "CANT OPEN INPUTFILE --- EXIT" << std::endl;
    exit(EXIT_FAILURE);
  } else {
    std::cout << "Input ROOT file NOT zombie" << std::endl;
  }

  // Save the different output PARAMS
  writeOutput = parser.isWriteOutput();
  outputDir = parser.getOutputDir();

  isInputFileFlat = parser.isIsDataFlat();

  if(!isInputFileFlat){ // CASE NORMAL

    // We will need to create the signals from PMTTree
    // Therefore need to measure the GND baseline

    SetGNDHist();

    // VOLT to ADC conversion is read through the PMTTree header normally

    voltConv = 1;

    // Init protected variable

    nbEntries = 0;

    // Open the PMTTree and create the histogram

    if(OpenPMTDataTTree()){
      std::cout << "Tree open successfully" << std::endl;
      CreateWaveformsHistogram();
    }


  } else { // CASE FLAT

    // Signal are already created, need to read the params from parser file

    boost::replace_all(dataFileName,"_FLAT","");

    if(parser.getNbSignals()){
      nbEntries=parser.getNbSignals();
    } else {
      nbEntries= static_cast<unsigned long>(inputFile->GetListOfKeys()->GetEntries());
      std::cout << "NbEntries read from flat: " << nbEntries << std::endl;
    }
    nbCh=parser.getNbCh();
    for(int iCh = 0; iCh<nbCh; iCh++){
      nbSamples[iCh] = parser.getNbSamples()[iCh];
    }
    tStep=parser.getTStep();

    // Open the PMTTree and READ the histogram

    if(OpenPMTDataFlatTree()){
      std::cout << "FLAT Tree open successfully" << std::endl;
    }

  }

}


PMTData::~PMTData() {

  inputFile->Close();
  for(unsigned int iFile=0; iFile<outputFile.size(); iFile++){
    delete outputFile[iFile];
  }
  delete inputFile;

  delete treeHeader;
  delete treePMTData;

  for (unsigned int iCh = 0; iCh < hGlobal->NumCh; iCh++){
    delete data[iCh];
    delete hCh[iCh];

    const unsigned long int nbEvt = hSignal[iCh].size();

    for(unsigned long int iEvt = 0; iEvt < nbEvt; iEvt++){
      delete hSignal[iCh][iEvt];
      delete gSignal[iCh][iEvt];
    }

  }

  delete hGlobal;
}

void PMTData::SetDummyRawHistogram(int iCh) {

  hRAWSignal = new TH1I("hRaw","hRaw",
                        nbSamples[iCh],
                        -tStep/2,
                        (nbSamples[iCh]-1)*tStep + tStep/2);

}

void PMTData::SetGNDHist(unsigned int nbBinsGND, int minGND, int maxGND){

  GND = 0;

  std::cout << "GND histogram params" << std::endl;
  std::cout << "nbBinsGND: " << nbBinsGND << std::endl;
  std::cout << "minGND: " << minGND << std::endl;
  std::cout << "maxGND: " << maxGND << std::endl;

  hGND = new TH1D("hGND",
                  "Extracted baseline values",
                  nbBinsGND,
                  minGND,
                  maxGND);
  hGND->SetXTitle("ADC Channels");
  hGND->SetMarkerSize(2);

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

      hSignal.emplace_back();
      gSignal.emplace_back();

    }

    nbEntries = static_cast<unsigned long>(treePMTData->GetEntries()-1);
    return nbEntries > 0;

  } else {

    std::cout << "Can't read TTree from file " << std::endl;
    return false;

  }

}

bool PMTData::OpenPMTDataFlatTree(){

  std::cout << "Starting loop on data flat tree" << std::endl;

  ////////////////////////////////////////
  // Progress bar init                  //
  ////////////////////////////////////////
  ProgressBar progressBar(nbEntries, 70);

  hSignal.reserve(nbCh);

  for(int iCh = 0; iCh < nbCh; iCh++) {

    for (int iEntry = 0; iEntry < nbEntries; iEntry++) {

      hSignal[iCh].push_back(
          (TH1D*)inputFile->Get(Form("hSignal_%s_%d_Ch%d", dataFileName.c_str(), iEntry, iCh))
      );
      hSignal[iCh][iEntry]->SetDirectory(gROOT);

      if(!hSignal[iCh][iEntry]){
        std::cout << "CANT OPEN SIGNAL: " << Form("hSignal_%s_%d_Ch%d", dataFileName.c_str(), iEntry, iCh)
                  << std::endl
                  << "Check if it exist in the flat tree"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      // record the tick
      ++progressBar;
      // display the bar
      progressBar.display();

    }

  }

  // tell the bar to finish
  progressBar.done();

  if(hSignal[nbCh-1][nbEntries-1]){
    return true;
  } else {
    std::cout << "Not all signal were read!" << std::endl;
    return false;
  }

}

void PMTData::CreateWaveformsHistogram() {

  std::cout << "Creating waveforms histogram hSignal" << std::endl;

  ////////////////////////////////////////
  // Progress bar init                  //
  ////////////////////////////////////////
  ProgressBar progressBar(nbEntries, 70);

  ////////////////////////////////////////
  // Creating Histograms from ROOT tree //
  ////////////////////////////////////////

  for(unsigned int iCh = 0; iCh < nbCh; iCh++){

    SetDummyRawHistogram(iCh);

    for (int iEntry = 0; iEntry < nbEntries; iEntry++) {

      // Point to data
      treePMTData->GetEntry(iEntry);

      // Create histogram and graph of signal
      hSignal[iCh].push_back(
          new TH1D(Form("hSignal_%s_%d_Ch%d", dataFileName.c_str(), iEntry,iCh),
                   Form("Signal #%d", iEntry),
                   nbSamples[iCh],
                   -tStep/2,
                   (nbSamples[iCh]-1)*tStep + tStep/2)
      );

      hSignal[iCh][iEntry]->GetXaxis()->SetTitle("Time (ns)");
      hSignal[iCh][iEntry]->GetYaxis()->SetTitle("Volts");

      ExtractGND(iCh, data[iCh]);

      // Fill hist and graph
      for (int i = 0; i < nbSamples[iCh]; i++) {

        hSignal[iCh][iEntry]->SetBinContent(i+1,-adc2V(data[iCh][i]));
        // gSignal[iCh][iEntry]->SetPoint(i,i*tStep,-adc2V(data[iCh][i]));

      } // END for iSample

      // record the tick
      ++progressBar;
      // display the bar
      progressBar.display();

    } // END for iEntry

  } // END for iCh

  // tell the bar to finish
  progressBar.done();

}

void PMTData::ExtractGND(int iCh, UInt_t *data) {

  // 1. Define fix value for GND
  // GND = 8215;

  // 2. Extract GND from Data
  hRAWSignal->Reset();
  // Extract baseline
  for (int i = 0; i < nbSamples[iCh]; i++) {
    hRAWSignal->SetBinContent(i + 1, data[i]);
  }

  // 2.1 Use Mean of histogram
  // GND = static_cast<int>(hRAWSignal->GetMean());

  // 2.2 Use Fit on data (Good for dark rates)
  hRAWSignal->Fit("pol0","Q0CF");
  TF1 *fit = hRAWSignal->GetFunction("pol0");

  if(fit){
    GND = (int)fit->GetParameter(0);
    hGND->Fill(GND);
  }

  delete fit;

}

void PMTData::SetOutputFile(int iFile) {

  std::string str;
  if(outputDir.empty()){
    if(iFile>-1)
      str = p.parent_path().string() + "/" + p.stem().string() + Form("_FLAT%d.root",iFile);
    else
      str = p.parent_path().string() + "/" + p.stem().string() + "_FLAT.root";
  } else {
    if(iFile>-1)
      str = outputDir + "/" + p.stem().string() + Form("_FLAT%d.root",iFile);
    else
      str = outputDir + "/" + p.stem().string() + "_FLAT.root";
  }

  outputFile.push_back( new TFile(str.c_str(),"RECREATE") );
  std::cout << "OUTPUT FILE Created " << str.c_str() << std::endl;

}

void PMTData::WriteOutputFile(int nbMaxEntriesPerFile){

  std::cout << "Starting PMTData::WriteOutputFile()" << std::endl;

  ////////////////////////////////////////
  // Progress bar init                  //
  ////////////////////////////////////////
  ProgressBar progressBar(nbEntries, 70);

  if(nbMaxEntriesPerFile > 0){

    // Define number of files to be created
    const auto nbFiles = static_cast<const unsigned int>(nbEntries / nbMaxEntriesPerFile);
    const auto nbEntriesInLastFile = static_cast<const unsigned int>(nbEntries % nbMaxEntriesPerFile);

    std::cout << "Nb of files needed: " << nbFiles << std::endl;
    std::cout << "Nb of entries in last file: " << nbEntriesInLastFile << std::endl;

    if(nbFiles > 0){

      ///////////////////////////////////////////////////
      // General case for constant nbMaxEntriesPerFile //
      ///////////////////////////////////////////////////

      for(int iFile=0; iFile<nbFiles; iFile++){

        SetOutputFile(iFile);
        outputFile[iFile]->cd();

        for(unsigned int iCh = 0; iCh < nbCh; iCh++) {
          for (int iEntry = iFile * nbMaxEntriesPerFile; iEntry < (iFile+1) * nbMaxEntriesPerFile; iEntry++) {

            //Write histogram into TFile
            if(hSignal[iCh][iEntry])
              hSignal[iCh][iEntry]->Write();

            // record the tick
            ++progressBar;
            // display the bar
            progressBar.display();

          } // END for iEntry
        } // END for iCh

        outputFile[iFile]->Close();

      } // END for iFile

      //////////////////////////////////////////////////////////////
      // SPECIAL case for last file with only nbEntriesInLastFile //
      //////////////////////////////////////////////////////////////

      SetOutputFile(nbFiles);
      outputFile[nbFiles]->cd();

      const auto iLastFile = nbFiles * nbMaxEntriesPerFile;

      for(unsigned int iCh = 0; iCh < nbCh; iCh++) {
        for (auto iEntry = iLastFile; iEntry < iLastFile + nbEntriesInLastFile; iEntry++) {

          //Write histogram into TFile
          if(hSignal[iCh][iEntry])
            hSignal[iCh][iEntry]->Write();

          // record the tick
          ++progressBar;
          // display the bar
          progressBar.display();

        } // END for iEntry
      } // END for iCh

      outputFile[nbFiles]->Close();

      // tell the bar to finish
      progressBar.done();


    } else {

      SetOutputFile();
      outputFile[0]->cd();

      for(unsigned int iCh = 0; iCh < nbCh; iCh++) {
        for (int iEntry = 0; iEntry < nbEntries; iEntry++) {

          if(hSignal[iCh][iEntry])
            hSignal[iCh][iEntry]->Write();

          // record the tick
          ++progressBar;
          // display the bar
          progressBar.display();

        } // END for iEntry
      } // END for iCh

      // tell the bar to finish
      progressBar.done();

      outputFile[0]->Close();

    }

  } else {

    SetOutputFile();
    outputFile[0]->cd();

    for(unsigned int iCh = 0; iCh < nbCh; iCh++) {
      for (int iEntry = 0; iEntry < nbEntries; iEntry++) {

        if(hSignal[iCh][iEntry])
          hSignal[iCh][iEntry]->Write();

        // record the tick
        ++progressBar;
        // display the bar
        progressBar.display();

      } // END for iEntry
    } // END for iCh

    // tell the bar to finish
    progressBar.done();

    outputFile[0]->Close();

  }

}

void PMTData::SetTGraphSignals(int iCh, int iEntry) {

  gSignal[iCh][iEntry]->SetTitle(Form("Signal #%d ; Time (ns) ; Volts",iEntry));
  gSignal[iCh][iEntry]->SetLineColor(kBlue-4);
  gSignal[iCh][iEntry]->SetLineStyle(2);
  gSignal[iCh][iEntry]->SetLineWidth(2.);
  gSignal[iCh][iEntry]->SetMarkerColor(kBlue-4);
  gSignal[iCh][iEntry]->SetMarkerStyle(kFullCross);
  gSignal[iCh][iEntry]->SetMarkerSize(1.);

}
