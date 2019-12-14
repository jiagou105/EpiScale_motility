
#include "common.hpp"
#define dt 0.001

class Mesh
{
public:
    Mesh () ;
    vector<double> triangleX ;
    vector<double> triangleY ;
    vector<double> center ;
    vector<double> area ;
    vector<double> length ;
    pair<int , int> connection ;   //represent the connection to the <int cellID,int MeshID>
    vector<double> flux ;
    
    double u1 = 0.0 ;        //protein concentration (old)
    double u2 = 0.0 ;         //protein concentration (new)
    double uStar ;
    double c = 0.0 ;         //secretion rate
    double d = 0.05 ;          //death rate\

    
    double eMin = 0 ;
    double eMAx = 1 ;
    double cMin = 0 ;
    double cMax = 0 ;
    double kCK = 1 ;
    double kcw = 1 ;
    vector< double> powers ;
    
    //--------------------------------Full model constants and variables---------------------------
    vector<double> concentrations ; //  < w , wc , wn , CLV , ck , ckr , CK >
    vector<double> concentrations2 ; 
    vector<double> diffusions ; // from 2nd to 5th, exclude 3rd
    vector<double> degradations ;
    vector<double> productions ;
    vector<double> rates ;          // <r_c, r_im, r_ex, 0 , k_on , k_on, k_off> //k_on sqrt for both or 1 for one of them
    vector<vector<double> > Flux ;
    
    
    
    
    
    //---------------------------------------------------------------------------------------------
    
    void Cal_Center() ;
    void Cal_UStar () ;
    void Euler () ;
    void UpdateU () ;
    
    void FullModel_Euler () ;
    vector<double> UpdateDegradation () ;
    vector<double> UpdateRates () ;
    double UpdateCp () ;
};
