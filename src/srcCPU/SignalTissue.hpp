
#include "SignalCell.hpp"

enum CellType
{
    plant = 0 ,
    wingDisc = 1
};
enum Equation
{
    simpleODE = 0 ,
    fullModel = 1
};
class SignalTissue
{
public:
    
    vector<SignalCell> cells;
    CellType cellType ;
    Equation equationsType ;
    bool readFileStatus ;
    vector<double> tissueLevelU ;
    vector<vector<double> > tissueLevelConcentration ;
    double areaTissue ;
    double TissueRadius ;
    double tissueHeight ;
    double tissueWidth ;
    vector<double> tissueCenter ;
    int frameIndex ;
    bool writeVtk ;
    bool frameIsNan = false ;
    double dppProd = 0.04 ;    
    int eulerIterator = 0 ;
    int eulerMaxIterator = 300000 ;
    
    double diffusion = 0;
    double degradation = 0;
    double timeStep = 0.01 ;
    double sourceSize = 0.12 ; 
    string folderName = "./signalVtkFiles/" ;
    double klr = 0.25 ;
    double kp2 = 0.3 ;
 
    void Cal_AllCellCenters () ;
    void Cal_TissueCenter () ;
    void Cal_TissueCenter2 () ;
    void Cal_AllCellCntrToCntr () ;
    void Find_AllCellNeighborCandidates () ;
    void Find_AllCellNeighbors () ;
    void FindInterfaceWithNeighbor() ;
    vector<SignalCell> ReadFile ( ) ;
    vector<SignalCell> ReadFile2 ( ) ;
    vector<SignalCell> ReadFile3 ( ) ;
    void Coupling (vector< vector<double> > locX , vector< vector<double> > locY ) ;
    void Find_AllCell_NeighborID_Cell () ;
    void Cal_AllCellNewEdge () ;
    void Find_CommonNeighbors () ;
    void Print_CommonNeighbors () ;             //can be deleted
    void Cal_Intersect () ;
    void Find_boundaries () ;                   //incomplete
    void Count_IntersectPoints () ;             //can be deleted
    void Cal_AllCellVertices () ;      
    void ParaViewVertices () ;
    void ParaViewTissue () ;
    void ParaViewBoundary () ;
    void ParaViewInitialConfiguration () ;
    void Cal_AllCellConnections () ;
    void Test () ;
    void Add_NewVerticesToBoundaryEdges () ;
    void Refine_VerticesInBoundaryCells () ;
    void Find_Cyclic4 () ;
    void Cyclic4Correction () ;
    void SortVertices () ;
    void AllCell_RefineNoBoundary () ;
    void Refine_CurvedInterface () ;
    void Print_VeritcesSize () ;
    void Cal_TissueDimensions () ; 
 
    void Find_AllMeshes () ;
    void Cal_AreaOfTissue () ;
    void Cal_AllSelfDiffusion () ;
    void Find_IntercellularMeshConnection () ;
    void IntercellularDiffusion () ;
    void EulerMethod () ;
    void EulerMethod2 () ;
    void Find_SecretingCell () ;
    void ParaViewMesh (int number) ;
    
    void FullModel_Diffusion () ;
    void FullModel_AllCellProductions () ;      //Initialization
    void FullModelEulerMethod ( ) ;              //Main loop
    
    void Cal_AllCellConcentration () ;
    void Cal_ReturnSignal () ;
    void Initialize_Concentrations (vector<vector<double> > oldConcentrations) ;
    void WriteConcentrations (string ) ;
    void ReadConcentrations () ;
    void UpdateNanStatus () ;
    void WriteSignalingProfile () ;
    void AddNoiseToChemical () ;    
    void AssignVariables () ;

    void AllCell_AbsorbingBoundaryCondition () ; 
    void CorrectionToConcentrations () ;
    void CombineTissueCenterX (double aX0 , double aXT, double x0) ;
};

