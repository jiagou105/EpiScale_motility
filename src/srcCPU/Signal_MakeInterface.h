#include <vector>
#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <chrono>
#include "GeoVector.h"
#include "commonData.h"
using namespace std ;


double NormalCDFInverse(double p);
double RationalApproximation(double t);

class Signal {
    
    public :
    uint maxAllNodePerCell ;
    uint maxMembrNodePerCell ;
    uint maxTotalNodes ;
    uint maxCellCount ;
    int periodCount ;
    double minX,maxX,minY,maxY ;
    double curTime ;
    int maxTotalNumActiveNodes ;
    int numActiveCells ;
    double minResol, resol ;
    double frameNumber = 1.0 ;
    auto startChemical ;
    auto stopChemical ;
    auto durationChemical ;
    auto durationMechanical ;

    std::vector<bool> nodeIsActiveHost ;
    std::vector<double> nodeLocXHost, nodeLocYHost, cellCenterX,cellCenterY ;
    std::vector<double> dppLevel ;
    std::vector<double> tkvLevel ;  //Alireza
    std::vector<double> dppTkvLevel ;  //Alireza
    std::vector<double> pMadLevel ;  //Alireza
    std::vector<double> dppDistV,dppLevelV ;
    
    void updateSignal(double minX, double maxX, double minY, double maxY, double curTime, int maxTotalNumActiveNodes, int numActiveCells) ;
    
    void Initialize(uint maxAllNodePerCell, uint maxMembrNodePerCellECM, uint maxTotalNodes, uint maxCellCount) ;
    void exportGeometryInfo() ;
    void importSignalInfoCellLevel() ;
    void processSignalInfoCellLevel() ;
    void importSignalInfoTissueLevel() ;
    void processSignalInfoTissueLevel() ;
    
} ;


