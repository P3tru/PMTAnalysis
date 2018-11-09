#include <iostream>

#include <TApplication.h>
#include <TCanvas.h>
#include <TRandom3.h>
#include <TColor.h>
#include <TSpectrum.h>

#include <PMTData.hh>
#include <PMTAnalyzer.hh>
#include <PMTParser.hh>

#include "utils.h"

#define MAXNUMFILES 10000 // MAX nb of files processed


int main(int argc, char *argv[]) {

  // Create TApp
  TApplication theApp("App", &argc, argv);

  PMTParser parserApp(&theApp);
  parserApp.LaunchApp();

  PMTData *data[MAXNUMFILES];
  PMTAnalyzer *analysis[MAXNUMFILES];

  TCanvas *c1;
  TF1 *fit;
  TF1 *fPED;
  TF1 *fEXP;
  TF1 *fSPE;
  TColor *Color = new TColor();
  std::vector<int> palette = createColor(Color);

  TH1D *hQMean = new TH1D("hQMean","Mean measured DN rates",100,0.,10000.);

  // INSERT FUNCTIONS BELOW
  /////////////////////////

  for(int iFile=0; iFile < parserApp.getNFiles();iFile++){

    std::cout << "Processing file " << parserApp.getPathSources()[iFile] << std::endl;
    data[iFile] = new PMTData(parserApp.getPathSources()[iFile]);
    analysis[iFile] = new PMTAnalyzer(data[iFile]);

    /////////////////
    // DO ANALYSIS //
    /////////////////

    const int iCh = 0;

    c1 = new TCanvas(Form("cQ_%d",iFile),Form("cQ_%d",iFile),1200,800);
    c1->SetLogy();
    c1->SetGrid();
    analysis[iFile]->ComputeQ(iCh);
    analysis[iFile]->getChargeSignal(iCh)->Draw("E");
    analysis[iFile]->FitDarkCounts(iCh,palette[0]);

    // Recover fit parameter and Draw
    fit = analysis[iFile]->getChargeSignal(iCh)->GetFunction("fitPMT");
    double minQ = analysis[iFile]->getMinQ();
    double maxQ = analysis[iFile]->getMaxQ();

    fPED = new TF1("fPED",simpleGaus,minQ,maxQ,3);
    fPED->SetParameter(0,fit->GetParameter(0));
    fPED->SetParameter(1,fit->GetParameter(1));
    fPED->SetParameter(2,fit->GetParameter(2));
    fPED->SetLineColor(palette[1]);
    fPED->SetLineStyle(2);
    fPED->SetLineWidth(2.);
    fPED->SetFillColor(palette[1]);
    fPED->SetFillStyle(3004);
    fPED->SetFillColorAlpha(palette[1], 0.35);
    fPED->Draw("SAME C");

    fEXP  = new TF1("fEXP",simpleExpBckg,minQ,maxQ,3);
    fEXP->SetParameter(0,fit->GetParameter(5));
    fEXP->SetParameter(1,fit->GetParameter(6));
    fEXP->SetParameter(2,fit->GetParameter(7));
    fEXP->SetLineColor(palette[3]);
    fEXP->SetLineStyle(2);
    fEXP->SetLineWidth(2.);
    fEXP->Draw("SAME");

    double lambda = fit->GetParameter(8);
    const double norm =  (int)(data[iFile]->getNbSamples(iCh) / analysis[iFile]->nTot) * data[iFile]->getNbEntries();
    for(int k=1; k<5 ; k++){
      fSPE = new TF1("fSPE",simpleGaus,minQ,maxQ,3);
      fSPE->SetParameter(0,fit->GetParameter(0)*std::pow(lambda,k)*TMath::Exp(-lambda)/TMath::Factorial(k));
      fSPE->SetParameter(1,fit->GetParameter(3));
      fSPE->SetParameter(2,fit->GetParameter(4));
      fSPE->SetLineColor(palette[2]);
      fSPE->SetFillColor(palette[2]);
      fSPE->SetFillStyle(3004);
      fSPE->SetFillColorAlpha(palette[2], 0.35);
      fSPE->SetLineStyle(2);
      fSPE->SetLineWidth(2.);
      fSPE->Draw("SAME C");

      std::cout << "Rate PE#" << k <<  " : " << fSPE->Integral(minQ,maxQ)*norm << " Hz" << std::endl;
      if(k==1) hQMean->Fill(fSPE->Integral(minQ,maxQ)*norm);
    }

  } // END FOR iFILE

  std::cout << "Mean DN value measured : " << hQMean->GetMean()
            << " +- " << hQMean->GetRMS() << std::endl;

  /////////////////////////
  // ...

  std::cout << std::endl;
  std::cout << "Hit Ctrl+C to exit" << std::endl;
  theApp.Run(kTRUE);

  return 0;
}
