//
// Created by zsoldos on 10/05/17.
//

#include <iostream>

#include <utils.h>
#include "PMTData.hh"

PMTData::PMTData() {
  tree = new TTree("PMTData","PMTData");
  GND=0;
}

PMTData::PMTData(PMTData const &copy) {
  inputFilePath = copy.getInputFilePath();
  inputFileName = copy.getInputFileName();
  file = copy.getROOTFile();
  tree = copy.getROOTTree();
  GND=0;
}

PMTData::PMTData(std::string fname) {
  inputFilePath = fname;
  tree = new TTree("PMTData","PMTData");
  GND=0;
}

PMTData::~PMTData() {

  delete file;
  delete tree;

}

bool PMTData::createTree() {

  // Parameter to read line from input data file
  std::string line;

  // Determine the ground baseline of signal
  TH1I hGND("hGND","hGND",100,8200,8300);

  // Open ASCII input data file
  inputFileName = getFileName(inputFilePath);
  openDataFile(inputFilePath);

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

    return true;

  } else {
    std::cout << "Unable to open file" << std::endl;
    return false;
  }

}
void PMTData::saveTree(std::string outputPath) {

  std::string str;
  if(outputPath == "0") str = "output" + inputFileName;
  else str = outputPath.append(inputFileName);

  file = new TFile(Form("%s.root",str.c_str()),"RECREATE");
  if(file->IsOpen()){
    std::cout << "Created " << Form("%s.root",str.c_str()) << std::endl;
    tree->Write();
    file->Close();
  }else{
    std::cout << "Couldnt open rootfile" << std::endl;
  }

}
