//
// Created by zsoldos on 10/05/17.
//

#ifndef PMTANALYSIS_UTILS_H
#define PMTANALYSIS_UTILS_H

#include <string>
#include <cmath>
#include <TMath.h>
#include <TColor.h>
#include <vector>

inline double err(double p, double q, double t){
  return sqrt(p)/(q*t);
}

inline double errBinomial(double k, double N){
  return sqrt(k*(1-k/N))/N;
}

inline Double_t BG(Double_t *x, Double_t *par) {
  Double_t X = x[0];

  Double_t C0 = par[0];
  Double_t Q0 = par[1];
  Double_t sigma0 = par[2];

  Double_t C1 = par[3];
  Double_t Q1 = par[4];
  Double_t sigma1 = par[5];

  Double_t alpha = par[6];
  Double_t beta = par[7];
  Double_t delta = par[8];

  Double_t mu = par[9];

  Double_t result = 0;

  if(X<Q0){
    result =
        C0*TMath::Exp(-(X-Q0)*(X-Q0)/(2*sigma0*sigma0));
  } else {
    result =
        C0*TMath::Exp(-(X-Q0)*(X-Q0)/(2*sigma0*sigma0)) +
            beta*TMath::Exp(-alpha*(X-delta)) +
            C1*TMath::Exp(-(X-Q1)*(X-Q1)/(2*sigma1*sigma1));
  }

  Double_t lambda = C1/C0;

  for(int k=2; k<5; k++){
    Double_t Gk = mu*std::pow(lambda,k)*TMath::Exp(-lambda)/TMath::Factorial(k) *
        TMath::Exp(-(X-k*Q1-Q0)*(X-k*Q1-Q0)/(2*k*sigma1*sigma1));
    result += Gk;
  }

  return result;
}

inline Double_t simplePMT(Double_t *x, Double_t *par) {
  Double_t X = x[0];

  Double_t C0 = par[0];
  Double_t Q0 = par[1];
  Double_t sigma0 = par[2];

  Double_t Q1 = par[3];
  Double_t sigma1 = par[4];

  Double_t tau = par[5];
  Double_t CExp = par[6];
  Double_t delta = par[7];

  Double_t lambda = par[8];

  Double_t result = 0;

  if(X<delta){
    result =
        C0*TMath::Exp(-(X-Q0)*(X-Q0)/(2*sigma0*sigma0));
  } else {
    result =
        C0*TMath::Exp(-(X-Q0)*(X-Q0)/(2*sigma0*sigma0)) +
        CExp*TMath::Exp(-tau*(X-delta));
  }

  for(int k=1; k<5; k++){
    Double_t Gk =
        C0*std::pow(lambda,k)*TMath::Exp(-lambda)/TMath::Factorial(k) *
        TMath::Exp(-(X-k*(Q1-Q0))*(X-k*(Q1-Q0))/(2*k*sigma1*sigma1));
    result += Gk;
  }

  return result;
}

inline Double_t simpleGaus(Double_t *x, Double_t *par) {
  Double_t X = x[0];

  Double_t C0 = par[0];
  Double_t Q0 = par[1];
  Double_t sigma0 = par[2];

  Double_t result = 0;
  result = C0*TMath::Exp(-(X-Q0)*(X-Q0)/(2*sigma0*sigma0));

  return result;
}

inline Double_t simpleExpBckg(Double_t *x, Double_t *par) {
  Double_t X = x[0];

  Double_t tau = par[0];
  Double_t CExp = par[1];
  Double_t delta = par[2];

  Double_t result = 0;
  if(X<delta){
    result = 0;
  } else {
    result = CExp*TMath::Exp(-tau*(X-delta));
  }

  return result;
}

inline std::vector<int> createColor(TColor *Color){

  std::vector<int> palette;

  // ARCADIA #00a591
  palette.push_back(Color->GetColor("#00a591"));
  // CHERRY TOMATO #e94b3c
  palette.push_back(Color->GetColor("#e94b3c"));
  // LITTLE BOY BLUE #6f9fd8
  palette.push_back(Color->GetColor("#6f9fd8"));
  // PINK LAVENDER #dbb1cd
  palette.push_back(Color->GetColor("#dbb1cd"));
  // BLOOMING DAHLIA #ec9787
  palette.push_back(Color->GetColor("#ec9787"));
  // ULTRA VIOLET #6b5b95
  palette.push_back(Color->GetColor("#ec9787"));

  return palette;

}

#endif //PMTANALYSIS_UTILS_H
