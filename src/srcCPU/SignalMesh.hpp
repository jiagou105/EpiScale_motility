
#include "SignalCommon.hpp"
class Mesh
{
public:
    Mesh () ;
    vector<double> triangleX ;
    vector<double> triangleY ;
    vector<double> center ;
    vector<double> channel ;
    vector<double> length ;
    pair<int , int> connection ;   //represent the connection to the <int cellID,int MeshID>
    vector<double> flux ;
    double area ;
    double dt = 0.005 ;
    
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
    double kP = 1 ;
    double kLR = 1 ;
    vector< double> powers ;
    
    //--------------------------------Full model-Plant constants and variables---------------------------
    vector<double> concentrations ; //  < w , wc , wn , CLV , ck , ckr , CK,pMad >
    vector<double> concentrations2 ; 
    vector<double> diffusions ; // from 2nd to 5th, exclude 3rd
    vector<double> selfDiffusions ;
    vector<double> degradations ;
    vector<double> productions ;
    vector<double> rates ;          // <r_c, r_im, r_ex, 0 , k_on , k_on, k_off> //k_on sqrt for both or 1 for one of them
    vector<vector<double> > Flux ;
    
    //--------------------------------Full model-WingDisc constants and variables---------------------------
//    vector<double> dpp ;
 //   vector<double> dpp2 ;
 //   double diff ;
 //   vector<double> degreDpp ;
 //   vector<double> producDpp ;
    
    
    //---------------------------------------------------------------------------------------------
    
    void Cal_MeshCenter() ;
    double Cal_MeshArea () ;
    void Cal_UStar () ;
    void Euler () ;
    void UpdateU () ;
    
    void FullModel_Euler (bool type, double radius ) ;
    vector<double> UpdateDegradation (bool type , double radius) ;
    vector<double> UpdateRates ( bool type ) ;
    double UpdateCp () ;
    
};
