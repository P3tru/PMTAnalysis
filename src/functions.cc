#include "functions.hh"


Double_t fit(Double_t* x, Double_t* par){
  /*
    par[0] N number of photoelectrons (integer,  must be fixed by user)
    par[1] Constant
    par[2] w
    par[3] Q0 mean of background distribution
    par[4] sigma0 standard deviation of background distribution
    par[5] alpha parameter of the decreasing exponential
    par[6] mu parameter of the poisson distribution
    par[7] Q1 mean of ideal distribution
    par[8] sigma1 standard deviation of ideal distribution   
   */
  Double_t N = par[0], Const = par[1], w = par[2], Q0 = par[3], sigma0 = par[4], alpha = par[5], X = x[0];
  Double_t mu = par[6], Q1 = par[7], sigma1 = par[8], r2pi = TMath::Sqrt(2*TMath::Pi()), Qsh = w/alpha, e_mu = TMath::Exp(-mu);
  Int_t n;
  
  Double_t theta = 0;
  Double_t ideal = 0, background = 0;
  Double_t spen = 0;

  if(X-Q0 < 0) theta = 0;
  else theta = 1;
  //background
  background = (1-w)/(sigma0*r2pi) * TMath::Exp(-(X-Q0)*(X-Q0)/(2*sigma0*sigma0)) + w*theta*alpha*TMath::Exp(-alpha*(X-Q0));
  background = e_mu*background;

  //ideal
  for(n = 1; n <= N; n++){
    float mu_n = TMath::Power(mu, n);
    float Rn = TMath::Sqrt(n);
    spen = mu_n*e_mu/(TMath::Factorial(n)*sigma1*r2pi*Rn)*TMath::Exp(-TMath::Power(X-Q0-Qsh-n*Q1, 2)/(2*n*sigma1*sigma1));
    ideal += spen;
  }
  
  return Const*(background + ideal);
}

Double_t realfit(Double_t* x, Double_t* par){
  /*
    par[0] N number of photoelectrons (integer,  must be fixed by user)
    par[1] w
    par[2] Q0 mean of background distribution
    par[3] sigma0 standard deviation of background distribution
    par[4] alpha parameter of the decreasing exponential
    par[5] mu parameter of the poisson distribution
    par[6] Q1 mean of ideal distribution
    par[7] sigma1 standard deviation of ideal distribution   
   */
  Double_t N = par[0], w = par[1], Q0 = par[2], sigma0 = par[3], alpha = par[4], X = x[0];
  Double_t mu = par[5], Q1 = par[6], sigma1 = par[7], r2pi = TMath::Sqrt(2*TMath::Pi()), e_mu = TMath::Exp(-mu);
  Int_t n;
  
  Double_t result = 0, spen = 0, Gn = 0, IGnE = 0, Qn = 0, sigman = 0, Rn = 0, mu_n = 0;
  Double_t sign = 0;
  
  for(n=1; n < N; n++){
    Qn = Q0 + n*Q1;
    sigman = TMath::Sqrt(sigma0*sigma0 + n*sigma1*sigma1);
    Rn = TMath::Sqrt(n);
    
    if(X-Qn-sigman*alpha < 0) sign = -1;
    else sign = 1;

    Gn = 1/(sigma1*r2pi*Rn)*TMath::Exp(-(X-n*Q1-Q0)*(X-n*Q1-Q0)/(2*n*sigma1*sigma1));
    IGnE = alpha/2*TMath::Exp(-alpha*(X-Qn-sigman*sigman*alpha))*TMath::Erf(TMath::Abs(Q0-Qn-sigman*sigman*alpha)/(sigman*TMath::Sqrt(2))) + sign*TMath::Erf(TMath::Abs(X-Qn-sigman*sigman*alpha)/(sigman*TMath::Sqrt(2)));

    spen = mu_n*e_mu/TMath::Factorial(n)*((1-w)*Gn + w*IGnE);

    result += spen;
  }
  return result;
}


//definitions of different parts of the fitting function, for display purposes
Double_t BG(Double_t* x, Double_t* par){
  Double_t Const = par[1], w = par[2], Q0 = par[3], sigma0 = par[4], X = x[0];
  Double_t mu = par[6], r2pi = TMath::Sqrt(2*TMath::Pi()), e_mu = TMath::Exp(-mu);

  return Const*(1-w)/(sigma0*r2pi)*TMath::Exp(-(X-Q0)*(X-Q0)/(2*sigma0*sigma0))*e_mu;
}
Double_t EXP(Double_t* x, Double_t* par){
  Double_t Const = par[1], w = par[2], Q0 = par[3], alpha = par[5], X = x[0], theta = 0;
  Double_t mu = par[6], e_mu = TMath::Exp(-mu);
  if(X-Q0 < 0) theta = 0;
  else theta = 1;

  return Const*w*theta*alpha*TMath::Exp(-alpha*(X-Q0))*e_mu;
}
Double_t SPE(Double_t* x, Double_t* par){
  Double_t N = par[0], Const = par[1], w = par[2], Q0 = par[3], alpha = par[5], X = x[0];
  Double_t mu = par[6], Q1 = par[7], sigma1 = par[8], r2pi = TMath::Sqrt(2*TMath::Pi()), Qsh = w/alpha, e_mu = TMath::Exp(-mu);

  return Const*TMath::Power(mu, N)*e_mu/TMath::Factorial(N)/(sigma1*r2pi*TMath::Sqrt(N))*TMath::Exp(-TMath::Power(X-Q0-Qsh-N*Q1, 2)/(2*N*sigma1*sigma1));
}
