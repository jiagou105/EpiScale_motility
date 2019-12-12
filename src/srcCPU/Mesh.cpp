
#include "Mesh.hpp"

Mesh::Mesh()
{
    connection.first = -1 ;
    connection.second = -1 ;
    area.resize(3) ;
    length.resize(3) ;
    concentrations.resize(7) ;
    concentrations2.resize(7) ;
    diffusions.resize(7) ;
    degradations.resize(7) ;
    productions.resize(7) ;
    rates.resize(7) ;
    diffusions = {0,2,0,2,2,0,0} ;
    degradations = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5} ;
    concentrations = {0,0,0,0,0,0,0} ;
    concentrations2 = {0,0,0,0,0,0,0} ;
    productions = {1,0,0,1,1,1,0} ;
    rates = {1,1,1,0,1,1,1} ; // <r_c, r_im, r_ex, 0 , k_on , k_on, k_off>
    powers.resize(4) ;
    powers = {1,1,1,1} ;
}

//--------------------------------------------------------------------------------------------
void Mesh::Cal_Center()
{
    
    center.push_back(accumulate(triangleX.begin(), triangleX.end(), 0.0)/triangleX.size ()) ;
    center.push_back(accumulate(triangleY.begin(), triangleY.end(), 0.0)/triangleY.size ()) ;
    
}
//---------------------------------------------------------------------------------------------
void Mesh::Cal_UStar ()
{
    double totalFlux = accumulate(flux.begin(), flux.end(), 0.0) ;
    uStar = u1 + dt * (totalFlux + c - d*u1) ;
}
//---------------------------------------------------------------------------------------------
void Mesh::Euler()
{
    double totalFlux = accumulate(flux.begin(), flux.end(), 0.0) ;
    u2 = u1 + dt * (totalFlux + c - d * u1 ) ;
    flux.clear() ;
}
//---------------------------------------------------------------------------------------------
void Mesh::UpdateU()
{
    u1 = u2   ;
    concentrations = concentrations2 ;
}
//---------------------------------------------------------------------------------------------
void Mesh::FullModel_Euler()
{
    
    vector<double> degradationChanges = UpdateDegradation() ;
    vector<double> rateChanges = UpdateRates() ;
    
    
    vector<double> totalFlux ;
    totalFlux.clear() ;
    for (int j= 0 ; j< diffusions.size() ; j++ )
    {
       totalFlux.push_back( sum_over_vec(Flux, j) ) ;
        
    }
    vector<double> tmpDebug = concentrations2 ;
    // Diffusion changes from 2nd to 5th
    transform(concentrations.begin()+1, concentrations.begin()+5, totalFlux.begin()+1, concentrations2.begin()+1, linearConfig (1, dt ) ) ;
    // degradation changes
    transform(concentrations2.begin(), concentrations2.end(),degradationChanges.begin() ,concentrations2.begin(), linearConfig( 1,-dt )  ) ;
    //rate changes ( dt is included )
    transform(concentrations2.begin()+1 , concentrations2.end(), rateChanges.begin()+1 ,
              concentrations2.begin()+1 , linearConfig(1 , -1 ) ) ;
    concentrations2.at(1) += rateChanges.at(0) + rateChanges.at(2) ;
    concentrations2.at(2) += rateChanges.at(1) ;
    concentrations2.at(4) += rateChanges.at(6) ;
    concentrations2.at(5) += rateChanges.at(6) ;
    concentrations2.at(6) += rateChanges.at(4) ;
    
    //production changes
    vector<double> tmpProduction = productions ;
    tmpProduction.at(0) *= 1/(1+ pow(concentrations.at(3)/kcw,powers.at(0) ) ) ;
    tmpProduction.at(3) *= UpdateCp() ;
    transform(concentrations2.begin(), concentrations2.end(), tmpProduction.begin(), concentrations2.begin(),linearConfig(1,dt) ) ; 
    
    
}
//---------------------------------------------------------------------------------------------
vector<double> Mesh::UpdateDegradation()
{
    degradations.at(1) = degradations.at(0) * (cMin + (cMax - cMin)/(1 + pow(concentrations.at(6)/ kCK,powers.at(1)) ) )
                                            * (eMin + (eMAx- eMin)/(1+pow(concentrations.at(3)/kcw,1)  ) ) ;
 //  degradations.at(2) = ( degradations.at(0)/(1 + pow(concentrations.at(6)/ kCK,powers.at(1) ) ) ) *
 //                                                   (1 / (1+pow(concentrations.at(3)/kcw, powers.at(3) ) ) ) ;
    degradations.at(2) = degradations.at(0) / (1 + pow(concentrations.at(2)/kcw,powers.at(3) ) ) ;
    vector<double> tmp ;
    tmp.resize(degradations.size() ) ;
    transform(concentrations.begin(), concentrations.end(), degradations.begin(), tmp.begin(), productVec() ) ;
    return tmp ;

}
//---------------------------------------------------------------------------------------------

vector<double> Mesh:: UpdateRates()
{
    // <r_c, r_im, r_ex, 0 , k_on , k_on, k_off>
    vector<double> tmp ;
    tmp.resize(concentrations.size()) ;
    transform(concentrations.begin(), concentrations.end(),rates.begin() , tmp.begin(), productVec() ) ;
    transform(tmp.begin(), tmp.end(), tmp.begin(), productNum( dt) ) ;
    tmp.at(2) *= 1/(1 + pow(concentrations.at(3)/kcw, powers.at(2) ) ) ;
    tmp.at(2) *= 1/(1 + pow(concentrations.at(6)/kCK, powers.at(1) ) ) ;
    tmp.at(4) *= concentrations.at(5) ;
    tmp.at(5) = tmp.at(4) ;
    
    return tmp ;
}
//---------------------------------------------------------------------------------------------
double Mesh::UpdateCp ()
{
    double w1 = 10 ;
    double w2 = 100 ;
    double kLow = 1 ;
    double wn = concentrations.at(2) ;
    double cp = 1 ;
    if (wn > w1)
    {
        cp *= 2/(1+(wn-w1)/(w2-w1) ) ;
    }
    else
    {
        cp *= 2/(1+(w1-wn)/kLow) ;
    }
    return cp ;
}
