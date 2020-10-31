
#ifndef Signal_Calculator_h
#define Signal_Calculator_h

using namespace std ;
#include <vector>

vector< vector<double> > Signal_Calculator (vector< vector<double> > locX , vector< vector<double> > locY , vector<double > centX , vector<double > centY ,vector< vector<double> > oldConcentrations , double index ) ;


class SignalGlobalVar{
public:
    double diffusion = 0;
    double degradation = 0;
    double timeStep = 0.01 ;
    double sourceSize = 0.12 ;
    string folderName = "./signalVtkFiles/" ;
void Signal_get_config();
};
#endif /* Signal_Calculator_h */
