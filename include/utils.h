//
// Created by zsoldos on 10/05/17.
//

#ifndef PMTANALYSIS_UTILS_H
#define PMTANALYSIS_UTILS_H

#include <string>
#include <cmath>

inline std::string getFileName(const std::string& str){
  size_t found = str.find_last_of("/\\");
  std::string path = str.substr(found+1); // check that is OK
  return path;
}

inline double err(double p, double q, double t){
  return sqrt(p)/(q*t);
}


#endif //PMTANALYSIS_UTILS_H
