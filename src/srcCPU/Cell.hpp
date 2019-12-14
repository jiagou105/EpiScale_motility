#include "Mesh.hpp"
using namespace std ;
#define thres_lateral 0.5 // 2.4 for plant cells and 0.5 for wing disc
#define searchAreaForNghbr 10.0 * thres_lateral
#define thres_corners 1.0 *thres_lateral    //to find a vertices for cells who does not have a comon neighbor
#define thres_cyclic4  0.5 * thres_lateral
#define thres_intersect 1.1 *thres_lateral
#define thres_noBoundary 20
#define thres_Production 20
#define thres_layer 3             //CLV3 layer 3+


class CommonNeighbor
{
public:
    int cellIDCommonNeighbor ;
    int nghbrIDCell_CommonNeighbor ;
    int nghbrIDNeighboringCell_CommonNeighbor ;
    int nghbrIDCommonNeighbor_Cell ;
    int nghbrIDCommonNeighbor_NeighboringCell ;
};




class Neighbor
{
public:
    int CellID_Neighbor ;       // stores cell ID number of the neighboring cell
    int NeighborID_Cell ;       // stores the neighbor ID number that points to our primary cell ID in the neighboring cell struct
    vector<vector<double > > nodeDistoNghbr ;
    vector<double> minDistNode ;
    vector< int > indexMinDistNode ;
    vector<int> indexCellNode ;
    vector<int> indexNeighborNode ;
    bool common_edge_ID ;
    vector <double> intfX ;
    vector <double> intfY ;
    vector <double> intersectX ;
    vector <double> intersectY ;
    vector<bool> realIntersect ;
    bool curvedInterface = false ;
    vector< CommonNeighbor > commonNeighbors ;

    
};


class Cell
{   public:
    Cell() ;
    vector<Mesh> meshes ;
    vector <Neighbor> neighbors ;
    int cellID ;
    int layer ;
    vector<double> nodesX ;
    vector<double> nodesY ;
    vector<double> nodesXNew ;
    vector<double> nodesYNew ;
    vector<double> noNeighboringNodesX ;
    vector<double> noNeighboringNodesY ;
    vector<double> verticesX ;
    vector<double> verticesY ;
    vector<double> newVertX ;
    vector<double> newVertY ;
    vector<bool> newVertUpdateStatus ;
    vector<int> connections ;
    bool boundary = false ;
    vector<int> cyclic4 ;
    
    double productionW ;
    double productionCk ;
    double productionC ;
    
    vector <double> centroid ;
    vector<double> cntrToCntr ;
    vector <int > nghbrCandidate ;
    vector<vector<vector<double> > > nodeDistoNghbrsCandidate ;
    vector<double> cellConcentration ;
    double cellU ;
    
    void Cal_Centroid() ;
    vector <vector <double> > Cal_NodeToNodeDist (vector<double> , vector<double>) ;
    void Find_NghbrCandidate () ;
    void Find_NghbrProperties () ;
    void DetectCommonEdge () ;
    void NewEdge () ;
    void SortCCW (); //vector<double> &vectX , vector<double> &vectY) ;
    void Cal_Vertices () ;
    void Cal_Connections () ;
    void Add_BoundaryVertice () ;
    void Add_BoundaryVertice2 () ;
    void Add_BoundaryVertice3 () ;
    
    
    void Find_Mesh () ;
    void Self_Diffusion () ;
    void Refine_NoBoundary () ;
    void Refine_NodeXNew () ;
    void FullModel_SelfDiffusion () ;
    void FullModel_ProductionCell () ;
    void CellLevelConcentration () ;
    
    
    
};





