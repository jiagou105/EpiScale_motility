
#ifndef Signal_Calculator_h
#define Signal_Calculator_h

using namespace std ;
#include <vector>

vector< vector<double> > Signal_Calculator (vector< vector<double> > locX , vector< vector<double> > locY , vector<double > centX , vector<double > centY ,vector< vector<double> > oldConcentrations , double index, double &tCentX0 , bool &isNan ) ;


class SignalGlobalVar{
public:
    double diffusion = 20;
    double degradation = 0.4;
    double timeStep = 0.01 ;
    double dppProduction = 4.0 ;
    double sourceSize = 0.12 ;
    string folderName = "./signalVtkFiles/" ;
    double klr = 0.5 ;
    double kp2 = 0.3 ;

void Signal_get_config();
};
#endif /* Signal_Calculator_h */
