# PMTAnalysis

A framework to analyse PMTData written in ROOT format, inside a TTree. (cf https://root.cern.ch/)
The ROOT file is written in a specific structure. The following table describe the structure of the 2 trees holding the DAQ information and the data per channel :

|PMTDataHeader             | PMTData                     |
|------------------------  |-----------------------------|
|TBranch* GlobalHeader     |UInt_t DataChXXX[nbSmp]      |
|TBranch* ChXXXHeader      |UInt_t DataChXXX[nbSmp]...   |
|TBranch* ChXXXHeader ...  |

The *ChXXX* value correspond to the channel number. 

The software will **only** work using  **ROOT files written in this format**. It can handles the analysis of multiple files at the same time too. It handles also uses of joker * to select the files.
Have a look at https://github.com/P3tru/QMUL-data-converter in order to write your data in the appropriate format.

Classes provided to build analysis software are describe in the following :

## PMTData

This object opens the ROOT TTrees and create the relevant signal histograms.
```C++
PMTData *Data = new PMTData(std::string inputFileName);
TH1I *hExample = Data->getSignalHistogram(0,0);
hExample->Draw();
```

## PMTAnalyzer

This object takes a PMTData in argument and contains the fundamental bricks and methods to analyze your signals. Construction of the class is on-going at the moment.

# Programs available (edit from 19/07/2018)

 - PMTAnalysis which is based on main.cc
This is a template program who takes data file in argument and open the TTrees inside, to create the signal histogram.
```bash
make PMTAnalysis
./PMTAnalysis /path/to/data1.root /path/to/data2.root ...
```

 - showPlots (based on showPlots.cc)
This is an example program plotting the first signal recorded on channel Ch0 on your data.
```bash
make showPlots
./showPlots /path/to/data1.root /path/to/data2.root ...
```

