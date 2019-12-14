
#include "Tissue.hpp"
 #include "Signal_Calculator.h"

//---------------------------------------------------------------------------------------------

vector<double> Signal_Calculator ( vector< vector<double> > locX , vector< vector<double> > locY , vector<double > centX , vector<double > centY )
{
 
    Tissue tissue ;
    tissue.cellType = wingDisc ;
    tissue.equationsType = simpeODE ;
    tissue.readFileStatus = true ;
    if (tissue.readFileStatus)
    {
        if (tissue.cellType == plant)
        {
          //  tissue.cells = tissue.ReadFile( ) ;     //for old files
            tissue.cells = tissue.ReadFile2( ) ;       // for new files
        }
        else if (tissue.cellType == wingDisc)
        {
            tissue.cells = tissue.ReadFile3( ) ;       // for Wing Disc files
        }
        tissue.Cal_AllCellCenters () ;
    }
    else
    {
        tissue.Coupling(locX , locY , centX , centY ) ;
    }
    
    
 //   tissue.ParaViewInitialConfiguration() ;       // Bug when I run this early in the code!!!! intx.size()= 0 !!!!!!
    
    tissue.Cal_AllCellCntrToCntr();
    tissue.Find_AllCellNeighborCandidates() ;
    tissue.Find_AllCellNeighbors () ;
    tissue.FindInterfaceWithNeighbor() ;
    tissue.Cal_AllCellNewEdge() ;
    tissue.Find_CommonNeighbors() ;
    tissue.Cal_Intersect() ;
    tissue.Cal_AllCellVertices() ;
    tissue.AllCell_RefineNoBoundary() ;
    tissue.Find_boundaries() ;
    tissue.Refine_VerticesInBoundaryCells() ;
    tissue.ParaViewBoundary() ;
    tissue.Add_NewVerticesToBoundaryEdges() ;
    tissue.Refine_CurvedInterface() ;
    tissue.Find_Cyclic4() ;
    tissue.Cyclic4Correction() ;
    tissue.SortVertices() ;
    tissue.Cal_AllCellConnections() ;
 // tissue.Print_VeritcesSize() ;
    tissue.ParaViewVertices() ;
    tissue.ParaViewTissue () ;
    tissue.ParaViewInitialConfiguration() ;
    
    tissue.Find_AllMeshes () ;
    tissue.Find_IntercellularMeshConnection () ;
 //   tissue.ParaViewMesh(0) ;
    if (tissue.equationsType == simpeODE)
    {
        tissue.Find_SecretingCell() ;
        tissue.EulerMethod () ;
    }
    else
    {
        tissue.FullModel_AllCellProductions() ;     //Initialize production values
        tissue.FullModelEulerMethod () ;
    }
    
    tissue.Cal_AllCellConcentration() ;
    
   return tissue.tissueLevelU ;
}


// Adjust index and other global variables
// be carefull about dt

