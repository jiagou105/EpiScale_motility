
#include "Tissue.hpp"

void Tissue::Cal_AllCellCenters()
{
    for (int i = 0 ; i< cells.size(); i++)
    {
        cells.at(i).Cal_Centroid() ;
    }
}
//---------------------------------------------------------------------------------------------
void Tissue::Find_AllCellNeighborCandidates ()
{
    for (int i=0; i < cells.size(); i++)
    {
        cells.at(i).Find_NghbrCandidate() ;
    }
}
//---------------------------------------------------------------------------------------------

void Tissue::Cal_AllCellCntrToCntr()
{
    for (int i=0 ; i< cells.size() ; i++ )
    {
        for (int j=0 ; j< cells.size(); j++)
        {
            cells.at(i).cntrToCntr.push_back(Dist2D(cells.at(i).centroid.at(0),cells.at(i).centroid.at(1),
                                                    cells.at(j).centroid.at(0),cells.at(j).centroid.at(1))) ;
            
        }
    }
}

//---------------------------------------------------------------------------------------------
void Tissue::FindInterfaceWithNeighbor()
{
    for (int i=0; i < cells.size(); i++)
    {
        for (int j= 0; j < cells.at(i).neighbors.size(); j++)
        {
            int cellIDNeighbor = cells.at(i).neighbors.at(j).CellID_Neighbor ;
            int neighborIDCell = cells.at(i).neighbors.at(j).NeighborID_Cell ;
            
            if (cells.at(i).neighbors.at(j).common_edge_ID == false )
            {
        
                for (int k =0; k < cells.at(i).neighbors.at(j).indexCellNode.size() ; k++)
                {
                    int nodeIDCell = cells.at(i).neighbors.at(j).indexCellNode.at(k) ;
                    int nodeIDNeighbor = cells.at(i).neighbors.at(j).indexMinDistNode.at(nodeIDCell) ;
                    double newX = 0.5*(cells.at(i).nodesX.at(nodeIDCell) + cells.at(cellIDNeighbor).nodesX.at(nodeIDNeighbor) ) ;
                    double newY = 0.5*(cells.at(i).nodesY.at(nodeIDCell) + cells.at(cellIDNeighbor).nodesY.at(nodeIDNeighbor) ) ;
                
                    cells.at(i).nodesXNew.at(nodeIDCell)= newX ;
                    cells.at(i).nodesYNew.at(nodeIDCell)= newY ;
                    cells.at(cellIDNeighbor).nodesXNew.at(nodeIDNeighbor) = newX ;
                    cells.at(cellIDNeighbor).nodesYNew.at(nodeIDNeighbor) = newY ;
        // we should work with either nodesXNew or intfX
                    
                    cells.at(i).neighbors.at(j).intfX.push_back(newX) ;
                    cells.at(i).neighbors.at(j).intfY.push_back(newY) ;
                    cells.at(cellIDNeighbor).neighbors.at(neighborIDCell).intfX.push_back(newX) ;
                    cells.at(cellIDNeighbor).neighbors.at(neighborIDCell).intfY.push_back(newY) ;
                
                  
                }
                cells.at(i).neighbors.at(j).common_edge_ID = true ;
                cells.at(cellIDNeighbor).neighbors.at(neighborIDCell).common_edge_ID = true ;
                
            }
        }
    }
    
}

//---------------------------------------------------------------------------------------------

vector<Cell> Tissue::ReadFile ( )
{
    double a,b,c ;
    double d ;
    vector<Cell> tmpCell ;
    string number = to_string(index) ;
    ifstream nodeData ("Locations_"+ number +".txt") ;
    if (nodeData.is_open())
    {
        cout << "file is open"<<endl ;
    }
    else{
        cout << "Error in file"<<endl ;
    }
    
    while (nodeData >> a >> b >> c >> d)
    {
        if (a==b)
        {
            Cell cell ;
            tmpCell.push_back(cell) ;
            tmpCell.back().cellID = a ;
            continue ;
        }
        
        if ( d == 1 )
        {
            
            tmpCell.back().nodesX.push_back(a) ;
            tmpCell.back().nodesY.push_back(b) ;
            tmpCell.back().nodesXNew.push_back(a) ;
            tmpCell.back().nodesYNew.push_back(b) ;
            tmpCell.back().noNeighboringNodesX.push_back(a) ;
            tmpCell.back().noNeighboringNodesY.push_back(b) ;
            
            
            //    cells.back().nodes.back().z = c ;
            //    cells.back().nodes.back().nodeType = d ;
        }
    }
    return tmpCell ;
}
//---------------------------------------------------------------------------------------------
vector<Cell> Tissue::ReadFile2 ( )
{
    int cellLayer = 0 ;
    int a,b ;
    double c,d ;
    vector<Cell> tmpCell ;
    string number = to_string(index) ;
    ifstream nodeData ("Locations_"+ number +".txt") ;
    if (nodeData.is_open())
    {
        cout << "file is open"<<endl ;
    }
    else{
        cout << "Error in file"<<endl ;
    }
    
    while (nodeData >> a >> b >> c >> d)
    {
        if (static_cast<int>( tmpCell.size() ) == b)
        {
            cellLayer = a ;
            Cell cell ;
            tmpCell.push_back(cell) ;
        }
        
        tmpCell.back().nodesX.push_back(c) ;
        tmpCell.back().nodesY.push_back(d) ;
        tmpCell.back().nodesXNew.push_back(c) ;
        tmpCell.back().nodesYNew.push_back(d) ;
        tmpCell.back().noNeighboringNodesX.push_back(c) ;
        tmpCell.back().noNeighboringNodesY.push_back(d) ;
        tmpCell.back().layer = cellLayer ;
            
            //    cells.back().nodes.back().z = c ;
            //    cells.back().nodes.back().nodeType = d ;
    }
    return tmpCell ;
}
//---------------------------------------------------------------------------------------------
vector<Cell> Tissue::ReadFile3 ( )
{
    int b ;
    double c,d ;
    vector<Cell> tmpCell ;
    string number = to_string(index) ;
    ifstream nodeData ("ExportCellProp_"+ number +".txt") ;
    if (nodeData.is_open())
    {
        cout << "file is open"<<endl ;
    }
    else{
        cout << "Error in file"<<endl ;
    }
    
    while ( nodeData  >> b >> c >> d )
    {
        if (static_cast<int>( tmpCell.end() - tmpCell.begin() ) == b )
        {
            Cell cell ;
            tmpCell.push_back(cell) ;
        }
        
        tmpCell.back().nodesX.push_back(c) ;
        tmpCell.back().nodesY.push_back(d) ;
        tmpCell.back().nodesXNew.push_back(c) ;
        tmpCell.back().nodesYNew.push_back(d) ;
        tmpCell.back().noNeighboringNodesX.push_back(c) ;
        tmpCell.back().noNeighboringNodesY.push_back(d) ;
        
        //    cells.back().nodes.back().z = c ;
        //    cells.back().nodes.back().nodeType = d ;
    }
    return tmpCell ;
}
//---------------------------------------------------------------------------------------------

void Tissue::Coupling (vector< vector<double> > locX , vector< vector<double> > locY , vector<double > centX , vector<double > centY )
{
    int cellSize = static_cast<int>( locX.size() ) ;
    for (int i =0 ; i< cellSize ; i++)
    {
        Cell tmpCell ;
        tmpCell.nodesX = locX.at(i) ;
        tmpCell.nodesXNew = locX.at(i) ;
        tmpCell.noNeighboringNodesX = locX.at(i) ;
        
        tmpCell.nodesY = locY.at(i) ;
        tmpCell.nodesYNew = locY.at(i) ;
        tmpCell.noNeighboringNodesY = locY.at(i) ;
        
        tmpCell.centroid.at(0) = centX.at(i) ;
        tmpCell.centroid.at(1) = centY.at(i) ;
        
        cells.push_back( tmpCell ) ;
    }
 
}

//---------------------------------------------------------------------------------------------

void Tissue::Find_AllCellNeighbors()
{
    for (int i=0; i < cells.size(); i++)
    {
        for (int j= 0; j < cells.at(i).nghbrCandidate.size(); j++)
        {
            int idNeighbor=cells.at(i).nghbrCandidate.at(j) ;
            
            cells.at(i).nodeDistoNghbrsCandidate.push_back (cells.at(i).Cal_NodeToNodeDist(cells.at(idNeighbor).nodesX ,
                                                                                           cells.at(idNeighbor).nodesY ) );
            
        }
        cells.at(i).Find_NghbrProperties() ;
    }
    Find_AllCell_NeighborID_Cell() ;
}

//---------------------------------------------------------------------------------------------
void Tissue::Find_AllCell_NeighborID_Cell()
{
    for (int i=0 ; i< cells.size(); i++)
    {
        for (int j=0; j< cells.at(i).neighbors.size(); j++)
        {
            int neighboringCell = cells.at(i).neighbors.at(j).CellID_Neighbor ;
            // search on all the neighbors of the neighboring cell
            for (int k=0; k < cells.at(neighboringCell).neighbors.size(); k++)
            {
              if (  cells.at(neighboringCell).neighbors.at(k).CellID_Neighbor == i )
              {
                  cells.at(i).neighbors.at(j).NeighborID_Cell = k ;
                  break ;
              }
            }
        }
    }
}
//---------------------------------------------------------------------------------------------

void Tissue::Cal_AllCellNewEdge()
{
    for (int i=0 ; i < cells.size(); i++)
    {
        cells.at(i).NewEdge() ;
    }
    
}
//---------------------------------------------------------------------------------------------

void Tissue::Find_CommonNeighbors()
{
    for (int i = 0; i < cells.size(); i++)
    {
        for (int j = 0; j < cells.at(i).neighbors.size(); j++)
        {
            int cellIDNeighbor = cells.at(i).neighbors.at(j).CellID_Neighbor ;
            int neighborIDCell = cells.at(i).neighbors.at(j).NeighborID_Cell ;
            
            if (i < cellIDNeighbor)
            {
                for (int k=j+1; k < cells.at(i).neighbors.size() ; k++)
                {
                    for (int l= 0; l < cells.at(cellIDNeighbor).neighbors.size() ; l++)
                    {
                        if ( cells.at(i).neighbors.at(k).CellID_Neighbor == cells.at(cellIDNeighbor).neighbors.at(l).CellID_Neighbor )
                        {
                            int cellID_Other = cells.at(i).neighbors.at(k).CellID_Neighbor ;
                            int nghbrIDCell_Other = cells.at(i).neighbors.at(k).NeighborID_Cell ;
                            int nghbrIDNeighboringCell_Other = cells.at(cellIDNeighbor).neighbors.at(l).NeighborID_Cell ;
                            
                            CommonNeighbor tmpAB ;
                            tmpAB.cellIDCommonNeighbor = cellID_Other ;
                            tmpAB.nghbrIDCell_CommonNeighbor = nghbrIDCell_Other ;
                            tmpAB.nghbrIDNeighboringCell_CommonNeighbor = nghbrIDNeighboringCell_Other ;
                            tmpAB.nghbrIDCommonNeighbor_Cell = k ;
                            tmpAB.nghbrIDCommonNeighbor_NeighboringCell = l ;
                            cells.at(i).neighbors.at(j).commonNeighbors.push_back( tmpAB ) ;
                            
                            CommonNeighbor tmpAC ;
                            tmpAC.cellIDCommonNeighbor = cellIDNeighbor ;
                            tmpAC.nghbrIDCell_CommonNeighbor = neighborIDCell ;
                            tmpAC.nghbrIDNeighboringCell_CommonNeighbor = l ;
                            tmpAC.nghbrIDCommonNeighbor_Cell = j ;
                            tmpAC.nghbrIDCommonNeighbor_NeighboringCell = nghbrIDNeighboringCell_Other ;
                            cells.at(i).neighbors.at(k).commonNeighbors.push_back( tmpAC ) ;
                            
                            CommonNeighbor tmpBA ;
                            tmpBA.cellIDCommonNeighbor = cellID_Other ;
                            tmpBA.nghbrIDCell_CommonNeighbor = nghbrIDNeighboringCell_Other ;
                            tmpBA.nghbrIDNeighboringCell_CommonNeighbor = nghbrIDCell_Other ;
                            tmpBA.nghbrIDCommonNeighbor_Cell = l ;
                            tmpBA.nghbrIDCommonNeighbor_NeighboringCell = k ;
                            cells.at(cellIDNeighbor).neighbors.at(neighborIDCell).commonNeighbors.push_back(tmpBA ) ;
                            
                            CommonNeighbor tmpBC ;
                            tmpBC.cellIDCommonNeighbor = i ;
                            tmpBC.nghbrIDCell_CommonNeighbor = j ;
                            tmpBC.nghbrIDNeighboringCell_CommonNeighbor = k ;
                            tmpBC.nghbrIDCommonNeighbor_Cell = neighborIDCell ;
                            tmpBC.nghbrIDCommonNeighbor_NeighboringCell = nghbrIDCell_Other ;
                            cells.at(cellIDNeighbor).neighbors.at(l).commonNeighbors.push_back(tmpBC) ;
                            
                            CommonNeighbor tmpCA ;
                            tmpCA.cellIDCommonNeighbor = cellIDNeighbor ;
                            tmpCA.nghbrIDCell_CommonNeighbor = l ;
                            tmpCA.nghbrIDNeighboringCell_CommonNeighbor = k ;
                            tmpCA.nghbrIDCommonNeighbor_NeighboringCell = j ;
                            tmpCA.nghbrIDCommonNeighbor_Cell = nghbrIDNeighboringCell_Other ;
                            
                            cells.at(cellID_Other).neighbors.at(nghbrIDCell_Other).commonNeighbors.push_back( tmpCA)  ;
                            
                            CommonNeighbor tmpCB ;
                            tmpCB.cellIDCommonNeighbor = i ;
                            tmpCB.nghbrIDCell_CommonNeighbor = k ;
                            tmpCB.nghbrIDNeighboringCell_CommonNeighbor = j ;
                            tmpCB.nghbrIDCommonNeighbor_Cell = nghbrIDCell_Other ;
                            tmpCB.nghbrIDCommonNeighbor_NeighboringCell = neighborIDCell ;
                            cells.at(cellID_Other).neighbors.at( nghbrIDNeighboringCell_Other).commonNeighbors.push_back( tmpCB ) ;
                            
                            
                        }
                    }
                }
            }
        }
    }

}
//---------------------------------------------------------------------------------------------
void Tissue::Print_CommonNeighbors()
{
    for (int i=0; i < cells.size(); i++)
    {
        for (int j=0; j< cells.at(i).neighbors.size(); j++)
        {
            cout<<i<<'\t'<<cells.at(i).neighbors.at(j).CellID_Neighbor<<" : "<<'\t' ;
            for (int k=0; k < cells.at(i).neighbors.at(j).commonNeighbors.size() ; k++)
            {
                cout<< cells.at(i).neighbors.at(j).commonNeighbors.at(k).cellIDCommonNeighbor <<'\t' ;
            }
            cout<<endl ;
        }
    }
}
//---------------------------------------------------------------------------------------------

void Tissue::Cal_Intersect()
{
    for (int i=0; i < cells.size(); i++)
    {
        for (int j=0; j< cells.at(i).neighbors.size(); j++)
        {
            int cellID_B = cells.at(i).neighbors.at(j).CellID_Neighbor ;
            int nghbrIDA_B = cells.at(i).neighbors.at(j).NeighborID_Cell ;
            if (i < cellID_B)
            {
                for (int k=0; k < cells.at(i).neighbors.at(j).commonNeighbors.size() ; k++)
                {
                    int cellID_C = cells.at(i).neighbors.at(j).commonNeighbors.at(k).cellIDCommonNeighbor ;
                    if (cellID_B < cellID_C )
                    {
                        if (i==7 && cellID_B == 15)
                        {
                      //      cout<< i << '\t'<<cellID_B<<'\t'<<cellID_C<<endl ;
                        }
                        int nghbrIDA_C = cells.at(i).neighbors.at(j).commonNeighbors.at(k).nghbrIDCell_CommonNeighbor ;
                        int nghbrIDC_A = cells.at(i).neighbors.at(j).commonNeighbors.at(k).nghbrIDCommonNeighbor_Cell ;
                        int nghbrIDB_C = cells.at(i).neighbors.at(j).commonNeighbors.at(k).nghbrIDNeighboringCell_CommonNeighbor ;
                        int nghbrIDC_B = cells.at(i).neighbors.at(j).commonNeighbors.at(k).nghbrIDCommonNeighbor_NeighboringCell ;
                        vector<int> indices ;
                        
                        vector<double> intfX_AB = cells.at(i).neighbors.at(j).intfX ;
                        vector<double> intfY_AB = cells.at(i).neighbors.at(j).intfY ;
                        
                        vector<double> intfX_AC = cells.at(i).neighbors.at(nghbrIDC_A).intfX ;
                        vector<double> intfY_AC = cells.at(i).neighbors.at(nghbrIDC_A).intfY ;
                        
                        vector<double> intfX_BC = cells.at(cellID_C).neighbors.at(nghbrIDB_C).intfX ;
                        vector<double> intfY_BC = cells.at(cellID_C).neighbors.at(nghbrIDB_C).intfY ;
                        
                        vector<vector<double> > AB_AC = Dist_VecToVec2D(intfX_AB, intfY_AB, intfX_AC, intfY_AC) ;
                        indices = Indices_MinMatrix(AB_AC) ;
                        
                        vector<vector<double> > tmp ;
                        tmp.push_back( Dist_PointToVec2D(intfX_AB.at(indices.at(0)), intfY_AB.at(indices.at(0)), intfX_BC, intfY_BC) );
                        indices.push_back(Indices_MinMatrix(tmp).at(1)) ;
                        
                        
                        double intrsctX = ( intfX_AB.at(indices.at(0))+ intfX_AC.at(indices.at(1))+ intfX_BC.at(indices.at(2)))/3.0 ;
                        double intrsctY = ( intfY_AB.at(indices.at(0))+ intfY_AC.at(indices.at(1))+ intfY_BC.at(indices.at(2)))/3.0 ;
                        
                        vector<double> tmpx {intfX_AB.at(indices.at(0)),intfX_AC.at(indices.at(1)),intfX_BC.at(indices.at(2))} ;
                        vector<double> tmpy {intfY_AB.at(indices.at(0)),intfY_AC.at(indices.at(1)),intfY_BC.at(indices.at(2))} ;
                        
                        vector<vector<double> > tmpDistVecToVec = Dist_VecToVec2D(tmpx, tmpy, tmpx, tmpy) ;
                        vector<double> tmpDist = *max_element(tmpDistVecToVec.begin(), tmpDistVecToVec.end()) ;
                        double maxDist = *max_element(tmpDist.begin(), tmpDist.end()) ;
                        
                        if ( maxDist < thres_intersect)
                        {
                            cells.at(i).neighbors.at(j).intersectX.push_back(intrsctX) ;
                            cells.at(i).neighbors.at(j).intersectY.push_back(intrsctY) ;
                            
                            cells.at(i).neighbors.at(nghbrIDC_A).intersectX.push_back(intrsctX) ;
                            cells.at(i).neighbors.at(nghbrIDC_A).intersectY.push_back(intrsctY) ;
                            
                            cells.at(cellID_B).neighbors.at(nghbrIDA_B).intersectX.push_back(intrsctX) ;
                            cells.at(cellID_B).neighbors.at(nghbrIDA_B).intersectY.push_back(intrsctY) ;
                            
                            cells.at(cellID_B).neighbors.at(nghbrIDC_B).intersectX.push_back(intrsctX) ;
                            cells.at(cellID_B).neighbors.at(nghbrIDC_B).intersectY.push_back(intrsctY) ;
                            
                            cells.at(cellID_C).neighbors.at(nghbrIDA_C).intersectX.push_back(intrsctX) ;
                            cells.at(cellID_C).neighbors.at(nghbrIDA_C).intersectY.push_back(intrsctY) ;
                            
                            cells.at(cellID_C).neighbors.at(nghbrIDB_C).intersectX.push_back(intrsctX) ;
                            cells.at(cellID_C).neighbors.at(nghbrIDB_C).intersectY.push_back(intrsctY) ;
                            
                            cells.at(i).neighbors.at(j).realIntersect.push_back( true ) ;
                            cells.at(i).neighbors.at(nghbrIDC_A).realIntersect.push_back(true) ;
                            cells.at(cellID_B).neighbors.at(nghbrIDA_B).realIntersect.push_back(true) ;
                            cells.at(cellID_B).neighbors.at(nghbrIDC_B).realIntersect.push_back(true) ;
                            cells.at(cellID_C).neighbors.at(nghbrIDA_C).realIntersect.push_back(true) ;
                            cells.at(cellID_C).neighbors.at(nghbrIDB_C).realIntersect.push_back(true) ;
                            
                        }
                        else
                        {
                            cells.at(i).neighbors.at(j).realIntersect.push_back( false) ;
                            cells.at(i).neighbors.at(nghbrIDC_A).realIntersect.push_back(false) ;
                            cells.at(cellID_B).neighbors.at(nghbrIDA_B).realIntersect.push_back(false) ;
                            cells.at(cellID_B).neighbors.at(nghbrIDC_B).realIntersect.push_back(false) ;
                            cells.at(cellID_C).neighbors.at(nghbrIDA_C).realIntersect.push_back(false) ;
                            cells.at(cellID_C).neighbors.at(nghbrIDB_C).realIntersect.push_back(false) ;
                            
                        }
                    }
                }
            }
        }
    }
}
//---------------------------------------------------------------------------------------------
void Tissue::Find_boundaries()
{
    vector<double> allNodesX ;
    vector<double> allNodesY ;
    for (int i = 0; i< cells.size(); i++)
    {
        for (int j =0; j < cells.at(i).neighbors.size(); j++)
        {
            int cellID_nghbr = cells.at(i).neighbors.at(j).CellID_Neighbor ;
            if (cells.at(i).neighbors.at(j).intersectX.size() < 2 || cells.at(i).noNeighboringNodesX.size() > 6 )
            {
                cells.at(i).boundary = true ;
                cells.at(cellID_nghbr).boundary = true ;
                if (cells.at(i).neighbors.at(j).intersectX.size() == 0 )
                {
                  vector<vector<double> > distTmp =  Dist_VecToVec2D(cells.at(i).neighbors.at(j).intfX,cells.at(i).neighbors.at(j).intfY,
                                                                    cells.at(i).neighbors.at(j).intfX, cells.at(i).neighbors.at(j).intfY ) ;
                    
                   vector<int> indecies =  Indices_MaxMatrix(distTmp) ;
                    
            //        cells.at(i).verticesX.push_back(cells.at(i).neighbors.at(j).intfX.at(indecies.at(0))) ;
            //        cells.at(i).verticesY.push_back(cells.at(i).neighbors.at(j).intfY.at(indecies.at(0))) ;
                    
            //        cells.at(i).verticesX.push_back(cells.at(i).neighbors.at(j).intfX.at(indecies.at(1))) ;
            //        cells.at(i).verticesY.push_back(cells.at(i).neighbors.at(j).intfY.at(indecies.at(1))) ;
                    
                    cells.at(i).newVertX.push_back(cells.at(i).neighbors.at(j).intfX.at(indecies.at(0))) ;
                    cells.at(i).newVertY.push_back(cells.at(i).neighbors.at(j).intfY.at(indecies.at(0))) ;
                    cells.at(i).newVertUpdateStatus.push_back(false) ;
                    
                    cells.at(i).newVertX.push_back(cells.at(i).neighbors.at(j).intfX.at(indecies.at(1))) ;
                    cells.at(i).newVertY.push_back(cells.at(i).neighbors.at(j).intfY.at(indecies.at(1))) ;
                    cells.at(i).newVertUpdateStatus.push_back(false) ;
                    
                }
                else if (cells.at(i).neighbors.at(j).intersectX.size() == 1)
                {
                    vector<double> distTmp = Dist_PointToVec2D(cells.at(i).neighbors.at(j).intersectX.at(0),
                                                               cells.at(i).neighbors.at(j).intersectY.at(0),
                                                               cells.at(i).neighbors.at(j).intfX,
                                                               cells.at(i).neighbors.at(j).intfY ) ;
                    
                    int indextmp = max_element(distTmp.begin(), distTmp.end() ) - distTmp.begin() ;
                    
            //        cells.at(i).verticesX.push_back( cells.at(i).neighbors.at(j).intfX.at(indextmp)) ;
            //        cells.at(i).verticesY.push_back( cells.at(i).neighbors.at(j).intfY.at(indextmp)) ;
                    
                    cells.at(i).newVertX.push_back( cells.at(i).neighbors.at(j).intfX.at(indextmp)) ;
                    cells.at(i).newVertY.push_back(cells.at(i).neighbors.at(j).intfY.at(indextmp)) ;
                    cells.at(i).newVertUpdateStatus.push_back(false) ;
                }
            }
        }
        
    }
}
//---------------------------------------------------------------------------------------------
void Tissue::Count_IntersectPoints()
{
    for (int i = 0; i< cells.size(); i++)
    {
        for (int j = 0; j < cells.at(i).neighbors.size(); j++)
        {
           cout<< cells.at(i).neighbors.at(j).intersectX.size()<< endl ;
        }
    }
    
}
//---------------------------------------------------------------------------------------------
void Tissue::Cal_AllCellVertices()
{
    for (int i =0; i < cells.size(); i++)
    {
        cells.at(i).Cal_Vertices() ;
    }
}
//---------------------------------------------------------------------------------------------
void Tissue::ParaViewVertices ()
{
    
    vector<double> allNodesX ;
    vector<double> allNodesY ;
    vector<int> links ;         // link to the other vertices
    vector<int > links2 ;       // link to the centroid of the cell
    int verticesSize ;
    for (int i=0 ; i< cells.size() ; i++)
    {
        for ( int j=0 ; j< cells.at(i).verticesX.size() ; j++)
        {
            allNodesX.push_back( cells.at(i).verticesX.at(j))   ;
            allNodesY.push_back( cells.at(i).verticesY.at(j))   ;
            links.push_back(cells.at(i).connections.at(j) ) ;
        }
    }
    verticesSize = allNodesX.size() ;       //centroids start from here in the vector
    for (int i=0 ; i< cells.size() ; i++)
    {
        allNodesX.push_back(cells.at(i).centroid.at(0) )   ;
        allNodesY.push_back(cells.at(i).centroid.at(1) )   ;
        for ( int j=0 ; j< cells.at(i).verticesX.size() ; j++)
        {
            // (verticesSize + i) is equal to index of centroid of the i_th cell
            links2.push_back(verticesSize + i ) ;
        }
    }
    string vtkFileName = "Vertices"+ to_string(index)+ ".vtk" ;
    ofstream ECMOut;
    ECMOut.open(vtkFileName.c_str());
    ECMOut<< "# vtk DataFile Version 3.0" << endl;
    ECMOut<< "Result for paraview 2d code" << endl;
    ECMOut << "ASCII" << endl;
    ECMOut << "DATASET UNSTRUCTURED_GRID" << endl;
    ECMOut << "POINTS " << allNodesX.size()   << " float" << endl;
    
    for (uint i = 0; i < allNodesX.size(); i++)
    {
        
        ECMOut << allNodesX.at(i) << " " << allNodesY.at(i) << " " << 0.0 << endl;
    }
    ECMOut<< endl;
    
    ECMOut<< "CELLS " << links.size()+ links2.size() << " " << 3 *( links.size()+ links2.size() )<< endl;
    
    for (uint i = 0; i < (links.size()); i++)           //number of connections per node
    {
        ECMOut << 2 << " " << i << " " << links.at(i) << endl;
        ECMOut << 2 << " " << i << " " << links2.at(i) << endl;
        
    }
    
    ECMOut << "CELL_TYPES " << links.size()+ links2.size()<< endl;             //connection type
    for (uint i = 0; i < links.size()+ links2.size(); i++) {
        ECMOut << "3" << endl;
    }
    
    ECMOut << "POINT_DATA "<<allNodesX.size() <<endl ;
    ECMOut << "SCALARS Cell_ID " << "float"<< endl;
    ECMOut << "LOOKUP_TABLE " << "default"<< endl;
    for (uint i = 0; i < cells.size() ; i++)
    {
        for ( uint j=0; j < cells.at(i).verticesX.size() ; j++)
        {
            ECMOut<< i <<endl ;
        }
    }
    for (uint i = 0; i < cells.size() ; i++)
    {
        ECMOut<< i <<endl ;
    }
    
    ECMOut.close();
}
//---------------------------------------------------------------------------------------------

void Tissue::ParaViewTissue ()
{
    vector<double> allNodesX ;
    vector<double> allNodesY ;
    for (int i=0 ; i< cells.size() ; i++)
    {
        for ( int j=0 ; j< cells.at(i).nodesXNew.size() ; j++)
        {
            allNodesX.push_back(cells.at(i).nodesXNew.at(j))   ;
            allNodesY.push_back(cells.at(i).nodesYNew.at(j))   ;
        }
    }
    int size =static_cast<int> (allNodesX.size() );
    string vtkFileName = "Tissue"+ to_string(index)+ ".vtk" ;
    ofstream TissueOut;
    TissueOut.open(vtkFileName.c_str());
    TissueOut<< "# vtk DataFile Version 3.0" << endl;
    TissueOut<< "Result for paraview 2d code" << endl;
    TissueOut << "ASCII" << endl;
    TissueOut << "DATASET UNSTRUCTURED_GRID" << endl;
    TissueOut << "POINTS " << size   << " float" << endl;
    
    for (uint i = 0; i < size ; i++)
    {
        
        TissueOut << allNodesX.at(i) << " " << allNodesY.at(i) << " " << 0.0 << endl;
    }
    TissueOut<< endl;
    TissueOut<< "CELLS " << size << " " << 3 *(size )<< endl;
    
    for (uint i = 0; i < size ; i++)           //number of connections per node
    {
        
        TissueOut << 2 << " " << i << " " << (i+1) % size << endl;
        
    }
    
    TissueOut << "CELL_TYPES " << size << endl;             //connection type
    for (uint i = 0; i < size ; i++) {
        TissueOut << "3" << endl;
    }
    
    TissueOut << "POINT_DATA "<< size <<endl ;
    TissueOut << "SCALARS Polygon " << "float"<< endl;
    TissueOut << "LOOKUP_TABLE " << "default"<< endl;
    for (uint i = 0; i < cells.size() ; i++)
    {
        for ( uint j=0; j< cells.at(i).nodesXNew.size()  ; j++)
        {
         //   TissueOut<< cells.at(i).verticesX.size() <<endl ;
            TissueOut<< cells.at(i).layer<<endl ;
        }
    }
    
    TissueOut.close();
}
//---------------------------------------------------------------------------------------------
void Tissue::Cal_AllCellConnections()
{
     int tmpShift = 0 ;
    int counter = 0 ;
    for (int i =0; i < cells.size(); i++)
    {
        tmpShift = counter ;
        for (int j=0 ; j< cells.at(i).verticesX.size(); j++)
        {
            cells.at(i).connections.push_back( (j+1) % cells.at(i).verticesX.size() + tmpShift ) ;
            counter += 1 ;
        }
    }
}
//---------------------------------------------------------------------------------------------
void Tissue:: Test()
{
    for (int i =0; i < cells.size(); i++)
    {
        cout<<cells.at(i).verticesX.size()<<endl ;
    }
}
//---------------------------------------------------------------------------------------------
void Tissue:: Add_NewVerticesToBoundaryEdges()
{
    for (int i =0; i < cells.size(); i++)
    {
        cells.at(i).Add_BoundaryVertice3 () ;
    }
}

//---------------------------------------------------------------------------------------------
void Tissue:: Refine_VerticesInBoundaryCells ()
{
    // Add vertices for nodes that are shared only between two cells but not three.
    for (int i =0; i < cells.size(); i++)
    {
        if (cells.at(i).boundary )
        {
            for (int j =0; j < cells.at(i).newVertX.size(); j++)
            {
                if (cells.at(i).newVertUpdateStatus.at(j) == false)
                {
                    double x1 = cells.at(i).newVertX.at(j) ;
                    double y1 = cells.at(i).newVertY.at(j) ;
                    vector<double> pointsX ;
                    vector<double> pointsY ;
                    vector<int > cellIDs ;
                    vector<int> newvertID ;
                    for (int k =0 ; k < cells.size() ; k++)
                    {
                        if (cells.at(k).boundary)
                        {
                            for (int l = 0; l < cells.at(k).newVertX.size(); l++)
                            {
                                double tmpDist = Dist2D(x1, y1, cells.at(k).newVertX.at(l), cells.at(k).newVertY.at(l)) ;
                                if (tmpDist < thres_corners )
                                {
                                    pointsX.push_back(cells.at(k).newVertX.at(l)) ;
                                    pointsY.push_back(cells.at(k).newVertY.at(l)) ;
                                    cellIDs.push_back(k) ;
                                    newvertID.push_back(l) ;
                                }
                                
                            }
                        }
                        
                    }
                    if (pointsX.size() > 4 || pointsX.size()==2)        // temporary works
                    {
                        double newvertX =  accumulate(pointsX.begin(), pointsX.end(), 0.0)/pointsX.size() ;
                        double newvertY =  accumulate(pointsY.begin(), pointsY.end(), 0.0)/pointsY.size() ;
                        
                        for (int n=0; n < cellIDs.size(); n++)
                        {
                            cells.at(cellIDs.at(n)).newVertUpdateStatus.at(newvertID.at(n)) = true ;
                        }
                        sort(cellIDs.begin(), cellIDs.end()) ;
                        cellIDs.erase( unique(cellIDs.begin(),cellIDs.end()) , cellIDs.end() ) ;
                        for (int n=0 ; n< cellIDs.size(); n++)
                        {
                            cells.at(cellIDs.at(n)).verticesX.push_back(newvertX) ;
                            cells.at(cellIDs.at(n)).verticesY.push_back(newvertY) ;
                            
                        }
                    }
                    else
                    {
                        cells.at(i).verticesX.push_back(x1) ;
                        cells.at(i).verticesY.push_back(y1) ;
                    }
                    
                }
            }
            
        }
    }
}
//---------------------------------------------------------------------------------------------

void Tissue::Cyclic4Correction()
{
    for (int i =0 ; i< cells.size(); i++)
    {
     //   int verticesSize = cells.at(i).verticesX.size() ;
        for (int j =0; j < cells.at(i).verticesX.size() ; j++)
        {
            vector<double> pointsX ;
            vector<double> pointsY ;
            vector<int > cellIDs ;
            vector<int> vertID ;
            
            for (int k =0; k < cells.at(i).cyclic4.size(); k++)
            {
                int cellID = cells.at(i).cyclic4.at(k) ;
              //  if (i < cellID)
                {
                    for (int l=0; l < cells.at(cellID).verticesX.size(); l++)
                    {
                        double x1 = cells.at(i).verticesX.at(j) ;
                        double y1 = cells.at(i).verticesY.at(j) ;
                        
                        double tmpDist = Dist2D(x1, y1, cells.at(cellID).verticesX.at(l), cells.at(cellID).verticesY.at(l)) ;
                        if (tmpDist < thres_cyclic4 )
                        {
                            pointsX.push_back(cells.at(cellID).verticesX.at(l)) ;
                            pointsY.push_back(cells.at(cellID).verticesY.at(l)) ;
                            cellIDs.push_back(cellID) ;
                            vertID.push_back(l) ;
                        }
                    }
                }
            }
            if (pointsX.size() > 0)
            {
                double newvertX =  accumulate(pointsX.begin(), pointsX.end(), 0.0)/pointsX.size() ;
                double newvertY =  accumulate(pointsY.begin(), pointsY.end(), 0.0)/pointsY.size() ;
                for (long n = vertID.size()-1 ; n >= 0 ; n--)
                {
                    cells.at(cellIDs.at(n)).verticesX.erase( cells.at(cellIDs.at(n)).verticesX.begin() + vertID.at(n) ) ;
                    cells.at(cellIDs.at(n)).verticesY.erase( cells.at(cellIDs.at(n)).verticesY.begin() + vertID.at(n) ) ;
                }
                
                sort(cellIDs.begin(), cellIDs.end()) ;
                cellIDs.erase( unique(cellIDs.begin(),cellIDs.end()) , cellIDs.end() ) ;
                for (int n=0 ; n< cellIDs.size(); n++)
                {
                    cells.at(cellIDs.at(n)).verticesX.push_back(newvertX) ;
                    cells.at(cellIDs.at(n)).verticesY.push_back(newvertY) ;
                    
                }
            }
                                
        }
    }
    
}

//---------------------------------------------------------------------------------------------
void Tissue:: Find_Cyclic4()
{
    for (int i =0; i < cells.size(); i++)
    {
        for (int j =0; j< cells.at(i).neighbors.size(); j++)
        {
            
            int cellID_B = cells.at(i).neighbors.at(j).CellID_Neighbor ;
            if ( i < cellID_B)
            {
                for (int k=0; k< cells.at(i).neighbors.at(j).commonNeighbors.size(); k++)
                {
                    for (int l = k+1; l < cells.at(i).neighbors.at(j).commonNeighbors.size() ; l++)
                    {
                        int cellID_C = cells.at(i).neighbors.at(j).commonNeighbors.at(k).cellIDCommonNeighbor ;
                        int cellID_D = cells.at(i).neighbors.at(j).commonNeighbors.at(l).cellIDCommonNeighbor ;
                        if (cellID_B < cellID_C && cellID_C < cellID_D )
                            {
                            
                            for (int m =0; m< cells.at(cellID_C).neighbors.size(); m++)
                            {
                                if (cells.at(cellID_C).neighbors.at(m).CellID_Neighbor == cellID_D )
                                {
                                    cells.at(i).cyclic4.push_back(i) ;
                                    cells.at(i).cyclic4.push_back(cellID_B) ;
                                    cells.at(i).cyclic4.push_back(cellID_C) ;
                                    cells.at(i).cyclic4.push_back(cellID_D) ;
                                    
                                    cells.at(cellID_B).cyclic4.push_back(i) ;
                                    cells.at(cellID_B).cyclic4.push_back(cellID_B) ;
                                    cells.at(cellID_B).cyclic4.push_back(cellID_C) ;
                                    cells.at(cellID_B).cyclic4.push_back(cellID_D) ;
                                    
                                    cells.at(cellID_C).cyclic4.push_back(i) ;
                                    cells.at(cellID_C).cyclic4.push_back(cellID_B) ;
                                    cells.at(cellID_C).cyclic4.push_back(cellID_C) ;
                                    cells.at(cellID_C).cyclic4.push_back(cellID_D) ;
                                    
                                    cells.at(cellID_D).cyclic4.push_back(i) ;
                                    cells.at(cellID_D).cyclic4.push_back(cellID_B) ;
                                    cells.at(cellID_D).cyclic4.push_back(cellID_C) ;
                                    cells.at(cellID_D).cyclic4.push_back(cellID_D) ;
                                    
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    for (int i =0; i < cells.size(); i++)
    {
        sort(cells.at(i).cyclic4.begin(), cells.at(i).cyclic4.end()) ;
        cells.at(i).cyclic4.erase( unique(cells.at(i).cyclic4.begin(),cells.at(i).cyclic4.end()) , cells.at(i).cyclic4.end() ) ;
    }
}

//---------------------------------------------------------------------------------------------

void Tissue::SortVertices()
{
    for (int i =0; i < cells.size(); i++)
    {
        cells.at(i).SortCCW(); // verticesX ,verticesY ) ;
    }
}
//---------------------------------------------------------------------------------------------
void Tissue::AllCell_RefineNoBoundary ()
{
    for (int i=0; i< cells.size(); i++)
    {
        cells.at(i).Refine_NoBoundary() ;
        //  cells.at(i).Refine_NodeXNew() ; 
    }
}
//---------------------------------------------------------------------------------------------
void Tissue::ParaViewBoundary ()
{
    vector<double> allNodesX ;
    vector<double> allNodesY ;
    for (int i=0 ; i< cells.size() ; i++)
    {
        for ( int j=0 ; j< cells.at(i).noNeighboringNodesX.size() ; j++)
        {
            allNodesX.push_back(cells.at(i).noNeighboringNodesX.at(j))   ;
            allNodesY.push_back(cells.at(i).noNeighboringNodesY.at(j))   ;
        }
    }
    int size =static_cast<int> (allNodesX.size() );
    string vtkFileName = "Boundary"+ to_string(index)+ ".vtk" ;
    ofstream TissueOut;
    TissueOut.open(vtkFileName.c_str());
    TissueOut<< "# vtk DataFile Version 3.0" << endl;
    TissueOut<< "Result for paraview 2d code" << endl;
    TissueOut << "ASCII" << endl;
    TissueOut << "DATASET UNSTRUCTURED_GRID" << endl;
    TissueOut << "POINTS " << size   << " float" << endl;
    
    for (uint i = 0; i < size ; i++)
    {
        
        TissueOut << allNodesX.at(i) << " " << allNodesY.at(i) << " " << 0.0 << endl;
    }
    TissueOut<< endl;
    TissueOut.close();
}
//---------------------------------------------------------------------------------------------
void Tissue::ParaViewInitialConfiguration ()
{
    vector<double> allNodesX ;
    vector<double> allNodesY ;
    for (int i=0 ; i< cells.size() ; i++)
    {
        for ( int j=0 ; j< cells.at(i).nodesX.size() ; j++)
        {
            allNodesX.push_back(cells.at(i).nodesX.at(j))   ;
            allNodesY.push_back(cells.at(i).nodesY.at(j))   ;
        }
    }
    int size =static_cast<int> (allNodesX.size() );
    string vtkFileName = "Initial"+ to_string(index)+ ".vtk" ;
    ofstream TissueOut;
    TissueOut.open(vtkFileName.c_str());
    TissueOut<< "# vtk DataFile Version 3.0" << endl;
    TissueOut<< "Result for paraview 2d code" << endl;
    TissueOut << "ASCII" << endl;
    TissueOut << "DATASET UNSTRUCTURED_GRID" << endl;
    TissueOut << "POINTS " << size   << " float" << endl;
    
    for (uint i = 0; i < size ; i++)
    {
        
        TissueOut << allNodesX.at(i) << " " << allNodesY.at(i) << " " << 0.0 << endl;
    }
    TissueOut<< endl;
    TissueOut.close();
}
//---------------------------------------------------------------------------------------------
void Tissue::Refine_CurvedInterface ()
{
    for (int i = 0; i< cells.size(); i++)
    {
        for (int j=0; j< cells.at(i).neighbors.size(); j++)
        {
            if (cells.at(i).neighbors.at(j).curvedInterface == false)
            {
                int cellID_Neighbor = cells.at(i).neighbors.at(j).CellID_Neighbor ;
                int neighborID_Cell = cells.at(i).neighbors.at(j).NeighborID_Cell ;
                vector<double> intfX = cells.at(i).neighbors.at(j).intfX ;
                vector<double> intfY = cells.at(i).neighbors.at(j).intfY ;
                pair<double, double> point ;
                vector<double> tmpX ;
                vector<double> tmpY ;
                //find the first point of the sequences and connect the whole curve to the nearest vertices
                if (cells.at(i).neighbors.at(j).intersectX.size() > 0)
                {
                    intfX.insert(intfX.end(), cells.at(i).neighbors.at(j).intersectX.begin(), cells.at(i).neighbors.at(j).intersectX.end() ) ;
                    intfY.insert(intfY.end(), cells.at(i).neighbors.at(j).intersectY.begin(), cells.at(i).neighbors.at(j).intersectY.end() ) ;
                    point.first = intfX.back() ;
                    point.second = intfY.back() ;
                }
                else
                {
                    vector<vector<double> > pairwiseDist =  Dist_VecToVec2D(intfX , intfY, intfX , intfY ) ;
                    vector<int> indecies =  Indices_MaxMatrix(pairwiseDist) ;
                    point.first = intfX.at( indecies.at(0) ) ;
                    point.second = intfY.at(indecies.at(0) ) ;
                }
                vector<double> tmpDist ;
                tmpDist = Dist_PointToVec2D(point.first, point.second, intfX, intfY) ;
                vector<int > idx(tmpDist.size());
                iota(idx.begin(), idx.end(), 0);
                sort(idx.begin(), idx.end(),
                     [&tmpDist](int i1, int i2) {return tmpDist[i1] < tmpDist[i2];});
                for (int k = 0; k < tmpDist.size(); k++)
                {
                    tmpX.push_back(intfX.at(idx.at(k) ) ) ;
                    tmpY.push_back(intfY.at(idx.at(k) ) ) ;
                }
                double area = 0.0 ;
                
                vector<double > cntx(tmpX.size(), cells.at(i).centroid.front() ) ;
                vector<double > cnty(tmpY.size(), cells.at(i).centroid.back() ) ;
                vector<double > distToCntrX ;
                distToCntrX.resize(tmpX.size() ) ;
                vector<double > distToCntrY ;
                distToCntrY.resize(tmpY.size() ) ;
                transform(tmpX.begin(), tmpX.end(), cntx.begin(), distToCntrX.begin(), linearConfig(1 ,-1) ) ;
                transform(tmpY.begin(), tmpY.end(), cnty.begin(), distToCntrY.begin(), linearConfig(1 ,-1) ) ;
                vector<double> tetta ;
                vector<double> tmpMag ;
                
                for (int k=0 ; k<distToCntrX.size(); k++)
                {
                    tetta.push_back(atan2(distToCntrY.at(k), distToCntrX.at(k) ) ) ;
                    tmpMag.push_back(MagnitudeVec(distToCntrX.at(k), distToCntrY.at(k))) ;
                }
                vector<double> dTetta ;
                for (int k=0 ; k<distToCntrX.size()-1; k++)
                {
                    dTetta.push_back( tetta.at(k+1) - tetta.at(k) ) ;
                    area += 0.5 * tmpMag.at(k+1) * tmpMag.at(k) * abs( sin( dTetta.back() ) ) ;
                }
                double refArea = 0.5 * tmpMag.front() * tmpMag.back() * abs( sin( tetta.back()-tetta.front() ) ) ;
                //   if (abs( (area - refArea )/ area ) > 0.3 )
                if ( (area - refArea )/ area > 0.5 )        // do it based on cells which are bigger than current vertices
                {
                    vector<double> a ;
                    vector<double> dA ;
                    for (int k=0; k< tetta.size(); k++)
                    {
                        a.push_back(0.5 * tmpMag.at(k) * ( abs( sin( tetta.at(k)- tetta.front() ) ) * tmpMag.front() +
                                                          abs( sin( tetta.at(k)- tetta.back()  ) ) * tmpMag.back() ) ) ;
                        dA.push_back(abs(a.back() - area ) ) ;
                    }
                    int idNode = min_element(dA.begin(), dA.end() )- dA.begin() ;
                    refArea = a.at(idNode) ;
                    
                    //       cells.at(i).neighbors.at(j).curvedInterface = true ;
                    cells.at(i).verticesX.push_back(tmpX.at(idNode) ) ;
                    cells.at(i).verticesY.push_back(tmpY.at(idNode) ) ;
                    
                    //       cells.at(cellID_Neighbor).neighbors.at(neighborID_Cell ).curvedInterface = true ;
                    cells.at(cellID_Neighbor).verticesX.push_back(tmpX.at(idNode)) ;
                    cells.at(cellID_Neighbor).verticesY.push_back(tmpY.at(idNode)) ;
                    
                    
                    
                }
            }
        }
    }
}

//---------------------------------------------------------------------------------------------
void Tissue::Print_VeritcesSize()
{
    for (int i=0; i<cells.size(); i++)
    {
        cout<<"Cell "<<i<<" :"<<'\t'<<cells.at(i).verticesX.size()<<endl ;
    }
}





//--------------------------------------Meshes----------------=--------------------------------
//---------------------------------------------------------------------------------------------
void Tissue::Find_AllMeshes()
{
    for (int i =0 ; i < cells.size(); i++)
    {
        cells.at(i).Find_Mesh() ;
    }
}
//---------------------------------------------------------------------------------------------
void Tissue::Cal_AllSelfDiffusion()
{
    for (int i =0 ; i < cells.size(); i++)
    {
        cells.at(i).Self_Diffusion() ;
    }
}
//---------------------------------------------------------------------------------------------
void Tissue::Find_IntercellularMeshConnection()
{
    for (int i =0; i < cells.size(); i++)
    {
        for (int k =0 ; k< cells.at(i).meshes.size(); k++)
        {
            int counter = 0 ;
            for (int j =0; j < cells.at(i).neighbors.size(); j++)
            {
                int cell2 = cells.at(i).neighbors.at(j).CellID_Neighbor ;
                for (int l = 0; l < cells.at(cell2).meshes.size() ; l++)
                {
                    double smallValue = 0.0001 ;
                    counter = 0 ;
                    
                    if ( Dist2D(cells.at(i).meshes.at(k).triangleX.at(1), cells.at(i).meshes.at(k).triangleY.at(1),
                                cells.at(cell2).meshes.at(l).triangleX.at(1), cells.at(cell2).meshes.at(l).triangleY.at(1)) < smallValue
                        || Dist2D(cells.at(i).meshes.at(k).triangleX.at(1), cells.at(i).meshes.at(k).triangleY.at(1),
                                  cells.at(cell2).meshes.at(l).triangleX.at(2), cells.at(cell2).meshes.at(l).triangleY.at(2)) < smallValue )
                    {
                        counter +=1 ;
                    }
                    if ( Dist2D(cells.at(i).meshes.at(k).triangleX.at(2), cells.at(i).meshes.at(k).triangleY.at(2),
                                cells.at(cell2).meshes.at(l).triangleX.at(1), cells.at(cell2).meshes.at(l).triangleY.at(1)) < smallValue
                        || Dist2D(cells.at(i).meshes.at(k).triangleX.at(2), cells.at(i).meshes.at(k).triangleY.at(2),
                                  cells.at(cell2).meshes.at(l).triangleX.at(2), cells.at(cell2).meshes.at(l).triangleY.at(2)) < smallValue )
                    {
                        counter +=1 ;
                    }
                    if (counter ==2)
                    {
                        cells.at(i).meshes.at(k).connection.first = cell2 ;
                        cells.at(i).meshes.at(k).connection.second = l ;
                        double length =Dist2D(cells.at(i).meshes.at(k).center.at(0),cells.at(i).meshes.at(k).center.at(1),
                                              cells.at(cell2).meshes.at(l).center.at(0), cells.at(cell2).meshes.at(l).center.at(1)) ;
                        
                        cells.at(i).meshes.at(k).length.at(2) = length ;
                        
                        cells.at(cell2).meshes.at(l).connection.first = i ;
                        cells.at(cell2).meshes.at(l).connection.second = k ;
                        cells.at(cell2).meshes.at(l).length.at(2) = length ;
                        
                        break ;
                    }
                }
                if (counter ==2)    break ;
                
            }
        }
    }
}

//---------------------------------------------------------------------------------------------

void Tissue::IntercellularDiffusion()
{
    double dInter = 4.0 ;
    for (int i =0 ; i < cells.size(); i++)
    {
        for (int j =0; j < cells.at(i).meshes.size(); j++)
        {
            int cellID = cells.at(i).meshes.at(j).connection.first ;
            int meshID = cells.at(i).meshes.at(j).connection.second ;
            //cellID== -1 means the mesh is a boundary mesh (internal connections only )
            if (cellID == -1)
            {
                continue ;
            }
            double area = cells.at(i).meshes.at(j).area.at(2) ;
            double length = cells.at(i).meshes.at(j).length.at(2) ;
            double deltaU = cells.at(cellID).meshes.at(meshID).u1 - cells.at(i).meshes.at(j).u1 ;
            cells.at(i).meshes.at(j).flux.push_back(dInter * area * deltaU / length) ;
            
        }
    }
}
//---------------------------------------------------------------------------------------------
void Tissue::EulerMethod()
{
    bool state = false ;
    int l = 0 ;
    cout<< "Signal_Calculator is running"<<endl ;
    while (state==false)
    {
        Cal_AllSelfDiffusion() ;
        IntercellularDiffusion() ;
        
   //     if (l%1000==0) cout<<l/1000<<endl ;
   //     if (l%1000==0) ParaViewMesh(l/1000) ;
        
        for (int i = 0; i < cells.size(); i++)
        {
            for (int j =0; j < cells.at(i).meshes.size(); j++)
            {
                cells.at(i).meshes.at(j).Euler() ;
            }
        }
        
        state = true ;
        for (int i = 0; i < cells.size(); i++)
        {
            for (int j =0; j < cells.at(i).meshes.size(); j++)
            {
                 if( abs(cells.at(i).meshes.at(j).u2- cells.at(i).meshes.at(j).u1)/(cells.at(i).meshes.at(j).u1 + 0.000001) > 0.000001)
                 {
                     state = false ;
                     break ;
                 }
                 
            }
            if (state == false) break ;
        }
        for (int i = 0; i < cells.size(); i++)
        {
            for (int j =0; j < cells.at(i).meshes.size(); j++)
            {
                cells.at(i).meshes.at(j).UpdateU() ;
            }
        }
        
        l++ ;
    }
 //    ParaViewMesh(index) ;
    double value = 0 ;
    for (int j =0 ; j < cells.size();j++)
    {
        for (int i =0; i< cells.at(j).meshes.size(); i++)
        {
            value += cells.at(j).meshes.at(i).u1 ;
        }
    }
    cout<< "value is "<<value<<endl ;
//    cout<< "number of steps needed is " << l <<endl ;
}
//---------------------------------------------------------------------------------------------
void Tissue::EulerMethod2 ()
{
    
    for (int l =0 ; l <= 100000; l++)
    {
        Cal_AllSelfDiffusion() ;
        IntercellularDiffusion() ;
        if (l%1000==0) cout<<l/1000<<endl ;
        if (l%1000==0)
        {
            ParaViewMesh(l) ;
        }
        
        for (int i = 0; i < cells.size(); i++)
        {
            for (int j =0; j < cells.at(i).meshes.size(); j++)
            {
                cells.at(i).meshes.at(j).Euler() ;
           //     cells.at(i).meshes.at(j).FullModel_Euler() ;
                cells.at(i).meshes.at(j).UpdateU() ;
            }
        }
    }
}
//---------------------------------------------------------------------------------------------
void Tissue::Find_SecretingCell()
{
    
    double minDis = Dist2D(25, 25, cells.at(0).centroid.at(0), cells.at(0).centroid.at(1)) ;
 //   double minDis = Dist2D(0, 0, cells.at(0).centroid.at(0), cells.at(0).centroid.at(1)) ;
    int cellID = 0 ;
    double dis ;
    for (int i = 1; i< cells.size(); i++)
    {
        dis = Dist2D(25, 25, cells.at(i).centroid.at(0), cells.at(i).centroid.at(1)) ;
     //   dis = Dist2D(0, 0, cells.at(i).centroid.at(0), cells.at(i).centroid.at(1)) ;
        if (dis < minDis)
        {
            minDis = dis ;
            cellID = i ;
        }
    }
    cout <<"cellID source " << cellID <<endl ;
    for (int j = 0; j < cells.at(cellID).meshes.size(); j++)
    {
        cells.at(cellID).meshes.at(j).c = 1.0 ;
    }
}
//---------------------------------------------------------------------------------------------
void Tissue::ParaViewMesh(int number)
{
    vector<double> allNodesX ;
    vector<double> allNodesY ;
    int numberOfCells = 0 ;
    for (int i=0 ; i< cells.size() ; i++)
    {
        for ( int j=0 ; j< cells.at(i).meshes.size() ; j++)
        {
            numberOfCells += 1 ;
            for (int k =0; k< cells.at(i).meshes.at(j).triangleX.size(); k++)
            {
                allNodesX.push_back( cells.at(i).meshes.at(j).triangleX.at(k))   ;
                allNodesY.push_back( cells.at(i).meshes.at(j).triangleY.at(k))   ;
            }
        }
    }
    
    string vtkFileName = "Mesh"+ to_string(number)+ ".vtk" ;
    ofstream ECMOut;
    ECMOut.open(vtkFileName.c_str());
    ECMOut<< "# vtk DataFile Version 3.0" << endl;
    ECMOut<< "Result for paraview 2d code" << endl;
    ECMOut << "ASCII" << endl;
    ECMOut << "DATASET UNSTRUCTURED_GRID" << endl;
    ECMOut << "POINTS " << allNodesX.size()   << " float" << endl;
    
    for (uint i = 0; i < allNodesX.size(); i++)
    {
        
        ECMOut << allNodesX.at(i) << " " << allNodesY.at(i) << " " << 0.0 << endl;
    }
    ECMOut<< endl;
    ECMOut<< "CELLS " << numberOfCells << " " << 4 *( numberOfCells )<< endl;
    
    for (int i=0 ; i< numberOfCells ; i++)
    {
            ECMOut << 3 << " " << 3*i << " " << 3*i +1  << " " << 3*i +2 <<endl;
    }
    
    ECMOut << "CELL_TYPES " << numberOfCells << endl;             //connection type
    for (uint i = 0; i < numberOfCells ; i++) {
        ECMOut << "5" << endl;
    }
    if (equationsType == simpeODE)
    {
        ECMOut << "POINT_DATA "<<allNodesX.size() <<endl ;
        ECMOut << "SCALARS WUS " << "float"<< endl;
        ECMOut << "LOOKUP_TABLE " << "default"<< endl;
        for (uint i = 0; i < cells.size() ; i++)
        {
            for ( uint j=0; j < cells.at(i).meshes.size() ; j++)
            {
                ECMOut << cells.at(i).meshes.at(j).u1 << " " <<cells.at(i).meshes.at(j).u1 << " "<<cells.at(i).meshes.at(j).u1 <<endl ;
            }
        }
    }
    if (equationsType == fullModel)
    {
        ECMOut << "POINT_DATA "<<allNodesX.size() <<endl ;
        ECMOut << "SCALARS WUS_mRNA " << "float"<< endl;
        ECMOut << "LOOKUP_TABLE " << "default"<< endl;
        for (uint i = 0; i < cells.size() ; i++)
        {
            for ( uint j=0; j < cells.at(i).meshes.size() ; j++)
            {
                   ECMOut << cells.at(i).meshes.at(j).concentrations2.at(0)<<" " <<cells.at(i).meshes.at(j).concentrations2.at(0) << " "<<cells.at(i).meshes.at(j).concentrations2.at(0) <<endl ;
            }
        }
        
        ECMOut << "SCALARS WUS_nucleus " << "float"<< endl;
        ECMOut << "LOOKUP_TABLE " << "default"<< endl;
        for (uint i = 0; i < cells.size() ; i++)
        {
            for ( uint j=0; j < cells.at(i).meshes.size() ; j++)
            {
                ECMOut << cells.at(i).meshes.at(j).concentrations2.at(1)<<" " <<cells.at(i).meshes.at(j).concentrations2.at(1) << " "<<cells.at(i).meshes.at(j).concentrations2.at(1) <<endl ;
            }
        }

        ECMOut << "SCALARS WUS_cytoplasm " << "float"<< endl;
        ECMOut << "LOOKUP_TABLE " << "default"<< endl;
        for (uint i = 0; i < cells.size() ; i++)
        {
            for ( uint j=0; j < cells.at(i).meshes.size() ; j++)
            {
                ECMOut << cells.at(i).meshes.at(j).concentrations2.at(2)<<" " <<cells.at(i).meshes.at(j).concentrations2.at(2) << " "<<cells.at(i).meshes.at(j).concentrations2.at(2) <<endl ;
            }
        }

        ECMOut << "SCALARS CLV3 " << "float"<< endl;
        ECMOut << "LOOKUP_TABLE " << "default"<< endl;
        for (uint i = 0; i < cells.size() ; i++)
        {
            for ( uint j=0; j < cells.at(i).meshes.size() ; j++)
            {
                ECMOut << cells.at(i).meshes.at(j).concentrations2.at(3)<<" " <<cells.at(i).meshes.at(j).concentrations2.at(3) << " "<<cells.at(i).meshes.at(j).concentrations2.at(3) <<endl ;
            }
        }
        
        ECMOut << "SCALARS ck " << "float"<< endl;
        ECMOut << "LOOKUP_TABLE " << "default"<< endl;
        for (uint i = 0; i < cells.size() ; i++)
        {
            for ( uint j=0; j < cells.at(i).meshes.size() ; j++)
            {
                ECMOut << cells.at(i).meshes.at(j).concentrations2.at(4)<<" " <<cells.at(i).meshes.at(j).concentrations2.at(4) << " "<<cells.at(i).meshes.at(j).concentrations2.at(4) <<endl ;
            }
        }

        ECMOut << "SCALARS ck_R " << "float"<< endl;
        ECMOut << "LOOKUP_TABLE " << "default"<< endl;
        for (uint i = 0; i < cells.size() ; i++)
        {
            for ( uint j=0; j < cells.at(i).meshes.size() ; j++)
            {
                ECMOut << cells.at(i).meshes.at(j).concentrations2.at(5)<<" " <<cells.at(i).meshes.at(j).concentrations2.at(5) << " "<<cells.at(i).meshes.at(j).concentrations2.at(5) <<endl ;
            }
        }
        

        ECMOut << "SCALARS CK " << "float"<< endl;
        ECMOut << "LOOKUP_TABLE " << "default"<< endl;
        for (uint i = 0; i < cells.size() ; i++)
        {
            for ( uint j=0; j < cells.at(i).meshes.size() ; j++)
            {
                ECMOut << cells.at(i).meshes.at(j).concentrations2.at(6)<<" " <<cells.at(i).meshes.at(j).concentrations2.at(6) << " "<<cells.at(i).meshes.at(j).concentrations2.at(6) <<endl ;
            }
        }
    }
    ECMOut.close();
}
 //---------------------------------------------------------------------------------------------
void Tissue::FullModel_Diffusion()
{
    for (int i=0; i<cells.size(); i++)
    {
        for (int j=0; j< cells.at(i).meshes.size(); j++)
        {
            cells.at(i).meshes.at(j).Flux.clear() ;
        }
    }
    for (int i =0 ; i<cells.size(); i++)
    {
        
       cells.at(i).FullModel_SelfDiffusion () ;
    }
   
    for (int i =0 ; i<cells.size(); i++)
    {
        for (int j =0; j < cells.at(i).meshes.size(); j++)
        {
            int cellID = cells.at(i).meshes.at(j).connection.first ;
            int meshID = cells.at(i).meshes.at(j).connection.second ;
            //cellID== -1 means the mesh is a boundary mesh (internal connections only )
            if (cellID == -1)
            {
                continue ;
            }
            else if (i < cellID )
            {
                double area = cells.at(i).meshes.at(j).area.at(2) ;
                double length = cells.at(i).meshes.at(j).length.at(2) ;
                vector<double> deltaU ;
                deltaU.resize(cells.at(i).meshes.at(j).concentrations.size() ) ;
               transform( cells.at(cellID).meshes.at(meshID ).concentrations.begin()+1, cells.at(cellID).meshes.at(meshID ).concentrations.begin()+5,
                          cells.at(i).meshes.at(j).concentrations.begin()+1, deltaU.begin()+1, linearConfig(1 ,-1) );
                vector<double> flux ;
                flux.clear() ;
                flux.resize(deltaU.size() ) ;
              transform(deltaU.begin()+1, deltaU.begin()+5, flux.begin()+1, productNum(area/length ) ) ;
                transform(flux.begin()+1, flux.begin()+5, cells.at(i).meshes.at(j).diffusions.begin()+1, flux.begin()+1 , productVec()  ) ;
                cells.at(i).meshes.at(j).Flux.push_back(flux) ;
                transform(flux.begin()+1, flux.begin()+5, flux.begin()+1, productNum(-1) ) ;
                cells.at(cellID).meshes.at(meshID).Flux.push_back( flux ) ;
    
            }
        }
    }
    
}

//---------------------------------------------------------------------------------------------

void Tissue::FullModel_AllCellProductions()         //call once, initialize the constants
{
    for (int i=0; i<cells.size(); i++)
    {
        if (cells.at(i).layer >= thres_layer && abs(cells.at(i).centroid.at(0) ) < thres_Production )
        {
            // CLV3 layer 3+
            cells.at(i).productionW = 1 ;
            cells.at(i).productionC = 1 ;
            // ck layer 4+
            cells.at(i).productionCk  = 1 ;
        }
        else
        {
            cells.at(i).productionW = 0 ;
            cells.at(i).productionC = 0 ;
            cells.at(i).productionCk  = 0 ;
        }
        cells.at(i).FullModel_ProductionCell() ;
    }
}
//---------------------------------------------------------------------------------------------
void Tissue::FullModelEulerMethod()
{
    bool state = false ;
    int l = 0 ;
    while (
           state==false
           && l<=70000
           )
    {
        FullModel_Diffusion() ;
   //     if (l%1000==0) cout<<l/1000<<endl ;
   //     if (l%1000==0) ParaViewMesh(l/1000) ;
        for (int i = 0; i < cells.size(); i++)
        {
            for (int j =0; j < cells.at(i).meshes.size(); j++)
            {
                cells.at(i).meshes.at(j).FullModel_Euler() ;
            }
        }
        
        state = true ;
        for (int i = 0; i < cells.size(); i++)
        {
            for (int j =0; j < cells.at(i).meshes.size(); j++)
            {
                if(
                   abs(cells.at(i).meshes.at(j).concentrations2.at(1) - cells.at(i).meshes.at(j).concentrations.at(1) )/(cells.at(i).meshes.at(j).concentrations.at(1) + 0.000001) > 0.000001
                 ||  abs(cells.at(i).meshes.at(j).concentrations2.at(3) - cells.at(i).meshes.at(j).concentrations.at(3) )/(cells.at(i).meshes.at(j).concentrations.at(3) + 0.000001) > 0.000001
                    || abs(cells.at(i).meshes.at(j).concentrations2.at(6) - cells.at(i).meshes.at(j).concentrations.at(6) )/(cells.at(i).meshes.at(j).concentrations.at(6) + 0.000001) > 0.000001
                   )
                {
                    state = false ;
                    break ;
                }
                
            }
            if (state == false) break ;
        }
        for (int i = 0; i < cells.size(); i++)
        {
            for (int j =0; j < cells.at(i).meshes.size(); j++)
            {
                cells.at(i).meshes.at(j).UpdateU() ;
            }
        }
        
        
        l++ ;
    }
    ParaViewMesh(index) ;
    cout<<"l is equal to "<<l << endl ;
    /*
    double value = 0 ;
    for (int j =0 ; j < cells.size();j++)
    {
        for (int i =0; i< cells.at(j).meshes.size(); i++)
        {
            value += cells.at(j).meshes.at(i).u1 ;
        }
    }
    cout<< "value is "<<value<<endl ;
    cout<< "number of steps needed is " << l <<endl ;
     */
}
//---------------------------------------------------------------------------------------------

void Tissue::Cal_AllCellConcentration()
{
    for (int i =0; i<cells.size(); i++)
    {
        cells.at(i).CellLevelConcentration() ;
        if (equationsType== simpeODE)
        {
            tissueLevelU.push_back(cells.at(i).cellU ) ;
        }
        else
        {
            tissueLevelConcentration.push_back(cells.at(i).cellConcentration ) ;
        }
    }
}
