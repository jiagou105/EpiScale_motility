#include <fstream>
#include <sstream>
#include "Signal_Calculator.h"
#include "Signal_MakeInterface.h"
#include <unistd.h>
#include <time.h>  
#include <stdlib.h>
#include <stdio.h>
//#include "engine.h"

//#include "matlab_engine.hpp"
//#include "engine_factory.hpp"
//#include "MatlabDataArray.hpp"
//#include "MatlabEngine.hpp"

namespace patch
{
template  < typename T> std::string to_string (const T & n)
{
std::ostringstream stm ; 
stm << n ; 
return stm.str(); 
}



}


void Signal::Initialize (uint maxAllNodePerCell, uint maxMembrNodePerCell, uint maxTotalNodes, uint maxCellCount) {
	
	this->maxAllNodePerCell=maxAllNodePerCell ; 
	this->maxMembrNodePerCell=maxMembrNodePerCell ;
	this->maxCellCount=maxCellCount ; 
	periodCount=0 ; 
	nodeLocXHost.resize(maxTotalNodes, 0.0) ; 
	nodeLocYHost.resize(maxTotalNodes, 0.0) ;
	nodeIsActiveHost.resize(maxTotalNodes,false) ; 
	cellCenterX.resize(maxCellCount,0.0) ; 
	cellCenterY.resize(maxCellCount,0.0) ; 
	dppLevel.resize(maxCellCount,0.0) ; 
	tkvLevel.resize(maxCellCount,0.0) ; //Alireza
	dppTkvLevel.resize(maxCellCount,0.0) ; //Alireza
	pMadLevel.resize(maxCellCount,0.0) ; //Alireza

	minResol=0.1 ;// max distance of the first imported coordinate of DPP from the tissue center to accept it for that cell    
	resol=501 ; // the number of imported DPP values
	cout << "I am at the end of signal initialization function" << endl ; 
	cout << "size of node is active in signal module initialization is " << nodeIsActiveHost.size() << endl ; 
	cout << "max of all nodes per cell in signal module initialization is " << maxAllNodePerCell << endl ; 
}
void Signal::updateSignal(double minX, double maxX, double minY, double maxY, double curTime, int maxTotalNumActiveNodes, int numActiveCells)  {
	this->maxX=maxX;
	this->maxY=maxY;
	this->minX=minX;
	this->minY=minY;
	this->curTime=curTime ; 
	this->maxTotalNumActiveNodes=maxTotalNumActiveNodes ; 
	this->numActiveCells=numActiveCells ; 
	cout << "I am in update signal started" << std::endl ; 
	
	//calls matlab and  all dpp info from matlab
	exportGeometryInfo()	; 
	
	//importSignalInfoCellLevel()	; 
    // Alireza: processSignalInfoCellLevel() no longer needed with new function
    // processSignalInfoCellLevel()    ;

	cout << "I am in update signal ended" << std::endl ;
}

void Signal::exportGeometryInfo() {

	//ALIREZA CODE BEGIN

 	double Center_X=minX+0.5*(maxX-minX); 
 	int cellRank ; 
	int totalNumActiveMembraneNodes=0 ; 
	for (uint i = 0; i < maxTotalNumActiveNodes; i++) 
	{
		cellRank = i / maxAllNodePerCell;
		if (nodeIsActiveHost[i] && (i%maxAllNodePerCell) < maxMembrNodePerCell) 
		{
			totalNumActiveMembraneNodes++ ; 
		}
	}
	vector<double > cntX ;
   	vector<double > cntY ;
	vector<vector<double > > concentrations ;
    	for (int k = 0; k < numActiveCells; k++)
    	{
		cntX.push_back( cellCenterX[k] ) ;
		cntY.push_back( cellCenterY[k] ) ;
		
		vector<double> newCell ;
		newCell.push_back( dppLevel[k] ) ;
		newCell.push_back( tkvLevel[k] ) ;
		newCell.push_back( dppTkvLevel[k] ) ;
		newCell.push_back( pMadLevel[k] ) ;
		concentrations.push_back( newCell ) ;
		
    	}
	vector <vector < double > > locX ;
   	vector <vector < double > > locY ;
	locX.clear() ;
	locY.clear() ;
	int size = 0 ;
    	for (uint i = 0; i < maxTotalNumActiveNodes; i++)
   	 {
		cellRank = i / maxAllNodePerCell ;
		if (nodeIsActiveHost[i] && (i % maxAllNodePerCell) < maxMembrNodePerCell)
        	{
           	 	if (size == cellRank)
		 	{
                		vector < double > newCell ;
                		locX.push_back( newCell ) ;
                		locY.push_back( newCell ) ;
				 size += 1 ;
			 }
			locX.back().push_back ( nodeLocXHost[i] ) ;
			locY.back().push_back ( nodeLocYHost[i] ) ;
       		 }
  	  }
	concentrations = Signal_Calculator ( locX , locY , cntX , cntY, concentrations, frameNumber ) ;       //output required
	for (int i = 0; i < numActiveCells; i++)
    	{
		dppLevel[i] = concentrations[i].at(0) ;
		tkvLevel[i] = concentrations[i].at(1) ;
		dppTkvLevel[i] = concentrations[i].at(2) ;
		pMadLevel[i] = concentrations[i].at(3) ;
	}
	
	//Ali code: writing nodes locations in a file, needed for debuging
	srand(time(NULL));
	bool writeLoc ;
	writeLoc = !fmod(static_cast<int>(round ( 100 * frameNumber) ), 100)
	if (writeLoc==true)
	{
		double max_Rx=max (maxX-Center_X,Center_X-minX) ; 
		cout << "The information is read from "<< periodCount<<" data" <<endl ; 
		cout << "size of node is active in signal module is " << nodeIsActiveHost.size() << endl ; 
		cout << "max total number of active nodes in signal module is " << maxTotalNumActiveNodes  << endl ; 
		cout << "max of all nodes per cell in signal module is " << maxAllNodePerCell << endl ; 
		//std :: string  txtFileName="ExportTisuProp_" + patch::to_string(periodCount)+".txt" ; 
		std :: string  txtFileName="ExportCellProp_" + patch::to_string(frameNumber)+".txt" ; 
		ofstream ExportOut ; 
		ExportOut.open(txtFileName.c_str()); 
		//ExportOut << "Time (s), Tissue_CenterX(micro meter),Max_Length_X(micro meter)"<<endl; 
		//ExportOut<<curTime<<","<<Center_X<<","<<max_Rx<<endl   ;
		//ExportOut << "CellRank,CellCenterX,CellCenterY"<<endl;
		/*
		// writing cell centers at the beginning of the file
		for (int k=0; k<numActiveCells; k++){
			ExportOut<<k<<","<<cellCenterX[k]<<","<<cellCenterY[k]<<endl   ;
		}
		*/
		//ExportOut << "CellRank,MembraneNodeX,MembraneNodeY"<<endl;

		for ( uint i=0 ; i< maxTotalNumActiveNodes ; i++) {

			cellRank= i/maxAllNodePerCell ; 
			if (nodeIsActiveHost[i] && (i%maxAllNodePerCell)<maxMembrNodePerCell) {
				ExportOut<<cellRank<<" "<<nodeLocXHost[i]<<" "<<nodeLocYHost[i]<<endl   ;
			}
		}
		//LastLineIndicator is not included in the Signal_Calculator function
		/*
		int lastLineIndicator=123456789 ;
		ExportOut<<lastLineIndicator<<","<<lastLineIndicator<<","<<lastLineIndicator<<endl   ;
		*/
		ExportOut.flush() ;
		cout << "I exported  the data for signaling model"<< endl ; 
		ExportOut.close() ;  
	}
	frameNumber += 0.1 ;
}

	/*
	std::unique_ptr<matlab::engine::MATLABEngine> matlabPtr = matlab::engine::startMATLAB();

	using namespace matlab::engine;

	//Create MATLAB data array factory 
	matlab::data::ArrayFactory factory;

	//set empty matlab matlab dataArrays
	
	auto data_cell_index = factory.createBuffer<unsigned>(numActiveCells);
	auto data_cell_pos_x = factory.createBuffer<double>(numActiveCells);
	auto data_cell_pos_y = factory.createBuffer<double>(numActiveCells);

	auto data_node_index = factory.createBuffer<unsigned>(totalNumActiveMembraneNodes);
	auto data_node_pos_x = factory.createBuffer<double>(totalNumActiveMembraneNodes);
	auto data_node_pos_y = factory.createBuffer<double>(totalNumActiveMembraneNodes);


	//set ptrs
	unsigned* data_cell_cell_index_ptr = data_cell_index.get();
	double* data_cell_pos_x_ptr = data_cell_pos_x.get();
	double* data_cell_pos_y_ptr = data_cell_pos_y.get();

	unsigned* data_node_index_ptr = data_node_index.get();
	double* data_node_pos_x_ptr = data_node_pos_x.get();
	double* data_node_pos_y_ptr = data_node_pos_y.get();


	for (int k = 0; k < numActiveCells; k++) {
		*(data_cell_cell_index_ptr++) = k + 1;
		*(data_cell_pos_x_ptr++) = cellCenterX[k];
		*(data_cell_pos_y_ptr++) = cellCenterY[k];
		//ExportOut << k << "," << cellCenterX[k] << "," << cellCenterY[k] << endl;
	}

	for (uint i = 0; i < maxTotalNumActiveNodes; i++) {

		cellRank = i / maxAllNodePerCell;
		if (nodeIsActiveHost[i] && (i%maxAllNodePerCell) < maxMembrNodePerCell) {
			*(data_node_index_ptr++) = cellRank + 1;
			*(data_node_pos_x_ptr++) = nodeLocXHost[i];
			*(data_node_pos_y_ptr++) = nodeLocYHost[i];
			//ExportOut << cellRank << "," << nodeLocXHost[i] << "," << nodeLocYHost[i] << endl;
		}
	}

	//create arrays from buffers
	//cells
	auto arr_data_cell_index = factory.createArrayFromBuffer<unsigned>(
		{ numActiveCells },
		std::move(data_cell_index));

	auto arr_data_cell_pos_x = factory.createArrayFromBuffer<double>(
		{ numActiveCells },
		std::move(data_cell_pos_x));

	auto arr_data_cell_pos_y = factory.createArrayFromBuffer<double>(
		{ numActiveCells },
		std::move(data_cell_pos_y));
	//nodes
	auto arr_data_node_index = factory.createArrayFromBuffer<unsigned>(
		{ totalNumActiveMembraneNodes },
		std::move(data_node_index));

	auto arr_data_node_pos_x = factory.createArrayFromBuffer<double>(
		{ totalNumActiveMembraneNodes },
		std::move(data_node_pos_x));

	auto arr_data_node_pos_y = factory.createArrayFromBuffer<double>(
		{ totalNumActiveMembraneNodes },
		std::move(data_node_pos_y));


 
	matlab::data::TypedArray<double> dpp_cell_output = matlabPtr->
		feval(convertUTF8StringToUTF16String("main_signaling"), {
			arr_data_cell_index, arr_data_cell_pos_x, arr_data_cell_pos_y,
			arr_data_node_index, arr_data_node_pos_x, arr_data_node_pos_y }); 


	//NOT SURE IF THIS VECTOR IS CORRECT
	dppLevelV.clear();
	//WARNING: CHECK IF INDEXING IS CORRECT IN OUTPUT
	for (unsigned i = 0; i < dpp_cell_output.getNumberOfElements(); i++) {
		double current_dpp_level = dpp_cell_output[i];

		dppLevelV.push_back(current_dpp_level);
	}
	for ( int i =0 ; i< dppLevelV.size() ; i++) {

		cout << "dpp level for node " << i << " is equal to " << dppLevelV.at(i) << endl ; 
	} 
		
}

//NOT NEEDED
void Signal::importSignalInfoCellLevel() {


		float dppDist,dppLevelTmp ; 
		dppLevelV.clear(); 
		dppDistV.clear();

		std:: string importDppFileName= "Dpp_cell_T" + patch::to_string(periodCount) + ".txt" ;


		std:: ifstream inputDpp ;
		bool fileIsOpen=false ;
			
		cout << "The file name I am looking for is " << importDppFileName <<endl ;
		
		sleep(300) ; 
		while (fileIsOpen==false) {
			inputDpp.open(importDppFileName.c_str()) ;
			if (inputDpp.is_open()){
				cout<< "dpp file opened successfully"<<endl; 
				cout << "the opened file name is " << importDppFileName <<endl ;
				fileIsOpen=true ; 
				 clock_t t;
				t = clock();
				cout << "I start to sleep. Time is"  <<endl ;
				sleep(30) ; 
				 t = clock() - t;
				cout << "Sleep takes "<< ((float)t)/CLOCKS_PER_SEC  <<endl ;
				cout << "the opened file name is " << importDppFileName <<endl ;
			}
			else {
		//		cout << "failed openining dpp file"<<endl ;
			}	
		}
		if (inputDpp.good()) {
		cout << " I passed opening the file in the while loop"<< endl ;
		}

 		periodCount+= 1 ;// abs(floor((curTime-InitTimeStage)/exchPeriod)) ;
		for (int i=0; i<numActiveCells ; i++) {
			inputDpp >> dppLevelTmp ;
			cout<<"zeroth dpp is"<<dppLevelTmp<< endl ; 
			dppLevelV.push_back(dppLevelTmp) ;  
		}	
		cout <<"first dpp value is"<< dppLevelV.at(0)<< endl ; 	

}
*/



//NOT NEEDED
void Signal::importSignalInfoTissueLevel() {


		float dppDist,dppLevelTmp ; 
		dppLevelV.clear(); 
		dppDistV.clear();

		std:: string importDppFileName= "DppImport_" + patch::to_string(periodCount) + ".txt" ;


		std:: ifstream inputDpp ;
		bool fileIsOpen=false ;
			
		cout << "the file name I am looking for is " << importDppFileName <<endl ;
		
		sleep(200) ; 
		while (fileIsOpen==false) {
			inputDpp.open(importDppFileName.c_str()) ;
			if (inputDpp.is_open()){
				cout<< "dpp file opened successfully"<<endl; 
				cout << "the opened file nameis " << importDppFileName <<endl ;
				fileIsOpen=true ; 
				 clock_t t;
				t = clock();
				cout << "I start to sleep time is"  <<endl ;
				sleep(30) ; 
				 t = clock() - t;
				cout << "Sleep takes"<< ((float)t)/CLOCKS_PER_SEC  <<endl ;
				cout << "the opened file name is " << importDppFileName <<endl ;
			}
			else {
				//cout << "failed openining dpp file"<<endl ;
			}	
		}
		if (inputDpp.good()) {
		cout << " I passed opening the file in the while loop"<< endl ;
		}
 		periodCount+= 1 ;// abs(floor((curTime-InitTimeStage)/exchPeriod)) ;
		for (int i=0; i<resol ; i++) {
			inputDpp >> dppDist >> dppLevelTmp ;
			//cout<<"zeroth dpp is"<<dppDist<<dppLevel<< endl ; 
			dppDistV.push_back(dppDist) ; 
			dppLevelV.push_back(dppLevelTmp) ;  
		}	
		cout <<"first dpp value is"<< dppLevelV.at(0)<< endl ; 	
}

//comment from Sam Britton
//This function should be changed to use be more safe. 
//If dppLevelV is empty, then it causes a memory error since the length of the vector is not checked. 
void Signal::processSignalInfoTissueLevel() {
	

	vector<double> dppLevels_Cell ;
	dppLevels_Cell.clear() ;


 	double Center_X=minX+0.5*(maxX-minX);
 	double Center_Y=minY+0.5*(maxY-minY);

  		for (int k=0; k<numActiveCells; k++){
     			double DistXCell=abs(cellCenterX[k]-Center_X); 
     			int StoredI=-1 ;  //We will figure out that there is an error if stays -1  
  
        	for (int i=0 ; i<resol ; i++){
        		if (DistXCell<dppDistV[i] || DistXCell<minResol) {

        			StoredI=i ;
           			break;  
        		}
				if (i==resol-1) {
        			StoredI=i ;
				}
       		}  
       		dppLevels_Cell.push_back(dppLevelV[StoredI]);
       	}

       	for (int k=numActiveCells; k<maxCellCount ; k++){
       		dppLevels_Cell.push_back(0.0) ;   //these cells are not active
       	}


		for (int k=0 ;  k<numActiveCells; k++) {
			double distYAbs=abs (cellCenterY[k]-Center_Y); 
          		
			double dummy = (static_cast<double>(rand()) / RAND_MAX);
			double ranNum = NormalCDFInverse(dummy);	

			dppLevel[k]=dppLevels_Cell[k]; //+
					  //dppLevels_Cell[k]*(0.1*sin(0.2*3.141592*distYAbs)+0.12*ranNum); 
		}	
}

void Signal::processSignalInfoCellLevel() {


	vector<double> dppLevels_Cell ;
	dppLevels_Cell.clear() ;

 	double Center_Y=minY+0.5*(maxY-minY);


  		for (int k=0; k<numActiveCells; k++){
        	dppLevels_Cell.push_back(dppLevelV[k]);
       	}

       	for (int k=numActiveCells; k<maxCellCount ; k++){
       		dppLevels_Cell.push_back(0.0) ;   //these cells are not active
       	}


		for (int k=0 ;  k<numActiveCells; k++) {
			double distYAbs=abs (cellCenterY[k]-Center_Y); 
          		
			double dummy = (static_cast<double>(rand()) / RAND_MAX);
			double ranNum = NormalCDFInverse(dummy);	

			dppLevel[k]=dppLevels_Cell[k] ;  //+ dppLevels_Cell[k]*(0.1*sin(0.2*3.141592*distYAbs)+0.12*ranNum); 
		}	
}





double NormalCDFInverse(double p) {
	
	if (p < 0.5) {
		return -RationalApproximation( sqrt(-2.0*log(p)));
	}
	else {
		return RationalApproximation( sqrt(-2.0*log(1-p)));
	}
}

double RationalApproximation(double t) {

	double c[] = {2.515517, 0.802853, 0.010328};
	double d[] = {1.432788, 0.189269, 0.001308};
	return (t - ((c[2]*t + c[1])*t + c[0]) / (((d[2]*t + d[1])*t + d[0])*t + 1.0));

}


