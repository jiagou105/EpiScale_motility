
#include "SignalTissue.hpp"
#include "Signal_Calculator.h"
#include <chrono>
#include "ConfigParser.h"
//---------------------------------------------------------------------------------------------
/*
vector<vector<double> > Signal_Calculator(vector< vector<double> > locX , vector< vector<double> > locY , vector<double > centX , vector<double > centY ,vector< vector<double> > oldConcentrations ,double index)
{
    
    vector<vector<double > > a ;
    return a ;
}

int main ()
 {
    //for(int tmpIndex=1 ; tmpIndex<7 ; tmpIndex ++)
     //{
     vector< vector<double> > locX ;
     vector< vector<double> > locY ;
     vector<double > centX ;
     vector<double > centY ;
     //int index = 40 * tmpIndex ;
     int index = 160 ;
     vector<vector<double> > oldConcentrations ;
 */

void SignalGlobalVar:: Signal_get_config(){

extern GlobalConfigVars globalConfigVars;
diffusion = globalConfigVars.getConfigValue("SignalDiffusion").toDouble();
degradation = globalConfigVars.getConfigValue("SignalDegradation").toDouble();
dppProduction = globalConfigVars.getConfigValue("SignalProduction").toDouble();
timeStep = globalConfigVars.getConfigValue("SignalTimeStep").toDouble();
sourceSize = globalConfigVars.getConfigValue("SignalSourceSize").toDouble();
folderName=globalConfigVars.getConfigValue("SignalFolderName").toString();
klr = globalConfigVars.getConfigValue("SignalKlr").toDouble();
kp2 = globalConfigVars.getConfigValue("SignalKp2").toDouble();
} 

vector< vector<double> > Signal_Calculator ( vector< vector<double> > locX , vector< vector<double> > locY , vector<double > centX , vector<double > centY,vector< vector<double> > oldConcentrations , double index, double &tCentX0 ){
    SignalGlobalVar signalGlobVar ;
    signalGlobVar.Signal_get_config() ;  
/*    extern GlobalConfigVars globalConfigVars;
    double diffusion = globalConfigVars.getConfigValue("SignalDiffusion").toDouble();
    double degradation = globalConfigVars.getConfigValue("SignalDegradation").toDouble();
    double timeStep = globalConfigVars.getConfigValue("SignalTimeStep").toDouble();
    double sourceSize = globalConfigVars.getConfigValue("SignalSourceSize").toDouble();
    Signal_get_config() ;   
*/  
  ofstream nanIndex ("NanIndex.txt", ofstream::app) ;    //everything will be written at the end of the existing file
    ofstream sgnlCalculator ("sgnlCalculator.txt", ofstream::app) ;    //everything will be written at the end of the existing file
    auto start = std::chrono::high_resolution_clock::now() ;
    SignalTissue tissue ;
    tissue.cellType = wingDisc ;
    tissue.equationsType = fullModel ;
    tissue.readFileStatus = false ;
    tissue.frameIndex = static_cast<int>(round ( 100 * index) ) / 100 ;
    //tissue.writeVtk = ! fmod(static_cast<int>(round ( 100 * index) ), 100) ;
    tissue.writeVtk = false ;
    tissue.diffusion = signalGlobVar.diffusion ;
tissue.degradation = signalGlobVar.degradation ;
tissue.timeStep = signalGlobVar.timeStep ;
tissue.sourceSize = signalGlobVar.sourceSize ;
tissue.dppProd = signalGlobVar.dppProduction ;
tissue.folderName = signalGlobVar.folderName ;
tissue.klr = signalGlobVar.klr ;
tissue.kp2 = signalGlobVar.kp2 ;
    cout<<"current index in Signal_Calculator function is "<<index<<endl ;
    sgnlCalculator<<"current index in Signal_Calculator function is "<<index<<endl ;
    if (tissue.readFileStatus)
    {
        if (tissue.cellType == plant)
        {
         //   tissue.cells = tissue.ReadFile( ) ;     //for old files
            tissue.cells = tissue.ReadFile2( ) ;       // for new files
        }
        else if (tissue.cellType == wingDisc)
        {
            //testing wingDisc model for plant
           // tissue.cells = tissue.ReadFile2( ) ;
            tissue.cells = tissue.ReadFile3( ) ;       // for Wing Disc files
        }
        tissue.Cal_AllCellCenters () ;
    }
    else
    {
        tissue.Coupling(locX , locY ) ;
        tissue.Cal_AllCellCenters () ;
    }
     cout<<"number of the cells is "<<tissue.cells.size()<<endl ;
     sgnlCalculator<<"number of the cells is "<<tissue.cells.size()<<endl ;
     
     
  // // // // //  tissue.ParaViewInitialConfiguration() ;       // Bug when I run this early in the code!!!! intx.size()= 0 !!!!!!
    
   // tissue.Cal_TissueCenter() ;
    tissue.Cal_AllCellCntrToCntr();
    tissue.Find_AllCellNeighborCandidates() ;
    tissue.Find_AllCellNeighbors () ;
    tissue.FindInterfaceWithNeighbor() ;
    tissue.Cal_AllCellNewEdge() ;
    tissue.Find_CommonNeighbors() ;
    tissue.Cal_Intersect() ;
    tissue.Cal_AllCellVertices() ;
    tissue.Find_boundaries() ;
    tissue.Refine_VerticesInBoundaryCells() ;
    tissue.AllCell_RefineNoBoundary() ;
  //  tissue.ParaViewBoundary() ;
    tissue.Add_NewVerticesToBoundaryEdges() ;
  //  tissue.Refine_CurvedInterface() ;
    tissue.Find_Cyclic4() ;
    tissue.Cyclic4Correction() ;
    tissue.SortVertices() ;
    tissue.Cal_AllCellConnections() ;
 // tissue.Print_VeritcesSize() ;
     
    tissue.ParaViewVertices() ;
    tissue.ParaViewTissue () ;
    tissue.ParaViewInitialConfiguration() ;
     
    //Genetating meshes
    tissue.Find_AllMeshes () ;
    tissue.Find_IntercellularMeshConnection () ;
    tissue.Cal_AreaOfTissue() ;
    tissue.AssignVariables() ; 
    tissue.Cal_TissueCenter2() ;
    if ( abs(index - 1.0 ) < 0.001 ) 
    {
    	tCentX0 = tissue.tissueCenter.at(0) ;
    }
    tissue.CombineTissueCenterX (1.0 , 0.0 , tCentX0) ;
    tissue.Cal_TissueDimensions() ;

    // Equations part
    if (tissue.equationsType == simpleODE)
    {
        tissue.Find_SecretingCell() ;
        tissue.EulerMethod () ;
    }
    else
    {
        if (tissue.readFileStatus)
        {
           // tissue.ReadConcentrations() ;
           // tissue.WriteConcentrations("old") ;
        }
        else if (oldConcentrations.size()== tissue.cells.size() )
        {
            
            tissue.Initialize_Concentrations( oldConcentrations ) ;
            tissue.tissueLevelConcentration = oldConcentrations ;
            tissue.WriteConcentrations("old") ;
        }
        else
        {
            nanIndex <<"frame index is "<<tissue.frameIndex<<'\t' <<"tissue size is "<<tissue.cells.size()<<'t'
                    <<"oldConcentration size is "<<oldConcentrations.size()<<endl ;
        }
        
        tissue.FullModel_AllCellProductions() ;     //Initialize production values
        tissue.FullModelEulerMethod () ;
    }
    
     tissue.Cal_AllCellConcentration() ;     // output depends on the cellType and equationType
     tissue.CorrectionToConcentrations() ;
     //tissue.AddNoiseToChemical() ;     
     
   //  tissue.Cal_ReturnSignal() ;             // returning Dpp level as U
     
     tissue.WriteConcentrations("new") ;
     tissue.UpdateNanStatus() ;
     if (tissue.frameIsNan)
     {
         nanIndex <<"This is a Nan frame :" <<tissue.frameIndex<<endl ;
     }
     nanIndex.close() ;
     
     auto stop = std::chrono::high_resolution_clock::now();
     auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
     cout << "Time taken by Singnal_Calculator is : " << duration.count() << " seconds" << endl;
     sgnlCalculator << "Time taken by Singnal_Calculator is : " << duration.count() << " seconds" << endl;
     sgnlCalculator.close() ;
     tissue.WriteSignalingProfile() ;
     
     // return tissue.tissueLevelU ;
   return tissue.tissueLevelConcentration ;
   //  return 0 ;
}

// Adjust index and other global variables
// be carefull about dt
//free diffusion in the cell

