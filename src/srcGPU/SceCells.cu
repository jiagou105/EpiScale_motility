#include "SceCells.h"
#include <cmath>

double epsilon = 1.0e-12;

__constant__ double membrEquLen;
__constant__ double membrStiff;
__constant__ double membrStiff_Mitotic; //Ali June 30
__constant__ double pI;
__constant__ double minLength;
__constant__ double minDivisor;
__constant__ uint maxAllNodePerCell;
__constant__ uint maxMembrPerCell;
__constant__ uint maxIntnlPerCell;
__constant__ uint maxIntnlPerFollower;
__constant__ double bendCoeff;
__constant__ double bendCoeff_Mitotic;//AAMIRI
__constant__ double bendCoeffLeader;

__constant__ double sceIB_M[5];
__constant__ double sceIBDiv_M[5];
__constant__ double sceII_M[5];
__constant__ double sceIIDiv_M[5];
__constant__ double grthPrgrCriEnd_M;
__constant__ double F_Ext_Incline_M2 ;  //Ali
__constant__ double maxAdhBondLen_M;
__constant__ double minAdhBondLen_M;
__constant__ double bondStiff_M;
__constant__ double bondStiff_Mitotic;
__constant__ double growthPrgrCriVal_M;

//Ali &  Abu June 30th
__device__
double calMembrForce_Mitotic(double& length, double& progress, double mitoticCri) {
	if (progress <= mitoticCri) {
		return (length - membrEquLen) * membrStiff;
		} else {
 		return (length - membrEquLen) *(membrStiff+ (membrStiff_Mitotic-membrStiff)* (progress-mitoticCri)/(1.0-mitoticCri));
 
         }

}
//
//Ali
__device__
double calExtForce(double& curTime) {
		return curTime * F_Ext_Incline_M2;
}


__device__
double obtainRandAngle(uint& cellRank, uint& seed) {
	thrust::default_random_engine rng(seed+cellRank);
	// discard n numbers to avoid correlation
	rng.discard(cellRank+seed);
	thrust::uniform_real_distribution<double> u0TwoPi(0, 2.0 * pI);
	double randomAngle = u0TwoPi(rng);
	return randomAngle;
}

__device__ uint obtainNewIntnlNodeIndex(uint& cellRank, uint& curActiveCount) {
	return (cellRank * maxAllNodePerCell + maxMembrPerCell + curActiveCount);
}

//AAMIRI
__device__ uint obtainLastIntnlNodeIndex(uint& cellRank, uint& curActiveCount) {
	return (cellRank * maxAllNodePerCell + maxMembrPerCell + curActiveCount );
}

//AAMIRI
__device__ uint obtainMembEndNode(uint& cellRank, uint& activeMembrNodeThis) {
	return (cellRank * maxAllNodePerCell + activeMembrNodeThis - 1 );
}


__device__
bool isAllIntnlFilled(uint& currentIntnlCount) {
	if (currentIntnlCount < maxIntnlPerFollower) { // change here 
		return false;
	} else {
		return true;
	}
}

//AAMIRI

__device__
int obtainRemovingMembrNodeID(uint &cellRank, uint& activeMembrNodes, uint& seed) {
	thrust::default_random_engine rng(seed+cellRank);
	// discard n numbers to avoid correlation
	rng.discard(activeMembrNodes+cellRank);
	thrust::uniform_int_distribution<double> dist(0, activeMembrNodes-1);
	int randomNode = dist(rng);
	return (cellRank * maxAllNodePerCell + randomNode);
}


//AAMIRI
__device__
bool isAllIntnlEmptied(uint& currentIntnlCount) {
	if (currentIntnlCount > 0) {
		return false;
	} else {
		return true;
	}
}


//AAMIRI
__device__
bool isAllMembrEmptied(uint& currentMembrCount) {
	if (currentMembrCount > 0) {
		return false;
	} else {
		return true;
	}
}

__device__
bool longEnough(double& length) {
	if (length > minLength) {
		return true;
	} else {
		return false;
	}
}

__device__
double compDist2D(double &xPos, double &yPos, double &xPos2, double &yPos2) {
	return sqrt(
			(xPos - xPos2) * (xPos - xPos2) + (yPos - yPos2) * (yPos - yPos2));
}

__device__
bool isInsideCell(double xPos, double yPos, uint intnlIndxMemBegin, uint activeMembrNodeCounts, double* nodeXAddr, double* nodeYAddr)
{
   double angle=0;
   double p1x, p1y, p2x, p2y;
   for (uint i=intnlIndxMemBegin;i<intnlIndxMemBegin+activeMembrNodeCounts;i++) {

      p1x = nodeXAddr[i] - xPos;
      p1y = nodeYAddr[i] - yPos;
	  if (i<intnlIndxMemBegin+activeMembrNodeCounts-1)
	  {
      	p2x = nodeXAddr[i+1] - xPos;
      	p2y = nodeYAddr[i+1] - yPos;
	  } else {
		p2x = nodeXAddr[0] - xPos;
      	p2y = nodeYAddr[0] - yPos;
	  }
      angle += Angle2D(p1x,p1y,p2x,p2y);
   }

   if (fabs(angle) < M_PI)
      return(false);
   else
      return(true);
}


__device__
double Angle2D(double x1, double y1, double x2, double y2)
{
   double dtheta,theta1,theta2;

   theta1 = atan2(y1,x1);
   theta2 = atan2(y2,x2);
   dtheta = theta2 - theta1;
   while (dtheta > M_PI)
      dtheta -= M_PI*2;
   while (dtheta < -M_PI)
      dtheta += M_PI*2;

   return(dtheta);
}

// compute menger curvature 
__device__
double computeCurvature(double& xpos1, double& ypos1, double& xpos2, double& ypos2, double& xpos3, double& ypos3){
	double curvature1;
	double areaA;
	double dist12, dist13, dist23;
	dist12 = compDist2D(xpos1, ypos1, xpos2, ypos2);
	dist13 = compDist2D(xpos1, ypos1, xpos3, ypos3);
	dist23 = compDist2D(xpos2, ypos2, xpos3, ypos3);
	areaA = (xpos2-xpos1)*(ypos3-ypos1) - (ypos2-ypos1)*(xpos3-xpos1);
	curvature1 = 4*areaA/(dist12*dist13*dist23);
}

/*
__device__
void handleAdhesionForceCell_M(int& adhereIndex, double& xPos, double& yPos,
		double& curAdherePosX, double& curAdherePosY, double& xRes,
		double& yRes, double& alpha, uint& curActLevel) {
	double curLen = computeDistCell2D(xPos, yPos, curAdherePosX, curAdherePosY);
	if (curLen > maxAdhBondLen_M) {
		adhereIndex = -1;
		return;
	} else {
		if (curLen > minAdhBondLen_M) {
			double forceValue = (curLen - minAdhBondLen_M) * (bondStiff_M * alpha + bondStiff_Mitotic * (1.0-alpha) );
			// if (curActLevel>0){forceValue = forceValue*1;}
			xRes = xRes + forceValue * (curAdherePosX - xPos) / curLen;
			yRes = yRes + forceValue * (curAdherePosY - yPos) / curLen;
		}
	}
}
*/



__device__
double getMitoticAdhCoefCell(double& growProg, double& growProgNeigh){
    double alpha = 1.0;


    if (growProg > growthPrgrCriVal_M && growProgNeigh > growthPrgrCriVal_M){
            alpha = 1.0 - ( 0.5*(growProg+growProgNeigh)-growthPrgrCriVal_M )/(1.0 - growthPrgrCriVal_M);
        //  adhSkipped = true;
        }
    else if (growProg > growthPrgrCriVal_M){
            alpha = 1.0 - (growProg-growthPrgrCriVal_M)/(1.0 - growthPrgrCriVal_M);
        //  adhSkipped = true;
        }
    else if (growProgNeigh > growthPrgrCriVal_M){
            alpha = 1.0 - (growProgNeigh-growthPrgrCriVal_M)/(1.0 - growthPrgrCriVal_M);
        //  adhSkipped = true;
        }


    return alpha;
}


__device__
double computeDistCell2D(double &xPos, double &yPos, double &xPos2, double &yPos2) {
	return sqrt(
			(xPos - xPos2) * (xPos - xPos2) + (yPos - yPos2) * (yPos - yPos2));
}


__device__
bool isValidFilopDirection(uint& intnlIndxMemBegin, uint _maxMemNodePerCell, bool* _isActiveAddr, double* _locXAddr, double* _locYAddr,
	double& cell_CenterX, double& cell_CenterY, double& randomAngleRd, int* _nodeAdhereIndexAddr, uint& activeMembrCount, uint _maxNodePerCell, double* _myosinLevelAddr){
	uint membrNodeIndex;
	double membrCenterAngle = 0;
	double membrNodeX;
	double membrNodeY;
	double membrCenterVecX=0;
	double membrCenterVecY=0;
	double membrCenterVecLen;
	double cosMCandRdA;
	double endMemActiveIndex;
	double membrLeftNodeX;
	double membrLeftNodeY;
	double largestCos1 = 0.02;
	double largestCos2 = 0.01;
	double dotP;
	double dotPL;
	uint tempLeftIndex;
	uint tempRightIndex;
	int cellIndex1, cellIndex11;
	int cellIndex2, cellIndex21;
	uint tempindex1 = 0;
	uint tempindex2 = 0;
	for (membrNodeIndex=intnlIndxMemBegin; membrNodeIndex<intnlIndxMemBegin + activeMembrCount; membrNodeIndex++){
		if (_isActiveAddr[membrNodeIndex]){ // does this represent an arbitrary node ??? yes 
			membrNodeX = _locXAddr[membrNodeIndex];
			membrNodeY = _locYAddr[membrNodeIndex];
			membrCenterVecX = membrNodeX - cell_CenterX;
			membrCenterVecY = membrNodeY - cell_CenterY;
			membrCenterVecLen = sqrt(membrCenterVecX*membrCenterVecX + membrCenterVecY*membrCenterVecY); // add a if to avoid small length
			cosMCandRdA =  (membrCenterVecX*cos(randomAngleRd)+membrCenterVecY*sin(randomAngleRd))/membrCenterVecLen;
			if (cosMCandRdA>largestCos1){ // find the closest membrane node?
				// smallAngle2=smallAngle1;
				largestCos1=cosMCandRdA;
				// tempindex2 = tempindex1;
				tempindex1 = membrNodeIndex;
				//} else if (cosMCandRdA>smallAngle2){ // add a condition that they are not on the same side 
				//	smallAngle2 = cosMCandRdA;
				//	tempindex2 = membrNodeIndex;
				}
		} // else { endMemActiveIndex = membrNodeIndex-1; break;} // endActiveIndex is the index of last active membrane node
	}
	// if (membrNodeIndex == intnlIndxMemBegin + _maxMemNodePerCell-1) {
	//	endMemActiveIndex=membrNodeIndex;
	//	}
	/*
	endMemActiveIndex = intnlIndxMemBegin + activeMembrCount - 1;
	if (tempindex1 == intnlIndxMemBegin) { 
		tempLeftIndex = endMemActiveIndex; // left and right is defined by assuming indexes are ordered clockwisely ??
	} else {
		tempLeftIndex = tempindex1-1;
	}
	if (tempindex1 == endMemActiveIndex){// this condition is still possible // double check this
		tempRightIndex = intnlIndxMemBegin;
	} else {
		tempRightIndex = tempindex1+1;
	}
	membrNodeX = _locXAddr[tempindex1];
	membrNodeY = _locYAddr[tempindex1];
	*/
	tempindex2 = tempindex1;
	// membrLeftNodeX = _locXAddr[tempLeftIndex];
	// membrLeftNodeY = _locYAddr[tempLeftIndex];
	// dotP = (membrNodeX-cell_CenterX)*(-sin(randomAngleRd)) + (membrNodeY-cell_CenterY)*cos(randomAngleRd);
	// dotPL = (membrLeftNodeX-cell_CenterX)*(-sin(randomAngleRd)) + (membrLeftNodeY-cell_CenterY)*cos(randomAngleRd);
	// if (dotPL*dotP>0){tempindex2 = tempRightIndex;}
	// else {tempindex2 = tempLeftIndex;} // idealy, tempindex1 and tempindex2 are two nodes on two sides of the filopodia
	// _myosinLevelAddr[tempindex1] = _myosinLevelAddr[tempindex1] + 5.0;
	if (_nodeAdhereIndexAddr[tempindex1]==-1 || _nodeAdhereIndexAddr[tempindex2]==-1){
		return true;
	} else {
		cellIndex11 = _nodeAdhereIndexAddr[tempindex1];
		// cellIndex1 = cellIndex11 /_maxNodePerCell; 
		cellIndex21 = _nodeAdhereIndexAddr[tempindex2];
		// cellIndex2 = cellIndex21 /_maxNodePerCell; 
		// if (cellIndex1 != cellIndex2){
		if (abs(cellIndex11-cellIndex21)!=1){
			return false;
		} else {
			return false;
		}
	}
}


__device__
bool isTangFilopDirection(uint& intnlIndxMemBegin, bool* _isActiveAddr, double* _locXAddr, double* _locYAddr,
	double& cell_CenterX, double& cell_CenterY, double& randomAngleRd, int* _nodeAdhereIndexAddr, 
	uint& activeMembrCount, uint _maxNodePerCell, int* _cellTypeAddr, uint leaderRank, double* _myosinLevelAddr, int& filopDirection){
	//1. determine whether this cell is attached to the leader cell. If yes, find the two nodes at the boundary of the contact line; If no, return prefered direction.
	//2. If yes, add a small angle to the direction connecting the follower center to the two boundary nodes; If this follower cell is adhered to another follower cell, 
	// choose a preferred direction 
	//3. return the direction
	// int membrNodeIndex;
	uint lowestAdhMemInd = 0;
	uint highestAdhMemInd = 0;
	int count = 0;
	int adhNodeIndex,adhCellIndex;
	int adhCellType;
	int intnlIndxMemBeginInt = (int) intnlIndxMemBegin;
	int activeMembrCountInt = (int) activeMembrCount;
	int _maxNodePerCellInt = (int) _maxNodePerCell;

	double highNodeX, highNodeY, highVecX, highVecY, lenHighVec;
	double lowNodeX, lowNodeY, lowVecX, lowVecY, lenLowVec;
	double dotPHigh, dotPLow;
	double bisecVecX, bisecVecY, dotP;
	double crossP;
	// check if there is any adhered nodes in both [0 ,5] and [end-5, end] ranges
	// if yes, means the total list of attached nodes are crossing 0, case 0, else case 1
	// for case 0, starting from 0, find the node with largest index n1 such that [0 n1] are all adhereed
	// then find the next first node that is adhered to leader
	// for case 1, just use the following code
	int maxGapInt = 3;
	int adhIndex0, tempNodeIndex, notAdhCounter;
	int clockwiseMostNodeIndex, counterClockwiseMostNodeIndex;
	int iterCount = 0;

	adhIndex0 = (int) intnlIndxMemBeginInt;
	if (_nodeAdhereIndexAddr[adhIndex0] == -1) {
		adhCellType = 0;
	} else {
		adhCellIndex = _nodeAdhereIndexAddr[adhIndex0]/_maxNodePerCellInt;
		adhCellType = _cellTypeAddr[adhCellIndex];
	}
	// 1st and 2nd conditions are duplicated
	while (_isActiveAddr[adhIndex0] && adhIndex0<intnlIndxMemBeginInt + activeMembrCountInt && adhCellType != 1){
		adhIndex0 += 1;
		adhCellIndex = _nodeAdhereIndexAddr[adhIndex0]/_maxNodePerCellInt;
		adhCellType = _cellTypeAddr[adhCellIndex];
	}
	
	if (adhIndex0<intnlIndxMemBeginInt + activeMembrCountInt){ 
		count = 1; // at least one node is adhered to the leader
		tempNodeIndex = adhIndex0;
		notAdhCounter = 0;
		while (notAdhCounter<maxGapInt && iterCount<activeMembrCountInt){
			iterCount +=1;
			tempNodeIndex = tempNodeIndex - 1; // why decreasing index???
			if (tempNodeIndex == intnlIndxMemBeginInt-1){tempNodeIndex = intnlIndxMemBeginInt + activeMembrCountInt-1;}
			if (_nodeAdhereIndexAddr[tempNodeIndex] == -1) {
				adhCellType = 0;
			} else {
				adhCellIndex = _nodeAdhereIndexAddr[tempNodeIndex]/_maxNodePerCellInt;
				adhCellType = _cellTypeAddr[adhCellIndex];
			}
			if (adhCellType == 1){
				notAdhCounter = 0;
			} else {
				notAdhCounter += 1;
			}
		}
		tempNodeIndex = tempNodeIndex + maxGapInt;
		if (tempNodeIndex>intnlIndxMemBeginInt + activeMembrCountInt-1) {tempNodeIndex = tempNodeIndex - activeMembrCountInt;}
		clockwiseMostNodeIndex = tempNodeIndex;

		iterCount = 0;
		tempNodeIndex = adhIndex0;
		notAdhCounter = 0;
		while (notAdhCounter<maxGapInt && iterCount<activeMembrCountInt){
			iterCount += 1;
			tempNodeIndex += 1;
			if (tempNodeIndex == intnlIndxMemBeginInt + activeMembrCountInt){tempNodeIndex=intnlIndxMemBeginInt;}
			if (_nodeAdhereIndexAddr[tempNodeIndex] == -1) {
				adhCellType = 0;
			} else {
				adhCellIndex = _nodeAdhereIndexAddr[tempNodeIndex]/_maxNodePerCellInt;
				adhCellType = _cellTypeAddr[adhCellIndex];
			}
			if (adhCellType == 1){
				notAdhCounter = 0;
			} else {
				notAdhCounter += 1;
			}
		}
		tempNodeIndex = tempNodeIndex - maxGapInt;
		if (tempNodeIndex<intnlIndxMemBeginInt) {tempNodeIndex = tempNodeIndex+activeMembrCountInt;}
		counterClockwiseMostNodeIndex = tempNodeIndex;
	} else {
		count = 0;
	}

	if (count == 0){
		return true;
	} else{
		// _myosinLevelAddr[counterClockwiseMostNodeIndex] = 50;
		// _myosinLevelAddr[clockwiseMostNodeIndex] = 10;
		highNodeX = _locXAddr[counterClockwiseMostNodeIndex];
		highNodeY = _locYAddr[counterClockwiseMostNodeIndex];
		lowNodeX = _locXAddr[clockwiseMostNodeIndex];
		lowNodeY = _locYAddr[clockwiseMostNodeIndex];
		highVecX = highNodeX - cell_CenterX;
		highVecY = highNodeY - cell_CenterY;
		lowVecX = lowNodeX - cell_CenterX;
		lowVecY = lowNodeY - cell_CenterY;

		// dotPHigh = cos(randomAngleRd)*highVecX + sin(randomAngleRd)*highVecY;
		// dotPLow = cos(randomAngleRd)*lowVecX + sin(randomAngleRd)*lowVecY;
		lenLowVec = sqrt(lowVecX*lowVecX + lowVecY*lowVecY);
		lenHighVec = sqrt(highVecX*highVecX + highVecY*highVecY);
		lowVecX = lowVecX/lenLowVec;
		lowVecY = lowVecY/lenLowVec;
		highVecX = highVecX/lenHighVec;
		highVecY = highVecY/lenHighVec;
		bisecVecX = lowVecX + highVecX;
		bisecVecY = lowVecY + highVecY;
		dotP = cos(randomAngleRd)*bisecVecX + sin(randomAngleRd)*bisecVecY;
		crossP = bisecVecX*sin(randomAngleRd) - cos(randomAngleRd)*bisecVecY;
		if (dotP>0.1){ // dotPHigh>0 || dotPLow>0 // &&crossP>0
			if (filopDirection==0){ // firstfilopodia 
				if (crossP>0){filopDirection = 1;}
				else {filopDirection = -1;}
				return true;
			} else if (filopDirection==1 && crossP>0) {
				return true;
			} else if (filopDirection==-1 && crossP<0){
				return true;
			}
		} 
	}
	return false;
}


void SceCells::distributeBdryIsActiveInfo() {
	thrust::fill(nodes->getInfoVecs().nodeIsActive.begin(),
			nodes->getInfoVecs().nodeIsActive.begin()
					+ allocPara.startPosProfile, true);
}

void SceCells::distributeProfileIsActiveInfo() {
	thrust::fill(
			nodes->getInfoVecs().nodeIsActive.begin()
					+ allocPara.startPosProfile,
			nodes->getInfoVecs().nodeIsActive.begin()
					+ allocPara.startPosProfile
					+ nodes->getAllocPara().currentActiveProfileNodeCount,
			true);
}

void SceCells::distributeECMIsActiveInfo() {
	uint totalNodeCountForActiveECM = allocPara.currentActiveECM
			* allocPara.maxNodePerECM;
	thrust::counting_iterator<uint> countingBegin(0);
	thrust::counting_iterator<uint> countingEnd(totalNodeCountForActiveECM);
	thrust::fill(
			nodes->getInfoVecs().nodeIsActive.begin() + allocPara.startPosECM,
			nodes->getInfoVecs().nodeIsActive.begin()
					+ totalNodeCountForActiveECM + allocPara.startPosECM, true);
}

void SceCells::distributeCellIsActiveInfo() {
	totalNodeCountForActiveCells = allocPara.currentActiveCellCount
			* allocPara.maxNodeOfOneCell;
	thrust::counting_iterator<uint> countingBegin(0);
	thrust::counting_iterator<uint> countingEnd(totalNodeCountForActiveCells);

	thrust::transform(
			thrust::make_transform_iterator(countingBegin,
					ModuloFunctor(allocPara.maxNodeOfOneCell)),
			thrust::make_transform_iterator(countingEnd,
					ModuloFunctor(allocPara.maxNodeOfOneCell)),
			thrust::make_permutation_iterator(
					cellInfoVecs.activeNodeCountOfThisCell.begin(),
					make_transform_iterator(countingBegin,
							DivideFunctor(allocPara.maxNodeOfOneCell))),
			nodes->getInfoVecs().nodeIsActive.begin() + allocPara.startPosCells,
			thrust::less<uint>());
}

void SceCells::distributeCellGrowthProgress() {
	totalNodeCountForActiveCells = allocPara.currentActiveCellCount
			* allocPara.maxNodeOfOneCell;
	thrust::counting_iterator<uint> countingBegin(0);
	thrust::counting_iterator<uint> countingEnd(totalNodeCountForActiveCells);

	thrust::copy(
			thrust::make_permutation_iterator(
					cellInfoVecs.growthProgress.begin(),
					make_transform_iterator(countingBegin,
							DivideFunctor(allocPara.maxNodeOfOneCell))),
			thrust::make_permutation_iterator(
					cellInfoVecs.growthProgress.begin(),
					make_transform_iterator(countingEnd,
							DivideFunctor(allocPara.maxNodeOfOneCell))),
			nodes->getInfoVecs().nodeGrowPro.begin() + allocPara.startPosCells);
}

void MembrPara::initFromConfig() {
	membrEquLenCPU = globalConfigVars.getConfigValue("MembrEquLen").toDouble();
	membrStiffCPU = globalConfigVars.getConfigValue("MembrStiff").toDouble();
	membrStiff_Mitotic = globalConfigVars.getConfigValue("MembrStiff_Mitotic").toDouble();  //Ali June30
	membrGrowCoeff_Ori =
			globalConfigVars.getConfigValue("MembrGrowCoeff").toDouble();
	membrGrowLimit_Ori =
			globalConfigVars.getConfigValue("MembrGrowLimit").toDouble();
	membrGrowCoeff = membrGrowCoeff_Ori;
	membrGrowLimit = membrGrowLimit_Ori;
        //Ali
        F_Ext_Incline = 
			globalConfigVars.getConfigValue("FExtIncline").toDouble();
        //Ali
	membrBendCoeff =
			globalConfigVars.getConfigValue("MembrBenCoeff").toDouble();

	membrBendCoeff_Mitotic =
			globalConfigVars.getConfigValue("MembrBenCoeff_Mitotic").toDouble();

	membrBendCoeffLeader =
			globalConfigVars.getConfigValue("MembrBenCoeffLeader").toDouble();

	adjustLimit =
			globalConfigVars.getConfigValue("MembrAdjustLimit").toDouble();
	adjustCoeff =
			globalConfigVars.getConfigValue("MembrAdjustCoeff").toDouble();

	growthConst_N =
			globalConfigVars.getConfigValue("MembrGrowthConst").toDouble();
	initMembrCt_N =
			globalConfigVars.getConfigValue("InitMembrNodeCount").toInt();
	initIntnlCt_N =
			globalConfigVars.getConfigValue("InitCellNodeCount").toInt();
}


SceCells::SceCells() {
	//curTime = 0 + 55800.0;//AAMIRI // Ali I comment that our safely on 04/04/2017
        std ::cout << "I am in SceCells constructor with zero element "<<InitTimeStage<<std::endl ; 
}

void SceCells::growAtRandom(double d_t) {
	totalNodeCountForActiveCells = allocPara.currentActiveCellCount
			* allocPara.maxNodeOfOneCell;

	// randomly select growth direction and speed.
	randomizeGrowth();

	//std::cout << "after copy grow info" << std::endl;
	updateGrowthProgress();
	//std::cout << "after update growth progress" << std::endl;
	decideIsScheduleToGrow();
	//std::cout << "after decode os schedule to grow" << std::endl;
	computeCellTargetLength();
	//std::cout << "after compute cell target length" << std::endl;
	computeDistToCellCenter();
	//std::cout << "after compute dist to center" << std::endl;
	findMinAndMaxDistToCenter();
	//std::cout << "after find min and max dist" << std::endl;
	computeLenDiffExpCur();
	//std::cout << "after compute diff " << std::endl;
	stretchCellGivenLenDiff();
	//std::cout << "after apply stretch force" << std::endl;
	cellChemotaxis();
	//std::cout << "after apply cell chemotaxis" << std::endl;
	addPointIfScheduledToGrow();
	//std::cout << "after adding node" << std::endl;
}

/**
 * Use the growth magnitude and dt to update growthProgress.
 */
void SceCells::updateGrowthProgress() {
	thrust::transform(cellInfoVecs.growthSpeed.begin(),
			cellInfoVecs.growthSpeed.begin() + allocPara.currentActiveCellCount,
			cellInfoVecs.growthProgress.begin(),
			cellInfoVecs.growthProgress.begin(), SaxpyFunctorWithMaxOfOne(dt));
}

/**
 * Decide if the cells are going to add a node or not.
 * Use lastCheckPoint and growthProgress to decide whether add point or not
 */
void SceCells::decideIsScheduleToGrow() {
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.growthProgress.begin(),
							cellInfoVecs.lastCheckPoint.begin(),
							cellInfoVecs.cell_Type.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.growthProgress.begin(),
							cellInfoVecs.lastCheckPoint.begin(),
							cellInfoVecs.cell_Type.begin()))
					+ allocPara.currentActiveCellCount,
			cellInfoVecs.isScheduledToGrow.begin(),
			decideIsScheduleToGrow_MDevice(miscPara.growThreshold));
}

/**
 * Calculate target length of cell given the cell growth progress.
 * length is along the growth direction.
 */
void SceCells::computeCellTargetLength() {
	thrust::transform(cellInfoVecs.growthProgress.begin(),
			cellInfoVecs.growthProgress.begin()
					+ allocPara.currentActiveCellCount,
			cellInfoVecs.expectedLength.begin(),
			CompuTarLen(bioPara.cellInitLength, bioPara.cellFinalLength));
}

/**
 * Compute distance of each node to its corresponding cell center.
 * The distantce could be either positive or negative, depending on the pre-defined
 * growth direction.
 */
void SceCells::computeDistToCellCenter() {
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							make_permutation_iterator(
									cellInfoVecs.centerCoordX.begin(),
									make_transform_iterator(countingBegin,
											DivideFunctor(
													allocPara.maxNodeOfOneCell))),
							make_permutation_iterator(
									cellInfoVecs.centerCoordY.begin(),
									make_transform_iterator(countingBegin,
											DivideFunctor(
													allocPara.maxNodeOfOneCell))),
							make_permutation_iterator(
									cellInfoVecs.growthXDir.begin(),
									make_transform_iterator(countingBegin,
											DivideFunctor(
													allocPara.maxNodeOfOneCell))),
							make_permutation_iterator(
									cellInfoVecs.growthYDir.begin(),
									make_transform_iterator(countingBegin,
											DivideFunctor(
													allocPara.maxNodeOfOneCell))),
							nodes->getInfoVecs().nodeLocX.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeLocY.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeIsActive.begin()
									+ allocPara.startPosCells)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							make_permutation_iterator(
									cellInfoVecs.centerCoordX.begin(),
									make_transform_iterator(countingBegin,
											DivideFunctor(
													allocPara.maxNodeOfOneCell))),
							make_permutation_iterator(
									cellInfoVecs.centerCoordY.begin(),
									make_transform_iterator(countingBegin,
											DivideFunctor(
													allocPara.maxNodeOfOneCell))),
							make_permutation_iterator(
									cellInfoVecs.growthXDir.begin(),
									make_transform_iterator(countingBegin,
											DivideFunctor(
													allocPara.maxNodeOfOneCell))),
							make_permutation_iterator(
									cellInfoVecs.growthYDir.begin(),
									make_transform_iterator(countingBegin,
											DivideFunctor(
													allocPara.maxNodeOfOneCell))),
							nodes->getInfoVecs().nodeLocX.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeLocY.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeIsActive.begin()
									+ allocPara.startPosCells))
					+ totalNodeCountForActiveCells,
			cellNodeInfoVecs.distToCenterAlongGrowDir.begin(), CompuDist());
}

/**
 * For nodes of each cell, find the maximum and minimum distance to the center.
 * We will then calculate the current length of a cell along its growth direction
 * using max and min distance to the center.
 */
void SceCells::findMinAndMaxDistToCenter() {
	thrust::reduce_by_key(
			make_transform_iterator(countingBegin,
					DivideFunctor(allocPara.maxNodeOfOneCell)),
			make_transform_iterator(countingBegin,
					DivideFunctor(allocPara.maxNodeOfOneCell))
					+ totalNodeCountForActiveCells,
			cellNodeInfoVecs.distToCenterAlongGrowDir.begin(),
			cellInfoVecs.cellRanksTmpStorage.begin(),
			cellInfoVecs.smallestDistance.begin(), thrust::equal_to<uint>(),
			thrust::minimum<double>());

	// for nodes of each cell, find the maximum distance from the node to the corresponding

	// cell center along the pre-defined growth direction.
	thrust::reduce_by_key(
			make_transform_iterator(countingBegin,
					DivideFunctor(allocPara.maxNodeOfOneCell)),
			make_transform_iterator(countingBegin,
					DivideFunctor(allocPara.maxNodeOfOneCell))
					+ totalNodeCountForActiveCells,
			cellNodeInfoVecs.distToCenterAlongGrowDir.begin(),
			cellInfoVecs.cellRanksTmpStorage.begin(),
			cellInfoVecs.biggestDistance.begin(), thrust::equal_to<uint>(),
			thrust::maximum<double>());

}

/**
 * Compute the difference for cells between their expected length and current length.
 */
void SceCells::computeLenDiffExpCur() {
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.expectedLength.begin(),
							cellInfoVecs.smallestDistance.begin(),
							cellInfoVecs.biggestDistance.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.expectedLength.begin(),
							cellInfoVecs.smallestDistance.begin(),
							cellInfoVecs.biggestDistance.begin()))
					+ allocPara.currentActiveCellCount,
			cellInfoVecs.lengthDifference.begin(), CompuDiff());
}

/**
 * Use the difference that just computed and growthXDir&growthYDir
 * to apply stretching force (velocity) on nodes of all cells
 */
void SceCells::stretchCellGivenLenDiff() {
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							cellNodeInfoVecs.distToCenterAlongGrowDir.begin(),
							make_permutation_iterator(
									cellInfoVecs.lengthDifference.begin(),
									make_transform_iterator(countingBegin,
											DivideFunctor(
													allocPara.maxNodeOfOneCell))),
							make_permutation_iterator(
									cellInfoVecs.growthXDir.begin(),
									make_transform_iterator(countingBegin,
											DivideFunctor(
													allocPara.maxNodeOfOneCell))),
							make_permutation_iterator(
									cellInfoVecs.growthYDir.begin(),
									make_transform_iterator(countingBegin,
											DivideFunctor(
													allocPara.maxNodeOfOneCell))),
							nodes->getInfoVecs().nodeVelX.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeVelY.begin()
									+ allocPara.startPosCells)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							cellNodeInfoVecs.distToCenterAlongGrowDir.begin(),
							make_permutation_iterator(
									cellInfoVecs.lengthDifference.begin(),
									make_transform_iterator(countingBegin,
											DivideFunctor(
													allocPara.maxNodeOfOneCell))),
							make_permutation_iterator(
									cellInfoVecs.growthXDir.begin(),
									make_transform_iterator(countingBegin,
											DivideFunctor(
													allocPara.maxNodeOfOneCell))),
							make_permutation_iterator(
									cellInfoVecs.growthYDir.begin(),
									make_transform_iterator(countingBegin,
											DivideFunctor(
													allocPara.maxNodeOfOneCell))),
							nodes->getInfoVecs().nodeVelX.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeVelY.begin()
									+ allocPara.startPosCells))
					+ totalNodeCountForActiveCells,
			thrust::make_zip_iterator(
					thrust::make_tuple(
							nodes->getInfoVecs().nodeVelX.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeVelY.begin()
									+ allocPara.startPosCells)),
			ApplyStretchForce(bioPara.elongationCoefficient));
}
/**
 * This is just an attempt. Cells move according to chemicals.
 */
void SceCells::cellChemotaxis() {
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							make_permutation_iterator(
									cellInfoVecs.growthSpeed.begin(),
									make_transform_iterator(countingBegin,
											DivideFunctor(
													allocPara.maxNodeOfOneCell))),
							make_permutation_iterator(
									cellInfoVecs.growthXDir.begin(),
									make_transform_iterator(countingBegin,
											DivideFunctor(
													allocPara.maxNodeOfOneCell))),
							make_permutation_iterator(
									cellInfoVecs.growthYDir.begin(),
									make_transform_iterator(countingBegin,
											DivideFunctor(
													allocPara.maxNodeOfOneCell))),
							nodes->getInfoVecs().nodeVelX.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeVelY.begin()
									+ allocPara.startPosCells)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							make_permutation_iterator(
									cellInfoVecs.growthSpeed.begin(),
									make_transform_iterator(countingBegin,
											DivideFunctor(
													allocPara.maxNodeOfOneCell))),
							make_permutation_iterator(
									cellInfoVecs.growthXDir.begin(),
									make_transform_iterator(countingBegin,
											DivideFunctor(
													allocPara.maxNodeOfOneCell))),
							make_permutation_iterator(
									cellInfoVecs.growthYDir.begin(),
									make_transform_iterator(countingBegin,
											DivideFunctor(
													allocPara.maxNodeOfOneCell))),
							nodes->getInfoVecs().nodeVelX.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeVelY.begin()
									+ allocPara.startPosCells))
					+ totalNodeCountForActiveCells,
			thrust::make_zip_iterator(
					thrust::make_tuple(
							nodes->getInfoVecs().nodeVelX.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeVelY.begin()
									+ allocPara.startPosCells)),
			ApplyChemoVel(bioPara.chemoCoefficient));
}
/**
 * Adjust the velocities of nodes.
 * For example, velocity of boundary nodes must be zero.
 */
void SceCells::adjustNodeVel() {
	thrust::counting_iterator<uint> countingIterBegin(0);
	thrust::counting_iterator<uint> countingIterEnd(
			totalNodeCountForActiveCells + allocPara.startPosCells);

	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin(),
							nodes->getInfoVecs().nodeIsActive.begin(),
							nodes->getInfoVecs().nodeCellType.begin(),
							countingIterBegin)),
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin(),
							nodes->getInfoVecs().nodeIsActive.begin(),
							nodes->getInfoVecs().nodeCellType.begin(),
							countingIterBegin)) + totalNodeCountForActiveCells
					+ allocPara.startPosCells,
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin())),
			VelocityModifier(allocPara.startPosProfile,
					allocPara.currentActiveProfileNodeCount));
}
/**
 * Move nodes according to the velocity we just adjusted.
 */
void SceCells::moveNodes() {
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin()))
					+ totalNodeCountForActiveCells + allocPara.startPosCells,
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeLocX.begin(),
							nodes->getInfoVecs().nodeLocY.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeLocX.begin(),
							nodes->getInfoVecs().nodeLocY.begin())),
			SaxpyFunctorDim2(dt));
}
/**
 * Add a point to a cell if it is scheduled to grow.
 * This step does not guarantee success ; If adding new point failed, it will not change
 * isScheduleToGrow and activeNodeCount;
 */
void SceCells::addPointIfScheduledToGrow() {

	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.isScheduledToGrow.begin(),
							cellInfoVecs.activeNodeCountOfThisCell.begin(),
							cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin(), countingBegin,
							cellInfoVecs.lastCheckPoint.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.isScheduledToGrow.begin(),
							cellInfoVecs.activeNodeCountOfThisCell.begin(),
							cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin(), countingBegin,
							cellInfoVecs.lastCheckPoint.begin()))
					+ allocPara.currentActiveCellCount,
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.isScheduledToGrow.begin(),
							cellInfoVecs.activeNodeCountOfThisCell.begin(),
							cellInfoVecs.lastCheckPoint.begin())),
			AddPtOp(allocPara.maxNodeOfOneCell, miscPara.addNodeDistance,
					miscPara.minDistanceToOtherNode,
					growthAuxData.nodeIsActiveAddress,
					growthAuxData.nodeXPosAddress,
					growthAuxData.nodeYPosAddress, time(NULL),
					miscPara.growThreshold));
}

//Ali commented this constructor in 04/04/2017
SceCells::SceCells(SceNodes* nodesInput,
		std::vector<uint>& numOfInitActiveNodesOfCells,
		std::vector<SceNodeType>& cellTypes) :
		countingBegin(0), initIntnlNodeCount(
				nodesInput->getAllocPara().maxNodeOfOneCell / 2), initGrowthProgress(
				0.0) {
	curTime = 0.0 + 55800.0;//AAMIRI

        std ::cout << "I am in SceCells constructor with polymorphism shape "<<InitTimeStage<<std::endl ; 
	initialize(nodesInput);

	copyInitActiveNodeCount(numOfInitActiveNodesOfCells);

	thrust::device_vector<SceNodeType> cellTypesToPass = cellTypes;
	setCellTypes(cellTypesToPass);

	distributeIsActiveInfo();
}


SceCells::SceCells(SceNodes* nodesInput,
		std::vector<uint>& initActiveMembrNodeCounts,
		std::vector<uint>& initActiveIntnlNodeCounts,
		std::vector<double> &initGrowProgVec, 
		std::vector<double> &initCellRadii, double InitTimeStage) {
//	curTime = 0.0 + 55800.0;//AAMIRIi
        curTime=InitTimeStage ; 
        std ::cout << "I am in SceCells constructor with number of inputs "<<InitTimeStage<<std::endl ; 
	lastTimeExchange=0  ;
	firstTimeReadDpp=true ; 
	//currentActiveCellCountOld=1 ; // small number 
	tmpDebug = false;
	aniDebug = false;
	membrPara.initFromConfig();
	shrinkRatio = globalConfigVars.getConfigValue("ShrinkRatio").toDouble();
	centerShiftRatio =
			globalConfigVars.getConfigValue("CenterShiftRatio").toDouble();

	memNewSpacing = globalConfigVars.getConfigValue("MembrLenDiv").toDouble();
	
	initialize_M(nodesInput,initCellRadii);

    cout<< "size of dpp in constructor  is "<< cellInfoVecs.cell_Dpp.size() << endl ;          
	copyToGPUConstMem();
	copyInitActiveNodeCount_M(initActiveMembrNodeCounts,
			initActiveIntnlNodeCounts,initGrowProgVec,initCellRadii);
	
}

void SceCells::initCellInfoVecs() {
	cellInfoVecs.growthProgress.resize(allocPara.maxCellCount, 0.0);
	cellInfoVecs.expectedLength.resize(allocPara.maxCellCount,
			bioPara.cellInitLength);
	cellInfoVecs.lengthDifference.resize(allocPara.maxCellCount, 0.0);
	cellInfoVecs.smallestDistance.resize(allocPara.maxCellCount);
	cellInfoVecs.biggestDistance.resize(allocPara.maxCellCount);
	cellInfoVecs.activeNodeCountOfThisCell.resize(allocPara.maxCellCount);
	cellInfoVecs.lastCheckPoint.resize(allocPara.maxCellCount, 0.0);
	cellInfoVecs.isDividing.resize(allocPara.maxCellCount);
	cellInfoVecs.cellTypes.resize(allocPara.maxCellCount, MX);
	cellInfoVecs.isScheduledToGrow.resize(allocPara.maxCellCount, false);
	cellInfoVecs.centerCoordX.resize(allocPara.maxCellCount);
	cellInfoVecs.centerCoordY.resize(allocPara.maxCellCount);
	cellInfoVecs.centerCoordZ.resize(allocPara.maxCellCount);
	cellInfoVecs.cellRanksTmpStorage.resize(allocPara.maxCellCount);
	cellInfoVecs.growthSpeed.resize(allocPara.maxCellCount, 0.0);
	cellInfoVecs.growthXDir.resize(allocPara.maxCellCount);
	cellInfoVecs.growthYDir.resize(allocPara.maxCellCount);
	cellInfoVecs.isRandGrowInited.resize(allocPara.maxCellCount, false);
}

void SceCells::initCellInfoVecs_M() {
	//std::cout << "max cell count = " << allocPara_m.maxCellCount << std::endl;
	cellInfoVecs.Cell_Damp.resize(allocPara_m.maxCellCount, 36.0);   //Ali
	cellInfoVecs.cell_Dpp.resize(allocPara_m.maxCellCount, 0.0);   //Ali
	cellInfoVecs.cell_DppOld.resize(allocPara_m.maxCellCount, 0.0);   //Ali
	cellInfoVecs.cell_Tkv.resize(allocPara_m.maxCellCount, 0.0);   //Alireza
	cellInfoVecs.cell_DppTkv.resize(allocPara_m.maxCellCount, 0.0);   //Alireza
	cellInfoVecs.cell_pMad.resize(allocPara_m.maxCellCount, 0.0);   //Alireza
	cellInfoVecs.cell_pMadOld.resize(allocPara_m.maxCellCount, 0.0);   //Alireza
	cellInfoVecs.cell_Type.resize(allocPara_m.maxCellCount, 0);   //Alireza
	
        //cout<< "size of dpp in init is "<< cellInfoVecs.cell_Dpp.size() << endl ;          
	cellInfoVecs.growthProgress.resize(allocPara_m.maxCellCount, 0.0); //A&A
    cellInfoVecs.growthProgressOld.resize(allocPara_m.maxCellCount, 0.0);//Ali
	cellInfoVecs.Cell_Time.resize(allocPara_m.maxCellCount, 0.0); //Ali
	cellInfoVecs.expectedLength.resize(allocPara_m.maxCellCount,
			bioPara.cellInitLength);
	cellInfoVecs.lengthDifference.resize(allocPara_m.maxCellCount, 0.0);
	cellInfoVecs.smallestDistance.resize(allocPara_m.maxCellCount);
	cellInfoVecs.biggestDistance.resize(allocPara_m.maxCellCount);
	cellInfoVecs.activeMembrNodeCounts.resize(allocPara_m.maxCellCount);
	cellInfoVecs.activeIntnlNodeCounts.resize(allocPara_m.maxCellCount);
	cellInfoVecs.lastCheckPoint.resize(allocPara_m.maxCellCount, 0.0);
	cellInfoVecs.isDividing.resize(allocPara_m.maxCellCount);
	cellInfoVecs.isEnteringMitotic.resize(allocPara_m.maxCellCount, false);  //A&A
	//cellInfoVecs.isRemoving.resize(allocPara.maxCellCount);//AAMIRI
	cellInfoVecs.isScheduledToGrow.resize(allocPara_m.maxCellCount, false);
	cellInfoVecs.isScheduledToShrink.resize(allocPara_m.maxCellCount, false);//AAMIRI
	cellInfoVecs.isCellActive.resize(allocPara_m.maxCellCount, false);//AAMIRI
	cellInfoVecs.centerCoordX.resize(allocPara_m.maxCellCount);
	cellInfoVecs.centerCoordY.resize(allocPara_m.maxCellCount);
	cellInfoVecs.centerCoordZ.resize(allocPara_m.maxCellCount);

    cellInfoVecs.HertwigXdir.resize(allocPara_m.maxCellCount,0.0); //A&A 
	cellInfoVecs.HertwigYdir.resize(allocPara_m.maxCellCount,0.0); //A&A 

	cellInfoVecs.cellRanksTmpStorage.resize(allocPara_m.maxCellCount);
	cellInfoVecs.growthSpeed.resize(allocPara_m.maxCellCount, 0.0);
	cellInfoVecs.growthXDir.resize(allocPara_m.maxCellCount);
	cellInfoVecs.growthYDir.resize(allocPara_m.maxCellCount);
	cellInfoVecs.isRandGrowInited.resize(allocPara_m.maxCellCount, false);
	cellInfoVecs.isMembrAddingNode.resize(allocPara_m.maxCellCount, false);
	cellInfoVecs.maxTenIndxVec.resize(allocPara_m.maxCellCount);
	cellInfoVecs.maxTenRiVec.resize(allocPara_m.maxCellCount);
	cellInfoVecs.maxDistToRiVec.resize(allocPara_m.maxCellCount); //Ali
	cellInfoVecs.maxTenRiMidXVec.resize(allocPara_m.maxCellCount);
	cellInfoVecs.maxTenRiMidYVec.resize(allocPara_m.maxCellCount);
	cellInfoVecs.aveTension.resize(allocPara_m.maxCellCount);
	cellInfoVecs.membrGrowProgress.resize(allocPara_m.maxCellCount, 0.0);
	cellInfoVecs.membrGrowSpeed.resize(allocPara_m.maxCellCount, 0.0);
	cellInfoVecs.cellAreaVec.resize(allocPara_m.maxCellCount, 0.0);
	cellInfoVecs.cellAreaInit.resize(allocPara_m.maxCellCount, 0.0);
    cellInfoVecs.cellPerimVec.resize(allocPara_m.maxCellCount, 0.0);//AAMIRI

	
	cellInfoVecs.cellFilopX.resize(allocPara_m.maxCellCount*5, 0.0);// 5 is the maximum number of filopodium in one cell, to be defined as a par later
	cellInfoVecs.cellFilopY.resize(allocPara_m.maxCellCount*5, 0.0);
	cellInfoVecs.cellFilopAngle.resize(allocPara_m.maxCellCount*5, 0.0);
	cellInfoVecs.cellFilopIsActive.resize(allocPara_m.maxCellCount*5, false); // boolean vector 
	cellInfoVecs.cellFilopBirthTime.resize(allocPara_m.maxCellCount*5, 0.0);
	cellInfoVecs.activeCellFilopCounts.resize(allocPara_m.maxCellCount); // uint type
	cellInfoVecs.filopDirection.resize(allocPara_m.maxCellCount,0);

	cellInfoVecs.cellPolarX.resize(allocPara_m.maxCellCount, 0.0);
	cellInfoVecs.cellPolarY.resize(allocPara_m.maxCellCount, 0.0);
	cellInfoVecs.cellPolarAngle.resize(allocPara_m.maxCellCount, 0.0);
	cellInfoVecs.cellRadius.resize(allocPara_m.maxCellCount, 0.0); // avg distance between cell center to membrane nodes

	// add the activation level of cells
	cellInfoVecs.activationLevel.resize(allocPara_m.maxCellCount, 0);
    std::cout << "finished " << std::endl;
}

void SceCells::initCellNodeInfoVecs() {
	cellNodeInfoVecs.cellRanks.resize(allocPara.maxTotalCellNodeCount);
	cellNodeInfoVecs.activeXPoss.resize(allocPara.maxTotalCellNodeCount);
	cellNodeInfoVecs.activeYPoss.resize(allocPara.maxTotalCellNodeCount);
	cellNodeInfoVecs.activeZPoss.resize(allocPara.maxTotalCellNodeCount);
	cellNodeInfoVecs.distToCenterAlongGrowDir.resize(
			allocPara.maxTotalCellNodeCount);
}

void SceCells::initCellNodeInfoVecs_M() {
	std::cout << "max total node count = " << allocPara_m.maxTotalNodeCount
			<< std::endl;
	cellNodeInfoVecs.cellRanks.resize(allocPara_m.maxTotalNodeCount);
	cellNodeInfoVecs.activeXPoss.resize(allocPara_m.maxTotalNodeCount);
	cellNodeInfoVecs.activeYPoss.resize(allocPara_m.maxTotalNodeCount);
	cellNodeInfoVecs.activeZPoss.resize(allocPara_m.maxTotalNodeCount);
	cellNodeInfoVecs.distToCenterAlongGrowDir.resize(
			allocPara_m.maxTotalNodeCount);
}

void SceCells::initGrowthAuxData() {
	growthAuxData.nodeIsActiveAddress = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeIsActive[allocPara.startPosCells]));
	growthAuxData.nodeXPosAddress = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocX[allocPara.startPosCells]));
	growthAuxData.nodeYPosAddress = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocY[allocPara.startPosCells]));
	growthAuxData.randomGrowthSpeedMin = globalConfigVars.getConfigValue(
			"RandomGrowthSpeedMin").toDouble();
	growthAuxData.randomGrowthSpeedMax = globalConfigVars.getConfigValue(
			"RandomGrowthSpeedMax").toDouble();
	growthAuxData.randGenAuxPara = globalConfigVars.getConfigValue(
			"RandomGenerationAuxPara").toDouble();

	if (controlPara.simuType == SingleCellTest) {
		growthAuxData.fixedGrowthSpeed = globalConfigVars.getConfigValue(
				"FixedGrowthSpeed").toDouble();
	}
}

void SceCells::initGrowthAuxData_M() {
	growthAuxData.nodeIsActiveAddress = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeIsActive[allocPara_m.bdryNodeCount]));
	growthAuxData.nodeXPosAddress = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocX[allocPara_m.bdryNodeCount]));
	growthAuxData.nodeYPosAddress = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocY[allocPara_m.bdryNodeCount]));
	growthAuxData.adhIndxAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeAdhereIndex[allocPara_m.bdryNodeCount]));
	growthAuxData.randomGrowthSpeedMin_Ori = globalConfigVars.getConfigValue(
			"RandomGrowthSpeedMin").toDouble();
	growthAuxData.randomGrowthSpeedMax_Ori = globalConfigVars.getConfigValue(
			"RandomGrowthSpeedMax").toDouble();
	growthAuxData.randomGrowthSpeedMin = growthAuxData.randomGrowthSpeedMin_Ori;
	growthAuxData.randomGrowthSpeedMax = growthAuxData.randomGrowthSpeedMax_Ori;
	growthAuxData.grthPrgrCriVal_M_Ori = globalConfigVars.getConfigValue(
			"GrowthPrgrCriVal").toDouble();
	growthAuxData.grthProgrEndCPU = globalConfigVars.getConfigValue(
			"GrowthPrgrValEnd").toDouble();
}




// 
void SceCells::initMyosinLevel() {
	totalNodeCountForActiveCells = allocPara_m.currentActiveCellCount
			* allocPara_m.maxAllNodePerCell;
	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;
	uint maxMemNodePerCell = allocPara_m.maxMembrNodePerCell;
	thrust::counting_iterator<uint> iBegin(0);

	double* nodeLocXAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocX[0]));
	double* nodeLocYAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocY[0]));
	bool* nodeIsActiveAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeIsActive[0]));
	double* myosinLevelAddr = thrust::raw_pointer_cast(
		&(nodes->getInfoVecs().myosinLevel[0])); // pointer to the vector storing myosin level
	// prescribed direction
	// for each node determine whether it is an internal node, comput cell center, calculate projection, return myosin level depending on projection 
	// input: isactive vector, nodelocX, nodelocY, 
	// return: myosin level at the corresponding node
	    thrust::transform(
            thrust::make_zip_iterator(
                    thrust::make_tuple(
                            thrust::make_permutation_iterator(
                                    cellInfoVecs.activeMembrNodeCounts.begin(),
                                    make_transform_iterator(iBegin,
                                            DivideFunctor(maxAllNodePerCell))),
                            thrust::make_permutation_iterator(
                                    cellInfoVecs.activeIntnlNodeCounts.begin(),
                                    make_transform_iterator(iBegin,
                                            DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.centerCoordX.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.centerCoordY.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
                            make_transform_iterator(iBegin,
                                    DivideFunctor(maxAllNodePerCell)),
                            make_transform_iterator(iBegin,
                                    ModuloFunctor(maxAllNodePerCell)),
							thrust::make_permutation_iterator(
									cellInfoVecs.cell_Type.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell)))
                            )),
            thrust::make_zip_iterator(
                    thrust::make_tuple(
                            thrust::make_permutation_iterator(
                                    cellInfoVecs.activeMembrNodeCounts.begin(),
                                    make_transform_iterator(iBegin,
                                            DivideFunctor(maxAllNodePerCell))),
                            thrust::make_permutation_iterator(
                                    cellInfoVecs.activeIntnlNodeCounts.begin(),
                                    make_transform_iterator(iBegin,
                                            DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.centerCoordX.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.centerCoordY.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
                            make_transform_iterator(iBegin,
                                    DivideFunctor(maxAllNodePerCell)),
                            make_transform_iterator(iBegin,
                                    ModuloFunctor(maxAllNodePerCell)),
							thrust::make_permutation_iterator(
									cellInfoVecs.cell_Type.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell)))
                            ))
                    + totalNodeCountForActiveCells,
            nodes->getInfoVecs().myosinLevel.begin(), 
            initMyosinLevelDevice(maxAllNodePerCell, maxMemNodePerCell, nodeLocXAddr,
                    nodeLocYAddr, nodeIsActiveAddr, myosinLevelAddr));
}
// 

// calculate the initial cell area
void SceCells::initCellArea() {
	calCellArea();
	thrust::copy(cellInfoVecs.cellAreaVec.begin(),cellInfoVecs.cellAreaVec.end(),cellInfoVecs.cellAreaInit.begin());
}

void SceCells::initialize(SceNodes* nodesInput) {
	nodes = nodesInput;
	controlPara = nodes->getControlPara();
	readMiscPara();
	readBioPara();

	allocPara = nodesInput->getAllocPara();

	// max internal node count must be even number.
	assert(allocPara_m.maxIntnlNodePerCell % 2 == 0);

	initCellInfoVecs();
	initCellNodeInfoVecs();
	initGrowthAuxData();
	//
	distributeIsCellRank();
}

// in the constructor 
void SceCells::initialize_M(SceNodes* nodesInput, std::vector<double> &initCellRadii) {
	std::cout << "Initializing cells ...... " << std::endl;
	//std::cout.flush();
	nodes = nodesInput;
	allocPara_m = nodesInput->getAllocParaM();
	// max internal node count must be even number.
	assert(allocPara_m.maxIntnlNodePerCell % 2 == 0);

	//std::cout << "break point 1 " << std::endl;
	//std::cout.flush();
	controlPara = nodes->getControlPara();
	//std::cout << "break point 2 " << std::endl;
	//std::cout.flush();
	readMiscPara_M();
	//std::cout << "break point 3 " << std::endl;
	//std::cout.flush();
	initCellInfoVecs_M(); // initialize cell info vecs 

    cout<< "size of dpp initilizie  is "<< cellInfoVecs.cell_Dpp.size() << endl ;          
	//std::cout << "break point 4 " << std::endl;
	//std::cout.flush();
	readBioPara();
	//std::cout << "break point 5 " << std::endl;
	//std::cout.flush();

	//std::cout << "break point 6 " << std::endl;
	//std::cout.flush();
	initCellNodeInfoVecs_M(); // initialize cell node info vec 
	//std::cout << "break point 7 " << std::endl;
	//std::cout.flush();
	initGrowthAuxData_M(); 
	//std::cout << "break point 8 " << std::endl;
	//std::cout.flush();
	allocPara_m.leaderExist = false;
	allocPara_m.leaderRank = 0;
	for (uint cellRank=0; cellRank<allocPara_m.currentActiveCellCount; cellRank++)
    {
        if (initCellRadii[cellRank]>2.0){ // 1 is leader // do not change the randii threshold from 2 to 3, it will not detect leader in this case
			cellInfoVecs.cell_Type[cellRank] = 1;
			// nodes->setLeaderRank(cellRank);
			allocPara_m.leaderRank = cellRank;
			allocPara_m.leaderExist = true;
			nodes->setAllocParaM(allocPara_m);// allocPara_m is of type NodeAllocPara_M
			break;
			} else {
				cellInfoVecs.cell_Type[cellRank] = 0;
				allocPara_m.leaderExist = false;
				nodes->setAllocParaM(allocPara_m);
			}
    }
	// assert(allocPara_m.leaderRank == ? );
	cout<< "leader Rank is "<< allocPara_m.leaderRank << endl ; // if the above condition is not satisfied, what is leaderRank?
	cout<< "Node leader Rank is"<<nodes->getAllocParaM().leaderRank << endl; 
	cout<< "Leader exists is "<<allocPara_m.leaderExist<<endl;
	initMyosinLevel(); 
	initCellArea();
}

void SceCells::copyInitActiveNodeCount(
		std::vector<uint>& numOfInitActiveNodesOfCells) {
	thrust::copy(numOfInitActiveNodesOfCells.begin(),
			numOfInitActiveNodesOfCells.end(),
			cellInfoVecs.activeNodeCountOfThisCell.begin());
}

void SceCells::allComponentsMove() {
	adjustNodeVel();
	moveNodes();
}

/**
 * Mark cell node as either activdistributeIsActiveInfo()e or inactive.
 * left part of the node array will be active and right part will be inactive.
 * the threshold is defined by array activeNodeCountOfThisCell.
 * e.g. activeNodeCountOfThisCell = {2,3} and  maxNodeOfOneCell = 5
 */
void SceCells::distributeIsActiveInfo() {
//std::cout << "before distribute bdry isActive" << std::endl;
	distributeBdryIsActiveInfo();
//std::cout << "before distribute profile isActive" << std::endl;
	distributeProfileIsActiveInfo();
//std::cout << "before distribute ecm isActive" << std::endl;
	distributeECMIsActiveInfo();
//std::cout << "before distribute cells isActive" << std::endl;
	distributeCellIsActiveInfo();
}

void SceCells::distributeIsCellRank() {
	uint totalNodeCountForActiveCells = allocPara.currentActiveCellCount
			* allocPara.maxNodeOfOneCell;
	thrust::counting_iterator<uint> countingBegin(0);
	thrust::counting_iterator<uint> countingCellEnd(
			totalNodeCountForActiveCells);
	std::cerr << "totalNodeCount for active cells "
			<< totalNodeCountForActiveCells << std::endl;

//thrust::counting_iterator<uint> countingECMEnd(countingECMEnd);

// only computes the cell ranks of cells. the rest remain unchanged.
	thrust::transform(countingBegin, countingCellEnd,
			nodes->getInfoVecs().nodeCellRank.begin() + allocPara.startPosCells,
			DivideFunctor(allocPara.maxNodeOfOneCell));
	std::cerr << "finished cellRank transformation" << std::endl;
}

/**
 * This method computes center of all cells.
 * more efficient then simply iterating the cell because of parallel reducing.
 */
void SceCells::computeCenterPos() {
	uint totalNodeCountForActiveCells = allocPara.currentActiveCellCount
			* allocPara.maxNodeOfOneCell;
	thrust::counting_iterator<uint> countingBegin(0);
	thrust::counting_iterator<uint> countingEnd(totalNodeCountForActiveCells);
	uint totalNumberOfActiveNodes = thrust::reduce(
			cellInfoVecs.activeNodeCountOfThisCell.begin(),
			cellInfoVecs.activeNodeCountOfThisCell.begin()
					+ allocPara.currentActiveCellCount);

	thrust::copy_if(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							make_transform_iterator(countingBegin,
									DivideFunctor(allocPara.maxNodeOfOneCell)),
							nodes->getInfoVecs().nodeLocX.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeLocY.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeLocZ.begin()
									+ allocPara.startPosCells)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							make_transform_iterator(countingBegin,
									DivideFunctor(allocPara.maxNodeOfOneCell)),
							nodes->getInfoVecs().nodeLocX.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeLocY.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeLocZ.begin()
									+ allocPara.startPosCells))
					+ totalNodeCountForActiveCells,
			nodes->getInfoVecs().nodeIsActive.begin() + allocPara.startPosCells,
			thrust::make_zip_iterator(
					thrust::make_tuple(cellNodeInfoVecs.cellRanks.begin(),
							cellNodeInfoVecs.activeXPoss.begin(),
							cellNodeInfoVecs.activeYPoss.begin(),
							cellNodeInfoVecs.activeZPoss.begin())), isTrue());

	thrust::reduce_by_key(cellNodeInfoVecs.cellRanks.begin(),
			cellNodeInfoVecs.cellRanks.begin() + totalNumberOfActiveNodes,
			thrust::make_zip_iterator(
					thrust::make_tuple(cellNodeInfoVecs.activeXPoss.begin(),
							cellNodeInfoVecs.activeYPoss.begin(),
							cellNodeInfoVecs.activeZPoss.begin())),
			cellInfoVecs.cellRanksTmpStorage.begin(),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin(),
							cellInfoVecs.centerCoordZ.begin())),
			thrust::equal_to<uint>(), CVec3Add());
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin(),
							cellInfoVecs.centerCoordZ.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin(),
							cellInfoVecs.centerCoordZ.begin()))
					+ allocPara.currentActiveCellCount,
			cellInfoVecs.activeNodeCountOfThisCell.begin(),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin(),
							cellInfoVecs.centerCoordZ.begin())), CVec3Divide());
}

/**
 * 2D version of cell division.
 * Division process is done by creating two temporary vectors to hold the node information
 * that are going to divide.
 *
 * step 1: based on lengthDifference, expectedLength and growthProgress,
 *     this process determines whether a certain cell is ready to divide and then assign
 *     a boolean value to isDivided.
 *
 * step 2. copy those cells that will divide in to the temp vectors created
 *
 * step 3. For each cell in the temp vectors, we sort its nodes by its distance to the
 * corresponding cell center.
 * This step is not very effcient when the number of cells going to divide is big.
 * but this is unlikely to happen because cells will divide according to external chemical signaling
 * and each will have different divide progress.
 *
 * step 4. copy the right part of each cell of the sorted array (temp1) to left part of each cell of
 * another array
 *
 * step 5. transform isActive vector of both temp1 and temp2, making only left part of each cell active.
 *
 * step 6. insert temp2 to the end of the cell array
 *
 * step 7. copy temp1 to the previous position of the cell array.
 *
 * step 8. add activeCellCount of the system.
 *
 * step 9. mark isDivide of all cells to false.
 */

void SceCells::divide2DSimplified() {
	bool isDivisionPresent = decideIfGoingToDivide();
	if (!isDivisionPresent) {
		return;
	}
	copyCellsPreDivision();
	sortNodesAccordingToDist();
	copyLeftAndRightToSeperateArrays();
	transformIsActiveArrayOfBothArrays();
	addSecondArrayToCellArray();
	copyFirstArrayToPreviousPos();
	updateActiveCellCount();
	markIsDivideFalse();
}

bool SceCells::decideIfGoingToDivide() {
// step 1
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.lengthDifference.begin(),
							cellInfoVecs.expectedLength.begin(),
							cellInfoVecs.growthProgress.begin(),
							cellInfoVecs.activeNodeCountOfThisCell.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.lengthDifference.begin(),
							cellInfoVecs.expectedLength.begin(),
							cellInfoVecs.growthProgress.begin(),
							cellInfoVecs.activeNodeCountOfThisCell.begin()))
					+ allocPara.currentActiveCellCount,
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.isDividing.begin(),
							cellInfoVecs.growthProgress.begin())),
			CompuIsDivide(miscPara.isDivideCriticalRatio,
					allocPara.maxNodeOfOneCell));
// sum all bool values which indicate whether the cell is going to divide.
// toBeDivideCount is the total number of cells going to divide.
	divAuxData.toBeDivideCount = thrust::reduce(cellInfoVecs.isDividing.begin(),
			cellInfoVecs.isDividing.begin() + allocPara.currentActiveCellCount,
			(uint) (0));
	if (divAuxData.toBeDivideCount > 0) {
		return true;
	} else {
		return false;
	}
}

void SceCells::copyCellsPreDivision() {
// step 2 : copy all cell rank and distance to its corresponding center with divide flag = 1
	totalNodeCountForActiveCells = allocPara.currentActiveCellCount
			* allocPara.maxNodeOfOneCell;

	divAuxData.nodeStorageCount = divAuxData.toBeDivideCount
			* allocPara.maxNodeOfOneCell;
	divAuxData.tmpIsActiveHold1 = thrust::device_vector<bool>(
			divAuxData.nodeStorageCount, true);
	divAuxData.tmpDistToCenter1 = thrust::device_vector<double>(
			divAuxData.nodeStorageCount, 0.0);
	divAuxData.tmpCellRankHold1 = thrust::device_vector<uint>(
			divAuxData.nodeStorageCount, 0.0);
	divAuxData.tmpXValueHold1 = thrust::device_vector<double>(
			divAuxData.nodeStorageCount, 0.0);
	divAuxData.tmpYValueHold1 = thrust::device_vector<double>(
			divAuxData.nodeStorageCount, 0.0);
	divAuxData.tmpZValueHold1 = thrust::device_vector<double>(
			divAuxData.nodeStorageCount, 0.0);

	divAuxData.tmpCellTypes = thrust::device_vector<SceNodeType>(
			divAuxData.nodeStorageCount);

	divAuxData.tmpIsActiveHold2 = thrust::device_vector<bool>(
			divAuxData.nodeStorageCount, false);
	divAuxData.tmpDistToCenter2 = thrust::device_vector<double>(
			divAuxData.nodeStorageCount, 0.0);
	divAuxData.tmpXValueHold2 = thrust::device_vector<double>(
			divAuxData.nodeStorageCount, 0.0);
	divAuxData.tmpYValueHold2 = thrust::device_vector<double>(
			divAuxData.nodeStorageCount, 0.0);
	divAuxData.tmpZValueHold2 = thrust::device_vector<double>(
			divAuxData.nodeStorageCount, 0.0);

// step 2 , continued
	thrust::copy_if(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							make_transform_iterator(countingBegin,
									DivideFunctor(allocPara.maxNodeOfOneCell)),
							cellNodeInfoVecs.distToCenterAlongGrowDir.begin(),
							nodes->getInfoVecs().nodeLocX.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeLocY.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeLocZ.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeCellType.begin()
									+ allocPara.startPosCells)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							make_transform_iterator(countingBegin,
									DivideFunctor(allocPara.maxNodeOfOneCell)),
							cellNodeInfoVecs.distToCenterAlongGrowDir.begin(),
							nodes->getInfoVecs().nodeLocX.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeLocY.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeLocZ.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeCellType.begin()
									+ allocPara.startPosCells))
					+ totalNodeCountForActiveCells,
			thrust::make_permutation_iterator(cellInfoVecs.isDividing.begin(),
					make_transform_iterator(countingBegin,
							DivideFunctor(allocPara.maxNodeOfOneCell))),
			thrust::make_zip_iterator(
					thrust::make_tuple(divAuxData.tmpCellRankHold1.begin(),
							divAuxData.tmpDistToCenter1.begin(),
							divAuxData.tmpXValueHold1.begin(),
							divAuxData.tmpYValueHold1.begin(),
							divAuxData.tmpZValueHold1.begin(),
							divAuxData.tmpCellTypes.begin())), isTrue());
}

/**
 * performance wise, this implementation is not the best because I can use only one sort_by_key
 * with speciialized comparision operator. However, This implementation is more robust and won't
 * compromise performance too much.
 */
void SceCells::sortNodesAccordingToDist() {
//step 3
	for (uint i = 0; i < divAuxData.toBeDivideCount; i++) {
		thrust::sort_by_key(
				divAuxData.tmpDistToCenter1.begin()
						+ i * allocPara.maxNodeOfOneCell,
				divAuxData.tmpDistToCenter1.begin()
						+ (i + 1) * allocPara.maxNodeOfOneCell,
				thrust::make_zip_iterator(
						thrust::make_tuple(
								divAuxData.tmpXValueHold1.begin()
										+ i * allocPara.maxNodeOfOneCell,
								divAuxData.tmpYValueHold1.begin()
										+ i * allocPara.maxNodeOfOneCell,
								divAuxData.tmpZValueHold1.begin()
										+ i * allocPara.maxNodeOfOneCell)));
	}
}

/**
 * scatter_if() is a thrust function.
 * inputIter1 first,
 * inputIter1 last,
 * inputIter2 map,
 * inputIter3 stencil
 * randomAccessIter output
 */
void SceCells::copyLeftAndRightToSeperateArrays() {
//step 4.
	thrust::scatter_if(
			thrust::make_zip_iterator(
					thrust::make_tuple(divAuxData.tmpXValueHold1.begin(),
							divAuxData.tmpYValueHold1.begin(),
							divAuxData.tmpZValueHold1.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(divAuxData.tmpXValueHold1.end(),
							divAuxData.tmpYValueHold1.end(),
							divAuxData.tmpZValueHold1.end())),
			make_transform_iterator(countingBegin,
					LeftShiftFunctor(allocPara.maxNodeOfOneCell)),
			make_transform_iterator(countingBegin,
					IsRightSide(allocPara.maxNodeOfOneCell)),
			thrust::make_zip_iterator(
					thrust::make_tuple(divAuxData.tmpXValueHold2.begin(),
							divAuxData.tmpYValueHold2.begin(),
							divAuxData.tmpZValueHold2.begin())));
}

void SceCells::transformIsActiveArrayOfBothArrays() {
	thrust::transform(countingBegin,
			countingBegin + divAuxData.nodeStorageCount,
			divAuxData.tmpIsActiveHold1.begin(),
			IsLeftSide(allocPara.maxNodeOfOneCell));
	thrust::transform(countingBegin,
			countingBegin + divAuxData.nodeStorageCount,
			divAuxData.tmpIsActiveHold2.begin(),
			IsLeftSide(allocPara.maxNodeOfOneCell));
	if (divAuxData.toBeDivideCount != 0) {
		std::cout << "before insert, active cell count in nodes:"
				<< nodes->getAllocPara().currentActiveCellCount << std::endl;
	}
}

void SceCells::addSecondArrayToCellArray() {
/// step 6. call SceNodes function to add newly divided cells
	nodes->addNewlyDividedCells(divAuxData.tmpXValueHold2,
			divAuxData.tmpYValueHold2, divAuxData.tmpZValueHold2,
			divAuxData.tmpIsActiveHold2, divAuxData.tmpCellTypes);
}

void SceCells::copyFirstArrayToPreviousPos() {
	thrust::scatter(
			thrust::make_zip_iterator(
					thrust::make_tuple(divAuxData.tmpIsActiveHold1.begin(),
							divAuxData.tmpXValueHold1.begin(),
							divAuxData.tmpYValueHold1.begin(),
							divAuxData.tmpZValueHold1.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(divAuxData.tmpIsActiveHold1.end(),
							divAuxData.tmpXValueHold1.end(),
							divAuxData.tmpYValueHold1.end(),
							divAuxData.tmpZValueHold1.end())),
			thrust::make_transform_iterator(
					thrust::make_zip_iterator(
							thrust::make_tuple(countingBegin,
									divAuxData.tmpCellRankHold1.begin())),
					CompuPos(allocPara.maxNodeOfOneCell)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							nodes->getInfoVecs().nodeIsActive.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeLocX.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeLocY.begin()
									+ allocPara.startPosCells,
							nodes->getInfoVecs().nodeLocZ.begin()
									+ allocPara.startPosCells)));

	/**
	 * after dividing, the cell should resume the initial
	 * (1) node count, which defaults to be half size of max node count
	 * (2) growth progress, which defaults to 0
	 * (3) last check point, which defaults to 0
	 */
	thrust::scatter_if(
			thrust::make_zip_iterator(
					thrust::make_tuple(initIntnlNodeCount, initGrowthProgress,
							initGrowthProgress)),
			thrust::make_zip_iterator(
					thrust::make_tuple(initIntnlNodeCount, initGrowthProgress,
							initGrowthProgress))
					+ allocPara.currentActiveCellCount, countingBegin,
			cellInfoVecs.isDividing.begin(),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							cellInfoVecs.activeNodeCountOfThisCell.begin(),
							cellInfoVecs.growthProgress.begin(),
							cellInfoVecs.lastCheckPoint.begin())), isTrue());

// TODO: combine this one with the previous scatter_if to improve efficiency.
	thrust::fill(
			cellInfoVecs.activeNodeCountOfThisCell.begin()
					+ allocPara.currentActiveCellCount,
			cellInfoVecs.activeNodeCountOfThisCell.begin()
					+ allocPara.currentActiveCellCount
					+ divAuxData.toBeDivideCount,
			allocPara.maxNodeOfOneCell / 2);

}

void SceCells::updateActiveCellCount() {
	allocPara.currentActiveCellCount = allocPara.currentActiveCellCount
			+ divAuxData.toBeDivideCount;
	NodeAllocPara para = nodes->getAllocPara();
	para.currentActiveCellCount = allocPara.currentActiveCellCount;
	nodes->setAllocPara(para);
}

void SceCells::markIsDivideFalse() {
	thrust::fill(cellInfoVecs.isDividing.begin(),
			cellInfoVecs.isDividing.begin() + allocPara.currentActiveCellCount,
			false);
}

void SceCells::readMiscPara() {
	miscPara.addNodeDistance = globalConfigVars.getConfigValue(
			"DistanceForAddingNode").toDouble();
	miscPara.minDistanceToOtherNode = globalConfigVars.getConfigValue(
			"MinDistanceToOtherNode").toDouble();
	miscPara.isDivideCriticalRatio = globalConfigVars.getConfigValue(
			"IsDivideCrticalRatio").toDouble();
// reason for adding a small term here is to avoid scenario when checkpoint might add many times
// up to 0.99999999 which is theoretically 1.0 but not in computer memory. If we don't include
// this small term we might risk adding one more node.
	int maxNodeOfOneCell =
			globalConfigVars.getConfigValue("MaxNodePerCell").toInt();
	miscPara.growThreshold = 1.0 / (maxNodeOfOneCell - maxNodeOfOneCell / 2)
			+ epsilon;
}

void SceCells::readMiscPara_M() {
	miscPara.addNodeDistance = globalConfigVars.getConfigValue(
			"DistanceForAddingNode").toDouble();
	miscPara.minDistanceToOtherNode = globalConfigVars.getConfigValue(
			"MinDistanceToOtherNode").toDouble();
	miscPara.isDivideCriticalRatio = globalConfigVars.getConfigValue(
			"IsDivideCrticalRatio").toDouble();
// reason for adding a small term here is to avoid scenario when checkpoint might add many times
// up to 0.99999999 which is theoretically 1.0 but not in computer memory. If we don't include
// this small term we might risk adding one more node.
	int maxIntnlNodePerCell = globalConfigVars.getConfigValue(
			"MaxIntnlNodeCountPerCell").toInt();
	miscPara.growThreshold = 1.0
			/ (maxIntnlNodePerCell - maxIntnlNodePerCell / 2) + epsilon;
	miscPara.prolifDecayCoeff = globalConfigVars.getConfigValue(
			"ProlifDecayCoeff").toDouble();
}

void SceCells::readBioPara() {
	if (controlPara.simuType != Disc_M) {
		bioPara.cellInitLength = globalConfigVars.getConfigValue(
				"CellInitLength").toDouble();
		std::cout << "break point 1 " << bioPara.cellInitLength << std::endl;
		std::cout.flush();
		bioPara.cellFinalLength = globalConfigVars.getConfigValue(
				"CellFinalLength").toDouble();
		std::cout << "break point 2 " << bioPara.cellFinalLength << std::endl;
		std::cout.flush();
		bioPara.elongationCoefficient = globalConfigVars.getConfigValue(
				"ElongateCoefficient").toDouble();

		std::cout << "break point 3 " << bioPara.elongationCoefficient
				<< std::endl;
		std::cout.flush();

	}

	if (controlPara.simuType == Beak) {
		std::cout << "break point 4 " << std::endl;
		std::cout.flush();
		bioPara.chemoCoefficient = globalConfigVars.getConfigValue(
				"ChemoCoefficient").toDouble();
	}
	//std::cin >> jj;
}

void SceCells::randomizeGrowth() {
	thrust::counting_iterator<uint> countingBegin(0);
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.growthXDir.begin(),
							cellInfoVecs.growthYDir.begin(),
							cellInfoVecs.isRandGrowInited.begin(),
							countingBegin)),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.growthXDir.begin(),
							cellInfoVecs.growthYDir.begin(),
							cellInfoVecs.isRandGrowInited.begin(),
							countingBegin)) + allocPara.currentActiveCellCount,
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.growthSpeed.begin(),
							cellInfoVecs.growthXDir.begin(),
							cellInfoVecs.growthYDir.begin(),
							cellInfoVecs.isRandGrowInited.begin())),
			AssignRandIfNotInit(growthAuxData.randomGrowthSpeedMin,
					growthAuxData.randomGrowthSpeedMax,
					allocPara.currentActiveCellCount,
					growthAuxData.randGenAuxPara));
}

/**
 * To run all the cell level logics.
 * First step we got center positions of cells.
 * Grow.
 */
void SceCells::runAllCellLevelLogicsDisc(double dt) {
	this->dt = dt;
//std::cerr << "enter run all cell level logics" << std::endl;
	computeCenterPos();
//std::cerr << "after compute center position." << std::endl;

	if (nodes->getControlPara().controlSwitchs.stab == OFF) {
		growAtRandom(dt);
		//grow2DTwoRegions(dt, region1, region2);
		//std::cerr << "after grow cells" << std::endl;
		//distributeIsActiveInfo();
		//std::cerr << "after distribute is active info." << std::endl;
		divide2DSimplified();
		//std::cerr << "after divide 2D simplified." << std::endl;
		distributeIsActiveInfo();
		//std::cerr << "after distribute is active info." << std::endl;
		distributeCellGrowthProgress();
	}

	allComponentsMove();
	//std::cerr << "after all components move." << std::endl;

}

//Ali void SceCells::runAllCellLogicsDisc_M(double dt) {
// void SceCells::runAllCellLogicsDisc_M(double dt, double Damp_Coef, double InitTimeStage, std::vector<SigptStateV2>& sigPtVecV2) {   //Ali
void SceCells::runAllCellLogicsDisc_M(double dt, double Damp_Coef, double InitTimeStage) { // , std::vector<double>& fluxWeightsVec
        
	std::cout << "     *** 1 ***" << endl;
	std::cout.flush();
	this->dt = dt;
    this->Damp_Coef=Damp_Coef ; //Ali 
    this->InitTimeStage=InitTimeStage; 
	// this->sigPtVec = sigPtVec;
	growthAuxData.prolifDecay =1.0   ;  // no decay for right now     exp(-curTime * miscPara.prolifDecayCoeff);
        cout<< "The important curTime used in simulation is here which is"<<curTime <<endl; 
	growthAuxData.randomGrowthSpeedMin = growthAuxData.prolifDecay
			* growthAuxData.randomGrowthSpeedMin_Ori;
	growthAuxData.randomGrowthSpeedMax = growthAuxData.prolifDecay
			* growthAuxData.randomGrowthSpeedMax_Ori;
	curTime = curTime + dt;

	std::cout << "     *** 2 ***" << endl;
	std::cout.flush();

	// applyMembrAdhCell_M(); // moved from node 
	// copyExtForcesCell_M(); // moved from node
	updateActivationLevel();
	distributeCellActivationLevel_M();
	distributeCell_Type_M();

	applySceCellDisc_M();
	// updateCellAdhIndex();
	updateCellPolar(); // update filopodia extension from followers // comment out start for no cell motion 
	int ruleNum = 3;
	if (ruleNum<3){ // means 1 or 2
		// do not update myosin dynamics via flux
		calSceCellMyosin();
		calSubAdhForce(); // comment out end 
	} else if (ruleNum==3){
		calFluxWeightsMyosin();
		calSceCellMyosin();
		updateCellPolarLeader();
		// applySceCellMyosin();
		// applySigForce(sigPtVecV2);
		calSubAdhForce(); // comment out end 
	}
	std::cout << "     *** 3 ***" << endl;
	std::cout.flush();
       
	computeCenterPos_M();
	// computeCellRadius();
    // exchSignal(); // use files in srcCPU for chemical concentrations
    // BC_Imp_M(); // do nothing for now, changing damping coef
//	test_SigPt(sigPtVec); // not used, aug 2023
	std::cout << "     *** 3.5 ***" << endl;
	std::cout.flush();
	
	applyMemForce_M();
	std::cout << "     *** 4 ***" << endl;
	std::cout.flush();

     //Ali cmment //
//	computeCenterPos_M();
	std::cout << "     *** 5 ***" << endl;
	std::cout.flush();
     //Ali cmment //
	growAtRandom_M(dt);
	std::cout << "     *** 6 ***" << endl;
	std::cout.flush();
	//if (curTime<3300.0)
	divide2D_M();
	std::cout << "     *** 7 ***" << endl;
	std::cout.flush();
	distributeCellGrowthProgress_M();
	
	std::cout << "     *** 8 ***" << endl;
	std::cout.flush();

    findTangentAndNormal_M(); //AAMIRI ADDED May29
	allComponentsMove_M();
	std::cout << "     *** 9 ***" << endl;
	std::cout.flush();
	handleMembrGrowth_M();
	std::cout << "     *** 10 ***" << endl;
	std::cout.flush();
}



void SceCells::exchSignal(){

if (firstTimeReadDpp) {	
		uint maxTotalNodes=nodes->getInfoVecs().nodeLocX.size() ; 
    	signal.Initialize(allocPara_m.maxAllNodePerCell,allocPara_m.maxMembrNodePerCell,maxTotalNodes, allocPara_m.maxCellCount) ;
		cout << " I passed the initializtion for signaling module" << endl ; 
	}

   	lastTimeExchange=lastTimeExchange+dt ;
	cout << "last time exchange is " << lastTimeExchange << endl ; 
	cout << "dt is " << dt << endl ;  
   	double exchPeriod=200 ; 
	if ( lastTimeExchange>exchPeriod) {
		lastTimeExchange=0 ; 
		//vector<CVector> cellCentersHost ; 
    	//cellCentersHost=getAllCellCenters();  //Ali
		cout << "I entered the function to update dpp" << endl ; 

		thrust::device_vector<double>::iterator  MinX_Itr=thrust::min_element(cellInfoVecs.centerCoordX.begin(),
                                       									  cellInfoVecs.centerCoordX.begin()+allocPara_m.currentActiveCellCount) ;
    		thrust::device_vector<double>::iterator  MaxX_Itr=thrust::max_element(cellInfoVecs.centerCoordX.begin(),
                                                                              cellInfoVecs.centerCoordX.begin()+ allocPara_m.currentActiveCellCount) ;
    		thrust::device_vector<double>::iterator  MinY_Itr=thrust::min_element(cellInfoVecs.centerCoordY.begin(),
                                                                              cellInfoVecs.centerCoordY.begin()+ allocPara_m.currentActiveCellCount) ;
   		thrust::device_vector<double>::iterator  MaxY_Itr=thrust::max_element(cellInfoVecs.centerCoordY.begin(),
                                                                              cellInfoVecs.centerCoordY.begin()+ allocPara_m.currentActiveCellCount) ;
   		Tisu_MinX= *MinX_Itr ; 
   		Tisu_MaxX= *MaxX_Itr ; 
   		Tisu_MinY= *MinY_Itr ; 
   		Tisu_MaxY= *MaxY_Itr ;

   		Tisu_R=0.5*(0.5*(Tisu_MaxX-Tisu_MinX)+0.5*(Tisu_MaxY-Tisu_MinY)) ; 

		totalNodeCountForActiveCells = allocPara_m.currentActiveCellCount
			* allocPara_m.maxAllNodePerCell;


		thrust:: copy (nodes->getInfoVecs().nodeIsActive.begin(),nodes->getInfoVecs().nodeIsActive.begin()+  totalNodeCountForActiveCells, signal.nodeIsActiveHost.begin()); 
		thrust:: copy (nodes->getInfoVecs().nodeLocX.begin(),nodes->getInfoVecs().nodeLocX.begin()+  totalNodeCountForActiveCells, signal.nodeLocXHost.begin()); 
		thrust:: copy (nodes->getInfoVecs().nodeLocY.begin(),nodes->getInfoVecs().nodeLocY.begin()+  totalNodeCountForActiveCells, signal.nodeLocYHost.begin()); 
		thrust:: copy (cellInfoVecs.centerCoordX.begin(),cellInfoVecs.centerCoordX.begin()+allocPara_m.currentActiveCellCount, signal.cellCenterX.begin()); 
		thrust:: copy (cellInfoVecs.centerCoordY.begin(),cellInfoVecs.centerCoordY.begin()+allocPara_m.currentActiveCellCount, signal.cellCenterY.begin());
		
		thrust:: copy (cellInfoVecs.cell_Dpp.begin(),cellInfoVecs.cell_Dpp.begin()+allocPara_m.currentActiveCellCount, signal.dppLevel.begin());		//Alireza
		thrust:: copy (cellInfoVecs.cell_Tkv.begin(),cellInfoVecs.cell_Tkv.begin()+allocPara_m.currentActiveCellCount, signal.tkvLevel.begin());		//Alireza
		thrust:: copy (cellInfoVecs.cell_DppTkv.begin(),cellInfoVecs.cell_DppTkv.begin()+allocPara_m.currentActiveCellCount, signal.dppTkvLevel.begin());	//Alireza
		thrust:: copy (cellInfoVecs.cell_pMad.begin(),cellInfoVecs.cell_pMad.begin()+allocPara_m.currentActiveCellCount, signal.pMadLevel.begin());		//Alireza
		// thrust:: copy (cellInfoVecs.cell_Type.begin(),cellInfoVecs.cell_Type.begin()+allocPara_m.currentActiveCellCount, signal.cellType.begin());             //Alireza

        signal.updateSignal(Tisu_MinX,Tisu_MaxX,Tisu_MinY,Tisu_MaxY,curTime,totalNodeCountForActiveCells,allocPara_m.currentActiveCellCount) ; //Ali
        assert(cellInfoVecs.cell_Dpp.size()==signal.dppLevel.size());
        thrust::copy(signal.dppLevel.begin(),signal.dppLevel.begin() + allocPara_m.currentActiveCellCount,cellInfoVecs.cell_Dpp.begin()) ;
		thrust::copy(signal.tkvLevel.begin(),signal.tkvLevel.begin() + allocPara_m.currentActiveCellCount,cellInfoVecs.cell_Tkv.begin()) ;		//Alireza
		thrust::copy(signal.dppTkvLevel.begin(),signal.dppTkvLevel.begin() + allocPara_m.currentActiveCellCount,cellInfoVecs.cell_DppTkv.begin()) ;	//Alireza
		thrust::copy(signal.pMadLevel.begin(),signal.pMadLevel.begin() + allocPara_m.currentActiveCellCount,cellInfoVecs.cell_pMad.begin()) ;		//Alireza
		// thrust::copy(signal.cellType.begin(),signal.cellType.begin() + allocPara_m.currentActiveCellCount,cellInfoVecs.cell_Type.begin()) ; 	
		//currentActiveCellCountOld=allocPara_m.currentActiveCellCount;
 
	}
	if (firstTimeReadDpp) {	 
	   	thrust::copy(signal.dppLevel.begin(),signal.dppLevel.end(),cellInfoVecs.cell_DppOld.begin()) ;
		thrust::copy(signal.pMadLevel.begin(),signal.pMadLevel.end(),cellInfoVecs.cell_pMadOld.begin()) ;	//Alireza
		firstTimeReadDpp=false ; 
	}

}

void SceCells::runStretchTest(double dt) {
	this->dt = dt;
	computeCenterPos();
	growAlongX(false, dt);
	moveNodes();
}

void SceCells::growAlongX(bool isAddPt, double d_t) {
	totalNodeCountForActiveCells = allocPara.currentActiveCellCount
			* allocPara.maxNodeOfOneCell;

	setGrowthDirXAxis();

//std::cout << "after copy grow info" << std::endl;
	updateGrowthProgress();
//std::cout << "after update growth progress" << std::endl;
	decideIsScheduleToGrow();
//std::cout << "after decode os schedule to grow" << std::endl;
	computeCellTargetLength();
//std::cout << "after compute cell target length" << std::endl;
	computeDistToCellCenter();
//std::cout << "after compute dist to center" << std::endl;
	findMinAndMaxDistToCenter();
//std::cout << "after find min and max dist" << std::endl;
	computeLenDiffExpCur();
//std::cout << "after compute diff " << std::endl;
	stretchCellGivenLenDiff();

	if (isAddPt) {
		addPointIfScheduledToGrow();
	}
}

void SceCells::growWithStress(double d_t) {
}

std::vector<CVector> SceCells::getAllCellCenters() {
//void SceCells::getAllCellCenters() {
	//thrust::host_vector<double> centerX = cellInfoVecs.centerCoordX;
	//thrust::host_vector<double> centerY = cellInfoVecs.centerCoordY;
	//thrust::host_vector<double> centerZ = cellInfoVecs.centerCoordZ;

	thrust::host_vector<double> centerX(
			allocPara_m.currentActiveCellCount);
	thrust::copy(cellInfoVecs.centerCoordX.begin(),
			cellInfoVecs.centerCoordX.begin()
					+ allocPara_m.currentActiveCellCount,
			centerX.begin());
 
        thrust::host_vector<double> centerY(
			allocPara_m.currentActiveCellCount);
	thrust::copy(cellInfoVecs.centerCoordY.begin(),
			cellInfoVecs.centerCoordY.begin()
					+ allocPara_m.currentActiveCellCount,
			centerY.begin());

	thrust::host_vector<double> centerZ(
			allocPara_m.currentActiveCellCount);
	thrust::copy(cellInfoVecs.centerCoordZ.begin(),
			cellInfoVecs.centerCoordZ.begin()
					+ allocPara_m.currentActiveCellCount,
			centerZ.begin());

        //infoForSignal.sCenterX=centerX[4] ; 
        //infoForSignal.sCenterY=centerY[4] ; 
        //infoForSignal.sCenterZ=centerZ[4] ; 


	std::vector<CVector> result;
	for (uint i = 0; i < allocPara_m.currentActiveCellCount; i++) {
		CVector pos = CVector(centerX[i], centerY[i], centerZ[i]);
        //infoForSignal.sCenterX=centerX[i] ; 
        //infoForSignal.sCenterY=centerY[i] ; 
        //infoForSignal.sCenterZ=centerZ[i] ; 
		result.push_back(pos);
	}
	return result; 
}

void SceCells::setGrowthDirXAxis() {
	thrust::fill(cellInfoVecs.growthXDir.begin(),
			cellInfoVecs.growthXDir.begin() + allocPara.currentActiveCellCount,
			1.0);
	thrust::fill(cellInfoVecs.growthYDir.begin(),
			cellInfoVecs.growthYDir.begin() + allocPara.currentActiveCellCount,
			0.0);
	thrust::fill(cellInfoVecs.growthSpeed.begin(),
			cellInfoVecs.growthSpeed.begin() + allocPara.currentActiveCellCount,
			growthAuxData.fixedGrowthSpeed);
}

std::vector<double> SceCells::getGrowthProgressVec() {
	thrust::host_vector<double> growthProVec = cellInfoVecs.growthProgress;
	std::vector<double> result;
	for (uint i = 0; i < allocPara.currentActiveCellCount; i++) {
		result.push_back(growthProVec[i]);
	}
	return result;
}

void SceCells::copyCellsPreDivision_M() {
	totalNodeCountForActiveCells = allocPara_m.currentActiveCellCount
			* allocPara_m.maxAllNodePerCell;

	divAuxData.nodeStorageCount = divAuxData.toBeDivideCount
			* allocPara_m.maxAllNodePerCell;

	divAuxData.tmpIsActive_M = thrust::device_vector<bool>(
			divAuxData.nodeStorageCount, true);
	divAuxData.tmpNodePosX_M = thrust::device_vector<double>(
			divAuxData.nodeStorageCount, 0.0);
	divAuxData.tmpNodePosY_M = thrust::device_vector<double>(
			divAuxData.nodeStorageCount, 0.0);

	divAuxData.tmpCellRank_M = thrust::device_vector<uint>(
			divAuxData.toBeDivideCount, 0);
	divAuxData.tmpDivDirX_M = thrust::device_vector<double>(
			divAuxData.toBeDivideCount, 0);
	divAuxData.tmpDivDirY_M = thrust::device_vector<double>(
			divAuxData.toBeDivideCount, 0);
	divAuxData.tmpCenterPosX_M = thrust::device_vector<double>(
			divAuxData.toBeDivideCount, 0);
	divAuxData.tmpCenterPosY_M = thrust::device_vector<double>(
			divAuxData.toBeDivideCount, 0);

	divAuxData.tmpIsActive1_M = thrust::device_vector<bool>(
			divAuxData.nodeStorageCount, false);
	divAuxData.tmpXPos1_M = thrust::device_vector<double>(
			divAuxData.nodeStorageCount, 0.0);
	divAuxData.tmpYPos1_M = thrust::device_vector<double>(
			divAuxData.nodeStorageCount, 0.0);

	divAuxData.tmpIsActive2_M = thrust::device_vector<bool>(
			divAuxData.nodeStorageCount, false);
	divAuxData.tmpXPos2_M = thrust::device_vector<double>(
			divAuxData.nodeStorageCount, 0.0);
	divAuxData.tmpYPos2_M = thrust::device_vector<double>(
			divAuxData.nodeStorageCount, 0.0);
        //A&A
        divAuxData.tmpHertwigXdir = thrust::device_vector<double>(
			divAuxData.nodeStorageCount, 0.0);
        divAuxData.tmpHertwigYdir = thrust::device_vector<double>(
			divAuxData.nodeStorageCount, 0.0);
        //A&A

// step 2 , continued // copy node info values ready for division /comment A&A
	thrust::counting_iterator<uint> iStart(0);
	thrust::copy_if(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							nodes->getInfoVecs().nodeIsActive.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeLocX.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeLocY.begin()
									+ allocPara_m.bdryNodeCount)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							nodes->getInfoVecs().nodeIsActive.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeLocX.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeLocY.begin()
									+ allocPara_m.bdryNodeCount))
					+ totalNodeCountForActiveCells,
			thrust::make_permutation_iterator(cellInfoVecs.isDividing.begin(),
					make_transform_iterator(iStart,
							DivideFunctor(allocPara_m.maxAllNodePerCell))),
			thrust::make_zip_iterator(
					thrust::make_tuple(divAuxData.tmpIsActive_M.begin(),
							divAuxData.tmpNodePosX_M.begin(),
							divAuxData.tmpNodePosY_M.begin())), isTrue());
// step 3 , continued  //copy cell info values ready for division /comment A&A
	thrust::counting_iterator<uint> iBegin(0);
	thrust::copy_if(
			thrust::make_zip_iterator(
					thrust::make_tuple(iBegin, cellInfoVecs.growthXDir.begin(),
							cellInfoVecs.growthYDir.begin(),
							cellInfoVecs.HertwigXdir.begin(),
							cellInfoVecs.HertwigYdir.begin(),
							cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(iBegin, cellInfoVecs.growthXDir.begin(),
							cellInfoVecs.growthYDir.begin(),
							cellInfoVecs.HertwigXdir.begin(),
							cellInfoVecs.HertwigYdir.begin(),
							cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin()))
					+ allocPara_m.currentActiveCellCount,
			cellInfoVecs.isDividing.begin(),
			thrust::make_zip_iterator(
					thrust::make_tuple(divAuxData.tmpCellRank_M.begin(),
							divAuxData.tmpDivDirX_M.begin(),
							divAuxData.tmpDivDirY_M.begin(),
                                                        divAuxData.tmpHertwigXdir.begin(),
                                                        divAuxData.tmpHertwigYdir.begin(),
							divAuxData.tmpCenterPosX_M.begin(),
							divAuxData.tmpCenterPosY_M.begin())), isTrue());
}

void SceCells::copyCellsEnterMitotic() {
	totalNodeCountForActiveCells = allocPara_m.currentActiveCellCount
			* allocPara_m.maxAllNodePerCell;

	divAuxData.nodeStorageCount = divAuxData.toEnterMitoticCount
			* allocPara_m.maxAllNodePerCell;

	divAuxData.tmpIsActive_M = thrust::device_vector<bool>(
			divAuxData.nodeStorageCount, true);
	divAuxData.tmpNodePosX_M = thrust::device_vector<double>(
			divAuxData.nodeStorageCount, 0.0);
	divAuxData.tmpNodePosY_M = thrust::device_vector<double>(
			divAuxData.nodeStorageCount, 0.0);

	divAuxData.tmpCellRank_M = thrust::device_vector<uint>(
			divAuxData.toEnterMitoticCount, 0);
	divAuxData.tmpDivDirX_M = thrust::device_vector<double>(
			divAuxData.toEnterMitoticCount, 0);
	divAuxData.tmpDivDirY_M = thrust::device_vector<double>(
			divAuxData.toEnterMitoticCount, 0);
	divAuxData.tmpCenterPosX_M = thrust::device_vector<double>(
			divAuxData.toEnterMitoticCount, 0);
	divAuxData.tmpCenterPosY_M = thrust::device_vector<double>(
			divAuxData.toEnterMitoticCount, 0);

	divAuxData.tmpIsActive1_M = thrust::device_vector<bool>(
			divAuxData.nodeStorageCount, false);
	divAuxData.tmpXPos1_M = thrust::device_vector<double>(
			divAuxData.nodeStorageCount, 0.0);
	divAuxData.tmpYPos1_M = thrust::device_vector<double>(
			divAuxData.nodeStorageCount, 0.0);

	divAuxData.tmpIsActive2_M = thrust::device_vector<bool>(
			divAuxData.nodeStorageCount, false);
	divAuxData.tmpXPos2_M = thrust::device_vector<double>(
			divAuxData.nodeStorageCount, 0.0);
	divAuxData.tmpYPos2_M = thrust::device_vector<double>(
			divAuxData.nodeStorageCount, 0.0);

// step 2 , continued // copy node info values ready for division /comment A&A
	thrust::counting_iterator<uint> iStart(0);
	thrust::copy_if(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							nodes->getInfoVecs().nodeIsActive.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeLocX.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeLocY.begin()
									+ allocPara_m.bdryNodeCount)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							nodes->getInfoVecs().nodeIsActive.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeLocX.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeLocY.begin()
									+ allocPara_m.bdryNodeCount))
					+ totalNodeCountForActiveCells,
			thrust::make_permutation_iterator(cellInfoVecs.isEnteringMitotic.begin(),
					make_transform_iterator(iStart,
							DivideFunctor(allocPara_m.maxAllNodePerCell))),
			thrust::make_zip_iterator(
					thrust::make_tuple(divAuxData.tmpIsActive_M.begin(),
							divAuxData.tmpNodePosX_M.begin(),
							divAuxData.tmpNodePosY_M.begin())), isTrue());
// step 3 , continued  //copy cell info values ready for division /comment A&A
	thrust::counting_iterator<uint> iBegin(0);
	thrust::copy_if(
			thrust::make_zip_iterator(
					thrust::make_tuple(iBegin, cellInfoVecs.growthXDir.begin(),
							cellInfoVecs.growthYDir.begin(),
							cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(iBegin, cellInfoVecs.growthXDir.begin(),
							cellInfoVecs.growthYDir.begin(),
							cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin()))
					+ allocPara_m.currentActiveCellCount,
			cellInfoVecs.isEnteringMitotic.begin(),
			thrust::make_zip_iterator(
					thrust::make_tuple(divAuxData.tmpCellRank_M.begin(),
							divAuxData.tmpDivDirX_M.begin(),
							divAuxData.tmpDivDirY_M.begin(),
							divAuxData.tmpCenterPosX_M.begin(),
							divAuxData.tmpCenterPosY_M.begin())), isTrue());
}




void SceCells::createTwoNewCellArr_M() {
	divAuxData.tmp1MemActiveCounts.clear();
	divAuxData.tmp1InternalActiveCounts.clear();
	divAuxData.tmp2MemActiveCounts.clear();
	divAuxData.tmp2InternalActiveCounts.clear();

	//divDebug();

	for (uint i = 0; i < divAuxData.toBeDivideCount; i++) {
		divAuxData.tmp1IntnlVec.clear();
		divAuxData.tmp2IntnlVec.clear();

		vector<CVector> membrNodes;
		vector<CVector> intnlNodes;
		obtainMembrAndIntnlNodes(i, membrNodes, intnlNodes);

		CVector oldCenter = obtainCenter(i);

                //A&A commented
		//CVector divDir = calDivDir_MajorAxis(oldCenter, membrNodes,
		//		lenAlongMajorAxis);
                                              
		/*CVector divDir = calDivDir_MajorAxis(oldCenter, membrNodes,
				lenAlongMajorAxis);*/


		CVector divDir;
		divDir.x = divAuxData.tmpHertwigXdir[i] ; //A&A
		divDir.y = divAuxData.tmpHertwigYdir[i] ; //A&A 
		
		double lenAlongHertwigAxis = calLengthAlongHertwigAxis(divDir, oldCenter, membrNodes);//A&A added
//
 
		std::vector<VecVal> tmp1Membr, tmp2Membr;
		CVector cell1Center, cell2Center;

		obtainTwoNewCenters(oldCenter, divDir, lenAlongHertwigAxis, cell1Center,
				cell2Center);

		prepareTmpVec(i, divDir, oldCenter, tmp1Membr, tmp2Membr);

		processMemVec(tmp1Membr, tmp2Membr);

		shiftIntnlNodesByCellCenter(cell1Center, cell2Center);

		assembleVecForTwoCells(i);
	}
	//divDebug();
}
//A&A
void SceCells::findHertwigAxis() {
	divAuxData.tmp1MemActiveCounts.clear();
	divAuxData.tmp1InternalActiveCounts.clear();
	divAuxData.tmp2MemActiveCounts.clear();
	divAuxData.tmp2InternalActiveCounts.clear();

	//divDebug();

	for (uint i = 0; i < divAuxData.toEnterMitoticCount; i++) {
                uint cellRank = divAuxData.tmpCellRank_M[i];
		vector<CVector> membrNodes;
		vector<CVector> intnlNodes;

		obtainMembrAndIntnlNodes(i, membrNodes, intnlNodes);

		CVector oldCenter = obtainCenter(i);
		double lenAlongMajorAxis;
		CVector divDir = calDivDir_MajorAxis(oldCenter, membrNodes,
				lenAlongMajorAxis);


               cellInfoVecs.HertwigXdir[cellRank]=divDir.x ; 
               cellInfoVecs.HertwigYdir[cellRank]=divDir.y ; 
               
               //std::cout<<cellInfoVecs.HertwigXdir[cellRank]<<"HertwigXdir Thrust" <<std::endl;  
               //std::cout<<cellInfoVecs.HertwigYdir[cellRank]<<"HertwigYdir Thrust" <<std::endl;  

               //std::cout<<divDir.x<<"HertwigXdir " <<std::endl;  
               //std::cout<<divDir.y<<"HertwigYdir " <<std::endl;  


	}
	//divDebug();
}

void SceCells::copyFirstCellArr_M() {
	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;
	for (uint i = 0; i < divAuxData.toBeDivideCount; i++) {
		uint cellRank = divAuxData.tmpCellRank_M[i];
		uint nodeStartIndx = cellRank * maxAllNodePerCell
				+ allocPara_m.bdryNodeCount;
		uint tmpStartIndx = i * maxAllNodePerCell;
		uint tmpEndIndx = (i + 1) * maxAllNodePerCell;
		thrust::constant_iterator<int> noAdhesion(-1), noAdhesion2(-1);
		thrust::copy(
				thrust::make_zip_iterator(
						thrust::make_tuple(divAuxData.tmpXPos1_M.begin(),
								divAuxData.tmpYPos1_M.begin(),
								divAuxData.tmpIsActive1_M.begin(), noAdhesion,
								noAdhesion2)) + tmpStartIndx,
				thrust::make_zip_iterator(
						thrust::make_tuple(divAuxData.tmpXPos1_M.begin(),
								divAuxData.tmpYPos1_M.begin(),
								divAuxData.tmpIsActive1_M.begin(), noAdhesion,
								noAdhesion2)) + tmpEndIndx,
				thrust::make_zip_iterator(
						thrust::make_tuple(
								nodes->getInfoVecs().nodeLocX.begin(),
								nodes->getInfoVecs().nodeLocY.begin(),
								nodes->getInfoVecs().nodeIsActive.begin(),
								nodes->getInfoVecs().nodeAdhereIndex.begin(),
								nodes->getInfoVecs().membrIntnlIndex.begin()))
						+ nodeStartIndx);
		cellInfoVecs.activeIntnlNodeCounts[cellRank] =
				divAuxData.tmp1InternalActiveCounts[i];
		cellInfoVecs.activeMembrNodeCounts[cellRank] =
				divAuxData.tmp1MemActiveCounts[i];
		cellInfoVecs.growthProgress[cellRank] = 0;
		cellInfoVecs.membrGrowProgress[cellRank] = 0.0;
		cellInfoVecs.isRandGrowInited[cellRank] = false;
		cellInfoVecs.lastCheckPoint[cellRank] = 0;
		cellInfoVecs.cell_DppOld[cellRank] = cellInfoVecs.cell_Dpp[cellRank];
		cellInfoVecs.cell_pMadOld[cellRank] = cellInfoVecs.cell_pMad[cellRank];
	}
}

void SceCells::copySecondCellArr_M() {
	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;
	for (uint i = 0; i < divAuxData.toBeDivideCount; i++) {
		
		uint cellRankMother = divAuxData.tmpCellRank_M[i];
		uint cellRank = allocPara_m.currentActiveCellCount + i;
		uint nodeStartIndx = cellRank * maxAllNodePerCell
				+ allocPara_m.bdryNodeCount;
		uint tmpStartIndx = i * maxAllNodePerCell;
		uint tmpEndIndx = (i + 1) * maxAllNodePerCell;
		thrust::constant_iterator<int> noAdhesion(-1), noAdhesion2(-1);
		thrust::copy(
				thrust::make_zip_iterator(
						thrust::make_tuple(divAuxData.tmpXPos2_M.begin(),
								divAuxData.tmpYPos2_M.begin(),
								divAuxData.tmpIsActive2_M.begin(), noAdhesion,
								noAdhesion2)) + tmpStartIndx,
				thrust::make_zip_iterator(
						thrust::make_tuple(divAuxData.tmpXPos2_M.begin(),
								divAuxData.tmpYPos2_M.begin(),
								divAuxData.tmpIsActive2_M.begin(), noAdhesion,
								noAdhesion2)) + tmpEndIndx,
				thrust::make_zip_iterator(
						thrust::make_tuple(
								nodes->getInfoVecs().nodeLocX.begin(),
								nodes->getInfoVecs().nodeLocY.begin(),
								nodes->getInfoVecs().nodeIsActive.begin(),
								nodes->getInfoVecs().nodeAdhereIndex.begin(),
								nodes->getInfoVecs().membrIntnlIndex.begin()))
						+ nodeStartIndx);
		cellInfoVecs.activeIntnlNodeCounts[cellRank] =
				divAuxData.tmp2InternalActiveCounts[i];
		cellInfoVecs.activeMembrNodeCounts[cellRank] =
				divAuxData.tmp2MemActiveCounts[i];
		cellInfoVecs.growthProgress[cellRank] = 0;
		cellInfoVecs.membrGrowProgress[cellRank] = 0;
		cellInfoVecs.isRandGrowInited[cellRank] = false;
		cellInfoVecs.lastCheckPoint[cellRank] = 0;
		cellInfoVecs.cell_DppOld[cellRank] = cellInfoVecs.cell_Dpp[cellRankMother];
		cellInfoVecs.cell_Dpp[cellRank]    = cellInfoVecs.cell_Dpp[cellRankMother];
		cellInfoVecs.cell_Tkv[cellRank]    = cellInfoVecs.cell_Tkv[cellRankMother];	//Alireza
		cellInfoVecs.cell_DppTkv[cellRank]    = cellInfoVecs.cell_DppTkv[cellRankMother];	//Alireza
		cellInfoVecs.cell_pMad[cellRank]    = cellInfoVecs.cell_pMad[cellRankMother];	//Alireza
		cellInfoVecs.cell_pMadOld[cellRank]    = cellInfoVecs.cell_pMad[cellRankMother];   //Alireza
	}
}

//AAMIRI
/*
void SceCells::removeCellArr_M() {
	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;
	for (uint i = 0; i < divAuxData.toBeDivideCount; i++) {
		uint cellRank = divAuxData.tmpCellRank_M[i];
		uint nodeStartIndx = cellRank * maxAllNodePerCell
				+ allocPara_m.bdryNodeCount;
		uint tmpStartIndx = i * maxAllNodePerCell;
		uint tmpEndIndx = (i + 1) * maxAllNodePerCell;
		thrust::constant_iterator<int> noAdhesion(-1), noAdhesion2(-1);
		thrust::copy(
				thrust::make_zip_iterator(
						thrust::make_tuple(divAuxData.tmpXPos1_M.begin(),
								divAuxData.tmpYPos1_M.begin(),
								divAuxData.tmpIsActive1_M.begin(), noAdhesion,
								noAdhesion2)) + tmpStartIndx,
				thrust::make_zip_iterator(
						thrust::make_tuple(divAuxData.tmpXPos1_M.begin(),
								divAuxData.tmpYPos1_M.begin(),
								divAuxData.tmpIsActive1_M.begin(), noAdhesion,
								noAdhesion2)) + tmpEndIndx,
				thrust::make_zip_iterator(
						thrust::make_tuple(
								nodes->getInfoVecs().nodeLocX.begin(),
								nodes->getInfoVecs().nodeLocY.begin(),
								nodes->getInfoVecs().nodeIsActive.begin(),
								nodes->getInfoVecs().nodeAdhereIndex.begin(),
								nodes->getInfoVecs().membrIntnlIndex.begin()))
						+ nodeStartIndx);
		cellInfoVecs.activeIntnlNodeCounts[cellRank] =
				divAuxData.tmp1InternalActiveCounts[i];
		cellInfoVecs.activeMembrNodeCounts[cellRank] =
				divAuxData.tmp1MemActiveCounts[i];
		cellInfoVecs.growthProgress[cellRank] = 0;
		cellInfoVecs.membrGrowProgress[cellRank] = 0.0;
		cellInfoVecs.isRandGrowInited[cellRank] = false;
		cellInfoVecs.lastCheckPoint[cellRank] = 0;
	}
}

*/

void SceCells::updateActiveCellCount_M() {
	allocPara_m.currentActiveCellCount = allocPara_m.currentActiveCellCount
			+ divAuxData.toBeDivideCount;
	nodes->setActiveCellCount(allocPara_m.currentActiveCellCount);
}

//AAMIRI
/*
void SceCells::updateActiveCellCountAfterRemoval_M() {
	allocPara_m.currentActiveCellCount = allocPara_m.currentActiveCellCount
			+ divAuxData.toBeDivideCount;
	nodes->setActiveCellCount(allocPara_m.currentActiveCellCount);
}

*/

void SceCells::markIsDivideFalse_M() {
	thrust::fill(cellInfoVecs.isDividing.begin(),
			cellInfoVecs.isDividing.begin()
					+ allocPara_m.currentActiveCellCount, false);
}

void SceCells::adjustNodeVel_M() {
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin()))
					+ allocPara_m.bdryNodeCount + totalNodeCountForActiveCells,
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin())),
			ForceZero());
}

void SceCells::moveNodes_M() {
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin()))
					+ totalNodeCountForActiveCells + allocPara_m.bdryNodeCount,
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeLocX.begin(),
							nodes->getInfoVecs().nodeLocY.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeLocX.begin(),
							nodes->getInfoVecs().nodeLocY.begin())),
//Ali		SaxpyFunctorDim2(dt));
			SaxpyFunctorDim2_Damp(dt,Damp_Coef));   //Ali
}



//Ali      // This function is written to assigned different damping coefficients to cells, therefore the boundary cells can have more damping


/*

void SceCells::moveNodes_BC_M() {
	thrust::counting_iterator<uint> iBegin2(0); 
	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;
    uint maxMemNodePerCell = allocPara_m.maxMembrNodePerCell;
    double* nodeLocXAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeLocX[0]));
    double* nodeLocYAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeLocY[0]));
    bool* nodeIsActiveAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeIsActive[0]));
	int timeStep = curTime/dt;

	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.Cell_Damp.begin(),
									make_transform_iterator(iBegin2,
											DivideFunctor(maxAllNodePerCell))),
                            nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin(),
							thrust::make_permutation_iterator(
                                    cellInfoVecs.centerCoordX.begin(),
                                    make_transform_iterator(iBegin2,
                                            DivideFunctor(maxAllNodePerCell))),
                            thrust::make_permutation_iterator(
                                    cellInfoVecs.centerCoordY.begin(),
                                    make_transform_iterator(iBegin2,
                                            DivideFunctor(maxAllNodePerCell))),
							make_transform_iterator(iBegin2,
                                    DivideFunctor(maxAllNodePerCell)),
                            make_transform_iterator(iBegin2,
                                    ModuloFunctor(maxAllNodePerCell)),
							thrust::make_permutation_iterator(
                                    cellInfoVecs.activeMembrNodeCounts.begin(),
                                    make_transform_iterator(iBegin2,
                                     DivideFunctor(maxAllNodePerCell)))
							)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.Cell_Damp.begin(),
									make_transform_iterator(iBegin2,
											DivideFunctor(maxAllNodePerCell))),
                            nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin(),
						    thrust::make_permutation_iterator(
                                    cellInfoVecs.centerCoordX.begin(),
                                    make_transform_iterator(iBegin2,
                                            DivideFunctor(maxAllNodePerCell))),
                            thrust::make_permutation_iterator(
                                    cellInfoVecs.centerCoordY.begin(),
                                    make_transform_iterator(iBegin2,
                                            DivideFunctor(maxAllNodePerCell))),
							make_transform_iterator(iBegin2,
                                    DivideFunctor(maxAllNodePerCell)),
                            make_transform_iterator(iBegin2,
                                    ModuloFunctor(maxAllNodePerCell)),
							thrust::make_permutation_iterator(
                                    cellInfoVecs.activeMembrNodeCounts.begin(),
                                    make_transform_iterator(iBegin2,
                                     DivideFunctor(maxAllNodePerCell)))
							))
					+ totalNodeCountForActiveCells + allocPara_m.bdryNodeCount,
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeLocX.begin(),
							nodes->getInfoVecs().nodeLocY.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeLocX.begin(),
							nodes->getInfoVecs().nodeLocY.begin())),
			SaxpyFunctorDim2_BC_Damp(dt,nodeLocXAddr, nodeLocYAddr,nodeIsActiveAddr,maxAllNodePerCell,maxMemNodePerCell,timeStep));  
}
*/



void SceCells::moveNodes_BC_M() {
	thrust::counting_iterator<uint> iBegin2(0); 
	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;

	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.Cell_Damp.begin(),
									make_transform_iterator(iBegin2,
											DivideFunctor(maxAllNodePerCell))),
                            nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.Cell_Damp.begin(),
									make_transform_iterator(iBegin2,
											DivideFunctor(maxAllNodePerCell))),
                            nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin()))
					+ totalNodeCountForActiveCells + allocPara_m.bdryNodeCount,
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeLocX.begin(),
							nodes->getInfoVecs().nodeLocY.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeLocX.begin(),
							nodes->getInfoVecs().nodeLocY.begin())),
			SaxpyFunctorDim2_BC_Damp(dt));  
}





void SceCells::applyMemForce_M() {
	totalNodeCountForActiveCells = allocPara_m.currentActiveCellCount
			* allocPara_m.maxAllNodePerCell;
	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;
	thrust::counting_iterator<uint> iBegin(0), iBegin1(0); 
 //Ali
        thrust::fill(cellInfoVecs.Cell_Time.begin(),cellInfoVecs.Cell_Time.begin() +allocPara_m.currentActiveCellCount,curTime);
        
       //Ali 
        /*
        thrust::device_vector<double>::iterator  MinX_Itr=thrust::min_element(nodes->getInfoVecs().nodeLocX.begin()+ allocPara_m.bdryNodeCount,
                                              nodes->getInfoVecs().nodeLocX.begin()+ allocPara_m.bdryNodeCount+ totalNodeCountForActiveCells) ;
        thrust::device_vector<double>::iterator  MaxX_Itr=thrust::max_element(nodes->getInfoVecs().nodeLocX.begin()+ allocPara_m.bdryNodeCount,
                                              nodes->getInfoVecs().nodeLocX.begin()+ allocPara_m.bdryNodeCount+ totalNodeCountForActiveCells) ;
        thrust::device_vector<double>::iterator  MinY_Itr=thrust::min_element(nodes->getInfoVecs().nodeLocY.begin()+ allocPara_m.bdryNodeCount,
                                              nodes->getInfoVecs().nodeLocY.begin()+ allocPara_m.bdryNodeCount+ totalNodeCountForActiveCells) ;
        thrust::device_vector<double>::iterator  MaxY_Itr=thrust::max_element(nodes->getInfoVecs().nodeLocY.begin()+ allocPara_m.bdryNodeCount,
                                              nodes->getInfoVecs().nodeLocY.begin()+ allocPara_m.bdryNodeCount+ totalNodeCountForActiveCells) ;
        Tisu_MinX= *MinX_Itr ; 
        Tisu_MaxX= *MaxX_Itr ; 
        Tisu_MinY= *MinY_Itr ; 
        Tisu_MaxY= *MaxY_Itr ;  
        */
        //cout<< "# of boundary nodes"<< allocPara_m.bdryNodeCount<<endl ;
        //cout<< "# of total active nodes"<<totalNodeCountForActiveCells <<endl ;

        //cout<<"The minimum location in X in applyMemForce_M is="<<Tisu_MinX<< endl;  
        //cout<<"The maximum location in X in applyMemForce_M is="<<Tisu_MaxX<< endl;  
        //cout<<"The minimum location in Y in applyMemForce_M is="<<Tisu_MinY<< endl;  
        //cout<<"The maximum location in Y in applyMemForce_M is="<<Tisu_MaxY<< endl;  
        //Ali 
	double* nodeLocXAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocX[0]));
	double* nodeLocYAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocY[0]));
	bool* nodeIsActiveAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeIsActive[0]));

//	double* myosinLevelAddr = thrust::raw_pointer_cast(
//		&(nodes->getInfoVecs().myosinLevel[0])); // pointer to the vector storing myosin level, // will not change the myosin level so not using pointer?

	double grthPrgrCriVal_M = growthAuxData.grthPrgrCriVal_M_Ori; // for now constant  //growthAuxData.grthProgrEndCPU
		//	- growthAuxData.prolifDecay
		//			* (growthAuxData.grthProgrEndCPU
		//					- growthAuxData.grthPrgrCriVal_M_Ori);

	// compute angular energy derived bending force
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.growthProgress.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.activeMembrNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.centerCoordX.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
                            thrust::make_permutation_iterator(
									cellInfoVecs.Cell_Time.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),

							make_transform_iterator(iBegin,
									DivideFunctor(maxAllNodePerCell)),
							make_transform_iterator(iBegin,
									ModuloFunctor(maxAllNodePerCell)),
							nodes->getInfoVecs().nodeLocX.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeLocY.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeVelX.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeVelY.begin()
									+ allocPara_m.bdryNodeCount)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.growthProgress.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.activeMembrNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.centerCoordX.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
                            thrust::make_permutation_iterator(
									cellInfoVecs.Cell_Time.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							make_transform_iterator(iBegin,
									DivideFunctor(maxAllNodePerCell)),
							make_transform_iterator(iBegin,
									ModuloFunctor(maxAllNodePerCell)),
							nodes->getInfoVecs().nodeLocX.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeLocY.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeVelX.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeVelY.begin()
									+ allocPara_m.bdryNodeCount))
					+ totalNodeCountForActiveCells,
			thrust::make_zip_iterator(
				thrust::make_tuple(
							nodes->getInfoVecs().actinForceX.begin()
							+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().actinForceY.begin()
							+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().myosinLevel.begin()
							+ allocPara_m.bdryNodeCount,
							thrust::make_permutation_iterator(
									cellInfoVecs.centerCoordY.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.cell_Type.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell)))
							)),
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin(),
							nodes->getInfoVecs().membrTensionMag.begin(),
							nodes->getInfoVecs().membrTenMagRi.begin(),
							nodes->getInfoVecs().membrLinkRiMidX.begin(),
							nodes->getInfoVecs().membrLinkRiMidY.begin(),
							nodes->getInfoVecs().membrBendLeftX.begin(),
							nodes->getInfoVecs().membrBendLeftY.begin(),
							nodes->getInfoVecs().membrBendRightX.begin(),
							nodes->getInfoVecs().membrBendRightY.begin()))
					+ allocPara_m.bdryNodeCount,
			AddMemAngDevice(allocPara_m.bdryNodeCount, maxAllNodePerCell,
					nodeLocXAddr, nodeLocYAddr, nodeIsActiveAddr, grthPrgrCriVal_M)); // , nodeActinXAddr, nodeActinYAddr



/**Ali Comment start

	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.activeMembrNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							make_transform_iterator(iBegin,
									DivideFunctor(maxAllNodePerCell)),
							make_transform_iterator(iBegin,
									ModuloFunctor(maxAllNodePerCell)),
							nodes->getInfoVecs().nodeLocX.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeLocY.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeVelX.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeVelY.begin()
									+ allocPara_m.bdryNodeCount)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.activeMembrNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							make_transform_iterator(iBegin,
									DivideFunctor(maxAllNodePerCell)),
							make_transform_iterator(iBegin,
									ModuloFunctor(maxAllNodePerCell)),
							nodes->getInfoVecs().nodeLocX.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeLocY.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeVelX.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeVelY.begin()
									+ allocPara_m.bdryNodeCount))
					+ totalNodeCountForActiveCells,
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin(),
							nodes->getInfoVecs().membrTensionMag.begin(),
							nodes->getInfoVecs().membrTenMagRi.begin(),
							nodes->getInfoVecs().membrLinkRiMidX.begin(),
							nodes->getInfoVecs().membrLinkRiMidY.begin(),
							nodes->getInfoVecs().membrBendLeftX.begin(),
							nodes->getInfoVecs().membrBendLeftY.begin(),
							nodes->getInfoVecs().membrBendRightX.begin(),
							nodes->getInfoVecs().membrBendRightY.begin()))
					+ allocPara_m.bdryNodeCount,
			AddMemAngDeviceNotUsed(allocPara_m.bdryNodeCount, maxAllNodePerCell,
					nodeLocXAddr, nodeLocYAddr, nodeIsActiveAddr));

**/
// bending force 
	double* bendLeftXAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().membrBendLeftX[0]));
	double* bendLeftYAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().membrBendLeftY[0]));
	double* bendRightXAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().membrBendRightX[0]));
	double* bendRightYAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().membrBendRightY[0]));

	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.activeMembrNodeCounts.begin(),
									make_transform_iterator(iBegin1,
											DivideFunctor(maxAllNodePerCell))),
							make_transform_iterator(iBegin1,
									DivideFunctor(maxAllNodePerCell)),
							make_transform_iterator(iBegin1,
									ModuloFunctor(maxAllNodePerCell)),
							nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.activeMembrNodeCounts.begin(),
									make_transform_iterator(iBegin1,
											DivideFunctor(maxAllNodePerCell))),
							make_transform_iterator(iBegin1,
									DivideFunctor(maxAllNodePerCell)),
							make_transform_iterator(iBegin1,
									ModuloFunctor(maxAllNodePerCell)),
							nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin()))
					+ totalNodeCountForActiveCells,
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin())),
			AddMembrBendDevice(maxAllNodePerCell, nodeIsActiveAddr, bendLeftXAddr,
					bendLeftYAddr, bendRightXAddr, bendRightYAddr));


// add the force accounting for the volume preservation

	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator( // source[map[0]], source[map[1]], etc...
									cellInfoVecs.activeMembrNodeCounts.begin(),
									make_transform_iterator(iBegin1,
											DivideFunctor(maxAllNodePerCell))), 
							make_transform_iterator(iBegin1,
									DivideFunctor(maxAllNodePerCell)),
							make_transform_iterator(iBegin1,
									ModuloFunctor(maxAllNodePerCell)),
							thrust::make_permutation_iterator(
									cellInfoVecs.cellAreaInit.begin(),
									make_transform_iterator(iBegin1,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.cellAreaVec.begin(),
									make_transform_iterator(iBegin1,
											DivideFunctor(maxAllNodePerCell))),
							nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.activeMembrNodeCounts.begin(),
									make_transform_iterator(iBegin1,
											DivideFunctor(maxAllNodePerCell))),
							make_transform_iterator(iBegin1,
									DivideFunctor(maxAllNodePerCell)),
							make_transform_iterator(iBegin1,
									ModuloFunctor(maxAllNodePerCell)),
							thrust::make_permutation_iterator(
									cellInfoVecs.cellAreaInit.begin(),
									make_transform_iterator(iBegin1,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.cellAreaVec.begin(),
									make_transform_iterator(iBegin1,
											DivideFunctor(maxAllNodePerCell))),
							nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin()))
					+ totalNodeCountForActiveCells,
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin())),
			AddMembrVolDevice(maxAllNodePerCell, nodeIsActiveAddr,nodeLocXAddr, nodeLocYAddr));

}


//AAMIRI
// 
void SceCells::findTangentAndNormal_M() {

	uint totalNodeCountForActiveCells = allocPara_m.currentActiveCellCount
			* allocPara_m.maxAllNodePerCell;

	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;

	thrust::counting_iterator<uint> iBegin(0), iBegin1(0);

	double* nodeLocXAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocX[0]));
	double* nodeLocYAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocY[0]));
	bool* nodeIsActiveAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeIsActive[0]));

	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.activeMembrNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							make_transform_iterator(iBegin,
									DivideFunctor(maxAllNodePerCell)),
							make_transform_iterator(iBegin1,
									ModuloFunctor(maxAllNodePerCell)),
							nodes->getInfoVecs().nodeF_MI_M_x.begin(), //AliE
							nodes->getInfoVecs().nodeF_MI_M_y.begin(), //AliE
 							nodes->getInfoVecs().nodeF_MI_M_T.begin(), //AliE
							nodes->getInfoVecs().nodeF_MI_M_N.begin(), //AliE
							nodes->getInfoVecs().nodeCurvature.begin(),
							nodes->getInfoVecs().nodeExtForceX.begin(),
							nodes->getInfoVecs().nodeExtForceY.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.activeMembrNodeCounts.begin(),
									make_transform_iterator(iBegin1,
											DivideFunctor(maxAllNodePerCell))),
							make_transform_iterator(iBegin1,
									DivideFunctor(maxAllNodePerCell)),
							make_transform_iterator(iBegin1,
									ModuloFunctor(maxAllNodePerCell)),
							nodes->getInfoVecs().nodeF_MI_M_x.begin(), //AliE
							nodes->getInfoVecs().nodeF_MI_M_y.begin(), //AliE
 							nodes->getInfoVecs().nodeF_MI_M_T.begin(), //AliE
							nodes->getInfoVecs().nodeF_MI_M_N.begin(), //AliE
							nodes->getInfoVecs().nodeCurvature.begin(),
							nodes->getInfoVecs().nodeExtForceX.begin(),
							nodes->getInfoVecs().nodeExtForceY.begin()))
					+ totalNodeCountForActiveCells,
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeF_MI_M_T.begin(),
							nodes->getInfoVecs().nodeF_MI_M_N.begin(), 
							nodes->getInfoVecs().nodeCurvature.begin(),
							nodes->getInfoVecs().nodeExtForceTangent.begin(),
							nodes->getInfoVecs().nodeExtForceNormal.begin(),
							nodes->getInfoVecs().membrDistToRi.begin())),
			CalCurvatures(maxAllNodePerCell, nodeIsActiveAddr, nodeLocXAddr, nodeLocYAddr));

}





void SceCells::runAblationTest(AblationEvent& ablEvent) {
	for (uint i = 0; i < ablEvent.ablationCells.size(); i++) {
		int cellRank = ablEvent.ablationCells[i].cellNum;
		std::vector<uint> removeSeq = ablEvent.ablationCells[i].nodeNums;
		cellInfoVecs.activeNodeCountOfThisCell[cellRank] =
				cellInfoVecs.activeNodeCountOfThisCell[cellRank]
						- removeSeq.size();
		nodes->removeNodes(cellRank, removeSeq);
	}
}

// compute cell center location
void SceCells::computeCenterPos_M() {
	totalNodeCountForActiveCells = allocPara_m.currentActiveCellCount
			* allocPara_m.maxAllNodePerCell;
	thrust::counting_iterator<uint> iBegin(0);
	thrust::counting_iterator<uint> countingEnd(totalNodeCountForActiveCells);

	//uint totalMembrActiveNodeCount = thrust::reduce(
	//		cellInfoVecs.activeMembrNodeCounts.begin(),
	//		cellInfoVecs.activeMembrNodeCounts.begin()
	//				+ allocPara_m.currentActiveCellCount);
	uint totalIntnlActiveNodeCount = thrust::reduce(
			cellInfoVecs.activeIntnlNodeCounts.begin(),
			cellInfoVecs.activeIntnlNodeCounts.begin()
					+ allocPara_m.currentActiveCellCount);

	thrust::copy_if(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							make_transform_iterator(iBegin,
									DivideFunctor(
											allocPara_m.maxAllNodePerCell)),
							nodes->getInfoVecs().nodeLocX.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeLocY.begin()
									+ allocPara_m.bdryNodeCount)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							make_transform_iterator(iBegin,
									DivideFunctor(
											allocPara_m.maxAllNodePerCell)),
							nodes->getInfoVecs().nodeLocX.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeLocY.begin()
									+ allocPara_m.bdryNodeCount))
					+ totalNodeCountForActiveCells,
			thrust::make_zip_iterator(
					thrust::make_tuple(
							nodes->getInfoVecs().nodeIsActive.begin(),
							nodes->getInfoVecs().nodeCellType.begin()))
					+ allocPara_m.bdryNodeCount,
			thrust::make_zip_iterator(
					thrust::make_tuple(cellNodeInfoVecs.cellRanks.begin(),
							cellNodeInfoVecs.activeXPoss.begin(),
							cellNodeInfoVecs.activeYPoss.begin())),
			ActiveAndIntnl());

	thrust::reduce_by_key(cellNodeInfoVecs.cellRanks.begin(),
			cellNodeInfoVecs.cellRanks.begin() + totalIntnlActiveNodeCount,
			thrust::make_zip_iterator(
					thrust::make_tuple(cellNodeInfoVecs.activeXPoss.begin(),
							cellNodeInfoVecs.activeYPoss.begin())),
			cellInfoVecs.cellRanksTmpStorage.begin(),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin())),
			thrust::equal_to<uint>(), CVec2Add());
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin()))
					+ allocPara_m.currentActiveCellCount,
			cellInfoVecs.activeIntnlNodeCounts.begin(),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin())), CVec2Divide());
}



void SceCells::computeCellRadius() {
	uint activeCellCount = allocPara_m.currentActiveCellCount;

	// double* myosinLevelAddr = thrust::raw_pointer_cast(
	//	&(nodes->getInfoVecs().myosinLevel[0])); // pointer to the vector storing myosin level
	
	// cell center, cell rank, maxmembranenode, 
	double* nodeLocXAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocX[0]));
	double* nodeLocYAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocY[0]));
	bool* nodeIsActiveAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeIsActive[0])); // 
//	double* cellCenterXAddr = thrust::raw_pointer_cast(
//            &(cellInfoVecs.centerCoordX[0]));
//	double* cellCenterYAddr = thrust::raw_pointer_cast(
//            &(cellInfoVecs.centerCoordY[0]));
	uint maxMembrNode = allocPara_m.maxMembrNodePerCell;
	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;

	thrust::counting_iterator<uint> iBegin(0);
	thrust::counting_iterator<uint> iEnd(activeCellCount); // make sure not iterate on inactive cells already 
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(iBegin,
							cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin(),
							cellInfoVecs.activeMembrNodeCounts.begin()
							)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							iEnd,
							cellInfoVecs.centerCoordX.begin() + activeCellCount,
							cellInfoVecs.centerCoordY.begin() + activeCellCount,
							cellInfoVecs.activeMembrNodeCounts.begin() + activeCellCount
							)),
			cellInfoVecs.cellRadius.begin(),
			calCellRadius(maxAllNodePerCell,maxMembrNode,nodeLocXAddr,nodeLocYAddr,nodeIsActiveAddr));
}







void SceCells::BC_Imp_M() {

        /*
	thrust::device_vector<double>::iterator  MinX_Itr=thrust::min_element(
                                       cellInfoVecs.centerCoordX.begin(),
                                       cellInfoVecs.centerCoordX.begin()+allocPara_m.currentActiveCellCount ) ;

        thrust::device_vector<double>::iterator  MaxX_Itr=thrust::max_element(
                                       cellInfoVecs.centerCoordX.begin(),
                                       cellInfoVecs.centerCoordX.begin()+allocPara_m.currentActiveCellCount ) ;

        thrust::device_vector<double>::iterator  MinY_Itr=thrust::min_element(
                                       cellInfoVecs.centerCoordY.begin(),
                                       cellInfoVecs.centerCoordY.begin()+allocPara_m.currentActiveCellCount ) ;

        thrust::device_vector<double>::iterator  MaxY_Itr=thrust::max_element(
                                       cellInfoVecs.centerCoordY.begin(),
                                       cellInfoVecs.centerCoordY.begin()+allocPara_m.currentActiveCellCount ) ;
        MinX= *MinX_Itr ; 
        MaxX= *MaxX_Itr ; 
        MinY= *MinY_Itr ; 
        MaxY= *MaxY_Itr ;
 */
 
        //cout<<"The minimum location of cell cetners in Y in  BC_Imp_M is="<<Tisu_MinX<< endl;  
        //cout<<"The maximum location of cell centers in Y in  BC_Imp_M is="<<Tisu_MaxX<< endl;  
        //cout<<"The minimum location of cell centers in Y in  BC_Imp_M  is="<<Tisu_MinY<< endl;  
        //cout<<"The maximum location of cell centers in Y in  BC_Imp_M  is="<<Tisu_MaxY<< endl;  
/**	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.centerCoordX.begin(),
							   cellInfoVecs.centerCoordY.begin())
						           ),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.centerCoordX.begin(),
							   cellInfoVecs.centerCoordY.begin())) + allocPara_m.currentActiveCellCount,
			thrust::make_zip_iterator(   
					thrust::make_tuple(cellInfoVecs.centerCoordX.begin(),
                                                           cellInfoVecs.centerCoordY.begin())),
			BC_Tissue_Damp(Damp_Coef)) ; 


**/
        int  NumActCells=allocPara_m.currentActiveCellCount ; 

        //Ali 
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.centerCoordX.begin(),
							    cellInfoVecs.centerCoordY.begin(),
                                cellInfoVecs.Cell_Damp.begin())
						           ),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.centerCoordX.begin(),
							   cellInfoVecs.centerCoordY.begin(),
							   cellInfoVecs.Cell_Damp.begin())) + allocPara_m.currentActiveCellCount,
			thrust::make_zip_iterator(   
					thrust::make_tuple(cellInfoVecs.centerCoordX.begin(),
                                        cellInfoVecs.Cell_Damp.begin())),
			BC_Tissue_Damp(Tisu_MinX,Tisu_MaxX,Tisu_MinY,Tisu_MaxY,Damp_Coef,NumActCells)) ; 


/**void SceCells::randomizeGrowth() {
	thrust::counting_iterator<uint> countingBegin(0);
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.growthXDir.begin(),
							cellInfoVecs.growthYDir.begin(),
							cellInfoVecs.isRandGrowInited.begin(),
							countingBegin)),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.growthXDir.begin(),
							cellInfoVecs.growthYDir.begin(),
							cellInfoVecs.isRandGrowInited.begin(),
							countingBegin)) + allocPara.currentActiveCellCount,
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.growthSpeed.begin(),
							cellInfoVecs.growthXDir.begin(),
							cellInfoVecs.growthYDir.begin(),
							cellInfoVecs.isRandGrowInited.begin())),
			AssignRandIfNotInit(growthAuxData.randomGrowthSpeedMin,
					growthAuxData.randomGrowthSpeedMax,
					allocPara.currentActiveCellCount,
					growthAuxData.randGenAuxPara));
}



**/


}




void SceCells::growAtRandom_M(double dt) {
	totalNodeCountForActiveCells = allocPara_m.currentActiveCellCount
			* allocPara_m.maxAllNodePerCell;

	randomizeGrowth_M();

	updateGrowthProgress_M();

	decideIsScheduleToGrow_M();

	//computeCellTargetLength_M();

	//computeDistToCellCenter_M();

	//findMinAndMaxDistToCenter_M();

	//computeLenDiffExpCur_M();

	//stretchCellGivenLenDiff_M();

	addPointIfScheduledToGrow_M();

	// addPointDueToActin(); 

	//decideIsScheduleToShrink_M();// AAMIRI May5

	//delPointIfScheduledToGrow_M();//AAMIRI - commented out on June20

	adjustGrowthInfo_M();
}

void SceCells::divide2D_M() {
	bool isDivisionPresent = decideIfGoingToDivide_M();
    bool isEnteringMitotic = decideIfAnyCellEnteringMitotic() ; //A&A
        
        //A&A
	if (isEnteringMitotic){
        std::cout<< "I am in EnteringMitotic"<< std::endl; 
	copyCellsEnterMitotic();
	findHertwigAxis();
	}
        //A&A
	std::cout<< "Is there a division present?"<< isDivisionPresent<<std::endl; 
	if (!isDivisionPresent) {
		return;
	}
	//aniDebug = true;
	copyCellsPreDivision_M();
	createTwoNewCellArr_M();
	copyFirstCellArr_M();
	copySecondCellArr_M();
	updateActiveCellCount_M();
	markIsDivideFalse_M();
	//divDebug();
}

void SceCells::distributeCellGrowthProgress_M() {
	uint maxIntnlNodePerFollower = globalConfigVars.getConfigValue(
            "MaxIntnlNodeCountPerFollower").toInt();
	totalNodeCountForActiveCells = allocPara_m.currentActiveCellCount
			* allocPara_m.maxAllNodePerCell;
	thrust::counting_iterator<uint> countingBegin(0);
	thrust::counting_iterator<uint> countingEnd(totalNodeCountForActiveCells);
	
	thrust::copy(
			thrust::make_permutation_iterator(
					cellInfoVecs.growthProgress.begin(),
					make_transform_iterator(countingBegin,
							DivideFunctor(allocPara_m.maxAllNodePerCell))),
			thrust::make_permutation_iterator(
					cellInfoVecs.growthProgress.begin(),
					make_transform_iterator(countingEnd,
							DivideFunctor(allocPara_m.maxAllNodePerCell))),
			nodes->getInfoVecs().nodeGrowPro.begin()
					+ allocPara_m.bdryNodeCount);
    std::cout << "the value of init time stage in distributeCellGrowthProgress_M is"<< InitTimeStage << std:: endl ; 
			if (curTime <= InitTimeStage+dt)//AAMIRI   /A & A 
				thrust::copy(
					cellInfoVecs.growthProgress.begin(),
					cellInfoVecs.growthProgress.end(),
					cellInfoVecs.lastCheckPoint.begin()
				);
}




void SceCells::distributeCellActivationLevel_M() {
	totalNodeCountForActiveCells = allocPara_m.currentActiveCellCount
			* allocPara_m.maxAllNodePerCell;
	thrust::counting_iterator<uint> countingBegin(0);
	thrust::counting_iterator<uint> countingEnd(totalNodeCountForActiveCells);
	
	thrust::copy(
			thrust::make_permutation_iterator(
					cellInfoVecs.activationLevel.begin(),
					make_transform_iterator(countingBegin,
							DivideFunctor(allocPara_m.maxAllNodePerCell))),
			thrust::make_permutation_iterator(
					cellInfoVecs.activationLevel.begin(),
					make_transform_iterator(countingEnd,
							DivideFunctor(allocPara_m.maxAllNodePerCell))),
			nodes->getInfoVecs().nodeActLevel.begin()
					+ allocPara_m.bdryNodeCount);
    std::cout << "Done distribute CellActivationLevel_M "<< std:: endl; 
}




void SceCells::distributeCell_Type_M() {
	totalNodeCountForActiveCells = allocPara_m.currentActiveCellCount
			* allocPara_m.maxAllNodePerCell;
	thrust::counting_iterator<uint> countingBegin(0);
	thrust::counting_iterator<uint> countingEnd(totalNodeCountForActiveCells);
	
	thrust::copy(
			thrust::make_permutation_iterator(
					cellInfoVecs.cell_Type.begin(),
					make_transform_iterator(countingBegin,
							DivideFunctor(allocPara_m.maxAllNodePerCell))),
			thrust::make_permutation_iterator(
					cellInfoVecs.cell_Type.begin(),
					make_transform_iterator(countingEnd,
							DivideFunctor(allocPara_m.maxAllNodePerCell))),
			nodes->getInfoVecs().nodeCell_Type.begin()
					+ allocPara_m.bdryNodeCount);
    std::cout << "Done distribute Cell_Type_M "<< std:: endl; 
}



void SceCells::allComponentsMove_M() {
	//moveNodes_M();  //Ali 
        moveNodes_BC_M();      //Ali
}


//Ali modified this function to introduce differential proliferation rates
void SceCells::randomizeGrowth_M() {

                
        double CntrTisuX=0.5*(Tisu_MaxX-Tisu_MinX) ; 
        double CntrTisuY=0.5*(Tisu_MaxY-Tisu_MinY) ; 
        
        //cout<<"The minimum location of cell cetners in Y in  randomizeGrowth_M is="<<Tisu_MinX<< endl;  
        //cout<<"The maximum location of cell centers in Y in  randomizeGrowth_M is="<<Tisu_MaxX<< endl;  
        //cout<<"The minimum location of cell centers in Y in randomizeGrowth_M  is="<<Tisu_MinY<< endl;  
        //cout<<"The maximum location of cell centers in Y in randomizeGrowth_M  is="<<Tisu_MaxY<< endl;  


	uint seed = time(NULL);
	thrust::counting_iterator<uint> countingBegin(0);
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.growthSpeed.begin(),
						           cellInfoVecs.centerCoordX.begin(),
							   cellInfoVecs.centerCoordY.begin(),
							   cellInfoVecs.isRandGrowInited.begin(),
							   countingBegin)),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.growthSpeed.begin(),
							cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin(),
							cellInfoVecs.isRandGrowInited.begin(),
							countingBegin))
					+ allocPara_m.currentActiveCellCount,
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.growthSpeed.begin(),
							cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin(),
							   cellInfoVecs.isRandGrowInited.begin())),
			RandomizeGrow_M(CntrTisuX,CntrTisuY,Tisu_R,growthAuxData.randomGrowthSpeedMin,
					growthAuxData.randomGrowthSpeedMax, seed));  
}

void SceCells::updateGrowthProgress_M() {
        thrust::copy(cellInfoVecs.growthProgress.begin(),
			cellInfoVecs.growthProgress.begin()
					+ allocPara_m.currentActiveCellCount,
			cellInfoVecs.growthProgressOld.begin());

/*        
	thrust::transform(cellInfoVecs.growthSpeed.begin(),
			cellInfoVecs.growthSpeed.begin()
					+ allocPara_m.currentActiveCellCount,
			cellInfoVecs.growthProgress.begin(),
			cellInfoVecs.growthProgress.begin(), SaxpyFunctorWithMaxOfOne(dt));
*/
cout << " I am trying to update growth progress" << endl ; 


	//double dummy=0 ;
    double mitoticCheckPoint=growthAuxData.grthPrgrCriVal_M_Ori ; 
 //Alireza : Dpp regulate
/*
thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.cell_Dpp.begin(),
						           cellInfoVecs.cell_DppOld.begin(),
							   cellInfoVecs.growthProgress.begin(),
							   cellInfoVecs.growthSpeed.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.cell_Dpp.begin(),
							           cellInfoVecs.cell_DppOld.begin(),
							           cellInfoVecs.growthProgress.begin(),
									   cellInfoVecs.growthSpeed.begin()))
					+ allocPara_m.currentActiveCellCount,
			                           cellInfoVecs.growthProgress.begin(),
			DppGrowRegulator(dt,mitoticCheckPoint));  

*/
//Alireza : pMad regulate

thrust::transform(
                        thrust::make_zip_iterator(
                                        thrust::make_tuple(cellInfoVecs.cell_pMad.begin(),
                                                           cellInfoVecs.cell_pMadOld.begin(),
                                                           cellInfoVecs.growthProgress.begin(),
                                                           cellInfoVecs.growthSpeed.begin(),
														   cellInfoVecs.cell_Type.begin())),
                        thrust::make_zip_iterator(
                                        thrust::make_tuple(cellInfoVecs.cell_pMad.begin(),
                                                            cellInfoVecs.cell_pMadOld.begin(),
                                                            cellInfoVecs.growthProgress.begin(),
                                                            cellInfoVecs.growthSpeed.begin(),
															cellInfoVecs.cell_Type.begin()))
                                        + allocPara_m.currentActiveCellCount,
						cellInfoVecs.growthProgress.begin(),
                        DppGrowRegulator(dt,mitoticCheckPoint));
}

void SceCells::decideIsScheduleToGrow_M() {
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.growthProgress.begin(),
							cellInfoVecs.lastCheckPoint.begin(),
							cellInfoVecs.cell_Type.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.growthProgress.begin(),
							cellInfoVecs.lastCheckPoint.begin(),
							cellInfoVecs.cell_Type.begin()))
					+ allocPara_m.currentActiveCellCount,
			cellInfoVecs.isScheduledToGrow.begin(),
			decideIsScheduleToGrow_MDevice(miscPara.growThreshold));
}

//AAMIRI May5
void SceCells::decideIsScheduleToShrink_M() {

	double laserCenterY = 25.0;
	double laserCenterX = 25.0;
	double laserRadius = 4.0; 

	thrust::counting_iterator<uint> iBegin(0);
	thrust::counting_iterator<uint> iEnd(allocPara_m.currentActiveCellCount);


	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(iBegin, 
							cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin(), 
							cellInfoVecs.isScheduledToShrink.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(iEnd, 
							cellInfoVecs.centerCoordX.begin()+allocPara_m.currentActiveCellCount,
							cellInfoVecs.centerCoordY.begin()+allocPara_m.currentActiveCellCount,
							cellInfoVecs.isScheduledToShrink.begin()+allocPara_m.currentActiveCellCount)),
			cellInfoVecs.isScheduledToShrink.begin(),
			isDelOp(laserCenterX, laserCenterY, laserRadius));
}



void SceCells::computeCellTargetLength_M() {
	thrust::transform(cellInfoVecs.growthProgress.begin(),
			cellInfoVecs.growthProgress.begin()
					+ allocPara_m.currentActiveCellCount,
			cellInfoVecs.expectedLength.begin(),
			CompuTarLen(bioPara.cellInitLength, bioPara.cellFinalLength));
}

void SceCells::computeDistToCellCenter_M() {
	thrust::counting_iterator<uint> iBegin(0);
	thrust::counting_iterator<uint> iEnd(totalNodeCountForActiveCells);
	uint endIndx = allocPara_m.bdryNodeCount + totalNodeCountForActiveCells;
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							make_permutation_iterator(
									cellInfoVecs.centerCoordX.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(
													allocPara_m.maxAllNodePerCell))),
							make_permutation_iterator(
									cellInfoVecs.centerCoordY.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(
													allocPara_m.maxAllNodePerCell))),
							make_permutation_iterator(
									cellInfoVecs.growthXDir.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(
													allocPara_m.maxAllNodePerCell))),
							make_permutation_iterator(
									cellInfoVecs.growthYDir.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(
													allocPara_m.maxAllNodePerCell))),
							nodes->getInfoVecs().nodeLocX.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeLocY.begin()
									+ allocPara_m.bdryNodeCount,
							nodes->getInfoVecs().nodeIsActive.begin()
									+ allocPara_m.bdryNodeCount)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							make_permutation_iterator(
									cellInfoVecs.centerCoordX.begin(),
									make_transform_iterator(iEnd,
											DivideFunctor(
													allocPara_m.maxAllNodePerCell))),
							make_permutation_iterator(
									cellInfoVecs.centerCoordY.begin(),
									make_transform_iterator(iEnd,
											DivideFunctor(
													allocPara_m.maxAllNodePerCell))),
							make_permutation_iterator(
									cellInfoVecs.growthXDir.begin(),
									make_transform_iterator(iEnd,
											DivideFunctor(
													allocPara_m.maxAllNodePerCell))),
							make_permutation_iterator(
									cellInfoVecs.growthYDir.begin(),
									make_transform_iterator(iEnd,
											DivideFunctor(
													allocPara_m.maxAllNodePerCell))),
							nodes->getInfoVecs().nodeLocX.begin() + endIndx,
							nodes->getInfoVecs().nodeLocY.begin() + endIndx,
							nodes->getInfoVecs().nodeIsActive.begin()
									+ endIndx)),
			cellNodeInfoVecs.distToCenterAlongGrowDir.begin(), CompuDist());
}

void SceCells::findMinAndMaxDistToCenter_M() {
	thrust::reduce_by_key(
			make_transform_iterator(countingBegin,
					DivideFunctor(allocPara_m.maxAllNodePerCell)),
			make_transform_iterator(countingBegin,
					DivideFunctor(allocPara_m.maxAllNodePerCell))
					+ totalNodeCountForActiveCells,
			cellNodeInfoVecs.distToCenterAlongGrowDir.begin(),
			cellInfoVecs.cellRanksTmpStorage.begin(),
			cellInfoVecs.smallestDistance.begin(), thrust::equal_to<uint>(),
			thrust::minimum<double>());

// for nodes of each cell, find the maximum distance from the node to the corresponding
// cell center along the pre-defined growth direction.

	thrust::reduce_by_key(
			make_transform_iterator(countingBegin,
					DivideFunctor(allocPara_m.maxAllNodePerCell)),
			make_transform_iterator(countingBegin,
					DivideFunctor(allocPara_m.maxAllNodePerCell))
					+ totalNodeCountForActiveCells,
			cellNodeInfoVecs.distToCenterAlongGrowDir.begin(),
			cellInfoVecs.cellRanksTmpStorage.begin(),
			cellInfoVecs.biggestDistance.begin(), thrust::equal_to<uint>(),
			thrust::maximum<double>());
}

void SceCells::computeLenDiffExpCur_M() {
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.expectedLength.begin(),
							cellInfoVecs.smallestDistance.begin(),
							cellInfoVecs.biggestDistance.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.expectedLength.begin(),
							cellInfoVecs.smallestDistance.begin(),
							cellInfoVecs.biggestDistance.begin()))
					+ allocPara_m.currentActiveCellCount,
			cellInfoVecs.lengthDifference.begin(), CompuDiff());
}

void SceCells::stretchCellGivenLenDiff_M() {
	uint count = allocPara_m.maxAllNodePerCell;
	uint bdry = allocPara_m.bdryNodeCount;
	uint actCount = totalNodeCountForActiveCells;
	uint all = bdry + actCount;
	thrust::counting_iterator<uint> iBegin(0);
	thrust::counting_iterator<uint> iEnd(actCount);
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							cellNodeInfoVecs.distToCenterAlongGrowDir.begin(),
							make_permutation_iterator(
									cellInfoVecs.lengthDifference.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(count))),
							make_permutation_iterator(
									cellInfoVecs.growthXDir.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(count))),
							make_permutation_iterator(
									cellInfoVecs.growthYDir.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(count))),
							nodes->getInfoVecs().nodeVelX.begin() + bdry,
							nodes->getInfoVecs().nodeVelY.begin() + bdry,
							make_transform_iterator(iBegin,
									ModuloFunctor(count)))),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							cellNodeInfoVecs.distToCenterAlongGrowDir.begin()
									+ actCount,
							make_permutation_iterator(
									cellInfoVecs.lengthDifference.begin(),
									make_transform_iterator(iEnd,
											DivideFunctor(count))),
							make_permutation_iterator(
									cellInfoVecs.growthXDir.begin(),
									make_transform_iterator(iEnd,
											DivideFunctor(count))),
							make_permutation_iterator(
									cellInfoVecs.growthYDir.begin(),
									make_transform_iterator(iEnd,
											DivideFunctor(count))),
							nodes->getInfoVecs().nodeVelX.begin() + all,
							nodes->getInfoVecs().nodeVelY.begin() + all,
							make_transform_iterator(iEnd,
									ModuloFunctor(count)))),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							nodes->getInfoVecs().nodeVelX.begin() + bdry,
							nodes->getInfoVecs().nodeVelY.begin() + bdry)),
			ApplyStretchForce_M(bioPara.elongationCoefficient,
					allocPara_m.maxMembrNodePerCell));
}

// to do: transfer myosin data and update myosin value on new node, JG040623
void SceCells::addPointIfScheduledToGrow_M() {
	uint seed = time(NULL);
	uint activeCellCount = allocPara_m.currentActiveCellCount;

	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;
    uint maxMemNodePerCell = allocPara_m.maxMembrNodePerCell;
	double* myosinLevelAddr = thrust::raw_pointer_cast(
		&(nodes->getInfoVecs().myosinLevel[0])); // pointer to the vector storing myosin level 
	thrust::counting_iterator<uint> iBegin(0);
	thrust::counting_iterator<uint> iEnd(activeCellCount);
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.isScheduledToGrow.begin(),
							cellInfoVecs.activeIntnlNodeCounts.begin(),
							cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin(), iBegin,
							cellInfoVecs.lastCheckPoint.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							cellInfoVecs.isScheduledToGrow.begin()
									+ activeCellCount,
							cellInfoVecs.activeIntnlNodeCounts.begin()
									+ activeCellCount,
							cellInfoVecs.centerCoordX.begin() + activeCellCount,
							cellInfoVecs.centerCoordY.begin() + activeCellCount,
							iEnd,
							cellInfoVecs.lastCheckPoint.begin()
									+ activeCellCount)),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.lastCheckPoint.begin(),
							cellInfoVecs.activeIntnlNodeCounts.begin())),
			addPointIfScheduledToGrow_MDevice(seed, miscPara.addNodeDistance, miscPara.growThreshold,
					growthAuxData.nodeXPosAddress,
					growthAuxData.nodeYPosAddress,
					growthAuxData.nodeIsActiveAddress,maxAllNodePerCell,maxMemNodePerCell,myosinLevelAddr));
}




void SceCells::addPointDueToActin() {
	random_device rd;
	// mt19937_64 host_rng(rd());
 	// uint seed = time(NULL);
 	// seed = host_rng();
	uint seed = rd();

	uint activeCellCount = allocPara_m.currentActiveCellCount;

	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;
    uint maxMemNodePerCell = allocPara_m.maxMembrNodePerCell;
	double* myosinLevelAddr = thrust::raw_pointer_cast(
		&(nodes->getInfoVecs().myosinLevel[0])); // pointer to the vector storing myosin level
	
	double* nodeVelXAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeVelX[0]));
    double* nodeVelYAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeVelY[0]));
	int timeStep = curTime/dt;
	double initTimePeriod = InitTimeStage;
	double ddt = dt;

	thrust::counting_iterator<uint> iBegin(0);
	thrust::counting_iterator<uint> iEnd(activeCellCount);
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.isScheduledToGrow.begin(),
							cellInfoVecs.activeIntnlNodeCounts.begin(),
							cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin(), iBegin,
							cellInfoVecs.lastCheckPoint.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							cellInfoVecs.isScheduledToGrow.begin()
									+ activeCellCount,
							cellInfoVecs.activeIntnlNodeCounts.begin()
									+ activeCellCount,
							cellInfoVecs.centerCoordX.begin() + activeCellCount,
							cellInfoVecs.centerCoordY.begin() + activeCellCount,
							iEnd,
							cellInfoVecs.lastCheckPoint.begin()
									+ activeCellCount)),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.lastCheckPoint.begin(),
					cellInfoVecs.activeIntnlNodeCounts.begin())),
			AddDelPtDueToActinTemp(seed, miscPara.addNodeDistance, miscPara.growThreshold,
					growthAuxData.nodeXPosAddress,
					growthAuxData.nodeYPosAddress,
					growthAuxData.nodeIsActiveAddress,
					maxAllNodePerCell,maxMemNodePerCell,myosinLevelAddr,nodeVelXAddr,nodeVelYAddr,
					timeStep, growthAuxData.adhIndxAddr, initTimePeriod, ddt));
}








//AAMIRI
void SceCells::delPointIfScheduledToGrow_M() {
	uint seed = time(NULL);
	uint activeCellCount = allocPara_m.currentActiveCellCount;
	thrust::counting_iterator<uint> iBegin(0);
	thrust::counting_iterator<uint> iEnd(activeCellCount);

	int timeStep = curTime/dt;

	if (curTime>70000.0 && curTime<70000.1){

	decideIsScheduleToShrink_M();// AAMIRI
	}

 
	if (curTime > 70000.0)
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.isScheduledToShrink.begin(),
							cellInfoVecs.activeIntnlNodeCounts.begin(),
							cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin(), iBegin,
							cellInfoVecs.activeMembrNodeCounts.begin(),
							cellInfoVecs.isCellActive.begin(),
							cellInfoVecs.growthSpeed.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							cellInfoVecs.isScheduledToShrink.begin()
									+ activeCellCount,
							cellInfoVecs.activeIntnlNodeCounts.begin()
									+ activeCellCount,
							cellInfoVecs.centerCoordX.begin() + activeCellCount,
							cellInfoVecs.centerCoordY.begin() + activeCellCount,
							iEnd,
							cellInfoVecs.activeMembrNodeCounts.begin()
									+ activeCellCount,
							cellInfoVecs.isCellActive.begin()
									+ activeCellCount,
							cellInfoVecs.growthSpeed.begin()
									+ activeCellCount)),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.activeMembrNodeCounts.begin(),
							cellInfoVecs.activeIntnlNodeCounts.begin(),												 cellInfoVecs.isCellActive.begin(),
							cellInfoVecs.growthSpeed.begin())),
			DelPtOp_M(seed, timeStep, growthAuxData.adhIndxAddr,
					growthAuxData.nodeXPosAddress,
					growthAuxData.nodeYPosAddress,
					growthAuxData.nodeIsActiveAddress));
}



bool SceCells::decideIfGoingToDivide_M() {
	uint maxIntnlNodeCountPerFollower = globalConfigVars.getConfigValue(
            "MaxIntnlNodeCountPerFollower").toInt();
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.growthProgress.begin(),
							cellInfoVecs.activeIntnlNodeCounts.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.growthProgress.begin(),
							cellInfoVecs.activeIntnlNodeCounts.begin()))
					+ allocPara_m.currentActiveCellCount,
			cellInfoVecs.isDividing.begin(),
			CompuIsDivide_M(maxIntnlNodeCountPerFollower));
	// sum all bool values which indicate whether the cell is going to divide.
	// toBeDivideCount is the total number of cells going to divide.
	divAuxData.toBeDivideCount = thrust::reduce(cellInfoVecs.isDividing.begin(),
			cellInfoVecs.isDividing.begin()
					+ allocPara_m.currentActiveCellCount, (uint) (0));
	if (divAuxData.toBeDivideCount > 0) {
		return true;
	} else {
		return false;
	}
}
//A&A
bool SceCells::decideIfAnyCellEnteringMitotic() {

        double grthPrgrCriVal_M =growthAuxData.grthPrgrCriVal_M_Ori ; // for now constant  growthAuxData.grthProgrEndCPU
		//	- growthAuxData.prolifDecay
		//			* (growthAuxData.grthProgrEndCPU
		//					- growthAuxData.grthPrgrCriVal_M_Ori);

	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.growthProgress.begin(),
							cellInfoVecs.growthProgressOld.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.growthProgress.begin(),
							cellInfoVecs.growthProgressOld.begin()))
					+ allocPara_m.currentActiveCellCount,
			cellInfoVecs.isEnteringMitotic.begin(),
			CompuIsEnteringMitotic_M(grthPrgrCriVal_M));
	// sum all bool values which indicate whether the cell is going to divide.
	// toBeDivideCount is the total number of cells going to divide.
	divAuxData.toEnterMitoticCount = thrust::reduce(cellInfoVecs.isEnteringMitotic.begin(),
			cellInfoVecs.isEnteringMitotic.begin()
					+ allocPara_m.currentActiveCellCount, (uint) (0));
	if (divAuxData.toEnterMitoticCount > 0) {
		return true;
	} else {
		return false;
	}
}

//AAMIRI
/*
bool SceCells::decideIfGoingToRemove_M() {
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.growthProgress.begin(),
							cellInfoVecs.activeIntnlNodeCounts.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.growthProgress.begin(),
							cellInfoVecs.activeIntnlNodeCounts.begin()))
					+ allocPara_m.currentActiveCellCount,
			cellInfoVecs.isRemoving.begin(),
			CompuIsRemoving_M(allocPara_m.maxIntnlNodePerCell));
	// sum all bool values which indicate whether the cell is going to divide.
	// toBeDivideCount is the total number of cells going to divide.
	divAuxData.toBeRemovingCount = thrust::reduce(cellInfoVecs.isRemoving.begin(),
			cellInfoVecs.isRemoving.begin()
					+ allocPara_m.currentActiveCellCount, (uint) (0));
	if (divAuxData.toBeRemovingCount > 0) {
		return true;
	} else {
		return false;
	}
}

*/

AniRawData SceCells::obtainAniRawData(AnimationCriteria& aniCri) {
	uint activeCellCount = allocPara_m.currentActiveCellCount;
	uint maxNodePerCell = allocPara_m.maxAllNodePerCell;
	uint maxMemNodePerCell = allocPara_m.maxMembrNodePerCell;
	uint beginIndx = allocPara_m.bdryNodeCount;

	AniRawData rawAniData;
	//cout << "size of potential pairs = " << pairs.size() << endl;

// unordered_map is more efficient than map, but it is a c++ 11 feature
// and c++ 11 seems to be incompatible with Thrust.
	IndexMap locIndexToAniIndexMap;

	uint maxActiveNode = activeCellCount * maxNodePerCell;
	thrust::host_vector<double> hostTmpVectorLocX(maxActiveNode);
	thrust::host_vector<double> hostTmpVectorLocY(maxActiveNode);
	thrust::host_vector<bool> hostIsActiveVec(maxActiveNode);
	thrust::host_vector<int> hostBondVec(maxActiveNode);
	thrust::host_vector<double> hostTmpVectorTenMag(maxActiveNode);

	thrust::copy(
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeLocX.begin(),
							nodes->getInfoVecs().nodeLocY.begin(),
							nodes->getInfoVecs().nodeIsActive.begin(),
							nodes->getInfoVecs().nodeAdhereIndex.begin(),
							nodes->getInfoVecs().membrTensionMag.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeLocX.begin(),
							nodes->getInfoVecs().nodeLocY.begin(),
							nodes->getInfoVecs().nodeIsActive.begin(),
							nodes->getInfoVecs().nodeAdhereIndex.begin(),
							nodes->getInfoVecs().membrTensionMag.begin()))
					+ maxActiveNode,
			thrust::make_zip_iterator(
					thrust::make_tuple(hostTmpVectorLocX.begin(),
							hostTmpVectorLocY.begin(),
							hostIsActiveVec.begin(),
							hostBondVec.begin(), hostTmpVectorTenMag.begin())));

	thrust::host_vector<uint> curActiveMemNodeCounts =
			cellInfoVecs.activeMembrNodeCounts;

	CVector tmpPos;
	uint index1;
	int index2;
	std::vector<BondInfo> bondInfoVec;

	double node1X, node1Y;
	double node2X, node2Y;
	double aniVal;

	for (uint i = 0; i < activeCellCount; i++) {
		for (uint j = 0; j < maxMemNodePerCell; j++) {
			index1 = beginIndx + i * maxNodePerCell + j;
			if (hostIsActiveVec[index1] == true) {
				index2 = hostBondVec[index1];
				if (index2 > index1 && index2 != -1) {
					BondInfo bond;
					bond.cellRank1 = i;
					bond.pos1 = CVector(hostTmpVectorLocX[index1],
							hostTmpVectorLocY[index1], 0);
					bond.cellRank2 = (index2 - beginIndx) / maxNodePerCell;
					bond.pos2 = CVector(hostTmpVectorLocX[index2],
							hostTmpVectorLocY[index2], 0);
					bondInfoVec.push_back(bond);
				}
			}
		}
	}

	rawAniData.bondsArr = bondInfoVec;

	uint curIndex = 0;

	for (uint i = 0; i < activeCellCount; i++) {
		for (uint j = 0; j < curActiveMemNodeCounts[i]; j++) {
			index1 = beginIndx + i * maxNodePerCell + j;
			if (j == curActiveMemNodeCounts[i] - 1) {
				index2 = beginIndx + i * maxNodePerCell;
			} else {
				index2 = beginIndx + i * maxNodePerCell + j + 1;
			}

			if (hostIsActiveVec[index1] == true
					&& hostIsActiveVec[index2] == true) {
				node1X = hostTmpVectorLocX[index1];
				node1Y = hostTmpVectorLocY[index1];
				node2X = hostTmpVectorLocX[index2];
				node2Y = hostTmpVectorLocY[index2];
				IndexMap::iterator it = locIndexToAniIndexMap.find(index1);
				if (it == locIndexToAniIndexMap.end()) {
					locIndexToAniIndexMap.insert(
							std::pair<uint, uint>(index1, curIndex));
					curIndex++;
					tmpPos = CVector(node1X, node1Y, 0);
					//aniVal = hostTmpVectorNodeType[index1];
					aniVal = hostTmpVectorTenMag[index1];
					rawAniData.aniNodePosArr.push_back(tmpPos);
					rawAniData.aniNodeVal.push_back(aniVal);
				}
				it = locIndexToAniIndexMap.find(index2);
				if (it == locIndexToAniIndexMap.end()) {
					locIndexToAniIndexMap.insert(
							std::pair<uint, uint>(index2, curIndex));
					curIndex++;
					tmpPos = CVector(node2X, node2Y, 0);
					//aniVal = hostTmpVectorNodeType[index2];
					aniVal = hostTmpVectorTenMag[index2];
					rawAniData.aniNodePosArr.push_back(tmpPos);
					rawAniData.aniNodeVal.push_back(aniVal);
				}

				it = locIndexToAniIndexMap.find(index1);
				uint aniIndex1 = it->second;
				it = locIndexToAniIndexMap.find(index2);
				uint aniIndex2 = it->second;

				LinkAniData linkData;
				linkData.node1Index = aniIndex1;
				linkData.node2Index = aniIndex2;
				rawAniData.memLinks.push_back(linkData);
			}
		}
	}

	for (uint i = 0; i < activeCellCount; i++) {
		for (uint j = 0; j < allocPara_m.maxIntnlNodePerCell; j++) {
			for (uint k = j + 1; k < allocPara_m.maxIntnlNodePerCell; k++) {
				index1 = i * maxNodePerCell + maxMemNodePerCell + j;
				index2 = i * maxNodePerCell + maxMemNodePerCell + k;
				if (hostIsActiveVec[index1] && hostIsActiveVec[index2]) {
					node1X = hostTmpVectorLocX[index1];
					node1Y = hostTmpVectorLocY[index1];
					node2X = hostTmpVectorLocX[index2];
					node2Y = hostTmpVectorLocY[index2];
					if (aniCri.isPairQualify_M(node1X, node1Y, node2X,
							node2Y)) {
						IndexMap::iterator it = locIndexToAniIndexMap.find(
								index1);
						if (it == locIndexToAniIndexMap.end()) {
							locIndexToAniIndexMap.insert(
									std::pair<uint, uint>(index1, curIndex));
							curIndex++;
							tmpPos = CVector(node1X, node1Y, 0);
							//aniVal = hostTmpVectorNodeType[index1];
							aniVal = -1;
							rawAniData.aniNodePosArr.push_back(tmpPos);
							rawAniData.aniNodeVal.push_back(aniVal);
						}
						it = locIndexToAniIndexMap.find(index2);
						if (it == locIndexToAniIndexMap.end()) {
							locIndexToAniIndexMap.insert(
									std::pair<uint, uint>(index2, curIndex));
							curIndex++;
							tmpPos = CVector(node2X, node2Y, 0);
							//aniVal = hostTmpVectorNodeType[index1];
							aniVal = -1;
							rawAniData.aniNodePosArr.push_back(tmpPos);
							rawAniData.aniNodeVal.push_back(aniVal);
						}

						it = locIndexToAniIndexMap.find(index1);
						uint aniIndex1 = it->second;
						it = locIndexToAniIndexMap.find(index2);
						uint aniIndex2 = it->second;

						LinkAniData linkData;
						linkData.node1Index = aniIndex1;
						linkData.node2Index = aniIndex2;
						rawAniData.internalLinks.push_back(linkData);
					}
				}
			}
		}
	}
	return rawAniData;
}

AniRawData SceCells::obtainAniRawDataGivenCellColor(vector<double>& cellColors,
		AnimationCriteria& aniCri, vector<double>& cellsPerimeter, vector <double> & cellsDppLevel) {   //AliE //apr 05
        cout << "I am in obtainAniRawDataGivenCellColor start"<<endl; 
	uint activeCellCount = allocPara_m.currentActiveCellCount;
	uint maxNodePerCell = allocPara_m.maxAllNodePerCell;
	uint maxMemNodePerCell = allocPara_m.maxMembrNodePerCell;
	uint maxIntnlNodePerCell = maxNodePerCell-maxMemNodePerCell;
	uint beginIndx = allocPara_m.bdryNodeCount;
	uint maxFilopPerCell = 5; // to be modified later
	uint maxFilopCount = activeCellCount * maxFilopPerCell;

	assert(cellColors.size() >= activeCellCount);
	assert(cellsPerimeter.size() == activeCellCount); //AliE

	AniRawData rawAniData;
	//cout << "size of potential pairs = " << pairs.size() << endl;

	// unordered_map is more efficient than map, but it is a c++ 11 feature
	// and c++ 11 seems to be incompatible with Thrust.
	IndexMap locIndexToAniIndexMap;
	IndexMap filopCenterIndexMap;

	uint maxActiveNode = activeCellCount * maxNodePerCell;

	thrust::host_vector<double> hostTmpVectorLocX(maxActiveNode);
	thrust::host_vector<double> hostTmpVectorLocY(maxActiveNode);
	thrust::host_vector<bool> hostIsActiveVec(maxActiveNode);
	thrust::host_vector<int> hostBondVec(maxActiveNode);
	thrust::host_vector<double> hostTmpVectorTenMag(maxActiveNode);
	thrust::host_vector<double> hostTmpVectorF_MI_M_x(maxActiveNode);//AAMIRI //AliE
	thrust::host_vector<double> hostTmpVectorF_MI_M_y(maxActiveNode);//AAMIRI //AliE
	thrust::host_vector<double> hostTmpVectorF_MI_M_T(maxActiveNode); //AliE
	thrust::host_vector<double> hostTmpVectorF_MI_M_N(maxActiveNode);//AliE
	thrust::host_vector<double> hostTmpVectorNodeCurvature(maxActiveNode);//AAMIRI
	thrust::host_vector<double> hostTmpVectorExtForceTangent(maxActiveNode);//AAMIRI
	thrust::host_vector<double> hostTmpVectorExtForceNormal(maxActiveNode);//AAMIRI
	thrust::host_vector<double> hostMyosinLevel(maxActiveNode); 
	thrust::host_vector<double> hostMyosinWeight(maxActiveNode);
	thrust::host_vector<double> hostActLevel(maxActiveNode);
	thrust::host_vector<double> hostSubAdhIsBound(maxActiveNode*10); 
	thrust::host_vector<double> hostFilopX(maxFilopCount);//= cellInfoVecs.cellFilopX; // 5 filopodia, 1 center location for each cell to be added later
	thrust::host_vector<double> hostFilopY(maxFilopCount);// = cellInfoVecs.cellFilopY; // inactive cells are also included
	thrust::host_vector<bool> hostFilopIsActive(maxFilopCount);
	thrust::host_vector<double> hostFilopAngle(maxFilopCount);
	thrust::host_vector<int> hostAdhNodeIndex(maxActiveNode);
	thrust::host_vector<double> hostMinToMDist(maxActiveNode);
	thrust::host_vector<double> hostFluxWeights = nodes->getInfoVecs().fluxWeights;

	thrust::host_vector<double> hostCellCenterX(activeCellCount);
	thrust::host_vector<double> hostCellCenterY(activeCellCount);
	thrust::host_vector<double> hostCellCenterZ(activeCellCount);

	thrust::host_vector<double> hostCellPolarAngle(activeCellCount);

	thrust::copy(
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeLocX.begin(),
							nodes->getInfoVecs().nodeLocY.begin(),
							nodes->getInfoVecs().nodeF_MI_M_x.begin(),//AAMIRI //AliE
							nodes->getInfoVecs().nodeF_MI_M_y.begin(),//AAMIRI //AliE
							nodes->getInfoVecs().nodeCurvature.begin(),//AAMIRI
							nodes->getInfoVecs().nodeIsActive.begin(),
							nodes->getInfoVecs().nodeAdhereIndex.begin(),
							nodes->getInfoVecs().membrTensionMag.begin(),
							nodes->getInfoVecs().nodeExtForceTangent.begin(),//AAMIRI
							nodes->getInfoVecs().nodeExtForceNormal.begin())),//AAMIRI
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeLocX.begin(),
							nodes->getInfoVecs().nodeLocY.begin(),
							nodes->getInfoVecs().nodeF_MI_M_x.begin(),//AAMIRI //AliE
							nodes->getInfoVecs().nodeF_MI_M_y.begin(),//AAMIRI //AliE
							nodes->getInfoVecs().nodeCurvature.begin(),//AAMIRI
							nodes->getInfoVecs().nodeIsActive.begin(),
							nodes->getInfoVecs().nodeAdhereIndex.begin(),
							nodes->getInfoVecs().membrTensionMag.begin(),
							nodes->getInfoVecs().nodeExtForceTangent.begin(),//AAMIRI
							nodes->getInfoVecs().nodeExtForceNormal.begin()))//AAMIRI
					+ maxActiveNode,
			thrust::make_zip_iterator(
					thrust::make_tuple(hostTmpVectorLocX.begin(),
							hostTmpVectorLocY.begin(), 
							hostTmpVectorF_MI_M_x.begin(), hostTmpVectorF_MI_M_y.begin(),//AAMIRI
							hostTmpVectorNodeCurvature.begin(), //AAMIRI
							hostIsActiveVec.begin(),
							hostBondVec.begin(), hostTmpVectorTenMag.begin(),
							hostTmpVectorExtForceTangent.begin(), hostTmpVectorExtForceNormal.begin())));//AAMIRI

//Copy more than 10 elements is not allowed so, I separate it
	thrust::copy(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							nodes->getInfoVecs().nodeF_MI_M_T.begin(), //Ali
							nodes->getInfoVecs().nodeF_MI_M_N.begin(), //Ali
							nodes->getInfoVecs().myosinLevel.begin(),
							nodes->getInfoVecs().myosinWeight.begin(),
							nodes->getInfoVecs().nodeActLevel.begin(),
							nodes->getInfoVecs().nodeAdhereIndex.begin(),
							nodes->getInfoVecs().minToMDist.begin()
							)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							nodes->getInfoVecs().nodeF_MI_M_T.begin(),//AliE
							nodes->getInfoVecs().nodeF_MI_M_N.begin(), //AliE
							nodes->getInfoVecs().myosinLevel.begin(),
							nodes->getInfoVecs().myosinWeight.begin(),
							nodes->getInfoVecs().nodeActLevel.begin(),
							nodes->getInfoVecs().nodeAdhereIndex.begin(),
							nodes->getInfoVecs().minToMDist.begin()
							))
					+ maxActiveNode,
			thrust::make_zip_iterator(
					thrust::make_tuple(
							hostTmpVectorF_MI_M_T.begin(), hostTmpVectorF_MI_M_N.begin(), hostMyosinLevel.begin(), hostMyosinWeight.begin(),
							hostActLevel.begin(),hostAdhNodeIndex.begin(),hostMinToMDist.begin()
							)));

	thrust::copy(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							nodes->getInfoVecs().subAdhIsBound.begin()
							)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							nodes->getInfoVecs().subAdhIsBound.begin()
							))
					+ maxActiveNode * 10, // to be changed to an input parameter later
			thrust::make_zip_iterator(
					thrust::make_tuple(
							hostSubAdhIsBound.begin()
				)));

	thrust::copy(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							cellInfoVecs.cellFilopX.begin(),
							cellInfoVecs.cellFilopY.begin(),
							cellInfoVecs.cellFilopIsActive.begin(),
							cellInfoVecs.cellFilopAngle.begin()
							)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							cellInfoVecs.cellFilopX.begin(),
							cellInfoVecs.cellFilopY.begin(),
							cellInfoVecs.cellFilopIsActive.begin(),
							cellInfoVecs.cellFilopAngle.begin()
							))
					+ maxFilopCount, 
			thrust::make_zip_iterator(
					thrust::make_tuple(
							hostFilopX.begin(), hostFilopY.begin(),hostFilopIsActive.begin(),hostFilopAngle.begin()
				)));

	thrust::copy(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin(),
							cellInfoVecs.centerCoordZ.begin(),
							cellInfoVecs.cellPolarAngle.begin()
							)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin(),
							cellInfoVecs.centerCoordZ.begin(),
							cellInfoVecs.cellPolarAngle.begin()
							))
					+ activeCellCount, 
			thrust::make_zip_iterator(
					thrust::make_tuple(
							hostCellCenterX.begin(), hostCellCenterY.begin(),hostCellCenterZ.begin(),hostCellPolarAngle.begin()
				)));

	thrust::host_vector<uint> curActiveFilopCounts =
			cellInfoVecs.activeCellFilopCounts;

	thrust::host_vector<uint> curActiveMemNodeCounts =
			cellInfoVecs.activeMembrNodeCounts;

	thrust::host_vector<uint> curActiveIntnlNodeCounts =
				cellInfoVecs.activeIntnlNodeCounts;


	CVector tmpPos;
	CVector tmpF_MI_M ;//AAmiri
	CVector tmpExtForce;//AAMIRI
	double tmpCurv;
	uint index1;
	int index2;
	std::vector<BondInfo> bondInfoVec;

	double node1X, node1Y;
	double node2X, node2Y;
	double node1F_MI_M_x, node1F_MI_M_y;//AAMIRI //AliE
	double nodeExtForceT, nodeExtForceN;//AAMIRI 
	double aniVal;
	double aniVal2;
	double tempMyosinLevel;
	double tempMyosinWeight;
	uint tempActLevel;
	int tempAdhNodeIndex;

	

	int tempAdhSiteCount = 0;
        double tmpF_MI_M_MagN_Int[activeCellCount-1] ; //AliE
//	for (uint i=0; i<hostFluxWeights.size();i++){
//		rawAniData.fluxWeights.push_back(hostFluxWeights[i]);
//	}

         //This is how the VTK file is intended to be written. First the membrane nodes are going to be written and then internal nodes.
        //loop on membrane nodes
	for (uint i = 0; i < activeCellCount; i++) {
		tmpF_MI_M_MagN_Int[i]=0.0;   
		for (uint j = 0; j < curActiveMemNodeCounts[i]; j++) {
			index1 = beginIndx + i * maxNodePerCell + j;
			if ( hostIsActiveVec[index1]==true) {
				tmpCurv = hostTmpVectorNodeCurvature[index1];//AAMIRI
				rawAniData.aniNodeCurvature.push_back(tmpCurv);//AAMIRI

				node1F_MI_M_x= hostTmpVectorF_MI_M_x[index1]; //AliE
				node1F_MI_M_y= hostTmpVectorF_MI_M_y[index1]; //AliE
				tmpF_MI_M= CVector(node1F_MI_M_x, node1F_MI_M_y, 0.0); //AliE
				rawAniData.aniNodeF_MI_M.push_back(tmpF_MI_M); //AliE
                               // tmpF_MI_M_MagN_Int[i]=tmpF_MI_M_MagN_Int[i]+sqrt(pow(hostTmpVectorF_MI_M_x[index1],2)+pow(hostTmpVectorF_MI_M_y[index1],2)) ; //AliE
                tmpF_MI_M_MagN_Int[i]=tmpF_MI_M_MagN_Int[i]+abs(hostTmpVectorF_MI_M_N[index1]) ; //AliE

				nodeExtForceT = hostTmpVectorExtForceTangent[index1];//AAMIRI
				nodeExtForceN = hostTmpVectorExtForceNormal[index1];//AAMIRI
				tmpExtForce = CVector(nodeExtForceT, nodeExtForceN, 0.0);//AAMIRI
				rawAniData.aniNodeExtForceArr.push_back(tmpExtForce);

				tempMyosinLevel = hostMyosinLevel[index1];
				rawAniData.myoLevel.push_back(tempMyosinLevel);

				tempMyosinWeight = hostMyosinWeight[index1];
				rawAniData.myoWeight.push_back(tempMyosinWeight);

				tempActLevel = hostActLevel[index1];
				rawAniData.actLevel.push_back(tempActLevel);

				tempAdhSiteCount = 0;
				for (uint k = 0; k<10; k++){
					tempAdhSiteCount += hostSubAdhIsBound[index1*10 + k]; //???
				}

				if (hostAdhNodeIndex[index1] == -1) {tempAdhNodeIndex = -1;}
				else {tempAdhNodeIndex = hostAdhNodeIndex[index1];};

				rawAniData.adhSiteCount.push_back(tempAdhSiteCount);
				rawAniData.adhNodeIndex.push_back(tempAdhNodeIndex);
				rawAniData.minToMDist.push_back(hostMinToMDist[index1]);
				rawAniData.fluxWeights.push_back(0);
				rawAniData.aniCellRank.push_back(i);
				rawAniData.aniNodeRank.push_back(index1);
				}
			
			}

	}
        //loop on internal nodes
	for (uint i=0; i<activeCellCount; i++){
			for (uint j = maxMemNodePerCell; j < maxNodePerCell; j++) {
			index1 = beginIndx + i * maxNodePerCell + j;
			if ( hostIsActiveVec[index1]==true ) {
				tmpCurv = hostTmpVectorNodeCurvature[index1];//AAMIRI
				rawAniData.aniNodeCurvature.push_back(tmpCurv);//AAMIRI
				node1F_MI_M_x= hostTmpVectorF_MI_M_x[index1]; //AliE
				node1F_MI_M_y= hostTmpVectorF_MI_M_y[index1]; //AliE
				tmpF_MI_M= CVector(node1F_MI_M_x, node1F_MI_M_y, 0.0); //AliE
				rawAniData.aniNodeF_MI_M.push_back(tmpF_MI_M);

				nodeExtForceT = hostTmpVectorExtForceTangent[index1];//AAMIRI
				nodeExtForceN = hostTmpVectorExtForceNormal[index1];//AAMIRI
				tmpExtForce = CVector(nodeExtForceT, nodeExtForceN, 0.0);//AAMIRI
				
				rawAniData.aniNodeExtForceArr.push_back(tmpExtForce);

				tempMyosinLevel = hostMyosinLevel[index1];
				rawAniData.myoLevel.push_back(tempMyosinLevel);

				tempMyosinWeight = hostMyosinWeight[index1];
				rawAniData.myoWeight.push_back(tempMyosinWeight);

				tempActLevel = hostActLevel[index1];
				rawAniData.actLevel.push_back(tempActLevel);

				tempAdhSiteCount = 0;
				for (uint k = 0; k<10; k++){
					tempAdhSiteCount += hostSubAdhIsBound[index1*10 + k]; //???
				}

				if (hostAdhNodeIndex[index1] == -1) {tempAdhNodeIndex = -1;}
				else {tempAdhNodeIndex = hostAdhNodeIndex[index1];};

				rawAniData.adhNodeIndex.push_back(tempAdhNodeIndex);
				rawAniData.adhSiteCount.push_back(tempAdhSiteCount);
				rawAniData.minToMDist.push_back(hostMinToMDist[index1]);
				rawAniData.aniCellRank.push_back(i);
				rawAniData.aniNodeRank.push_back(index1);
				}
			
			}

	}



	for (uint i = 0; i < activeCellCount; i++) {
		for (uint j = 0; j < maxMemNodePerCell; j++) {
			index1 = beginIndx + i * maxNodePerCell + j;
			if (hostIsActiveVec[index1] == true) {
				index2 = hostBondVec[index1];
				if (index2 > index1 && index2 != -1) {
					BondInfo bond;
					bond.cellRank1 = i;
					bond.pos1 = CVector(hostTmpVectorLocX[index1],
							hostTmpVectorLocY[index1], 0);
					bond.cellRank2 = (index2 - beginIndx) / maxNodePerCell;
					bond.pos2 = CVector(hostTmpVectorLocX[index2],
							hostTmpVectorLocY[index2], 0);
					bondInfoVec.push_back(bond);
				}
			}
		}
	}

	rawAniData.bondsArr = bondInfoVec;

	uint curIndex = 0;
        //loop on membrane nodes
	for (uint i = 0; i < activeCellCount; i++) {
		for (uint j = 0; j < curActiveMemNodeCounts[i]; j++) {
			index1 = beginIndx + i * maxNodePerCell + j;
			if (j == curActiveMemNodeCounts[i] - 1) {
				index2 = beginIndx + i * maxNodePerCell;
			} else {
				index2 = beginIndx + i * maxNodePerCell + j + 1;
			}

			if (hostIsActiveVec[index1] == true
					&& hostIsActiveVec[index2] == true) {
				node1X = hostTmpVectorLocX[index1];
				node1Y = hostTmpVectorLocY[index1];
				node2X = hostTmpVectorLocX[index2];
				node2Y = hostTmpVectorLocY[index2];

				IndexMap::iterator it = locIndexToAniIndexMap.find(index1);
				if (it == locIndexToAniIndexMap.end()) {
					locIndexToAniIndexMap.insert(
							std::pair<uint, uint>(index1, curIndex));
					curIndex++;
					tmpPos = CVector(node1X, node1Y, 0);
					//aniVal = hostTmpVectorNodeType[index1];
					aniVal = cellColors[i];
                                        rawAniData.aniNodeF_MI_M_MagN_Int.push_back(tmpF_MI_M_MagN_Int[i]/cellsPerimeter[i]) ; //Ali added 
                                        //aniVal2=dppLevels_Cell[i] ; 
                                        aniVal2=cellsDppLevel[i] ; 
                                        rawAniData.dppLevel.push_back(aniVal2) ; //Ali Added 
					rawAniData.aniNodePosArr.push_back(tmpPos);
					rawAniData.aniNodeVal.push_back(aniVal);

				}
				it = locIndexToAniIndexMap.find(index2);
				if (it == locIndexToAniIndexMap.end()) {
					locIndexToAniIndexMap.insert(
							std::pair<uint, uint>(index2, curIndex));
					curIndex++;
					tmpPos = CVector(node2X, node2Y, 0);
					//aniVal = hostTmpVectorNodeType[index2];
					aniVal = cellColors[i];
                                        rawAniData.aniNodeF_MI_M_MagN_Int.push_back(tmpF_MI_M_MagN_Int[i]/cellsPerimeter[i]) ; //Ali Added 
					//aniVal2=dppLevels_Cell[i]; 
					aniVal2=cellsDppLevel[i]; 
                    rawAniData.dppLevel.push_back(aniVal2) ; //Ali Added 
					rawAniData.aniNodePosArr.push_back(tmpPos);
					rawAniData.aniNodeVal.push_back(aniVal);

				}

				it = locIndexToAniIndexMap.find(index1);
				uint aniIndex1 = it->second;
				it = locIndexToAniIndexMap.find(index2);
				uint aniIndex2 = it->second;

				LinkAniData linkData;
				linkData.node1Index = aniIndex1;
				linkData.node2Index = aniIndex2;
				rawAniData.memLinks.push_back(linkData);
			}
		}
	} 
        //loop on internal nodes
	for (uint i = 0; i < activeCellCount; i++) {
	//	for (uint j = 0; j < allocPara_m.maxAllNodePerCell; j++) {
		for (uint j = 0; j < allocPara_m.maxIntnlNodePerCell; j++) {
			for (uint k = 0; k < allocPara_m.maxAllNodePerCell; k++) {   //Ali
			//for (uint k = j + 1; k < allocPara_m.maxIntnlNodePerCell; k++) {  //Ali comment 
				index1 = i * maxNodePerCell + maxMemNodePerCell + j;
				index2 = i * maxNodePerCell  + k;         //Ali
			//	index2 = i * maxNodePerCell + maxMemNodePerCell + k;   //Ali comment
			//	if (hostIsActiveVec[index1] && hostIsActiveVec[index2]) {
				if (hostIsActiveVec[index1] && hostIsActiveVec[index2]&& index1 !=index2 ) {
					node1X = hostTmpVectorLocX[index1];
					node1Y = hostTmpVectorLocY[index1];
					node2X = hostTmpVectorLocX[index2];
					node2Y = hostTmpVectorLocY[index2];
					if (aniCri.isPairQualify_M(node1X, node1Y, node2X,
							node2Y)) {
						IndexMap::iterator it = locIndexToAniIndexMap.find(
								index1);
						if (it == locIndexToAniIndexMap.end()) {
							locIndexToAniIndexMap.insert(
									std::pair<uint, uint>(index1, curIndex));
							curIndex++;
							tmpPos = CVector(node1X, node1Y, 0);
							//aniVal = hostTmpVectorNodeType[index1];
							aniVal = cellColors[i];
                                                        rawAniData.aniNodeF_MI_M_MagN_Int.push_back(tmpF_MI_M_MagN_Int[i]/cellsPerimeter[i]) ; //Ali Added
						        //aniVal2=dppLevels_Cell[i]; 
						        aniVal2=cellsDppLevel[i]; 
                                                        rawAniData.dppLevel.push_back(aniVal2) ; //Ali Added 
	 
							rawAniData.aniNodePosArr.push_back(tmpPos);
							rawAniData.aniNodeVal.push_back(aniVal);
						}
						it = locIndexToAniIndexMap.find(index2);
						if (it == locIndexToAniIndexMap.end()) {
							locIndexToAniIndexMap.insert(
									std::pair<uint, uint>(index2, curIndex));
							curIndex++;
							tmpPos = CVector(node2X, node2Y, 0);
							//aniVal = hostTmpVectorNodeType[index1];
							aniVal = cellColors[i];
                            rawAniData.aniNodeF_MI_M_MagN_Int.push_back(tmpF_MI_M_MagN_Int[i]/cellsPerimeter[i]) ; //Ali Added
                                        		//aniVal2=dppLevels_Cell[i]; 
                                        		aniVal2=cellsDppLevel[i]; 
                                                        rawAniData.dppLevel.push_back(aniVal2) ; //Ali Added 

							rawAniData.aniNodePosArr.push_back(tmpPos);
							rawAniData.aniNodeVal.push_back(aniVal);
						}

						it = locIndexToAniIndexMap.find(index1);
						uint aniIndex1 = it->second;
						it = locIndexToAniIndexMap.find(index2);
						uint aniIndex2 = it->second;

						LinkAniData linkData;
						linkData.node1Index = aniIndex1;
						linkData.node2Index = aniIndex2;
						rawAniData.internalLinks.push_back(linkData);
					}
				}
			}
		}
	}

	// save filopodia information 
	CVector tempCenterPos, tempFilopPos,tempCellPolar;
	double filopX, filopY;
	uint curtotFilopCounts = 0;
	uint tempFilopCounts;
	double tempRadius, tempLen;
	// double PI = acos(-1.0);
    for (uint i=0; i<activeCellCount; i++){ // i is the index of an active cell 
			tempCenterPos = CVector(hostCellCenterX[i],hostCellCenterY[i],0); // store cell center 
			rawAniData.aniFilopPos.push_back(tempCenterPos);
			tempRadius = cellsPerimeter[i]/(2.0*PI);
			tempFilopCounts = 0;
			for (uint j=0; j<maxFilopPerCell; j++){ // loop through all all filopodia slots in each cell 
				index1 = i * maxFilopPerCell + j; 
				if (hostFilopIsActive[index1]== true){
						tempLen = sqrt(hostFilopX[index1]*hostFilopX[index1] + hostFilopY[index1]* hostFilopY[index1]);
						filopX = hostCellCenterX[i] + (tempLen + tempRadius) * cos(hostFilopAngle[index1]); //hostFilopX[index1]/tempLen;
						filopY = hostCellCenterY[i] + (tempLen + tempRadius) * sin(hostFilopAngle[index1]); //hostFilopY[index1]/tempLen;
						tempFilopPos = CVector(filopX, filopY, 0);
						rawAniData.aniFilopPos.push_back(tempFilopPos);
						tempFilopCounts = tempFilopCounts + 1;
						
						LinkAniCellData linkCellData;
						linkCellData.node1Index = curtotFilopCounts; 
						linkCellData.node2Index = curtotFilopCounts+tempFilopCounts; // double check if this index is correct or not
						rawAniData.aniFilopLinks.push_back(linkCellData);
					}

			}
			curtotFilopCounts = curtotFilopCounts + curActiveFilopCounts[i] + 1;
		}

	// save cell polarization information 
	uint curtCellPolarCounts = 0;
    for (uint i=0; i<activeCellCount; i++){ // i is the index of an active cell 
			tempCenterPos = CVector(hostCellCenterX[i],hostCellCenterY[i],0); // store cell center 
			rawAniData.aniCellPolar.push_back(tempCenterPos);
			tempRadius = cellsPerimeter[i]/(2.0*PI);
			tempCellPolar = CVector(hostCellCenterX[i]+tempRadius*cos(hostCellPolarAngle[i]),hostCellCenterY[i]+tempRadius*sin(hostCellPolarAngle[i]),0);
			rawAniData.aniCellPolar.push_back(tempCellPolar);
			
			LinkAniCellData linkCellPolarData;
			linkCellPolarData.node1Index = curtCellPolarCounts; 
			linkCellPolarData.node2Index = curtCellPolarCounts+1;
			rawAniData.aniCellPolarLinks.push_back(linkCellPolarData);

			curtCellPolarCounts = curtCellPolarCounts  + 2;
		}

	// save cell-cell (nodes on different cell membranes) adhesion information 
	CVector tempCCnode1, tempCCnode2;
	uint curCCAhesionCounts = 0;
	for (uint i = 0; i < activeCellCount; i++) {
		for (uint j = 0; j < curActiveMemNodeCounts[i]; j++) {
			index1 = beginIndx + i * maxNodePerCell + j;
			if ( hostIsActiveVec[index1]==true) {
				index2 = hostBondVec[index1];
				if (index2 > index1 && index2 != -1){
				
					tempCCnode1 = CVector(CVector(hostTmpVectorLocX[index1],
								hostTmpVectorLocY[index1], 0));
					tempCCnode2 = CVector(hostTmpVectorLocX[index2],
								hostTmpVectorLocY[index2], 0);
					rawAniData.aniCC.push_back(tempCCnode1);
					rawAniData.aniCC.push_back(tempCCnode2);

					LinkAniCellData linkCCData;
					linkCCData.node1Index = curCCAhesionCounts;
					linkCCData.node2Index = curCCAhesionCounts+1;
					rawAniData.aniCCLinks.push_back(linkCCData);

					curCCAhesionCounts = curCCAhesionCounts + 2;
				}
			}
		}
	}

	cout << "I am in obtainAniRawDataGivenCellColor end"<<endl; 
	return rawAniData;
}

void SceCells::copyInitActiveNodeCount_M(
		std::vector<uint>& initMembrActiveNodeCounts,
		std::vector<uint>& initIntnlActiveNodeCounts,
		std::vector<double> &initGrowProgVec,
		std::vector<double> &initCellRadii) {
	assert(
			initMembrActiveNodeCounts.size()
					== initIntnlActiveNodeCounts.size());
	totalNodeCountForActiveCells = initMembrActiveNodeCounts.size()
			* allocPara_m.maxAllNodePerCell;

	thrust::copy(initMembrActiveNodeCounts.begin(),
			initMembrActiveNodeCounts.end(),
			cellInfoVecs.activeMembrNodeCounts.begin());
	thrust::copy(initIntnlActiveNodeCounts.begin(),
			initIntnlActiveNodeCounts.end(),
			cellInfoVecs.activeIntnlNodeCounts.begin());
	thrust::copy(initGrowProgVec.begin(), initGrowProgVec.end(),
			cellInfoVecs.growthProgress.begin());
	/*
	for (int i=0;i<30;i++){
        double tempProg = cellInfoVecs.growthProgress[i];
        cout << tempProg << endl;
    }
	*/	
}

void SceCells::myDebugFunction() {

	uint maxActiveNodeCount = allocPara_m.currentActiveCellCount
			* allocPara_m.maxAllNodePerCell;
	uint maxActiveCellCount = allocPara_m.currentActiveCellCount;
	std::cout << "totalNodeCountforActiveCells: "
			<< totalNodeCountForActiveCells << std::endl;
	std::cout << "maxAllNodePerCell: " << allocPara_m.maxAllNodePerCell
			<< std::endl;
	std::cout << "maxActiveCellCount: " << maxActiveCellCount << std::endl;
	std::cout << "bdryNodeCount: " << allocPara_m.bdryNodeCount << std::endl;

	std::cout << "grow threshold: " << miscPara.growThreshold << std::endl;

	std::cout << std::endl;
	for (uint i = 0; i < maxActiveCellCount; i++) {
		std::cout << cellInfoVecs.growthProgress[i] << " ";
	}
	std::cout << std::endl;
	for (uint i = 0; i < maxActiveCellCount; i++) {
		std::cout << cellInfoVecs.isScheduledToGrow[i] << " ";
	}
	std::cout << std::endl;
	for (uint i = 0; i < maxActiveCellCount; i++) {
		std::cout << cellInfoVecs.lastCheckPoint[i] << " ";
	}
	std::cout << std::endl;
	for (uint i = 0; i < maxActiveNodeCount; i++) {
		if (nodes->getInfoVecs().nodeIsActive[i]
				&& nodes->getInfoVecs().nodeCellType[i] == CellIntnl) {
			std::cout << nodes->getInfoVecs().nodeVelX[i] << " ";
		}
	}
	std::cout << std::endl;
	for (uint i = 0; i < maxActiveCellCount; i++) {
		std::cout << cellInfoVecs.activeIntnlNodeCounts[i] << " ";
	}
	std::cout << std::endl;

	for (uint i = 0; i < maxActiveCellCount; i++) {
		std::cout << cellInfoVecs.expectedLength[i] << " ";
	}
	std::cout << std::endl;
	for (uint i = 0; i < maxActiveCellCount; i++) {
		std::cout << cellInfoVecs.smallestDistance[i] << " ";
	}
	std::cout << std::endl;
	for (uint i = 0; i < maxActiveCellCount; i++) {
		std::cout << cellInfoVecs.biggestDistance[i] << " ";
	}
	std::cout << std::endl;

	for (uint i = 0; i < maxActiveCellCount; i++) {
		std::cout << cellInfoVecs.lengthDifference[i] << " ";
	}
	std::cout << std::endl;
	for (uint i = 0; i < maxActiveCellCount; i++) {
		std::cout << cellInfoVecs.centerCoordX[i] << " ";
	}
	std::cout << std::endl;
	for (uint i = 0; i < maxActiveCellCount; i++) {
		std::cout << cellInfoVecs.centerCoordY[i] << " ";
	}
	std::cout << std::endl;
	for (uint i = 0; i < maxActiveCellCount; i++) {
		std::cout << cellInfoVecs.growthXDir[i] << " ";
	}
	std::cout << std::endl;
	for (uint i = 0; i < maxActiveCellCount; i++) {
		std::cout << cellInfoVecs.growthYDir[i] << " ";
	}
	std::cout << std::endl;

	int jj;
	std::cin >> jj;
}

void SceCells::divDebug() {

	std::cout << "tmpIsActive_M: ";
	for (uint i = 0; i < divAuxData.tmpIsActive_M.size(); i++) {
		std::cout << divAuxData.tmpIsActive_M[i] << " ";
	}
	std::cout << std::endl;
	std::cout << "tmpNodePosX_M: ";
	for (uint i = 0; i < divAuxData.tmpNodePosX_M.size(); i++) {
		std::cout << divAuxData.tmpNodePosX_M[i] << " ";
	}
	std::cout << std::endl;
	std::cout << "tmpNodePosY_M : ";
	for (uint i = 0; i < divAuxData.tmpNodePosY_M.size(); i++) {
		std::cout << divAuxData.tmpNodePosY_M[i] << " ";
	}
	std::cout << std::endl;

	std::cout << "tmpCellRank_M : ";
	for (uint i = 0; i < divAuxData.tmpCellRank_M.size(); i++) {
		std::cout << divAuxData.tmpCellRank_M[i] << " ";
	}
	std::cout << std::endl;
	std::cout << "tmpDivDirX_M : ";
	for (uint i = 0; i < divAuxData.tmpDivDirX_M.size(); i++) {
		std::cout << divAuxData.tmpDivDirX_M[i] << " ";
	}
	std::cout << std::endl;
	std::cout << "tmpDivDirY_M : ";
	for (uint i = 0; i < divAuxData.tmpDivDirY_M.size(); i++) {
		std::cout << divAuxData.tmpDivDirY_M[i] << " ";
	}
	std::cout << std::endl;
	std::cout << "tmpCenterPosX_M : ";
	for (uint i = 0; i < divAuxData.tmpCenterPosX_M.size(); i++) {
		std::cout << divAuxData.tmpCenterPosX_M[i] << " ";
	}
	std::cout << std::endl;
	std::cout << "tmpCenterPosY_M : ";
	for (uint i = 0; i < divAuxData.tmpCenterPosY_M.size(); i++) {
		std::cout << divAuxData.tmpCenterPosY_M[i] << " ";
	}
	std::cout << std::endl;

	std::cout << "tmpIsActive1_M : ";
	for (uint i = 0; i < divAuxData.tmpIsActive1_M.size(); i++) {
		std::cout << divAuxData.tmpIsActive1_M[i] << " ";
	}
	std::cout << std::endl;

	std::cout << "tmpXPos1_M : ";
	for (uint i = 0; i < divAuxData.tmpXPos1_M.size(); i++) {
		std::cout << divAuxData.tmpXPos1_M[i] << " ";
		if (i > 0 && i < allocPara_m.maxMembrNodePerCell
				&& divAuxData.tmpIsActive1_M[i]
				&& divAuxData.tmpIsActive1_M[i - 1]
				&& fabs(divAuxData.tmpXPos1_M[i] - divAuxData.tmpXPos1_M[i - 1])
						> 0.1) {
			std::cout << "11111111111111111111111, " << i << std::endl;
			int jj;
			cin >> jj;
		}
	}
	std::cout << std::endl;
	std::cout << "XPos1_onDevice : ";
	for (uint i = 0; i < divAuxData.tmpCellRank_M.size(); i++) {
		for (uint j = 0; j < allocPara_m.maxAllNodePerCell; j++) {
			uint index = divAuxData.tmpCellRank_M[i]
					* allocPara_m.maxAllNodePerCell + j;
			std::cout << nodes->getInfoVecs().nodeLocX[index] << " ";
		}
	}
	std::cout << std::endl;

	std::cout << "tmpYPos1_M : ";
	for (uint i = 0; i < divAuxData.tmpYPos1_M.size(); i++) {
		std::cout << divAuxData.tmpYPos1_M[i] << " ";
	}
	std::cout << std::endl;

	std::cout << "tmpIsActive2_M: ";
	for (uint i = 0; i < divAuxData.tmpIsActive2_M.size(); i++) {
		std::cout << divAuxData.tmpIsActive2_M[i] << " ";
	}
	std::cout << std::endl;

	std::cout << "tmpXPos2_M : ";
	for (uint i = 0; i < divAuxData.tmpXPos2_M.size(); i++) {
		std::cout << divAuxData.tmpXPos2_M[i] << " ";
		if (i > 0 && i < allocPara_m.maxMembrNodePerCell
				&& divAuxData.tmpIsActive2_M[i]
				&& divAuxData.tmpIsActive2_M[i - 1]
				&& fabs(divAuxData.tmpXPos2_M[i] - divAuxData.tmpXPos2_M[i - 1])
						> 0.1) {
			std::cout << "2222222222222222222, " << i << std::endl;
			int jj;
			cin >> jj;
		}
	}
	std::cout << std::endl;
	std::cout << "tmpYPos2_M : ";
	for (uint i = 0; i < divAuxData.tmpYPos2_M.size(); i++) {
		std::cout << divAuxData.tmpYPos2_M[i] << " ";
	}
	std::cout << std::endl;

	std::cout << "tmp1InternalActiveCounts: ";
	for (uint i = 0; i < divAuxData.tmp1InternalActiveCounts.size(); i++) {
		std::cout << divAuxData.tmp1InternalActiveCounts[i] << " ";
	}
	std::cout << std::endl;

	std::cout << "tmp2InternalActiveCounts: ";
	for (uint i = 0; i < divAuxData.tmp2InternalActiveCounts.size(); i++) {
		std::cout << divAuxData.tmp2InternalActiveCounts[i] << " ";
	}
	std::cout << std::endl;

	std::cout << "tmp1MemActiveCounts: ";
	for (uint i = 0; i < divAuxData.tmp1MemActiveCounts.size(); i++) {
		std::cout << divAuxData.tmp1MemActiveCounts[i] << " ";
	}
	std::cout << std::endl;

	std::cout << "tmp2MemActiveCounts: ";
	for (uint i = 0; i < divAuxData.tmp2MemActiveCounts.size(); i++) {
		std::cout << divAuxData.tmp2MemActiveCounts[i] << " ";
	}
	std::cout << std::endl;

	int jj;
	std::cin >> jj;
}

void SceCells::adjustGrowthInfo_M() {
	uint maxIntnlNodePerFollower = globalConfigVars.getConfigValue(
            "MaxIntnlNodeCountPerFollower").toInt();
	uint halfMax = maxIntnlNodePerFollower / 2;
	// uint halfMax = allocPara_m.maxIntnlNodePerCell / 2;
	thrust::transform_if(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							cellInfoVecs.activeIntnlNodeCounts.begin(),
							cellInfoVecs.growthProgress.begin(),
							cellInfoVecs.lastCheckPoint.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							cellInfoVecs.activeIntnlNodeCounts.begin(),
							cellInfoVecs.growthProgress.begin(),
							cellInfoVecs.lastCheckPoint.begin()))
					+ allocPara_m.currentActiveCellCount,
			cellInfoVecs.isScheduledToGrow.begin(),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.isScheduledToGrow.begin(),
							cellInfoVecs.growthProgress.begin(),
							cellInfoVecs.lastCheckPoint.begin())),
			AdjustGrowth(halfMax), thrust::identity<bool>());
}

VtkAnimationData SceCells::outputVtkData(AniRawData& rawAniData, 
		AnimationCriteria& aniCri) {
	VtkAnimationData vtkData;
	for (uint i = 0; i < rawAniData.aniNodePosArr.size(); i++) {
		PointAniData ptAniData;
		ptAniData.pos = rawAniData.aniNodePosArr[i];
		ptAniData.F_MI_M_MagN_Int= rawAniData.aniNodeF_MI_M_MagN_Int[i]; //AliE
		ptAniData.dppLevel1= rawAniData.dppLevel[i]; //AliE
		ptAniData.F_MI_M = rawAniData.aniNodeF_MI_M[i];//AAMIRI
		ptAniData.colorScale = rawAniData.aniNodeVal[i];
		ptAniData.colorScale2 = rawAniData.aniNodeCurvature[i];//AAMIRI
		ptAniData.rankScale = rawAniData.aniCellRank[i];//AAMIRI
		ptAniData.nodeRankScale = rawAniData.aniNodeRank[i];
		ptAniData.extForce = rawAniData.aniNodeExtForceArr[i];//AAMIRI
		ptAniData.myoLevel1= rawAniData.myoLevel[i];
		ptAniData.myoWeight1= rawAniData.myoWeight[i];
		ptAniData.actLevel1= rawAniData.actLevel[i];  
		ptAniData.adhSiteCount1= rawAniData.adhSiteCount[i];
		ptAniData.adhNodeIndex1= rawAniData.adhNodeIndex[i];
		ptAniData.minToMDist1= rawAniData.minToMDist[i];
		vtkData.pointsAniData.push_back(ptAniData);
	}
	// vtk file one 
	for (uint i = 0; i < rawAniData.internalLinks.size(); i++) {
		LinkAniData linkData = rawAniData.internalLinks[i];
		vtkData.linksAniData.push_back(linkData);
	}
	for (uint i = 0; i < rawAniData.memLinks.size(); i++) {
		LinkAniData linkData = rawAniData.memLinks[i];
		vtkData.linksAniData.push_back(linkData);
	}
	// vtk file two 
	for (uint i = 0; i<rawAniData.aniFilopPos.size(); i++){
		PointAniCellData ptAniCellData; // to be changed
		ptAniCellData.filopPos = rawAniData.aniFilopPos[i]; // ???
		vtkData.pointsAniCellData.push_back(ptAniCellData);
	}
	for (uint i = 0; i < rawAniData.aniFilopLinks.size(); i++) {
		LinkAniCellData linkCellData = rawAniData.aniFilopLinks[i];
		vtkData.linksAniCellData.push_back(linkCellData);
	}

	// vtk file three, shows cell polarity 
	for (uint i = 0; i<rawAniData.aniCellPolar.size(); i++){
		PointAniCellPolarData ptAniCellPolarData; 
		ptAniCellPolarData.cellPolarAngle = rawAniData.aniCellPolar[i];
		vtkData.pointsAniCellPolarData.push_back(ptAniCellPolarData);
	}
	for (uint i = 0; i < rawAniData.aniCellPolarLinks.size(); i++) {
		LinkAniCellData linkCellPolarData = rawAniData.aniCellPolarLinks[i];
		vtkData.linksAniCellPolarData.push_back(linkCellPolarData);
	}

	// vtk file four, to show cell-cell adhesion bonds
	for (uint i = 0; i<rawAniData.aniCC.size(); i++){
		PointAniCCData ptAniCCData; 
		ptAniCCData.ccAdhesion = rawAniData.aniCC[i];
		vtkData.pointsAniCCData.push_back(ptAniCCData);
	}
	for (uint i = 0; i < rawAniData.aniCCLinks.size(); i++) {
		LinkAniCellData linkCCData = rawAniData.aniCCLinks[i];
		vtkData.linksAniCCData.push_back(linkCCData);
	}

	vtkData.isArrowIncluded = false;
	return vtkData;
}

void SceCells::copyToGPUConstMem() {
	double pI_CPU = acos(-1.0);
	double minLengthCPU =
			globalConfigVars.getConfigValue("MinLength").toDouble();
	cudaMemcpyToSymbol(minLength, &minLengthCPU, sizeof(double));
	double minDivisorCPU =
			globalConfigVars.getConfigValue("MinDivisor").toDouble();
	cudaMemcpyToSymbol(minDivisor, &minDivisorCPU, sizeof(double));
	cudaMemcpyToSymbol(membrEquLen, &membrPara.membrEquLenCPU, sizeof(double));
	cudaMemcpyToSymbol(membrStiff, &membrPara.membrStiffCPU, sizeof(double));
	cudaMemcpyToSymbol(membrStiff_Mitotic, &membrPara.membrStiff_Mitotic, sizeof(double)); // Ali June 30
	cudaMemcpyToSymbol(pI, &pI_CPU, sizeof(double));

	cudaMemcpyToSymbol(bendCoeff, &membrPara.membrBendCoeff, sizeof(double));

	cudaMemcpyToSymbol(bendCoeff_Mitotic, &membrPara.membrBendCoeff_Mitotic, sizeof(double));//AAMIRI
	
	cudaMemcpyToSymbol(bendCoeffLeader, &membrPara.membrBendCoeffLeader, sizeof(double));

	cudaMemcpyToSymbol(F_Ext_Incline_M2, &membrPara.F_Ext_Incline, sizeof(double)); //Ali
	double maxAdhBondLenCPU_M = globalConfigVars.getConfigValue("MaxAdhBondLen").toDouble();
    cudaMemcpyToSymbol(maxAdhBondLen_M, &maxAdhBondLenCPU_M,
            sizeof(double));


    double minAdhBondLen =
            globalConfigVars.getConfigValue("MinAdhBondLen").toDouble();
    double bondStiff = globalConfigVars.getConfigValue("BondStiff").toDouble();
    double bondStiff_Mitotic = globalConfigVars.getConfigValue("BondStiff_Mitotic").toDouble();
    double growthPrgrCriVal = globalConfigVars.getConfigValue(
            "GrowthPrgrCriVal").toDouble();

    cudaMemcpyToSymbol(minAdhBondLen_M, &minAdhBondLen,
            sizeof(double));
    cudaMemcpyToSymbol(bondStiff_M, &bondStiff, sizeof(double));
    cudaMemcpyToSymbol(bondStiff_Mitotic, &bondStiff_Mitotic, sizeof(double));
    cudaMemcpyToSymbol(growthPrgrCriVal_M, &growthPrgrCriVal, sizeof(double));

	uint maxAllNodePerCellCPU = globalConfigVars.getConfigValue(
			"MaxAllNodeCountPerCell").toInt();
	uint maxMembrNodePerCellCPU = globalConfigVars.getConfigValue(
			"MaxMembrNodeCountPerCell").toInt();
	uint maxIntnlNodePerCellCPU = globalConfigVars.getConfigValue(
			"MaxIntnlNodeCountPerCell").toInt();
	uint maxIntnlNodePerFollowerCPU = globalConfigVars.getConfigValue(
			"MaxIntnlNodeCountPerFollower").toInt();

	cudaMemcpyToSymbol(maxAllNodePerCell, &maxAllNodePerCellCPU, sizeof(uint));
	cudaMemcpyToSymbol(maxMembrPerCell, &maxMembrNodePerCellCPU, sizeof(uint));
	cudaMemcpyToSymbol(maxIntnlPerFollower, &maxIntnlNodePerFollowerCPU, sizeof(uint));

	double sceIntnlBParaCPU_M[5];
	double sceIntraParaCPU_M[5];
	double sceIntraParaDivCPU_M[5];

	double U0_IntnlB =
			globalConfigVars.getConfigValue("SceIntnlB_U0").toDouble();
	double V0_IntnlB =
			globalConfigVars.getConfigValue("SceIntnlB_V0").toDouble();
	double k1_IntnlB =
			globalConfigVars.getConfigValue("SceIntnlB_k1").toDouble();
	double k2_IntnlB =
			globalConfigVars.getConfigValue("SceIntnlB_k2").toDouble();
	double intnlBEffectiveRange = globalConfigVars.getConfigValue(
			"IntnlBEffectRange").toDouble();
	sceIntnlBParaCPU_M[0] = U0_IntnlB;
	sceIntnlBParaCPU_M[1] = V0_IntnlB;
	sceIntnlBParaCPU_M[2] = k1_IntnlB;
	sceIntnlBParaCPU_M[3] = k2_IntnlB;
	sceIntnlBParaCPU_M[4] = intnlBEffectiveRange;


        
 
	//////////////////////
	//// Block 3 /////////
	//////////////////////
	double U0_Intra =
			globalConfigVars.getConfigValue("IntraCell_U0").toDouble();
	double V0_Intra =
			globalConfigVars.getConfigValue("IntraCell_V0").toDouble();
	double k1_Intra =
			globalConfigVars.getConfigValue("IntraCell_k1").toDouble();
	double k2_Intra =
			globalConfigVars.getConfigValue("IntraCell_k2").toDouble();
	double intraLinkEffectiveRange = globalConfigVars.getConfigValue(
			"IntraEffectRange").toDouble();
	sceIntraParaCPU_M[0] = U0_Intra;
	sceIntraParaCPU_M[1] = V0_Intra;
	sceIntraParaCPU_M[2] = k1_Intra;
	sceIntraParaCPU_M[3] = k2_Intra;
	sceIntraParaCPU_M[4] = intraLinkEffectiveRange;

	//////////////////////
	//// Block 4 /////////
	//////////////////////
	double U0_Intra_Div =
			globalConfigVars.getConfigValue("IntraCell_U0_Div").toDouble();
	double V0_Intra_Div =
			globalConfigVars.getConfigValue("IntraCell_V0_Div").toDouble();
	double k1_Intra_Div =
			globalConfigVars.getConfigValue("IntraCell_k1_Div").toDouble();
	double k2_Intra_Div =
			globalConfigVars.getConfigValue("IntraCell_k2_Div").toDouble();
	double intraDivEffectiveRange = globalConfigVars.getConfigValue(
			"IntraDivEffectRange").toDouble();
	sceIntraParaDivCPU_M[0] = U0_Intra_Div;
	sceIntraParaDivCPU_M[1] = V0_Intra_Div;
	sceIntraParaDivCPU_M[2] = k1_Intra_Div;
	sceIntraParaDivCPU_M[3] = k2_Intra_Div;
	sceIntraParaDivCPU_M[4] = intraDivEffectiveRange;

	cudaMemcpyToSymbol(grthPrgrCriEnd_M, &growthAuxData.grthProgrEndCPU,
			sizeof(double));
	//cudaMemcpyToSymbol(grthPrgrCriVal_M, &growthPrgrCriVal, sizeof(double));
	cudaMemcpyToSymbol(sceIB_M, sceIntnlBParaCPU_M, 5 * sizeof(double));
	cudaMemcpyToSymbol(sceII_M, sceIntraParaCPU_M, 5 * sizeof(double));
	cudaMemcpyToSymbol(sceIIDiv_M, sceIntraParaDivCPU_M, 5 * sizeof(double));

	double IBDivHost[5];
	IBDivHost[0] =
			globalConfigVars.getConfigValue("SceIntnlB_U0_Div").toDouble();
	IBDivHost[1] =
			globalConfigVars.getConfigValue("SceIntnlB_V0_Div").toDouble();
	IBDivHost[2] =
			globalConfigVars.getConfigValue("SceIntnlB_k1_Div").toDouble();
	IBDivHost[3] =
			globalConfigVars.getConfigValue("SceIntnlB_k2_Div").toDouble();
	IBDivHost[4] =
			globalConfigVars.getConfigValue("IntnlBDivEffectRange").toDouble();
	cudaMemcpyToSymbol(sceIBDiv_M, IBDivHost, 5 * sizeof(double));
}

void SceCells::handleMembrGrowth_M() {
	// figure out membr growth speed
	calMembrGrowSpeed_M();
	// figure out which cells will add new point

	adjustMembrGrowSpeed_M();

	decideIfAddMembrNode_M();
// add membr nodes
	addMembrNodes_M();
	//membrDebug();
	
	/*
	for (int i=0;i<30;i++){
		double tempProg = cellInfoVecs.growthProgress[i];
		int tempType = cellInfoVecs.cell_Type[i];
		cout << "progress " <<tempProg;
		cout << " cell_type " <<tempType<<endl;
	}
	*/
}





void SceCells::calMembrGrowSpeed_M() {
	membrPara.membrGrowCoeff = growthAuxData.prolifDecay
			* membrPara.membrGrowCoeff_Ori;
	membrPara.membrGrowLimit = growthAuxData.prolifDecay
			* membrPara.membrGrowLimit_Ori;
// reduce_by_key, find value of max tension and their index
	thrust::counting_iterator<uint> iBegin(0);
	uint maxNPerCell = allocPara_m.maxAllNodePerCell;
    //uint maxNPerCell = globalConfigVars.getConfigValue(
    //        "MaxIntnlNodeCountPerCell").toInt();
	thrust::reduce_by_key(
			make_transform_iterator(iBegin, DivideFunctor(maxNPerCell)),
			make_transform_iterator(iBegin, DivideFunctor(maxNPerCell))
					+ totalNodeCountForActiveCells,
			thrust::make_zip_iterator(
					thrust::make_tuple(
							nodes->getInfoVecs().membrTenMagRi.begin(),
							make_transform_iterator(iBegin,
									ModuloFunctor(maxNPerCell)),
							nodes->getInfoVecs().membrLinkRiMidX.begin(),
							nodes->getInfoVecs().membrLinkRiMidY.begin(),
							nodes->getInfoVecs().membrDistToRi.begin())),
			cellInfoVecs.cellRanksTmpStorage.begin(),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.maxTenRiVec.begin(),
							cellInfoVecs.maxTenIndxVec.begin(),
							cellInfoVecs.maxTenRiMidXVec.begin(),
							cellInfoVecs.maxTenRiMidYVec.begin(),
							cellInfoVecs.maxDistToRiVec.begin())),
			thrust::equal_to<uint>(), MaxWInfo());

	thrust::reduce_by_key(
			make_transform_iterator(iBegin, DivideFunctor(maxNPerCell)),
			make_transform_iterator(iBegin, DivideFunctor(maxNPerCell))
					+ totalNodeCountForActiveCells,
			nodes->getInfoVecs().membrTensionMag.begin(),
			cellInfoVecs.cellRanksTmpStorage.begin(),
			cellInfoVecs.aveTension.begin(), thrust::equal_to<uint>(),
			thrust::plus<double>());

	thrust::transform(cellInfoVecs.aveTension.begin(),
			cellInfoVecs.aveTension.begin()
					+ allocPara_m.currentActiveCellCount,
			cellInfoVecs.activeMembrNodeCounts.begin(),
			cellInfoVecs.aveTension.begin(), thrust::divides<double>());

	// linear relationship with highest tension; capped by a given value
	thrust::transform(cellInfoVecs.aveTension.begin(),
			cellInfoVecs.aveTension.begin()
					+ allocPara_m.currentActiveCellCount,
			cellInfoVecs.membrGrowSpeed.begin(),
			MultiWithLimit(membrPara.membrGrowCoeff, membrPara.membrGrowLimit));
}

void SceCells::adjustMembrGrowSpeed_M() {
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							cellInfoVecs.activeMembrNodeCounts.begin(),
							cellInfoVecs.activeIntnlNodeCounts.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							cellInfoVecs.activeMembrNodeCounts.begin(),
							cellInfoVecs.activeIntnlNodeCounts.begin()))
					+ allocPara_m.currentActiveCellCount,
			cellInfoVecs.membrGrowSpeed.begin(),
			AdjustMembrGrow(membrPara.growthConst_N, membrPara.initMembrCt_N,
					membrPara.initIntnlCt_N));
}

void SceCells::decideIfAddMembrNode_M() {
// decide if add membrane node given current active node count and
// membr growth progress
	uint curActCellCt = allocPara_m.currentActiveCellCount;
	thrust::transform(cellInfoVecs.membrGrowSpeed.begin(),
			cellInfoVecs.membrGrowSpeed.begin() + curActCellCt,
			cellInfoVecs.membrGrowProgress.begin(),
			cellInfoVecs.membrGrowProgress.begin(), SaxpyFunctor(dt));

	uint maxMembrNode = allocPara_m.maxMembrNodePerCell;
/**Ali	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.membrGrowProgress.begin(),
							cellInfoVecs.activeMembrNodeCounts.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.membrGrowProgress.begin(),
							cellInfoVecs.activeMembrNodeCounts.begin()))
					+ curActCellCt,
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.isMembrAddingNode.begin(),
							cellInfoVecs.membrGrowProgress.begin())),
			MemGrowFunc(maxMembrNode));
*/
         thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.activeMembrNodeCounts.begin(),
							   cellInfoVecs.membrGrowProgress.begin(),cellInfoVecs.maxDistToRiVec.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.activeMembrNodeCounts.begin(),
							   cellInfoVecs.membrGrowProgress.begin(),cellInfoVecs.maxDistToRiVec.begin()))
					+ curActCellCt,
			thrust::make_zip_iterator(
					thrust::make_tuple(cellInfoVecs.isMembrAddingNode.begin(),
							cellInfoVecs.membrGrowProgress.begin())),
			MemGrowFunc(maxMembrNode));

}

/**
 * Add new membrane elements to cells.
 * This operation is relatively expensive because of memory rearrangement.
 */
void SceCells::addMembrNodes_M() {
	thrust::counting_iterator<uint> iBegin(0);
	uint curAcCCount = allocPara_m.currentActiveCellCount;
	uint maxNodePerCell = allocPara_m.maxAllNodePerCell;
	thrust::transform_if(
			thrust::make_zip_iterator(
					thrust::make_tuple(iBegin,
							cellInfoVecs.maxTenIndxVec.begin(),
							cellInfoVecs.activeMembrNodeCounts.begin(),
							cellInfoVecs.maxTenRiMidXVec.begin(),
							cellInfoVecs.maxTenRiMidYVec.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(iBegin,
							cellInfoVecs.maxTenIndxVec.begin(),
							cellInfoVecs.activeMembrNodeCounts.begin(),
							cellInfoVecs.maxTenRiMidXVec.begin(),
							cellInfoVecs.maxTenRiMidYVec.begin()))
					+ curAcCCount, cellInfoVecs.isMembrAddingNode.begin(),
			cellInfoVecs.activeMembrNodeCounts.begin(),
			AddMemNode(maxNodePerCell, growthAuxData.nodeIsActiveAddress,
					growthAuxData.nodeXPosAddress,
					growthAuxData.nodeYPosAddress, growthAuxData.adhIndxAddr),
			thrust::identity<bool>());
}

void SceCells::membrDebug() {
	uint curAcCCount = allocPara_m.currentActiveCellCount;
	uint maxActiveNodeC = curAcCCount * allocPara_m.maxAllNodePerCell;
	uint maxNodePC = allocPara_m.maxAllNodePerCell;
	//uint tmp = 0;
	//for (uint i = 0; i < curAcCCount; i++) {
	//	tmp += cellInfoVecs.isMembrAddingNode[i];
	//}
	//if (tmp != 0) {
	//	tmpDebug = true;
	//}
	//if (!tmpDebug) {
	//	return;
	//}
	for (uint i = 0; i < maxActiveNodeC; i++) {
		if (i % maxNodePC == 0 || i % maxNodePC == 199
				|| i % maxNodePC == 200) {
			std::cout << nodes->getInfoVecs().membrTensionMag[i] << " ";
		}
	}
	std::cout << std::endl;
	for (uint i = 0; i < maxActiveNodeC; i++) {
		if (i % maxNodePC == 0 || i % maxNodePC == 199
				|| i % maxNodePC == 200) {
			std::cout << nodes->getInfoVecs().membrTenMagRi[i] << " ";
		}
	}
	std::cout << std::endl;
	for (uint i = 0; i < maxActiveNodeC; i++) {
		if (i % maxNodePC == 0 || i % maxNodePC == 199
				|| i % maxNodePC == 200) {
			std::cout << nodes->getInfoVecs().membrLinkRiMidX[i] << " ";
		}
	}
	std::cout << std::endl;
	for (uint i = 0; i < maxActiveNodeC; i++) {
		if (i % maxNodePC == 0 || i % maxNodePC == 199
				|| i % maxNodePC == 200) {
			std::cout << nodes->getInfoVecs().membrLinkRiMidY[i] << " ";
		}
	}
	std::cout << std::endl;
	for (uint i = 0; i < maxActiveNodeC; i++) {
		std::cout << nodes->getInfoVecs().membrBendLeftX[i] << " ";
	}
	std::cout << std::endl;
	for (uint i = 0; i < maxActiveNodeC; i++) {
		std::cout << nodes->getInfoVecs().membrBendLeftY[i] << " ";
	}
	std::cout << std::endl;
	for (uint i = 0; i < maxActiveNodeC; i++) {
		std::cout << nodes->getInfoVecs().membrBendRightX[i] << " ";
	}
	std::cout << std::endl;
	for (uint i = 0; i < maxActiveNodeC; i++) {
		std::cout << nodes->getInfoVecs().membrBendRightX[i] << " ";
	}
	std::cout << std::endl;
	for (uint i = 0; i < curAcCCount; i++) {
		std::cout << "(" << cellInfoVecs.maxTenIndxVec[i] << ","
				<< cellInfoVecs.activeMembrNodeCounts[i] << ","
				<< cellInfoVecs.maxTenRiMidXVec[i] << ","
				<< cellInfoVecs.maxTenRiMidYVec[i] << ")" << std::endl;
	}
	int jj;
	std::cin >> jj;
}

void SceCells::assembleVecForTwoCells(uint i) {
	uint membThreshold = allocPara_m.maxMembrNodePerCell;
	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;
	uint index;
	for (uint j = 0; j < membThreshold; j++) {
		index = i * maxAllNodePerCell + j;
		if (j < divAuxData.tmp1VecMem.size()) {
			divAuxData.tmpXPos1_M[index] = divAuxData.tmp1VecMem[j].x;
			divAuxData.tmpYPos1_M[index] = divAuxData.tmp1VecMem[j].y;
			divAuxData.tmpIsActive1_M[index] = true;
		} else {
			divAuxData.tmpIsActive1_M[index] = false;
		}
	}
	for (uint j = 0; j < membThreshold; j++) {
		index = i * maxAllNodePerCell + j;
		if (j < divAuxData.tmp2VecMem.size()) {
			divAuxData.tmpXPos2_M[index] = divAuxData.tmp2VecMem[j].x;
			divAuxData.tmpYPos2_M[index] = divAuxData.tmp2VecMem[j].y;
			divAuxData.tmpIsActive2_M[index] = true;
		} else {
			divAuxData.tmpIsActive2_M[index] = false;
		}
	}

	divAuxData.tmp1MemActiveCounts.push_back(divAuxData.tmp1VecMem.size());
	divAuxData.tmp2MemActiveCounts.push_back(divAuxData.tmp2VecMem.size());

	for (uint j = membThreshold; j < maxAllNodePerCell; j++) {
		index = i * maxAllNodePerCell + j;
		uint shift_j = j - membThreshold;
		if (shift_j < divAuxData.tmp1IntnlVec.size()) {
			divAuxData.tmpXPos1_M[index] = divAuxData.tmp1IntnlVec[shift_j].x;
			divAuxData.tmpYPos1_M[index] = divAuxData.tmp1IntnlVec[shift_j].y;
			divAuxData.tmpIsActive1_M[index] = true;
		} else {
			divAuxData.tmpIsActive1_M[index] = false;
		}
		if (shift_j < divAuxData.tmp2IntnlVec.size()) {
			divAuxData.tmpXPos2_M[index] = divAuxData.tmp2IntnlVec[shift_j].x;
			divAuxData.tmpYPos2_M[index] = divAuxData.tmp2IntnlVec[shift_j].y;
			divAuxData.tmpIsActive2_M[index] = true;
		} else {
			divAuxData.tmpIsActive2_M[index] = false;
		}
	}
	divAuxData.tmp1InternalActiveCounts.push_back(
			divAuxData.tmp1IntnlVec.size());
	divAuxData.tmp2InternalActiveCounts.push_back(
			divAuxData.tmp2IntnlVec.size());
}

void SceCells::shiftIntnlNodesByCellCenter(CVector cell1Center,
		CVector cell2Center) {
	CVector tmpCell1Center(0, 0, 0);
	for (uint j = 0; j < divAuxData.tmp1IntnlVec.size(); j++) {
		tmpCell1Center = tmpCell1Center + divAuxData.tmp1IntnlVec[j];
	}
	tmpCell1Center = tmpCell1Center / divAuxData.tmp1IntnlVec.size();
	CVector shiftVec1 = cell1Center - tmpCell1Center;
	for (uint j = 0; j < divAuxData.tmp1IntnlVec.size(); j++) {
		divAuxData.tmp1IntnlVec[j] = divAuxData.tmp1IntnlVec[j] + shiftVec1;
	}

	CVector tmpCell2Center(0, 0, 0);
	for (uint j = 0; j < divAuxData.tmp2IntnlVec.size(); j++) {
		tmpCell2Center = tmpCell2Center + divAuxData.tmp2IntnlVec[j];
	}
	tmpCell2Center = tmpCell2Center / divAuxData.tmp2IntnlVec.size();
	CVector shiftVec2 = cell2Center - tmpCell2Center;
	for (uint j = 0; j < divAuxData.tmp2IntnlVec.size(); j++) {
		divAuxData.tmp2IntnlVec[j] = divAuxData.tmp2IntnlVec[j] + shiftVec2;
	}
}

void SceCells::processMemVec(std::vector<VecVal>& tmp1,
		std::vector<VecVal>& tmp2) {
	divAuxData.tmp1VecMem.clear();
	divAuxData.tmp2VecMem.clear();

	uint membThreshold = allocPara_m.maxMembrNodePerCell;

	std::sort(tmp1.begin(), tmp1.end());
	std::sort(tmp2.begin(), tmp2.end());

	//assert(tmp1.size() < allocPara_m.maxMembrNodePerCell);
	//assert(tmp2.size() < allocPara_m.maxMembrNodePerCell);

	uint maxDivMembrNodeCount1 = allocPara_m.maxMembrNodePerCell - tmp1.size();
	uint maxDivMembrNodeCount2 = allocPara_m.maxMembrNodePerCell - tmp2.size();

	std::vector<CVector> ptsBetween1, ptsBetween2;

	// if size is less than 1, the situation would have already been very bad.
	// Just keep this statement so no seg fault would happen.
	if (tmp1.size() >= 1) {
		ptsBetween1 = obtainPtsBetween(tmp1[tmp1.size() - 1].vec, tmp1[0].vec,
				memNewSpacing, maxDivMembrNodeCount1);
	}
	// if size is less than 1, the situation would have already been very bad.
	// Just keep this statement so no seg fault would happen.
	if (tmp2.size() >= 1) {
		ptsBetween2 = obtainPtsBetween(tmp2[tmp2.size() - 1].vec, tmp2[0].vec,
				memNewSpacing, maxDivMembrNodeCount2);
	}

	for (uint j = 0; j < tmp1.size(); j++) {
		divAuxData.tmp1VecMem.push_back(tmp1[j].vec);
	}
	for (uint j = 0; j < tmp2.size(); j++) {
		divAuxData.tmp2VecMem.push_back(tmp2[j].vec);
	}
	for (uint j = 0; j < ptsBetween1.size(); j++) {
		divAuxData.tmp1VecMem.push_back(ptsBetween1[j]);
	}
	for (uint j = 0; j < ptsBetween2.size(); j++) {
		divAuxData.tmp2VecMem.push_back(ptsBetween2[j]);
	}

	assert(divAuxData.tmp1VecMem.size() <= membThreshold);
	assert(divAuxData.tmp2VecMem.size() <= membThreshold);
}

void SceCells::obtainMembrAndIntnlNodes(uint i, vector<CVector>& membrNodes,
		vector<CVector>& intnlNodes) {
	membrNodes.clear();
	intnlNodes.clear();

	uint membThreshold = allocPara_m.maxMembrNodePerCell;
	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;
	uint index;
	for (uint j = 0; j < maxAllNodePerCell; j++) {
		index = i * maxAllNodePerCell + j;
		if (divAuxData.tmpIsActive_M[index] != true) {
			continue;
		}
		double posX = divAuxData.tmpNodePosX_M[index];
		double posY = divAuxData.tmpNodePosY_M[index];
		if (j < membThreshold) {
			// means node type is membrane
			CVector memPos(posX, posY, 0);
			membrNodes.push_back(memPos);
		} else {
			CVector intnlPos(posX, posY, 0);
			intnlNodes.push_back(intnlPos);
		}
	}
}

CVector SceCells::obtainCenter(uint i) {
	double oldCenterX = divAuxData.tmpCenterPosX_M[i];
	double oldCenterY = divAuxData.tmpCenterPosY_M[i];
	CVector centerPos(oldCenterX, oldCenterY, 0);
	return centerPos;
}

CVector SceCells::calDivDir_MajorAxis(CVector center,
		vector<CVector>& membrNodes, double& lenAlongMajorAxis) {
// not the optimal algorithm but easy to code
	double maxDiff = 0;
	CVector majorAxisDir;
	for (uint i = 0; i < membrNodes.size(); i++) {
		CVector tmpDir = membrNodes[i] - center;
		CVector tmpUnitDir = tmpDir.getUnitVector();
		double min = 0, max = 0;
		for (uint j = 0; j < membrNodes.size(); j++) {
			CVector tmpDir2 = membrNodes[j] - center;
			double tmpVecProduct = tmpDir2 * tmpUnitDir;
			if (tmpVecProduct < min) {
				min = tmpVecProduct;
			}
			if (tmpVecProduct > max) {
				max = tmpVecProduct;
			}
		}
		double diff = max - min;
		if (diff > maxDiff) {
			maxDiff = diff;
			majorAxisDir = tmpUnitDir;
		}
	}
	lenAlongMajorAxis = maxDiff;
	return majorAxisDir;
}


//A&A
double SceCells::calLengthAlongHertwigAxis(CVector divDir, CVector center,
		vector<CVector>& membrNodes) {

	CVector divDirUnit = divDir.getUnitVector();


	double minUnit = 0, maxUnit = 0;
	double minOveral = 0, maxOveral = 0;
	for (uint i = 0; i < membrNodes.size(); i++) {
		CVector tmpDir = membrNodes[i] - center;
		CVector tmpUnitDir = tmpDir.getUnitVector();
			double tmpVecProductUnit = divDirUnit * tmpUnitDir;
			double tmpVecProductOveral = divDirUnit * tmpDir;
			if (tmpVecProductUnit < minUnit) {
				minUnit = tmpVecProductUnit;
				minOveral = tmpVecProductOveral;
			}
			if (tmpVecProductUnit > maxUnit) {
				maxUnit = tmpVecProductUnit;
				maxOveral = tmpVecProductOveral;
			}
	}
	
		double lenAlongHertwigAxis = maxOveral - minOveral;
	return lenAlongHertwigAxis;
}


void SceCells::obtainTwoNewCenters(CVector& oldCenter, CVector& divDir,
		double len_MajorAxis, CVector& centerNew1, CVector& centerNew2) {

	CVector divDirUnit = divDir.getUnitVector();
	double lenChange = len_MajorAxis / 2.0 * centerShiftRatio;
	centerNew1 = oldCenter + lenChange * divDirUnit;
	centerNew2 = oldCenter - lenChange * divDirUnit;
}

void SceCells::prepareTmpVec(uint i, CVector divDir, CVector oldCenter,
		std::vector<VecVal>& tmp1, std::vector<VecVal>& tmp2) {
	tmp1.clear();
	tmp2.clear();
	uint membThreshold = allocPara_m.maxMembrNodePerCell;
	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;
	uint index;
	VecVal tmpData;
	CVector splitDir = divDir.rotateNintyDeg_XY_CC();
	for (uint j = 0; j < maxAllNodePerCell; j++) {
		index = i * maxAllNodePerCell + j;
		if (j < membThreshold) {
			// means node type is membrane
			if (divAuxData.tmpIsActive_M[index] == true) {
				CVector memPos(divAuxData.tmpNodePosX_M[index],
						divAuxData.tmpNodePosY_M[index], 0);
				CVector centerToPosDir = memPos - oldCenter;
				CVector centerToPosUnit = centerToPosDir.getUnitVector();
				CVector crossProduct = Cross(centerToPosDir, splitDir);
				double dotProduct = centerToPosUnit * splitDir;
				tmpData.val = dotProduct;
				tmpData.vec = memPos;
				if (crossProduct.z >= 0) {
					// counter-cloce wise
					tmp1.push_back(tmpData);
				} else {
					// cloce wise
					tmp2.push_back(tmpData);
				}
			}
		} else {
			if (divAuxData.tmpIsActive_M[index] == true) {
				CVector internalPos(divAuxData.tmpNodePosX_M[index],
						divAuxData.tmpNodePosY_M[index], 0);
				CVector centerToPosDir = internalPos - oldCenter;
				CVector shrinkedPos = centerToPosDir * shrinkRatio + oldCenter;
				double dotProduct = centerToPosDir * divDir;
				if (dotProduct > 0) {
					divAuxData.tmp1IntnlVec.push_back(shrinkedPos);
				} else {
					divAuxData.tmp2IntnlVec.push_back(shrinkedPos);
				}
			}
		}
	}
}

void SceCells::calCellArea() {
	thrust::counting_iterator<uint> iBegin(0), iBegin2(0);
	totalNodeCountForActiveCells = allocPara_m.currentActiveCellCount
			* allocPara_m.maxAllNodePerCell;
	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;

	double* nodeLocXAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocX[0]));
	double* nodeLocYAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocY[0]));
	bool* nodeIsActiveAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeIsActive[0]));

	thrust::reduce_by_key(
			make_transform_iterator(iBegin, DivideFunctor(maxAllNodePerCell)),
			make_transform_iterator(iBegin, DivideFunctor(maxAllNodePerCell))
					+ totalNodeCountForActiveCells,
			thrust::make_transform_iterator(
					thrust::make_zip_iterator(
							thrust::make_tuple(
									thrust::make_permutation_iterator(
											cellInfoVecs.activeMembrNodeCounts.begin(),
											make_transform_iterator(iBegin,
													DivideFunctor(
															maxAllNodePerCell))),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell)),
									make_transform_iterator(iBegin,
											ModuloFunctor(maxAllNodePerCell)),
									make_permutation_iterator(
											cellInfoVecs.centerCoordX.begin(),
											make_transform_iterator(iBegin,
													DivideFunctor(
															maxAllNodePerCell))),
									make_permutation_iterator(
											cellInfoVecs.centerCoordY.begin(),
											make_transform_iterator(iBegin,
													DivideFunctor(
															maxAllNodePerCell))))),
					CalTriAreaV1(maxAllNodePerCell, nodeIsActiveAddr,
							nodeLocXAddr, nodeLocYAddr)),
			cellInfoVecs.cellRanksTmpStorage.begin(),
			cellInfoVecs.cellAreaVec.begin(), thrust::equal_to<uint>(),
			thrust::plus<double>());
	// thurst make positive
	thrust::transform(cellInfoVecs.cellAreaVec.begin(), cellInfoVecs.cellAreaVec.begin()+allocPara_m.currentActiveCellCount, cellInfoVecs.cellAreaVec.begin(), calAbs(totalNodeCountForActiveCells));
	// for (uint tempCellInd=0; tempCellInd<allocPara_m.currentActiveCellCount; tempCellInd++){
		
	// }
}


   //AAMIRI added to calculate Perimeter of each cell
 void SceCells::calCellPerim() {
 	thrust::counting_iterator<uint> iBegin(0), iBegin2(0);
 	totalNodeCountForActiveCells = allocPara_m.currentActiveCellCount
 			* allocPara_m.maxAllNodePerCell;
 	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;
 
 	double* nodeLocXAddr = thrust::raw_pointer_cast(
 			&(nodes->getInfoVecs().nodeLocX[0]));
 	double* nodeLocYAddr = thrust::raw_pointer_cast(
 			&(nodes->getInfoVecs().nodeLocY[0]));
 	bool* nodeIsActiveAddr = thrust::raw_pointer_cast(
 			&(nodes->getInfoVecs().nodeIsActive[0]));
 
 	thrust::reduce_by_key(
 			make_transform_iterator(iBegin, DivideFunctor(maxAllNodePerCell)),
 			make_transform_iterator(iBegin, DivideFunctor(maxAllNodePerCell))
 					+ totalNodeCountForActiveCells,
 			thrust::make_transform_iterator(
 					thrust::make_zip_iterator(
 							thrust::make_tuple(
 									thrust::make_permutation_iterator(
 											cellInfoVecs.activeMembrNodeCounts.begin(),
 											make_transform_iterator(iBegin,
 													DivideFunctor(
 															maxAllNodePerCell))),
 									make_transform_iterator(iBegin,
 											DivideFunctor(maxAllNodePerCell)),
 									make_transform_iterator(iBegin,
 											ModuloFunctor(maxAllNodePerCell)),
 									make_permutation_iterator(
 											cellInfoVecs.centerCoordX.begin(),
 											make_transform_iterator(iBegin,
 													DivideFunctor(
 															maxAllNodePerCell))),
 									make_permutation_iterator(
 											cellInfoVecs.centerCoordY.begin(),
 											make_transform_iterator(iBegin,
 													DivideFunctor(
 															maxAllNodePerCell))))),
 					CalPerim(maxAllNodePerCell, nodeIsActiveAddr,
 							nodeLocXAddr, nodeLocYAddr)),
 			cellInfoVecs.cellRanksTmpStorage.begin(),
 			cellInfoVecs.cellPerimVec.begin(), thrust::equal_to<uint>(),
 			thrust::plus<double>());
 }
 










CellsStatsData SceCells::outputPolyCountData() {
       
        cout << " I am at begining of outpolycount"<< std::flush  ; 
	std::cout.flush();
       double sumX,sumY,cntr_X_Domain,cntr_Y_Domain ; 
       int BdryApproach ; 
       BdryApproach=1 ; 
	totalNodeCountForActiveCells = allocPara_m.currentActiveCellCount
			* allocPara_m.maxAllNodePerCell;
        cout << " I am before cells area"<< endl ; 
	calCellArea();
        cout << " I am after cells area" << endl ;

        calCellPerim();//AAMIRI
 
	CellsStatsData result;

        cout << " I am after result" << endl ; 
	uint bdryCriteria =
			globalConfigVars.getConfigValue("BdryCellCriteria").toInt();
	// already on host; no need to call thrust::copy
	thrust::host_vector<int> adhIndxHost =
			nodes->getInfoVecs().nodeAdhIndxHostCopy;
	thrust::host_vector<double> myosinLevelHost =
			nodes->getInfoVecs().myosinLevelHostCopy;
	double totalMyosinPerCell = 0;

	thrust::host_vector<double> growthProVecHost(
			allocPara_m.currentActiveCellCount);
	thrust::copy(cellInfoVecs.growthProgress.begin(),
			cellInfoVecs.growthProgress.begin()
					+ allocPara_m.currentActiveCellCount,
			growthProVecHost.begin());
	thrust::host_vector<double> growthProMembrVecHost(
			allocPara_m.currentActiveCellCount);
	thrust::copy(cellInfoVecs.membrGrowProgress.begin(),
			cellInfoVecs.membrGrowProgress.begin()
					+ allocPara_m.currentActiveCellCount,
			growthProMembrVecHost.begin());
	thrust::host_vector<uint> activeMembrNodeCountHost(
			allocPara_m.currentActiveCellCount);
	thrust::copy(cellInfoVecs.activeMembrNodeCounts.begin(),
			cellInfoVecs.activeMembrNodeCounts.begin()
					+ allocPara_m.currentActiveCellCount,
			activeMembrNodeCountHost.begin());
	thrust::host_vector<uint> activeIntnlNodeCountHost(
			allocPara_m.currentActiveCellCount);
	thrust::copy(cellInfoVecs.activeIntnlNodeCounts.begin(),
			cellInfoVecs.activeIntnlNodeCounts.begin()
					+ allocPara_m.currentActiveCellCount,
			activeIntnlNodeCountHost.begin());
	thrust::host_vector<double> centerCoordXHost(
			allocPara_m.currentActiveCellCount);
	thrust::host_vector<double> centerCoordYHost(
			allocPara_m.currentActiveCellCount);
	thrust::copy(cellInfoVecs.centerCoordX.begin(),
			cellInfoVecs.centerCoordX.begin()
					+ allocPara_m.currentActiveCellCount,
			centerCoordXHost.begin());
	thrust::copy(cellInfoVecs.centerCoordY.begin(),
			cellInfoVecs.centerCoordY.begin()
					+ allocPara_m.currentActiveCellCount,
			centerCoordYHost.begin());

	thrust::host_vector<double> cellAreaHost(
			allocPara_m.currentActiveCellCount);

    thrust::host_vector<double> cellPerimHost(
 			allocPara_m.currentActiveCellCount);//AAMIRI

	thrust::host_vector<double> cellDppHost(
 			allocPara_m.currentActiveCellCount);//Ali

	thrust::host_vector<int> cell_TypeHost(
                        allocPara_m.currentActiveCellCount);//Alireza

	thrust::copy(cellInfoVecs.cellAreaVec.begin(),
			cellInfoVecs.cellAreaVec.begin()
					+ allocPara_m.currentActiveCellCount, cellAreaHost.begin());

        thrust::copy(cellInfoVecs.cellPerimVec.begin(),
 			cellInfoVecs.cellPerimVec.begin()
					+ allocPara_m.currentActiveCellCount, cellPerimHost.begin());//AAMIRI

		thrust::copy(cellInfoVecs.cell_Dpp.begin(),
 			cellInfoVecs.cell_Dpp.begin()
					+ allocPara_m.currentActiveCellCount, cellDppHost.begin());//Ali

		thrust::copy(cellInfoVecs.cell_Type.begin(),
                        cellInfoVecs.cell_Type.begin()
                                        + allocPara_m.currentActiveCellCount, cell_TypeHost.begin());//Alireza
        sumX=0 ; 
        sumY=0 ; 
	for (uint i = 0; i < allocPara_m.currentActiveCellCount; i++) {
		CellStats cellStatsData;
		cellStatsData.cellGrowthProgress = growthProVecHost[i];
		cellStatsData.cellRank = i;
		bool isBdry = false;
		std::set<int> neighbors;
		std::vector<int> neighborsV; //Ali
                int neighborStrength[10]; //Ali
		int continousNoAdh = 0;
                map <int, int> cellAndNeighborRank ;  //Ali
		//std::cout << "printing adhesion indicies ";
                //for (int ii=0 ; ii<neighborStrength.size() ; ii++){
                for (int ii=0 ; ii< 10; ii++){ //Ali
                      
                  neighborStrength[ii]=0  ;
                }
                          
                cellAndNeighborRank.clear();  //Ali

		for (uint j = 0; j < activeMembrNodeCountHost[i]; j++) {
			uint index = i * allocPara_m.maxAllNodePerCell + j;
			//std::cout << adhIndxHost[index] << ",";
                        
			if (adhIndxHost[index] != -1) {
				uint adhCellRank = adhIndxHost[index]
						/ allocPara_m.maxAllNodePerCell;
				//std::cout << adhCellRank << " ";
				neighbors.insert(adhCellRank);
                                 map <int, int>:: iterator iteratorMap=cellAndNeighborRank.find(adhCellRank); //Ali
                                 if (iteratorMap==cellAndNeighborRank.end()) {  //Ali
                                   int NewneighborRank= neighbors.size()-1; //Ali
                                   cellAndNeighborRank[adhCellRank]=NewneighborRank; //Ali
                                   neighborStrength[NewneighborRank]=neighborStrength[NewneighborRank]+1 ; //Ali
				   neighborsV.push_back(adhCellRank); //Ali
                                   }
                                 else {   //Ali
                                   int oldNeighborRank=(*iteratorMap).second ; 
                                   neighborStrength[oldNeighborRank]=neighborStrength[oldNeighborRank]+1 ; //Ali
                                 }      
				continousNoAdh = 0;
			} else {
				continousNoAdh = continousNoAdh + 1;
				if (continousNoAdh > bdryCriteria) {
					isBdry = true;
				}
			}
			if (j == activeMembrNodeCountHost[i] - 1
					&& adhIndxHost[index] == -1) {
				int k = 0;
				uint indexNew;
				while (k < activeMembrNodeCountHost[i] - 1) {
					indexNew = i * allocPara_m.maxAllNodePerCell + k;
					if (adhIndxHost[indexNew] == -1) {
						continousNoAdh = continousNoAdh + 1;
						if (continousNoAdh > bdryCriteria) {
							isBdry = true;
						}
						k++;
					} else {
						break;
					}
				}
			}

		}
        
		// 090324
		uint indexNew;
		totalMyosinPerCell = 0;
		for (uint j=allocPara_m.maxMembrNodePerCell; j<allocPara_m.maxAllNodePerCell;j++){
			indexNew = i*allocPara_m.maxAllNodePerCell + j;
			totalMyosinPerCell += myosinLevelHost[indexNew];
		}
                
		cellStatsData.isBdryCell = isBdry;
		cellStatsData.numNeighbors = neighbors.size();
		cellStatsData.currentActiveMembrNodes = activeMembrNodeCountHost[i];
		cellStatsData.currentActiveIntnlNodes = activeIntnlNodeCountHost[i];
		cellStatsData.neighborVec = neighbors;
		cellStatsData.neighborVecV = neighborsV; //Ali
        for (int iiii=0; iiii<10 ; iiii++){
				cellStatsData.cellNeighborStrength[iiii] = neighborStrength[iiii];
                 }    //Ali
		cellStatsData.membrGrowthProgress = growthProMembrVecHost[i];
		cellStatsData.cellCenter = CVector(centerCoordXHost[i],
				centerCoordYHost[i], 0);
		cellStatsData.cellArea = cellAreaHost[i];
        cellStatsData.cellPerim = cellPerimHost[i];//AAMIRI
        cellStatsData.cellDpp = cellDppHost[i];//Ali
		cellStatsData.cell_Type = cell_TypeHost[i];
		cellStatsData.myosinLevel = totalMyosinPerCell;
		result.cellsStats.push_back(cellStatsData);
        sumX=sumX+cellStatsData.cellCenter.x ; 
        sumY=sumY+cellStatsData.cellCenter.y ;
                
	}
//Ali
        if (BdryApproach==2) {  
          cout << "sumX=" << sumX << endl ; 
          cout << "sumY=" << sumY << endl ; 
          cntr_X_Domain=sumX/result.cellsStats.size() ; 
          cntr_Y_Domain=sumY/result.cellsStats.size() ;  
          cout << "cntr_X=" << cntr_X_Domain << endl ; 
          cout << "cntr_Y=" << cntr_Y_Domain << endl ;

          double R_Max ;
          double Distance ;
          R_Max=0 ;  
	  for (uint i = 0; i < allocPara_m.currentActiveCellCount; i++) {
            Distance=sqrt( pow(centerCoordXHost[i]-cntr_X_Domain,2) +pow(centerCoordYHost[i]-cntr_Y_Domain,2) ) ; 
            if (Distance > R_Max) {
              R_Max=Distance ; 
            }
          }
        
          cout << "R_Max=" << R_Max << endl ;

	  for (uint i = 0; i < allocPara_m.currentActiveCellCount; i++) {
            Distance=sqrt( pow(centerCoordXHost[i]-cntr_X_Domain,2) +pow(centerCoordYHost[i]-cntr_Y_Domain,2) ) ; 
            if (Distance > 0.9* R_Max) {
	      result.cellsStats[i].isBdryCell = true;
              cout << "isBdryCell"<< i<< endl ; 
            }
            else {
	      result.cellsStats[i].isBdryCell = false;
              cout << "isNormalCell"<< i << endl ; 
            }
          }
        }
        //Ali
        cout << "I want to write data" << endl ;  
       // ofstream  Stress_Strain_Single ; 
        //Stress_Strain_Single.open("Stress_Strain_Single.txt"); 
        //Stress_Strain_Single.close() ;
       //Ali
        result.MaxDistanceX=abs(centerCoordXHost[1]-centerCoordXHost[0]); //Ali
        result.Cells_Extrem_Loc[0]=Tisu_MinX; 
        result.Cells_Extrem_Loc[1]=Tisu_MaxX; 
        result.Cells_Extrem_Loc[2]=Tisu_MinY;
        result.Cells_Extrem_Loc[3]=Tisu_MaxY ;
        result.F_Ext_Out=membrPara.F_Ext_Incline*curTime ; 
        //if (dt==curTime) { 
        //result.Init_Displace=MaxX-MinX ; 
       // }
       //Ali
	return result;
}

__device__ bool bigEnough(double& num) {
	if (num > minDivisor) {
		return true;
	} else {
		return false;
	}
}

__device__ double cross_Z(double vecA_X, double vecA_Y, double vecB_X,
		double vecB_Y) {
	return vecA_X * vecB_Y - vecA_Y * vecB_X;
}
/*
__device__ double calBendMulti(double& angle, uint activeMembrCt) {
	double equAngle = PI - PI / activeMembrCt;
	return bendCoeff * (angle - equAngle);
}
*/

//AAMIRI
__device__ double calBendMulti_Mitotic(double& angle, uint activeMembrCt, double& progress, double mitoticCri, int cell_Type) {

	// double equAngle = PI - PI / activeMembrCt; // PI - 2*PI/activeMembrCt?
	double equAngle = PI;
	double newbendCoeff;
	if (cell_Type == 1) {newbendCoeff = bendCoeffLeader;}
	else {newbendCoeff = bendCoeff;}
	if (progress <= mitoticCri){
		return newbendCoeff * (angle - equAngle);}
	else{
		return (angle - equAngle)*(newbendCoeff + (bendCoeff_Mitotic - newbendCoeff) * (progress - mitoticCri)/(1.0 - mitoticCri));
	}
}



/*
void SceCells::applyMembrAdhCell_M() {
    thrust::counting_iterator<uint> iBegin(0);
    totalNodeCountForActiveCells = allocPara_m.currentActiveCellCount
			* allocPara_m.maxAllNodePerCell;
	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;
    double* nodeLocXAddress = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocX[0]));
    double* nodeLocYAddress = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocY[0]));
    double* nodeGrowProAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeGrowPro[0]));

    thrust::transform(
            thrust::make_zip_iterator(
                    thrust::make_tuple(nodes->getInfoVecs().nodeIsActive.begin(),iBegin,
                            thrust::make_permutation_iterator(
                                    cellInfoVecs.activationLevel.begin(),
                                    make_transform_iterator(iBegin,
                                            DivideFunctor(maxAllNodePerCell))),
                            nodes->getInfoVecs().nodeAdhereIndex.begin(), 
                            nodes->getInfoVecs().nodeVelX.begin(),
                            nodes->getInfoVecs().nodeVelY.begin())),
            thrust::make_zip_iterator(
                    thrust::make_tuple(nodes->getInfoVecs().nodeIsActive.begin(),iBegin,
                            thrust::make_permutation_iterator(
                                    cellInfoVecs.activationLevel.begin(),
                                    make_transform_iterator(iBegin,
                                            DivideFunctor(maxAllNodePerCell))),
                            nodes->getInfoVecs().nodeAdhereIndex.begin(), 
                            nodes->getInfoVecs().nodeVelX.begin(),
                            nodes->getInfoVecs().nodeVelY.begin())) + totalNodeCountForActiveCells,
            thrust::make_zip_iterator(
                    thrust::make_tuple(nodes->getInfoVecs().nodeVelX.begin(),
                            nodes->getInfoVecs().nodeVelY.begin())),
            ApplyAdhCell(nodeLocXAddress, nodeLocYAddress, nodeGrowProAddr));
}
*/


/*
void SceCells::copyExtForcesCell_M(){

    thrust::copy(nodes->getInfoVecs().nodeVelX.begin(), nodes->getInfoVecs().nodeVelX.end(),
            nodes->getInfoVecs().nodeExtForceX.begin());

    thrust::copy(nodes->getInfoVecs().nodeVelY.begin(), nodes->getInfoVecs().nodeVelY.end(),
            nodes->getInfoVecs().nodeExtForceY.begin());

}
*/


void SceCells::applySceCellDisc_M() {
	totalNodeCountForActiveCells = allocPara_m.currentActiveCellCount
			* allocPara_m.maxAllNodePerCell;
	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;
	uint maxMemNodePerCell = allocPara_m.maxMembrNodePerCell;
	thrust::counting_iterator<uint> iBegin(0);

	double* nodeLocXAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocX[0]));
	double* nodeLocYAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocY[0]));
	bool* nodeIsActiveAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeIsActive[0]));

	double grthPrgrCriVal_M =growthAuxData.grthPrgrCriVal_M_Ori ; // for now constant  growthAuxData.grthProgrEndCPU
	//		- growthAuxData.prolifDecay
	//				* (growthAuxData.grthProgrEndCPU
	//						- growthAuxData.grthPrgrCriVal_M_Ori);

	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.activeMembrNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.activeIntnlNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							make_transform_iterator(iBegin,
									DivideFunctor(maxAllNodePerCell)),
							make_transform_iterator(iBegin,
									ModuloFunctor(maxAllNodePerCell)),
							thrust::make_permutation_iterator(
									cellInfoVecs.growthProgress.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin())),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.activeMembrNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.activeIntnlNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							make_transform_iterator(iBegin,
									DivideFunctor(maxAllNodePerCell)),
							make_transform_iterator(iBegin,
									ModuloFunctor(maxAllNodePerCell)),
							thrust::make_permutation_iterator(
									cellInfoVecs.growthProgress.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin()))
					+ totalNodeCountForActiveCells,
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeVelX.begin(),
							   nodes->getInfoVecs().nodeVelY.begin(),
							   nodes->getInfoVecs().nodeF_MI_M_x.begin(),  //Ali added for cell pressure calculation 
							   nodes->getInfoVecs().nodeF_MI_M_y.begin())),// ALi added for cell pressure calculation
			applySceCellDisc_MDevice(maxAllNodePerCell, maxMemNodePerCell, nodeLocXAddr,
					nodeLocYAddr, nodeIsActiveAddr, grthPrgrCriVal_M));
}




// apply the myosin force to membrane nodes? 
void SceCells::applySceCellMyosin() {
	totalNodeCountForActiveCells = allocPara_m.currentActiveCellCount
			* allocPara_m.maxAllNodePerCell;
	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;
	uint maxMemNodePerCell = allocPara_m.maxMembrNodePerCell;
	thrust::counting_iterator<uint> iBegin(0);

	double* nodeLocXAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocX[0]));
	double* nodeLocYAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocY[0]));
	bool* nodeIsActiveAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeIsActive[0]));
	double* myosinLevelAddr = thrust::raw_pointer_cast(
		&(nodes->getInfoVecs().myosinLevel[0])); // pointer to the vector storing myosin level

	//		- growthAuxData.prolifDecay
	//				* (growthAuxData.grthProgrEndCPU
	//						- growthAuxData.grthPrgrCriVal_M_Ori);

	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.activeMembrNodeCounts.begin(), // comments: the two first arguments are used to find the active nodes for a cell calculated by divide function
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.activeIntnlNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							make_transform_iterator(iBegin,
									DivideFunctor(maxAllNodePerCell)),
							make_transform_iterator(iBegin,
									ModuloFunctor(maxAllNodePerCell)),
							nodes->getInfoVecs().myosinLevel.begin(),
							nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin(),
							thrust::make_permutation_iterator(
									cellInfoVecs.centerCoordX.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.centerCoordY.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))))),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.activeMembrNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.activeIntnlNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							make_transform_iterator(iBegin,
									DivideFunctor(maxAllNodePerCell)),
							make_transform_iterator(iBegin,
									ModuloFunctor(maxAllNodePerCell)),
							nodes->getInfoVecs().myosinLevel.begin(),
							nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin(),
							thrust::make_permutation_iterator(
									cellInfoVecs.centerCoordX.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.centerCoordY.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell)))))
					+ totalNodeCountForActiveCells,
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeVelX.begin(),
							   nodes->getInfoVecs().nodeVelY.begin()
								)),
			addSceCellMyosinForce(maxAllNodePerCell, maxMemNodePerCell, nodeLocXAddr,
					nodeLocYAddr, nodeIsActiveAddr, myosinLevelAddr));
}









// 
void SceCells::applySigForce(std::vector<SigptStateV2>& sigPtVecV2) {
	totalNodeCountForActiveCells = allocPara_m.currentActiveCellCount
			* allocPara_m.maxAllNodePerCell;
	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;
	uint maxMemNodePerCell = allocPara_m.maxMembrNodePerCell;
	thrust::counting_iterator<uint> iBegin(0);

	double* nodeLocXAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocX[0]));
	double* nodeLocYAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocY[0]));
	bool* nodeIsActiveAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeIsActive[0]));
	double* myosinLevelAddr = thrust::raw_pointer_cast(
		&(nodes->getInfoVecs().myosinLevel[0])); // pointer to the vector storing myosin level

	double timeNow = curTime;

	thrust::device_vector<SigptStateV2> d_sigPtVec(sigPtVecV2);
	SigptStateV2* sigPtAddr = thrust::raw_pointer_cast(&d_sigPtVec[0]);

	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.activeMembrNodeCounts.begin(), // comments: the two first arguments are used to find the active nodes for a cell calculated by divide function
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.activeIntnlNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							make_transform_iterator(iBegin,
									DivideFunctor(maxAllNodePerCell)),
							make_transform_iterator(iBegin,
									ModuloFunctor(maxAllNodePerCell)),
							nodes->getInfoVecs().myosinLevel.begin(),
							nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin(),
							thrust::make_permutation_iterator(
									cellInfoVecs.centerCoordX.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.centerCoordY.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
                                    cellInfoVecs.cell_Type.begin(),
                                    make_transform_iterator(iBegin,
                                            DivideFunctor(maxAllNodePerCell)))
											)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.activeMembrNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.activeIntnlNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							make_transform_iterator(iBegin,
									DivideFunctor(maxAllNodePerCell)),
							make_transform_iterator(iBegin,
									ModuloFunctor(maxAllNodePerCell)),
							nodes->getInfoVecs().myosinLevel.begin(),
							nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin(),
							thrust::make_permutation_iterator(
									cellInfoVecs.centerCoordX.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.centerCoordY.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
                                    cellInfoVecs.cell_Type.begin(),
                                    make_transform_iterator(iBegin,
                                            DivideFunctor(maxAllNodePerCell)))
											))
					+ totalNodeCountForActiveCells,
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeVelX.begin(),
							   nodes->getInfoVecs().nodeVelY.begin()
								)),
			addSceCellSigForce(maxAllNodePerCell, maxMemNodePerCell, nodeLocXAddr,
					nodeLocYAddr, nodeIsActiveAddr, sigPtAddr, timeNow));
}




// define a host vector, size maxIntNode by maxIntNode 
void SceCells::calFluxWeightsMyosin() { // std::vector<double>& fluxWeightsVec
	cout << "Start computing flux weights" << endl;
	totalNodeCountForActiveCells = allocPara_m.currentActiveCellCount
			* allocPara_m.maxAllNodePerCell;
	uint totalActCellCount = allocPara_m.currentActiveCellCount;
	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;
	uint maxMemNodePerCell = allocPara_m.maxMembrNodePerCell;
	uint maxIntnlNodePerCell = allocPara_m.maxIntnlNodePerCell;

	thrust::counting_iterator<uint> iBegin(0);

	double* nodeLocXAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocX[0]));
	double* nodeLocYAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocY[0]));
	bool* nodeIsActiveAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeIsActive[0])); // 

    int* nodeAdhIdxAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeAdhereIndex[0]));

	double* myosinLevelAddr = thrust::raw_pointer_cast(
		&(nodes->getInfoVecs().myosinLevel[0]));
	double* minToMDistAddr = thrust::raw_pointer_cast(
		&(nodes->getInfoVecs().minToMDist[0]));

	// thrust::device_vector<double> d_fluxWeightsVec(fluxWeightsVec);
    // double* fluxWeightsAddr = thrust::raw_pointer_cast(&d_fluxWeightsVec[0]);
	 double* fluxWeightsAddr = thrust::raw_pointer_cast(
		&(nodes->getInfoVecs().fluxWeights[0]));

	// compute minimal distance from internal nodes of a leader cell to membrane nodes 
	
	thrust::transform(
		thrust::make_zip_iterator(
				thrust::make_tuple(
						thrust::make_permutation_iterator(
								cellInfoVecs.activeMembrNodeCounts.begin(),
								make_transform_iterator(iBegin,
										DivideFunctor(maxAllNodePerCell))),
						thrust::make_permutation_iterator(
								cellInfoVecs.activeIntnlNodeCounts.begin(),
								make_transform_iterator(iBegin,
										DivideFunctor(maxAllNodePerCell))),
						make_transform_iterator(iBegin,
								DivideFunctor(maxAllNodePerCell)),
						make_transform_iterator(iBegin,
								ModuloFunctor(maxAllNodePerCell)),
						thrust::make_permutation_iterator(
								cellInfoVecs.cell_Type.begin(),
								make_transform_iterator(iBegin,
										DivideFunctor(maxAllNodePerCell))),
						thrust::make_permutation_iterator(
									cellInfoVecs.centerCoordX.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
						thrust::make_permutation_iterator(
									cellInfoVecs.centerCoordY.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
						nodes->getInfoVecs().minToMDist.begin()
						)),
		thrust::make_zip_iterator(
				thrust::make_tuple(
						thrust::make_permutation_iterator(
								cellInfoVecs.activeMembrNodeCounts.begin(),
								make_transform_iterator(iBegin,
										DivideFunctor(maxAllNodePerCell))),
						thrust::make_permutation_iterator(
								cellInfoVecs.activeIntnlNodeCounts.begin(),
								make_transform_iterator(iBegin,
										DivideFunctor(maxAllNodePerCell))),
						make_transform_iterator(iBegin,
								DivideFunctor(maxAllNodePerCell)),
						make_transform_iterator(iBegin,
								ModuloFunctor(maxAllNodePerCell)),
						thrust::make_permutation_iterator(
								cellInfoVecs.cell_Type.begin(),
								make_transform_iterator(iBegin,
										DivideFunctor(maxAllNodePerCell))),
						thrust::make_permutation_iterator(
									cellInfoVecs.centerCoordX.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
						thrust::make_permutation_iterator(
									cellInfoVecs.centerCoordY.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
						nodes->getInfoVecs().minToMDist.begin()
						))
				+ totalNodeCountForActiveCells,
				thrust::make_zip_iterator(
					thrust::make_tuple(
						nodes->getInfoVecs().minToMDist.begin(), 
						nodes->getInfoVecs().cenToAdhMDist.begin())),
		updateMinToMDist(maxAllNodePerCell, maxMemNodePerCell, nodeLocXAddr,
				nodeLocYAddr, nodeIsActiveAddr, nodeAdhIdxAddr));
	

	
	// compute the flux weight matrix for all internal nodes in a leader cell
	thrust::counting_iterator<uint> iBegin1(0);
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.activeMembrNodeCounts.begin(),
									make_transform_iterator(iBegin1,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.activeIntnlNodeCounts.begin(),
									make_transform_iterator(iBegin1,
											DivideFunctor(maxAllNodePerCell))),
							make_transform_iterator(iBegin1,
									DivideFunctor(maxAllNodePerCell)),
							make_transform_iterator(iBegin1,
									ModuloFunctor(maxAllNodePerCell)),
							thrust::make_permutation_iterator(
									cellInfoVecs.cell_Type.begin(),
									make_transform_iterator(iBegin1,
											DivideFunctor(maxAllNodePerCell))),
							nodes->getInfoVecs().minToMDist.begin(),
							nodes->getInfoVecs().cenToAdhMDist.begin()
							)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.activeMembrNodeCounts.begin(),
									make_transform_iterator(iBegin1,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.activeIntnlNodeCounts.begin(),
									make_transform_iterator(iBegin1,
											DivideFunctor(maxAllNodePerCell))),
							make_transform_iterator(iBegin1,
									DivideFunctor(maxAllNodePerCell)),
							make_transform_iterator(iBegin1,
									ModuloFunctor(maxAllNodePerCell)),
							thrust::make_permutation_iterator(
									cellInfoVecs.cell_Type.begin(),
									make_transform_iterator(iBegin1,
											DivideFunctor(maxAllNodePerCell))),
							nodes->getInfoVecs().minToMDist.begin(),
							nodes->getInfoVecs().cenToAdhMDist.begin()
							))
					+ totalNodeCountForActiveCells,
			nodes->getInfoVecs().minToMDist.begin(), 
			calFluxWeightsMyosinDevice(maxAllNodePerCell, maxMemNodePerCell, maxIntnlNodePerCell, nodeLocXAddr,
					nodeLocYAddr, nodeIsActiveAddr, myosinLevelAddr, nodeAdhIdxAddr, minToMDistAddr, fluxWeightsAddr));
					
		//	thrust::copy(d_fluxWeightsVec.begin(), d_fluxWeightsVec.end(), fluxWeightsVec.begin());
}


// add actin force on each membrane node, maganitude depending on neighboring myosin concentration
// apply to all internal nodes
void SceCells::calSceCellMyosin() {
	totalNodeCountForActiveCells = allocPara_m.currentActiveCellCount
			* allocPara_m.maxAllNodePerCell;
	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;
	uint maxMemNodePerCell = allocPara_m.maxMembrNodePerCell;
	uint maxIntnlNodePerCell = allocPara_m.maxIntnlNodePerCell;
	thrust::counting_iterator<uint> iBegin(0);

	double* nodeLocXAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocX[0]));
	double* nodeLocYAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocY[0]));
	bool* nodeIsActiveAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeIsActive[0])); // 

	double* myosinLevelAddr = thrust::raw_pointer_cast(
		&(nodes->getInfoVecs().myosinLevel[0])); // pointer to the vector storing myosin level

    int* nodeAdhIdxAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeAdhereIndex[0]));

	double* fluxWeightsAddr = thrust::raw_pointer_cast(
		&(nodes->getInfoVecs().fluxWeights[0]));
	// double grthPrgrCriVal_M =growthAuxData.grthPrgrCriVal_M_Ori ; // for now constant  growthAuxData.grthProgrEndCPU
	//		- growthAuxData.prolifDecay
	//				* (growthAuxData.grthProgrEndCPU
	//						- growthAuxData.grthPrgrCriVal_M_Ori);
	double timeStep = dt;
	double myosinDiffusionThreshold = 100.0;
	double timeNow = curTime;

	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.activeMembrNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.activeIntnlNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
                                    cellInfoVecs.centerCoordX.begin(),
                                    make_transform_iterator(iBegin,
                                            DivideFunctor(maxAllNodePerCell))),
                            thrust::make_permutation_iterator(
                                    cellInfoVecs.centerCoordY.begin(),
                                    make_transform_iterator(iBegin,
                                            DivideFunctor(maxAllNodePerCell))),
							make_transform_iterator(iBegin,
									DivideFunctor(maxAllNodePerCell)),
							make_transform_iterator(iBegin,
									ModuloFunctor(maxAllNodePerCell)),
							nodes->getInfoVecs().myosinLevel.begin(),
							thrust::make_permutation_iterator(
									cellInfoVecs.cellPolarAngle.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.cellRadius.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.cell_Type.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell)))
							)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.activeMembrNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.activeIntnlNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
						    thrust::make_permutation_iterator(
                                    cellInfoVecs.centerCoordX.begin(),
                                    make_transform_iterator(iBegin,
                                            DivideFunctor(maxAllNodePerCell))),
                            thrust::make_permutation_iterator(
                                    cellInfoVecs.centerCoordY.begin(),
                                    make_transform_iterator(iBegin,
                                            DivideFunctor(maxAllNodePerCell))),
							make_transform_iterator(iBegin,
									DivideFunctor(maxAllNodePerCell)),
							make_transform_iterator(iBegin,
									ModuloFunctor(maxAllNodePerCell)),
							nodes->getInfoVecs().myosinLevel.begin(),
							thrust::make_permutation_iterator(
									cellInfoVecs.cellPolarAngle.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.cellRadius.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.cell_Type.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell)))
							))
					+ totalNodeCountForActiveCells,
			nodes->getInfoVecs().tempMyosinLevel.begin(), 
			calSceCellMyosinDevice(maxAllNodePerCell, maxMemNodePerCell, maxIntnlNodePerCell, nodeLocXAddr,
					nodeLocYAddr, nodeIsActiveAddr, myosinLevelAddr, myosinDiffusionThreshold, nodeAdhIdxAddr, fluxWeightsAddr, timeStep, timeNow));
			thrust::copy(nodes->getInfoVecs().tempMyosinLevel.begin(),nodes->getInfoVecs().tempMyosinLevel.end(),nodes->getInfoVecs().myosinLevel.begin());
			// nodes->getInfoVecs().myosinLevel = nodes->getInfoVecs().tempMyosinLevel;

}







void SceCells::updateActivationLevel() {
	uint activeCellCount = allocPara_m.currentActiveCellCount;
	uint maxMemNodePerCell = allocPara_m.maxMembrNodePerCell;
	uint maxNodePerCell = allocPara_m.maxAllNodePerCell;
    double* nodeLocXAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeLocX[0]));
    double* nodeLocYAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeLocY[0]));
    bool* nodeIsActiveAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeIsActive[0])); // 
 	int* nodeAdhIdxAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeAdhereIndex[0]));
 	uint* actLevelAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.activationLevel[0]));

	int* cellTypeAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.cell_Type[0]));
	thrust::counting_iterator<uint> iBegin(0);
	thrust::counting_iterator<uint> iEnd(activeCellCount); // make sure not iterate on inactive cells already 
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(iBegin,
							cellInfoVecs.activeMembrNodeCounts.begin(),
							cellInfoVecs.activationLevel.begin(),
							cellInfoVecs.cell_Type.begin()
							)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							iEnd,
							cellInfoVecs.activeMembrNodeCounts.begin() + activeCellCount,
							cellInfoVecs.activationLevel.begin() + activeCellCount,
							cellInfoVecs.cell_Type.begin() + activeCellCount
							)),
			cellInfoVecs.activationLevel.begin(),
			updateActivationLevelDevice(activeCellCount,maxMemNodePerCell,maxNodePerCell,nodeLocXAddr,nodeLocYAddr,nodeIsActiveAddr,nodeAdhIdxAddr,actLevelAddr,cellTypeAddr));
}



void SceCells::updateCellPolar() {
	random_device rd;
 	// uint seed = time(NULL);
 	// uint temp_seed = rd();
	// mt19937_64 host_rng(temp_seed);
 	// uint seed = time(NULL);
 	// uint64_t seed = host_rng();
	// uint64_t t = static_cast<uint64_t>(time(nullptr));
	// uint64_t r = static_cast<uint64_t>(rd());
	// uint64_t mixed = (r << 32) ^ t;   // mix high‑entropy rd() into high bits, time() into low bits
	uint seed = rd();
	cout << "Rd is " << seed << endl;
	// cout << "Rd is " << temp_seed << " and Host rng is " << seed << endl;
	// unsigned int seed = static_cast<unsigned int>(mixed);
	uint activeCellCount = allocPara_m.currentActiveCellCount;

	// double* myosinLevelAddr = thrust::raw_pointer_cast(
	//	&(nodes->getInfoVecs().myosinLevel[0])); // pointer to the vector storing myosin level
	
	double* cellFilopXAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.cellFilopX[0]));
    double* cellFilopYAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.cellFilopY[0]));
    double* cellFilopAngleAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.cellFilopAngle[0]));
    bool* cellFilopIsActiveAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.cellFilopIsActive[0]));
    double* cellFilopBirthTimeAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.cellFilopBirthTime[0]));
	double* cellCenterXAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.centerCoordX[0]));
	double* cellCenterYAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.centerCoordY[0]));
	double* cellRadiusAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.cellRadius[0]));
	double* cellPolarAngleAddr = thrust::raw_pointer_cast(
			&(cellInfoVecs.cellPolarAngle[0]));
 	int* nodeAdhIdxAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeAdhereIndex[0]));
 	uint* nodeActLevelAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeActLevel[0]));
	double timeNow = curTime;
	// double initTimePeriod = InitTimeStage;
	double ddt = dt;
	uint* cellActiveFilopCountsAddr = thrust::raw_pointer_cast(
        &(cellInfoVecs.activeCellFilopCounts[0]));

	uint maxMemNodePerCell = allocPara_m.maxMembrNodePerCell;
	uint maxNodePerCell = allocPara_m.maxAllNodePerCell;
    double* nodeLocXAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeLocX[0]));
    double* nodeLocYAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeLocY[0]));
    bool* nodeIsActiveAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeIsActive[0])); // 
	double* myosinLevelAddr = thrust::raw_pointer_cast(
		&(nodes->getInfoVecs().myosinLevel[0]));
	int* cellTypeAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.cell_Type[0]));
	uint leaderRank = allocPara_m.leaderRank; // 
	// uint leaderRank = nodes->getLeaderRank();
	bool leaderExist = allocPara_m.leaderExist;
	uint* cellActLevelAddr = thrust::raw_pointer_cast(
        &(cellInfoVecs.activationLevel[0]));

	thrust::counting_iterator<uint> iBegin(0);
	thrust::counting_iterator<uint> iEnd(activeCellCount); // make sure not iterate on inactive cells already 
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(iBegin,
							cellInfoVecs.activationLevel.begin(),
							cellInfoVecs.activeMembrNodeCounts.begin(),
							cellInfoVecs.cell_Type.begin(),
							cellInfoVecs.filopDirection.begin(),
							cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin(),
							cellInfoVecs.cellRadius.begin(),
							cellInfoVecs.cellPolarAngle.begin()
							)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							iEnd,
							cellInfoVecs.activationLevel.begin() + activeCellCount,
							cellInfoVecs.activeMembrNodeCounts.begin() + activeCellCount,
							cellInfoVecs.cell_Type.begin() + activeCellCount,
							cellInfoVecs.filopDirection.begin() + activeCellCount,
							cellInfoVecs.centerCoordX.begin() + activeCellCount,
							cellInfoVecs.centerCoordY.begin() + activeCellCount,
							cellInfoVecs.cellRadius.begin() + activeCellCount,
							cellInfoVecs.cellPolarAngle.begin() + activeCellCount
							)),
			cellInfoVecs.cellPolarAngle.begin(),
			updateCellPolarDevice(seed, ddt, timeNow, 
			cellFilopXAddr,cellFilopYAddr,cellFilopAngleAddr,cellFilopIsActiveAddr,
			cellFilopBirthTimeAddr,activeCellCount,cellCenterXAddr,cellCenterYAddr,cellRadiusAddr,
			cellActiveFilopCountsAddr,maxMemNodePerCell,maxNodePerCell,nodeLocXAddr,nodeLocYAddr,
			nodeIsActiveAddr,nodeAdhIdxAddr,nodeActLevelAddr,myosinLevelAddr,cellTypeAddr,leaderRank,leaderExist,cellPolarAngleAddr,cellActLevelAddr));
	/*
	for (uint i=0;i<activeCellCount*5;i++){
		double tempAngle = cellInfoVecs.cellFilopAngle[i];
		if (tempAngle !=0 ){
        cout << "Filop " << i << " has angle " << tempAngle << endl;
		}
	}
	*/
}





void SceCells::updateCellPolarLeader() {
	random_device rd;
 	uint seed = rd();
	cout << "Start computing leader cell polar" << endl;
	uint activeCellCount = allocPara_m.currentActiveCellCount;

	// double* myosinLevelAddr = thrust::raw_pointer_cast(
	//	&(nodes->getInfoVecs().myosinLevel[0])); // pointer to the vector storing myosin level
	

	double* cellCenterXAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.centerCoordX[0]));
	double* cellCenterYAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.centerCoordY[0]));
	double* cellRadiusAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.cellRadius[0]));
	double* cellPolarAngleAddr = thrust::raw_pointer_cast(
			&(cellInfoVecs.cellPolarAngle[0]));
 	int* nodeAdhIdxAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeAdhereIndex[0]));
 	uint* nodeActLevelAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeActLevel[0]));
	double timeNow = curTime;

	double ddt = dt;
	uint* cellActiveFilopCountsAddr = thrust::raw_pointer_cast(
        &(cellInfoVecs.activeCellFilopCounts[0]));

	uint maxMemNodePerCell = allocPara_m.maxMembrNodePerCell;
	uint maxNodePerCell = allocPara_m.maxAllNodePerCell;
    double* nodeLocXAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeLocX[0]));
    double* nodeLocYAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeLocY[0]));
    bool* nodeIsActiveAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeIsActive[0])); // 
	double* myosinLevelAddr = thrust::raw_pointer_cast(
		&(nodes->getInfoVecs().myosinLevel[0]));
	double* myosinWeightAddr = thrust::raw_pointer_cast(
		&(nodes->getInfoVecs().myosinWeight[0]));
	int* cellTypeAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.cell_Type[0]));
	uint leaderRank = allocPara_m.leaderRank; // 
	// uint leaderRank = nodes->getLeaderRank();
	uint* cellActLevelAddr = thrust::raw_pointer_cast(
        &(cellInfoVecs.activationLevel[0]));

	thrust::counting_iterator<uint> iBegin(0);
	thrust::counting_iterator<uint> iEnd(activeCellCount); // make sure not iterate on inactive cells already 
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(iBegin,
							cellInfoVecs.activationLevel.begin(),
							cellInfoVecs.activeMembrNodeCounts.begin(),
							cellInfoVecs.cell_Type.begin(),
							cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin(),
							cellInfoVecs.cellRadius.begin(),
							cellInfoVecs.cellPolarAngle.begin()
							)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							iEnd,
							cellInfoVecs.activationLevel.begin() + activeCellCount,
							cellInfoVecs.activeMembrNodeCounts.begin() + activeCellCount,
							cellInfoVecs.cell_Type.begin() + activeCellCount,
							cellInfoVecs.centerCoordX.begin() + activeCellCount,
							cellInfoVecs.centerCoordY.begin() + activeCellCount,
							cellInfoVecs.cellRadius.begin() + activeCellCount,
							cellInfoVecs.cellPolarAngle.begin() + activeCellCount
							)),
			cellInfoVecs.cellPolarAngle.begin(),
			updateCellPolarLeaderDevice(seed, ddt, timeNow, 
			activeCellCount,cellCenterXAddr,cellCenterYAddr,cellRadiusAddr,
			cellActiveFilopCountsAddr,maxMemNodePerCell,maxNodePerCell,nodeLocXAddr,nodeLocYAddr,
			nodeIsActiveAddr,nodeAdhIdxAddr,nodeActLevelAddr,myosinLevelAddr,cellTypeAddr,leaderRank,cellPolarAngleAddr,cellActLevelAddr,myosinWeightAddr));
}










void SceCells::test_SigPt(std::vector<SigptState>& sigPtVec) {
	uint activeCellCount = allocPara_m.currentActiveCellCount;
	
	double* cellFilopXAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.cellFilopX[0]));
    double* cellFilopYAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.cellFilopY[0]));
    double* cellFilopAngleAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.cellFilopAngle[0]));
    bool* cellFilopIsActiveAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.cellFilopIsActive[0]));
    double* cellFilopBirthTimeAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.cellFilopBirthTime[0]));
	double* cellCenterXAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.centerCoordX[0]));
	double* cellCenterYAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.centerCoordY[0]));
	double* cellRadiusAddr = thrust::raw_pointer_cast(
            &(cellInfoVecs.cellRadius[0]));
 	int* nodeAdhIdxAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeAdhereIndex[0]));
	thrust::device_vector<SigptState> d_sigPtVec(sigPtVec);
	SigptState* sigPtAddr = thrust::raw_pointer_cast(&d_sigPtVec[0]);
	double timeNow = curTime;

	double ddt = dt;
	uint* cellActiveFilopCountsAddr = thrust::raw_pointer_cast(
        &(cellInfoVecs.activeCellFilopCounts[0]));

	uint maxMemNodePerCell = allocPara_m.maxMembrNodePerCell;
	uint maxNodePerCell = allocPara_m.maxAllNodePerCell;
    double* nodeLocXAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeLocX[0]));
    double* nodeLocYAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeLocY[0]));
    bool* nodeIsActiveAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeIsActive[0])); // 

	thrust::counting_iterator<uint> iBegin(0);
	thrust::counting_iterator<uint> iEnd(activeCellCount);
	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(iBegin,
							cellInfoVecs.centerCoordX.begin(),
							cellInfoVecs.centerCoordY.begin(),
							cellInfoVecs.cellRadius.begin(),
							cellInfoVecs.cellPolarAngle.begin(),
							cellInfoVecs.cell_Type.begin(),
							cellInfoVecs.activeMembrNodeCounts.begin()
							)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							iEnd,
							cellInfoVecs.centerCoordX.begin() + activeCellCount,
							cellInfoVecs.centerCoordY.begin() + activeCellCount,
							cellInfoVecs.cellRadius.begin() + activeCellCount,
							cellInfoVecs.cellPolarAngle.begin() + activeCellCount,
							cellInfoVecs.cell_Type.begin() + activeCellCount,
                            cellInfoVecs.activeMembrNodeCounts.begin() + activeCellCount
							)),
			cellInfoVecs.cellPolarAngle.begin(),
			updateSigPtState(ddt, timeNow, 
			activeCellCount,cellCenterXAddr,cellCenterYAddr,cellRadiusAddr,
			maxMemNodePerCell,maxNodePerCell,nodeLocXAddr,nodeLocYAddr,nodeIsActiveAddr,nodeAdhIdxAddr,sigPtAddr));
	thrust::copy(d_sigPtVec.begin(), d_sigPtVec.end(), sigPtVec.begin());
}











void SceCells::calSubAdhForce() {
	cout<< "I am starting to compute adhesion forces "<< endl;
	totalNodeCountForActiveCells = allocPara_m.currentActiveCellCount
			* allocPara_m.maxAllNodePerCell;
	uint maxAllNodePerCell = allocPara_m.maxAllNodePerCell;
	uint maxMemNodePerCell = allocPara_m.maxMembrNodePerCell;
	thrust::counting_iterator<uint> iBegin(0);

	double* nodeLocXAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocX[0]));
	double* nodeLocYAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeLocY[0]));
	bool* nodeIsActiveAddr = thrust::raw_pointer_cast(
			&(nodes->getInfoVecs().nodeIsActive[0])); // 

	double* myosinLevelAddr = thrust::raw_pointer_cast(
		&(nodes->getInfoVecs().myosinLevel[0])); 
		
	double* subAdhLocXAddr = thrust::raw_pointer_cast(
		&(nodes->getInfoVecs().subAdhLocX[0])); 
	double* subAdhLocYAddr = thrust::raw_pointer_cast(
		&(nodes->getInfoVecs().subAdhLocY[0])); 
	bool* subAdhIsBoundAddr = thrust::raw_pointer_cast(
		&(nodes->getInfoVecs().subAdhIsBound[0])); 
	uint* nodeActLevelAddr = thrust::raw_pointer_cast(
            &(nodes->getInfoVecs().nodeActLevel[0]));

	uint* cellActLevelAddr = thrust::raw_pointer_cast(
        &(cellInfoVecs.activationLevel[0]));
	double* cellPolarAngleAddr = thrust::raw_pointer_cast(
        &(cellInfoVecs.cellPolarAngle[0]));

	double timeStep = dt;
	double timeNow = curTime;
	// uint maxSubSitePerNode = 10;

	random_device rd;
    uint seed = time(NULL);
    seed = rd();
	// uint64_t t = static_cast<uint64_t>(time(nullptr));
	// uint64_t r = static_cast<uint64_t>(rd());
	// uint64_t mixed = (r << 32) ^ t;   // mix high‑entropy rd() into high bits, time() into low bits
	// unsigned int seed = static_cast<unsigned int>(mixed);

	thrust::transform(
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.activeMembrNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.cell_Type.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.activeIntnlNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
                                    cellInfoVecs.centerCoordX.begin(),
                                    make_transform_iterator(iBegin,
                                            DivideFunctor(maxAllNodePerCell))),
                            thrust::make_permutation_iterator(
                                    cellInfoVecs.centerCoordY.begin(),
                                    make_transform_iterator(iBegin,
                                            DivideFunctor(maxAllNodePerCell))),
							make_transform_iterator(iBegin,
									DivideFunctor(maxAllNodePerCell)),
							make_transform_iterator(iBegin,
									ModuloFunctor(maxAllNodePerCell)),
							nodes->getInfoVecs().myosinLevel.begin(),
							nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin()
							//thrust::make_permutation_iterator(
                            //        cellInfoVecs.cellPolarAngle.begin(),
                            //        make_transform_iterator(iBegin,
                            //                DivideFunctor(maxAllNodePerCell)))
							)),
			thrust::make_zip_iterator(
					thrust::make_tuple(
							thrust::make_permutation_iterator(
									cellInfoVecs.activeMembrNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.cell_Type.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
							thrust::make_permutation_iterator(
									cellInfoVecs.activeIntnlNodeCounts.begin(),
									make_transform_iterator(iBegin,
											DivideFunctor(maxAllNodePerCell))),
						    thrust::make_permutation_iterator(
                                    cellInfoVecs.centerCoordX.begin(),
                                    make_transform_iterator(iBegin,
                                            DivideFunctor(maxAllNodePerCell))),
                            thrust::make_permutation_iterator(
                                    cellInfoVecs.centerCoordY.begin(),
                                    make_transform_iterator(iBegin,
                                            DivideFunctor(maxAllNodePerCell))),
							make_transform_iterator(iBegin,
									DivideFunctor(maxAllNodePerCell)),
							make_transform_iterator(iBegin,
									ModuloFunctor(maxAllNodePerCell)),
							nodes->getInfoVecs().myosinLevel.begin(),
							nodes->getInfoVecs().nodeVelX.begin(),
							nodes->getInfoVecs().nodeVelY.begin()
							//thrust::make_permutation_iterator(
                            //       cellInfoVecs.cellPolarAngle.begin(),
                            //        make_transform_iterator(iBegin,
                            //                DivideFunctor(maxAllNodePerCell)))
							))
					+ totalNodeCountForActiveCells,
			thrust::make_zip_iterator(
					thrust::make_tuple(nodes->getInfoVecs().nodeVelX.begin(),
							   nodes->getInfoVecs().nodeVelY.begin()
							   //nodes->getInfoVecs().subAdhLocX.begin(), // different length compared with VelXY, Okay? probably not!!! change in place
							   //nodes->getInfoVecs().subAdhLocY.begin(),
							   //nodes->getInfoVecs().subAdhIsBound.begin(),
								)), 
			calSubAdhForceDevice(maxAllNodePerCell, maxMemNodePerCell, nodeLocXAddr,
					nodeLocYAddr, nodeIsActiveAddr, myosinLevelAddr, timeStep, timeNow, 
					subAdhLocXAddr, subAdhLocYAddr, subAdhIsBoundAddr, cellActLevelAddr, seed,nodeActLevelAddr,cellPolarAngleAddr));
}









__device__
void calAndAddIB_M(double& xPos, double& yPos, double& xPos2, double& yPos2,
		double& growPro, double& xRes, double& yRes, double grthPrgrCriVal_M) {
	double linkLength = compDist2D(xPos, yPos, xPos2, yPos2);

	double forceValue = 0;
	if (growPro > grthPrgrCriEnd_M) {
		if (linkLength < sceIBDiv_M[4]) {
			forceValue = -sceIBDiv_M[0] / sceIBDiv_M[2]
					* exp(-linkLength / sceIBDiv_M[2])
					+ sceIBDiv_M[1] / sceIBDiv_M[3]
							* exp(-linkLength / sceIBDiv_M[3]);
		}
	} else if (growPro > grthPrgrCriVal_M) {
		double percent = (growPro - grthPrgrCriVal_M)
				/ (grthPrgrCriEnd_M - grthPrgrCriVal_M);
		double lenLimit = percent * (sceIBDiv_M[4])
				+ (1.0 - percent) * sceIB_M[4];
		if (linkLength < lenLimit) {
			double intnlBPara0 = percent * (sceIBDiv_M[0])
					+ (1.0 - percent) * sceIB_M[0];
			double intnlBPara1 = percent * (sceIBDiv_M[1])
					+ (1.0 - percent) * sceIB_M[1];
			double intnlBPara2 = percent * (sceIBDiv_M[2])
					+ (1.0 - percent) * sceIB_M[2];
			double intnlBPara3 = percent * (sceIBDiv_M[3])
					+ (1.0 - percent) * sceIB_M[3];
			forceValue = -intnlBPara0 / intnlBPara2
					* exp(-linkLength / intnlBPara2)
					+ intnlBPara1 / intnlBPara3
							* exp(-linkLength / intnlBPara3);
		}
	} else {
		if (linkLength < sceIB_M[4]) {
			forceValue = -sceIB_M[0] / sceIB_M[2]
					* exp(-linkLength / sceIB_M[2])
					+ sceIB_M[1] / sceIB_M[3] * exp(-linkLength / sceIB_M[3]);
		}
	}
	xRes = xRes + forceValue * (xPos2 - xPos) / linkLength;
	yRes = yRes + forceValue * (yPos2 - yPos) / linkLength;
}

//Ali function added for eventually computing pressure for each cells
// same as the above function except this function also updates F_MI_M_x and F_MI_M_y
__device__
void calAndAddIB_M2(double& xPos, double& yPos, double& xPos2, double& yPos2,
		double& growPro, double& xRes, double& yRes, double & F_MI_M_x, double & F_MI_M_y, double grthPrgrCriVal_M) {
	double linkLength = compDist2D(xPos, yPos, xPos2, yPos2);

	double forceValue = 0;
	if (growPro > grthPrgrCriEnd_M) {
		if (linkLength < sceIBDiv_M[4]) {
			forceValue = -sceIBDiv_M[0] / sceIBDiv_M[2]
					* exp(-linkLength / sceIBDiv_M[2])
					+ sceIBDiv_M[1] / sceIBDiv_M[3]
							* exp(-linkLength / sceIBDiv_M[3]);
		}
	} else if (growPro > grthPrgrCriVal_M) {
		double percent = (growPro - grthPrgrCriVal_M)
				/ (grthPrgrCriEnd_M - grthPrgrCriVal_M);
		double lenLimit = percent * (sceIBDiv_M[4])
				+ (1.0 - percent) * sceIB_M[4];
		if (linkLength < lenLimit) {
			double intnlBPara0 = percent * (sceIBDiv_M[0])
					+ (1.0 - percent) * sceIB_M[0];
			double intnlBPara1 = percent * (sceIBDiv_M[1])
					+ (1.0 - percent) * sceIB_M[1];
			double intnlBPara2 = percent * (sceIBDiv_M[2])
					+ (1.0 - percent) * sceIB_M[2];
			double intnlBPara3 = percent * (sceIBDiv_M[3])
					+ (1.0 - percent) * sceIB_M[3];
			forceValue = -intnlBPara0 / intnlBPara2
					* exp(-linkLength / intnlBPara2)
					+ intnlBPara1 / intnlBPara3
							* exp(-linkLength / intnlBPara3);
		}
	} else {
		if (linkLength < sceIB_M[4]) {
			forceValue = -sceIB_M[0] / sceIB_M[2]
					* exp(-linkLength / sceIB_M[2])
					+ sceIB_M[1] / sceIB_M[3] * exp(-linkLength / sceIB_M[3]);
		}
	}

	F_MI_M_x=F_MI_M_x+forceValue * (xPos2 - xPos) / linkLength;
	F_MI_M_y=F_MI_M_y+forceValue * (yPos2 - yPos) / linkLength;
       
	xRes = xRes + forceValue * (xPos2 - xPos) / linkLength;
	yRes = yRes + forceValue * (yPos2 - yPos) / linkLength;
}

__device__
void calAndAddII_M(double& xPos, double& yPos, double& xPos2, double& yPos2,
		double& growPro, double& xRes, double& yRes, double grthPrgrCriVal_M) {
	double linkLength = compDist2D(xPos, yPos, xPos2, yPos2);

	double forceValue = 0;
	if (growPro > grthPrgrCriEnd_M) {
		if (linkLength < sceIIDiv_M[4]) {
			forceValue = -sceIIDiv_M[0] / sceIIDiv_M[2]
					* exp(-linkLength / sceIIDiv_M[2])
					+ sceIIDiv_M[1] / sceIIDiv_M[3]
							* exp(-linkLength / sceIIDiv_M[3]);
		}
	} else if (growPro > grthPrgrCriVal_M) {
		double percent = (growPro - grthPrgrCriVal_M)
				/ (grthPrgrCriEnd_M - grthPrgrCriVal_M);
		double lenLimit = percent * (sceIIDiv_M[4])
				+ (1.0 - percent) * sceII_M[4];
		if (linkLength < lenLimit) {
			double intraPara0 = percent * (sceIIDiv_M[0])
					+ (1.0 - percent) * sceII_M[0];
			double intraPara1 = percent * (sceIIDiv_M[1])
					+ (1.0 - percent) * sceII_M[1];
			double intraPara2 = percent * (sceIIDiv_M[2])
					+ (1.0 - percent) * sceII_M[2];
			double intraPara3 = percent * (sceIIDiv_M[3])
					+ (1.0 - percent) * sceII_M[3];
			forceValue = -intraPara0 / intraPara2
					* exp(-linkLength / intraPara2)
					+ intraPara1 / intraPara3 * exp(-linkLength / intraPara3);
		}
	} else {
		if (linkLength < sceII_M[4]) {
			forceValue = -sceII_M[0] / sceII_M[2]
					* exp(-linkLength / sceII_M[2])
					+ sceII_M[1] / sceII_M[3] * exp(-linkLength / sceII_M[3]);
		}
	}
	xRes = xRes + forceValue * (xPos2 - xPos) / linkLength;
	yRes = yRes + forceValue * (yPos2 - yPos) / linkLength;
}
