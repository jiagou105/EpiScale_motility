
#include "common.hpp"



double Dist2D (double x1 , double y1 , double x2 , double y2 )
{
    return sqrt(pow(x1- x2,2) + pow(y1- y2,2)) ;
}

//---------------------------------------------------------------------------------------------
vector<double> Dist_PointToVec2D (double x1, double y1, vector<double> X2, vector<double> Y2 )
{
    vector<double> point_vecDist ;
    for (int j =0 ; j < X2.size(); j++)
    {
        point_vecDist.push_back( Dist2D(x1, y1 ,X2.at(j), Y2.at(j)) ) ;
    }
    return point_vecDist ;
}

//---------------------------------------------------------------------------------------------
vector<vector <double> > Dist_VecToVec2D ( vector<double> X1 , vector<double> Y1 , vector<double> X2, vector<double> Y2 )
{
    vector<vector<double> > vec_vecDist ;
    for (int i=0 ; i < X1.size(); i ++)
    {
        vec_vecDist.push_back(Dist_PointToVec2D( X1.at(i) , Y1.at(i) ,X2 , Y2 ) ) ;
        
    }
    return vec_vecDist ;
}
//---------------------------------------------------------------------------------------------
vector<int> Indices_MinMatrix (vector<vector<double> > A )
{
    vector<int> indexRow ;
    vector<double> minRow ;
    int indexMin_Row ;
    int indexMin_Col ;
    vector<int> result ;
    for (int l = 0; l < A.size(); l++)
    {
        indexRow.push_back( static_cast<int>(min_element(A.at(l).begin(), A.at(l).end() ) -A.at(l).begin() ) ) ;
        minRow.push_back(*min_element(A.at(l).begin(), A.at(l).end())) ;
    }
    indexMin_Row = static_cast<int>(min_element(minRow.begin(), minRow.end()) - minRow.begin()) ;
    indexMin_Col = indexRow.at( indexMin_Row ) ;
    result.push_back(indexMin_Row) ;
    result.push_back(indexMin_Col) ;
    
    return result ;
}
//---------------------------------------------------------------------------------------------
vector<int> Indices_MaxMatrix (vector<vector<double> > A )
{
    vector<int> indexRow ;
    vector<double> maxRow ;
    int indexMax_Row ;
    int indexMax_Col ;
    vector<int> result ;
    for (int l = 0; l < A.size(); l++)
    {
        indexRow.push_back( static_cast<int>(max_element(A.at(l).begin(), A.at(l).end() ) -A.at(l).begin() ) ) ;
        maxRow.push_back(*max_element(A.at(l).begin(), A.at(l).end())) ;
    }
    indexMax_Row = static_cast<int>(max_element(maxRow.begin(), maxRow.end()) - maxRow.begin()) ;
    indexMax_Col = indexRow.at( indexMax_Row ) ;
    result.push_back(indexMax_Row) ;
    result.push_back(indexMax_Col) ;
    
    return result ;
}
//---------------------------------------------------------------------------------------------
vector<double> Dist_pointToVec1D (double x1, vector<double> X2 )
{
    vector<double> point_vecDist ;
    for (int j =0 ; j < X2.size(); j++)
    {
        point_vecDist.push_back( abs(X2.at(j)-x1) ) ;
    }
    return point_vecDist ;
}
//---------------------------------------------------------------------------------------------
double DotProduct (double x1 , double y1 , double x2 , double y2)
{
    return x1 * x2 + y1 * y2 ;
}
//---------------------------------------------------------------------------------------------
double MagnitudeVec (double x , double y)
{
    return sqrt(DotProduct(x, y, x, y)) ;
}
//---------------------------------------------------------------------------------------------
double TriangleArea (double x1, double y1, double x2 , double y2)
{
    double tetta1 = atan2(y1 , x1 ) ;
    double tetta2 = atan2(y2 , x2 ) ;
    double dTetta = fmod( abs( tetta2 -tetta1 ),2 * pi ) ;
    
    double area = 0.5 * MagnitudeVec(x1, y1 ) * MagnitudeVec(x2, y2 ) * abs( sin( dTetta ) ) ;
    return area ;
    
}
//---------------------------------------------------------------------------------------------
double AngleOfTwoVectors (double x1 , double y1 , double x2 , double y2)
{
    double Cos = DotProduct(x1, y1, x2, y2)/(MagnitudeVec(x1, y1) * MagnitudeVec(x2, y2)) ;
    return acos(Cos) ;
}

double sum_over_vec(const vector<vector<double> >& v, int a ) 
{
    return accumulate(v.begin(), v.end(), 0.0 ,
                      [&](double sum, vector<double> curr) { return sum + curr.at(a) ; });
};


//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------

