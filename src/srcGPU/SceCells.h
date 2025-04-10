#ifndef SCECELLS_H_
#define SCECELLS_H_

#include "SceNodes.h"
#include "../srcCPU/Signal_MakeInterface.h"
#include <time.h>
#include <thrust/tabulate.h>
#include <cmath>
#define PI 3.141592653589793238462643383279

typedef thrust::tuple<double, double, SceNodeType> CVec2Type;
typedef thrust::tuple<bool, double, double> BoolDD;
typedef thrust::tuple<uint, double, double> UiDD;
typedef thrust::tuple<uint, double, double, uint> UiDDU;
typedef thrust::tuple<double, double, double,double> DDDD; //Ali 
typedef thrust::tuple<double, double, double,double,int> DDDDi; //Ali 
typedef thrust::tuple<uint, double, double, bool> UiDDBool;//AAMIRI
typedef thrust::tuple<uint, uint> UiUi;
typedef thrust::tuple<bool, SceNodeType> boolType;
typedef thrust::tuple<double, double, bool, SceNodeType, uint> Vel2DActiveTypeRank;
//Ali comment 
//typedef thrust::tuple<uint, uint, uint, double, double, double, double> TensionData;
//Ali comment
//Ali
typedef thrust::tuple<double, uint, double, double,uint, uint, double, double, double, double> TensionData;
//Ali 
typedef thrust::tuple<uint, uint, uint, double, double> BendData;
typedef thrust::tuple<uint, uint, uint, double, double, double, double, double, double, double> CurvatureData;//AAMIRI
typedef thrust::tuple<uint, uint, uint, uint, double, double, double> CellData;
typedef thrust::tuple<uint, uint, uint, uint, double> UUUUD;
typedef thrust::tuple<uint, uint, uint, uint, int, double, double> UUUUIDD;
typedef thrust::tuple<uint, uint, uint, uint, int, double, double, double> UUUUIDDD;
typedef thrust::tuple<uint, uint, double, double, uint, uint,int> UUDDUUi;
typedef thrust::tuple<uint, uint, uint, double, double, double, double> UUUDDDD;
typedef thrust::tuple<uint, uint, uint, uint, double, double, double, double, double> UUUUDDDDD;
typedef thrust::tuple<uint, uint, uint, uint, double, double, double, double, double, int> UUUUDDDDDi;
typedef thrust::tuple<uint, uint, double, double, uint, uint, double> UUDDUUD;
typedef thrust::tuple<uint, uint, double, double, uint, uint, double, double, double,int> UUDDUUDDDi;
typedef thrust::tuple<uint, int, uint, double, double, uint, uint, double, double, double> UIUDDUUDDD;

//typedef pair<device_vector<double>::iterator,device_vector<double>::iterator> MinMaxNode ; 
// maxMemThres, cellRank, nodeRank , locX, locY, velX, velY

/*
 __device__
 SceNodeType indxToType(uint& indx);

 SceNodeType indxToType(uint& indx);
 */

__device__
double calExtForce(double& curTime);

//Ali comment 
//__device__
//double calMembrForce(double& length);

//Ali & Abu June 30th
__device__
double calMembrForce_Mitotic(double& length, double& progress, double mitoticCri);
__device__
double calBendMulti(double& angle, uint activeMembrCt);

//AAMIRI
__device__
double calBendMulti_Mitotic(double& angle, uint activeMembrCt, double& progress, double mitoticCri, int cell_Type);

__device__
double obtainRandAngle(uint& cellRank, uint& seed);
// comment prevents bad formatting issues of __host__ and __device__ in Nsight

__device__
int obtainRemovingMembrNodeID(uint &cellRank, uint& activeMembrNodes, uint& seed);//AAMIRI
// comment prevents bad formatting issues of __host__ and __device__ in Nsight

__device__
uint obtainMembEndNode(uint &cellRank, uint& activeMembrNodes);//AAMIRI
// comment prevents bad formatting issues of __host__ and __device__ in Nsight

__device__ uint obtainNewIntnlNodeIndex(uint& cellRank, uint& curActiveCount);
// comment prevents bad formatting issues of __host__ and __device__ in Nsight

__device__ uint obtainLastIntnlNodeIndex(uint& cellRank, uint& curActiveCount);//AAMIRI
// comment prevents bad formatting issues of __host__ and __device__ in Nsight

__device__
bool isAllIntnlFilled(uint& currentIntnlCount);
// comment prevents bad formatting issues of __host__ and __device__ in Nsight

__device__
bool isAllIntnlEmptied(uint& currentIntnlCount);//AAMIRI
// comment prevents bad formatting issues of __host__ and __device__ in Nsight

__device__
bool isAllMembrEmptied(uint& currentMembrCount);//AAMIRI
// comment prevents bad formatting issues of __host__ and __device__ in Nsight


__device__
bool longEnough(double& length);

__device__
bool bigEnough(double& num);

__device__
double cross_Z(double vecA_X, double vecA_Y, double vecB_X, double vecB_Y);

__device__
void calAndAddIB_M(double& xPos, double& yPos, double& xPos2, double& yPos2,
		double& growPro, double& xRes, double& yRes, double grthPrgrCriVal_M);
//AliA 
__device__
void calAndAddIB_M2(double& xPos, double& yPos, double& xPos2, double& yPos2,
		double& growPro, double& xRes, double& yRes, double &F_MI_M_x, double & F_MI_M_y,double grthPrgrCriVal_M);

__device__
void calAndAddII_M(double& xPos, double& yPos, double& xPos2, double& yPos2,
		double& growPro, double& xRes, double& yRes, double grthPrgrCriVal_M);

__device__
double compDist2D(double &xPos, double &yPos, double &xPos2, double &yPos2);


__device__
bool isInsideCell(double xPos, double yPos, uint intnlIndxMemBegin, uint activeMembrNodeCounts, double* nodeXAddr, double* nodeYAddr);


__device__
double Angle2D(double x1, double y1, double x2, double y2);

/*
__device__
void handleAdhesionForceCell_M(int& adhereIndex, double& xPos, double& yPos,
		double& curAdherePosX, double& curAdherePosY, double& xRes,
		double& yRes, double& alpha, uint& curActLevel);
*/

__device__
double getMitoticAdhCoefCell(double& growProg, double& growProgNeigh);

__device__
double computeDistCell2D(double &xPos, double &yPos, double &xPos2, double &yPos2);

__device__
bool isValidFilopDirection(uint& intnlIndxMemBegin, uint _maxMemNodePerCell, bool* _isActiveAddr, double* _locXAddr, double* _locYAddr,
	double& cell_CenterX, double& cell_CenterY, double& randomAngleRd, int* _nodeAdhereIndexAddr, uint& activeMembrCount, uint _maxNodePerCell, double* _myosinLevelAddr);

__device__
bool isTangFilopDirection(uint& intnlIndxMemBegin, bool* _isActiveAddr, double* _locXAddr, double* _locYAddr,
	double& cell_CenterX, double& cell_CenterY, double& randomAngleRd, int* _nodeAdhereIndexAddr, 
	uint& activeMembrCount, uint _maxNodePerCell, int* _cellTypeAddr, uint leaderRank, double* _myosinLevelAddr, int& filopDirection);

__device__
double computeCurvature(double& xpos1, double& ypos1, double& xpos2, double& ypos2, double& xpos3, double& ypos3);



/**
 * Functor for divide operation.
 * @param dividend divisor for divide operator.
 * @param input1: number to be divided
 * @return output: result from division
 */
struct DivideFunctor: public thrust::unary_function<uint, uint> {
	uint dividend;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ DivideFunctor(uint dividendInput) :
			dividend(dividendInput) {
	}
	__host__ __device__ uint operator()(const uint &num) {
		return num / dividend;
	}
};

struct PlusNum: public thrust::unary_function<uint, uint> {
	uint _addNum;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ PlusNum(uint num) :
			_addNum(num) {
	}
	__host__ __device__
	double operator()(const uint &num1) {
		return num1 + _addNum;
	}
};

/**
 * Functor for modulo operation.
 * @param dividend divisor for modulo operator.
 * @param input1: number to be moduled
 * @return output: result from modulo
 */
struct ModuloFunctor: public thrust::unary_function<uint, uint> {
	uint dividend;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ ModuloFunctor(uint dividendInput) :
			dividend(dividendInput) {
	}
	__host__ __device__ uint operator()(const uint &num) {
		return num % dividend;
	}
};

struct isActiveNoneBdry {
	__host__ __device__
	bool operator()(boolType b) {
		bool isActive = thrust::get<0>(b);
		SceNodeType type = thrust::get<1>(b);
		if (isActive && type != Boundary) {
			return true;
		} else {
			return false;
		}
	}
};

struct MaxWInfo: public thrust::binary_function<DUiDDD, DUiDDD, DUiDDD> {
	__host__ __device__ DUiDDD operator()(const DUiDDD& data1,
			const DUiDDD& data2) {
		double num1 = thrust::get<0>(data1);
		double num2 = thrust::get<0>(data2);
		if (num1 > num2) {
			return data1;
		} else {
			return data2;
		}
	}
};

/**
 * Functor for add two three dimensional vectors.
 * @param input1 first three dimensional vector to add
 * @param input2 second three dimensional vector to add
 * @return output result of addition
 */
struct CVec3Add: public thrust::binary_function<CVec3, CVec3, CVec3> {
	__host__ __device__ CVec3 operator()(const CVec3 &vec1, const CVec3 &vec2) {
		return thrust::make_tuple(thrust::get<0>(vec1) + thrust::get<0>(vec2),
				thrust::get<1>(vec1) + thrust::get<1>(vec2),
				thrust::get<2>(vec1) + thrust::get<2>(vec2));
	}
};

struct CVec2Add: public thrust::binary_function<CVec2, CVec2, CVec2> {
	__host__ __device__ CVec2 operator()(const CVec2 &vec1, const CVec2 &vec2) {
		return thrust::make_tuple(thrust::get<0>(vec1) + thrust::get<0>(vec2),
				thrust::get<1>(vec1) + thrust::get<1>(vec2));
	}
};

/**
 * Divide three inputs by one same number.
 * @param input1 first number to be divide \n
 *        input2 second number to be divide \n
 *        input3 third number to be divide \n
 * @return output1 first division result \n
 *        output2 second division result \n
 *        output3 third division result \n
 */
struct CVec3Divide: public thrust::binary_function<CVec3, double, CVec3> {
	__host__ __device__ CVec3 operator()(const CVec3 &vec1,
			const double &divisor) {
		return thrust::make_tuple(thrust::get<0>(vec1) / divisor,
				thrust::get<1>(vec1) / divisor, thrust::get<2>(vec1) / divisor);
	}
};

struct CVec2Divide: public thrust::binary_function<CVec2, double, CVec2> {
	__host__ __device__ CVec2 operator()(const CVec2 &vec1,
			const double &divisor) {
		return thrust::make_tuple(thrust::get<0>(vec1) / divisor,
				thrust::get<1>(vec1) / divisor);
	}
};

struct LessEqualTo: public thrust::unary_function<UiB, bool> {
	uint _limit;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ LessEqualTo(uint limit) :
			_limit(limit) {
	}
	__device__
	bool operator()(const UiB& numBool) const {
		uint num = thrust::get<0>(numBool);
		uint isGrow = thrust::get<1>(numBool);
		if (num <= _limit && isGrow) {
			return true;
		} else {
			return false;
		}
	}
};

// maxMemThres, cellRank, nodeRank , locX, locY, velX, velY
//Ali comment
/**
struct AddMemAngDeviceNotUsed: public thrust::unary_function<TensionData, CVec10> {
	uint _bdryCount;
	uint _maxNodePerCell;
	double* _locXAddr;
	double* _locYAddr;
	bool* _isActiveAddr;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ AddMemAngDeviceNotUsed(uint bdryCount, uint maxNodePerCell,
			double* locXAddr, double* locYAddr, bool* isActiveAddr) :
			_bdryCount(bdryCount), _maxNodePerCell(maxNodePerCell), _locXAddr(
					locXAddr), _locYAddr(locYAddr), _isActiveAddr(isActiveAddr) {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ CVec10 operator()(const TensionData &tData) const {
		uint activeMembrCount = thrust::get<0>(tData);
		uint cellRank = thrust::get<1>(tData);
		uint nodeRank = thrust::get<2>(tData);
		double locX = thrust::get<3>(tData);
		double locY = thrust::get<4>(tData);
		double velX = thrust::get<5>(tData);
		double velY = thrust::get<6>(tData);

		uint index = _bdryCount + cellRank * _maxNodePerCell + nodeRank;

		double mag = 0;
		double rightMag = 0;
		double midX = 0;
		double midY = 0;
		double bendLeftX = 0;
		double bendLeftY = 0;
		double bendRightX = 0;
		double bendRightY = 0;

		double leftPosX;
		double leftPosY;
		double leftDiffX;
		double leftDiffY;
		double lenLeft;

		double rightPosX;
		double rightPosY;
		double rightDiffX;
		double rightDiffY;
		double lenRight;

		if (_isActiveAddr[index] == false || nodeRank >= activeMembrCount) {
			return thrust::make_tuple(velX, velY, mag, rightMag, midX, midY,
					bendLeftX, bendLeftY, bendRightX, bendRightY);
		} else {
			int index_left = nodeRank - 1;
			if (index_left == -1) {
				index_left = activeMembrCount - 1;
			}
			index_left = index_left + _bdryCount + cellRank * _maxNodePerCell;
			// apply tension force from left
			if (_isActiveAddr[index_left]) {
				leftPosX = _locXAddr[index_left];
				leftPosY = _locYAddr[index_left];
				leftDiffX = leftPosX - locX;
				leftDiffY = leftPosY - locY;
				lenLeft = sqrt(leftDiffX * leftDiffX + leftDiffY * leftDiffY);
				double forceVal = calMembrForce(lenLeft);
				if (longEnough(lenLeft)) {
					velX = velX + forceVal * leftDiffX / lenLeft;
					velY = velY + forceVal * leftDiffY / lenLeft;
					mag = forceVal + mag;
				}
			}

			int index_right = nodeRank + 1;
			if (index_right == (int) activeMembrCount) {
				index_right = 0;
			}
			index_right = index_right + _bdryCount + cellRank * _maxNodePerCell;
			// apply tension force from right
			if (_isActiveAddr[index_right]) {
				rightPosX = _locXAddr[index_right];
				rightPosY = _locYAddr[index_right];
				rightDiffX = rightPosX - locX;
				rightDiffY = rightPosY - locY;
				lenRight = sqrt(
						rightDiffX * rightDiffX + rightDiffY * rightDiffY);
				double forceVal = calMembrForce(lenRight);
				if (longEnough(lenRight)) {
					velX = velX + forceVal * rightDiffX / lenRight;
					velY = velY + forceVal * rightDiffY / lenRight;
					mag = forceVal + mag;
					rightMag = forceVal;
					midX = (rightPosX + locX) / 2;
					midY = (rightPosY + locY) / 2;
				}
			}
			// applies bending force.
			if (_isActiveAddr[index_left] && _isActiveAddr[index_right]) {
				if (longEnough(lenLeft) && longEnough(lenRight)) {
					double dotP = -leftDiffX * rightDiffX
							- leftDiffY * rightDiffY;
					double vecP = dotP / (lenLeft * lenRight);

					// because of numerical error, 1 - vecP*vecP could be less than 0, although it is not possible in mathematics.
					// sqrt(negative number) would cause term0 to be nan.
					// if an nan number is produced, it will not be accepted by bigEnough function.
					// this is OK, because we know at that time bending energy should be 0.
					double term0 = sqrt(1 - vecP * vecP);
					// this if statement is required for numerical purpose only.
					// Whole term would go to zero when term 0 close to zero, but the computation
					// would cause numerical errors, so need to make sure term0 is big enough.
					if (bigEnough(term0)) {
						double angle;
						// value of cross product in z direction: vecA_X * vecB_Y - vecA_Y * vecB_X
						double crossZ = leftDiffY * rightDiffX
								- leftDiffX * rightDiffY;
						if (crossZ > 0) {
							// means angle > PI (concave)
							angle = PI + acos(vecP);
						} else {
							// means angle < PI (convex)
							angle = PI - acos(vecP);
						}
						// leftDiffX = ax-bx
						// rightDiffX = cx-bx
						double term1x = -rightDiffX / (lenLeft * lenRight);
						double term2x = leftDiffX / (lenLeft * lenRight);
						double term3x = (dotP * leftDiffX)
								/ (lenLeft * lenLeft * lenLeft * lenRight);
						double term4x = (-dotP * rightDiffX)
								/ (lenLeft * lenRight * lenRight * lenRight);
						double term1y = -rightDiffY / (lenLeft * lenRight);
						double term2y = leftDiffY / (lenLeft * lenRight);
						double term3y = (dotP * leftDiffY)
								/ (lenLeft * lenLeft * lenLeft * lenRight);
						double term4y = (-dotP * rightDiffY)
								/ (lenLeft * lenRight * lenRight * lenRight);

						double bendMultiplier = -calBendMulti(angle,
								activeMembrCount);
						// because sign of angle formula would change if crossZ < 0
						if (crossZ > 0) {
							bendMultiplier = -bendMultiplier;
						}

						// following are values obtained from matlab: (derivative of angle to each coordinate:
						//-((bx - cx)/(Lab*Lbc) - (DotP*(2*ax - 2*bx))/(2*Lab^3*Lbc))/(1 - DotP^2/(Lab^2*Lbc^2))^(1/2)
						//-((ax - 2*bx + cx)/(Lab*Lbc) + (DotP*(2*ax - 2*bx))/(2*Lab^3*Lbc) - (DotP*(2*bx - 2*cx))/(2*Lab*Lbc^3))/(1 - DotP^2/(Lab^2*Lbc^2))^(1/2)
						//((ax - bx)/(Lab*Lbc) - (DotP*(2*bx - 2*cx))/(2*Lab*Lbc^3))/(1 - DotP^2/(Lab^2*Lbc^2))^(1/2)
						//-((by - cy)/(Lab*Lbc) - (DotP*(2*ay - 2*by))/(2*Lab^3*Lbc))/(1 - DotP^2/(Lab^2*Lbc^2))^(1/2)
						//-((ay - 2*by + cy)/(Lab*Lbc) + (DotP*(2*ay - 2*by))/(2*Lab^3*Lbc) - (DotP*(2*by - 2*cy))/(2*Lab*Lbc^3))/(1 - DotP^2/(Lab^2*Lbc^2))^(1/2)
						//((ay - by)/(Lab*Lbc) - (DotP*(2*by - 2*cy))/(2*Lab*Lbc^3))/(1 - DotP^2/(Lab^2*Lbc^2))^(1/2)

						// f = -dE/dx, so the values added below are negative, compared with the symbolics shown above.

						bendLeftX = bendMultiplier * (term1x - term3x) / term0;
                                                if (locX > 25) {
						velX = velX
								+ bendMultiplier
										* (term2x - term1x + term3x - term4x)
										/ term0   ;
                                                }
                                                else {
						velX = velX
								+ bendMultiplier
										* (term2x - term1x + term3x - term4x)
										/ term0   ;
                                                }

						bendRightX = bendMultiplier * (term4x - term2x) / term0;

						bendLeftY = bendMultiplier * (term1y - term3y) / term0;

						velY = velY
								+ bendMultiplier
										* (term2y - term1y + term3y - term4y)
										/ term0;

						bendRightY = bendMultiplier * (term4y - term2y) / term0;

					}
				}
			}
			return thrust::make_tuple(velX, velY, mag, rightMag, midX, midY,
					bendLeftX, bendLeftY, bendRightX, bendRightY);
		}
	}
};
*/ 
//Ali comment end

//Ali
struct AddMemAngDevice: public thrust::binary_function<TensionData, CVec4Int, CVec10> {
	uint _bdryCount;
	uint _maxNodePerCell;
	double* _locXAddr;
	double* _locYAddr;
	bool* _isActiveAddr;
	double _mitoticCri;

	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ AddMemAngDevice(uint bdryCount, uint maxNodePerCell,
			double* locXAddr, double* locYAddr, bool* isActiveAddr, double mitoticCri) :
			_bdryCount(bdryCount), _maxNodePerCell(maxNodePerCell), _locXAddr(
					locXAddr), _locYAddr(locYAddr), _isActiveAddr(isActiveAddr), _mitoticCri(mitoticCri) {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ CVec10 operator()(const TensionData &tData, const CVec4Int &tData1) const {

		double progress = thrust::get<0>(tData);
		uint activeMembrCount = thrust::get<1>(tData);
		double Cell_CenterX = thrust::get<2>(tData);
		double Cell_Time_F  = thrust::get<3>(tData);
		uint   cellRank = thrust::get<4>(tData);
		uint   nodeRank = thrust::get<5>(tData);
		double locX = thrust::get<6>(tData);
		double locY = thrust::get<7>(tData);
		double velX = thrust::get<8>(tData);
		double velY = thrust::get<9>(tData);

		double actinX = thrust::get<0>(tData1);
		double actinY = thrust::get<1>(tData1);
		double myosinLevel = thrust::get<2>(tData1); 
		double Cell_CenterY = thrust::get<3>(tData1);
		int cell_Type = thrust::get<4>(tData1);

		uint index = _bdryCount + cellRank * _maxNodePerCell + nodeRank;

		double mag = 0;
		double rightMag = 0;
		double midX = 0;
		double midY = 0;
		double bendLeftX = 0;
		double bendLeftY = 0;
		double bendRightX = 0;
		double bendRightY = 0;

		double leftPosX;
		double leftPosY;
		double leftDiffX;
		double leftDiffY;
		double lenLeft;

		double rightPosX;
		double rightPosY;
		double rightDiffX;
		double rightDiffY;
		double lenRight;

		if (_isActiveAddr[index] == false || nodeRank >= activeMembrCount) {
			return thrust::make_tuple(velX, velY, mag, rightMag, midX, midY,
					bendLeftX, bendLeftY, bendRightX, bendRightY);
		} else {
			int index_left = nodeRank - 1;
			if (index_left == -1) {
				index_left = activeMembrCount - 1;
			}
			index_left = index_left + _bdryCount + cellRank * _maxNodePerCell;
			// apply tension force from left
			if (_isActiveAddr[index_left]) {
				leftPosX = _locXAddr[index_left];
				leftPosY = _locYAddr[index_left];
				leftDiffX = leftPosX - locX;
				leftDiffY = leftPosY - locY;
				lenLeft = sqrt(leftDiffX * leftDiffX + leftDiffY * leftDiffY);
				double forceVal = calMembrForce_Mitotic(lenLeft,progress, _mitoticCri); //Ali & Abu June 30th
				if (longEnough(lenLeft)) {
					velX = velX + forceVal * leftDiffX / lenLeft;
					velY = velY + forceVal * leftDiffY / lenLeft;
					mag = forceVal + mag;
				}
			}

			int index_right = nodeRank + 1;
			if (index_right == (int) activeMembrCount) {
				index_right = 0;
			}
			index_right = index_right + _bdryCount + cellRank * _maxNodePerCell;
			// apply tension force from right
			if (_isActiveAddr[index_right]) {
				rightPosX = _locXAddr[index_right];
				rightPosY = _locYAddr[index_right];
				rightDiffX = rightPosX - locX;
				rightDiffY = rightPosY - locY;
				lenRight = sqrt(
						rightDiffX * rightDiffX + rightDiffY * rightDiffY);
				double forceVal = calMembrForce_Mitotic(lenRight,progress, _mitoticCri); // Ali & June 30th 
				if (longEnough(lenRight)) {
					velX = velX + forceVal * rightDiffX / lenRight;
					velY = velY + forceVal * rightDiffY / lenRight;
					mag = forceVal + mag;
					rightMag = forceVal;
					midX = (rightPosX + locX) / 2;
					midY = (rightPosY + locY) / 2;
				}
			}
			


			// applies bending force.
			if (_isActiveAddr[index_left] && _isActiveAddr[index_right]) {
				if (longEnough(lenLeft) && longEnough(lenRight)) {
					double dotP = -leftDiffX * rightDiffX
							- leftDiffY * rightDiffY;
					double vecP = dotP / (lenLeft * lenRight);

					// because of numerical error, 1 - vecP*vecP could be less than 0, although it is not possible in mathematics.
					// sqrt(negative number) would cause term0 to be nan.
					// if an nan number is produced, it will not be accepted by bigEnough function.
					// this is OK, because we know at that time bending energy should be 0.
					double term0 = sqrt(1 - vecP * vecP);
					// this if statement is required for numerical purpose only.
					// Whole term would go to zero when term 0 close to zero, but the computation
					// would cause numerical errors, so need to make sure term0 is big enough.
					if (bigEnough(term0)) {
						double angle;
						// value of cross product in z direction: vecA_X * vecB_Y - vecA_Y * vecB_X
						double crossZ = leftDiffY * rightDiffX
								- leftDiffX * rightDiffY;
						if (crossZ > 0) {
							// means angle > PI (concave)
							angle = PI + acos(vecP);
						} else {
							// means angle < PI (convex)
							angle = PI - acos(vecP);
						}
						// leftDiffX = ax-bx
						// rightDiffX = cx-bx
						double term1x = -rightDiffX / (lenLeft * lenRight);
						double term2x = leftDiffX / (lenLeft * lenRight);
						double term3x = (dotP * leftDiffX)
								/ (lenLeft * lenLeft * lenLeft * lenRight);
						double term4x = (-dotP * rightDiffX)
								/ (lenLeft * lenRight * lenRight * lenRight);
						double term1y = -rightDiffY / (lenLeft * lenRight);
						double term2y = leftDiffY / (lenLeft * lenRight);
						double term3y = (dotP * leftDiffY)
								/ (lenLeft * lenLeft * lenLeft * lenRight);
						double term4y = (-dotP * rightDiffY)
								/ (lenLeft * lenRight * lenRight * lenRight);

						double bendMultiplier = -calBendMulti_Mitotic(angle,
								activeMembrCount, progress, _mitoticCri, cell_Type);//AAMIRI modified the arguments
						// because sign of angle formula would change if crossZ < 0
						if (crossZ > 0) {
							bendMultiplier = -bendMultiplier;
						}

						// following are values obtained from matlab: (derivative of angle to each coordinate:
						//-((bx - cx)/(Lab*Lbc) - (DotP*(2*ax - 2*bx))/(2*Lab^3*Lbc))/(1 - DotP^2/(Lab^2*Lbc^2))^(1/2)
						//-((ax - 2*bx + cx)/(Lab*Lbc) + (DotP*(2*ax - 2*bx))/(2*Lab^3*Lbc) - (DotP*(2*bx - 2*cx))/(2*Lab*Lbc^3))/(1 - DotP^2/(Lab^2*Lbc^2))^(1/2)
						//((ax - bx)/(Lab*Lbc) - (DotP*(2*bx - 2*cx))/(2*Lab*Lbc^3))/(1 - DotP^2/(Lab^2*Lbc^2))^(1/2)
						//-((by - cy)/(Lab*Lbc) - (DotP*(2*ay - 2*by))/(2*Lab^3*Lbc))/(1 - DotP^2/(Lab^2*Lbc^2))^(1/2)
						//-((ay - 2*by + cy)/(Lab*Lbc) + (DotP*(2*ay - 2*by))/(2*Lab^3*Lbc) - (DotP*(2*by - 2*cy))/(2*Lab*Lbc^3))/(1 - DotP^2/(Lab^2*Lbc^2))^(1/2)
						//((ay - by)/(Lab*Lbc) - (DotP*(2*by - 2*cy))/(2*Lab*Lbc^3))/(1 - DotP^2/(Lab^2*Lbc^2))^(1/2)

						// f = -dE/dx, so the values added below are negative, compared with the symbolics shown above.
                                                double  F_Ext=calExtForce (Cell_Time_F) ;  //Ali 
						bendLeftX = bendMultiplier * (term1x - term3x) / term0;
                                                //if (locX > Cell_CenterX && Cell_CenterX>23.53) {
                                                if (locX > Cell_CenterX) {
						velX = velX
								+ bendMultiplier
										* (term2x - term1x + term3x - term4x)
										/ term0 +F_Ext  ;
                                                }
                                                //else if (locX < Cell_CenterX && Cell_CenterX<23.53) {
                                                else {
						velX = velX
								+ bendMultiplier
										* (term2x - term1x + term3x - term4x)
										/ term0 -F_Ext  ;
                                                }
                                               // else {
					//	velX = velX
					//			+ bendMultiplier
					//					* (term2x - term1x + term3x - term4x)
					//					/ term0   ;
                                          //      }

                                  
						bendRightX = bendMultiplier * (term4x - term2x) / term0;

						bendLeftY = bendMultiplier * (term1y - term3y) / term0;

						velY = velY
								+ bendMultiplier
										* (term2y - term1y + term3y - term4y)
										/ term0;

						bendRightY = bendMultiplier * (term4y - term2y) / term0;

					}
				}
			}
			return thrust::make_tuple(velX, velY, mag, rightMag, midX, midY,
					bendLeftX, bendLeftY, bendRightX, bendRightY);
		}
	}
}; 


//AAMIRI

struct CalCurvatures: public thrust::unary_function<CurvatureData, CVec6> {

	uint _maxNodePerCell;
	bool* _isActiveAddr;
	double* _locXAddr;
	double* _locYAddr;

	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ CalCurvatures(uint maxNodePerCell, bool* isActiveAddr,
			double* locXAddr, double* locYAddr) :
			_maxNodePerCell(maxNodePerCell), _isActiveAddr(isActiveAddr), _locXAddr(
					locXAddr), _locYAddr(locYAddr) {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ CVec6 operator()(const CurvatureData &bData) const {
		uint activeMembrCount = thrust::get<0>(bData);
		uint cellRank = thrust::get<1>(bData);
		uint nodeRank = thrust::get<2>(bData);
		double f_MI_M_X = thrust::get<3>(bData); 
		double f_MI_M_Y = thrust::get<4>(bData);
		double f_MI_M_T= thrust::get<5>(bData);
		double f_MI_M_N = thrust::get<6>(bData);
		double curvature = thrust::get<7>(bData);
		double extForceX = thrust::get<8>(bData);
		double extForceY = thrust::get<9>(bData);
		//double extForceT = thrust::get<10>(bData);
		//double extForceN = thrust::get<11>(bData);

		curvature = 0.0;
		 f_MI_M_T= 0.0;
		 f_MI_M_N= 0.0;
		double extForceT = 0.0;
		double extForceN = 0.0;
                double DistToRi=0.0 ; 

		uint index = cellRank * _maxNodePerCell + nodeRank;
		if (_isActiveAddr[index] == false || nodeRank >= activeMembrCount) {
			return thrust::make_tuple(f_MI_M_T,f_MI_M_N, curvature, extForceT, extForceN,DistToRi);
		}

		int index_left = nodeRank - 1;
		if (index_left == -1) {
			index_left = activeMembrCount - 1;
		}
		index_left = index_left + cellRank * _maxNodePerCell;

		int index_right = nodeRank + 1;
		if (index_right == (int) activeMembrCount) {
			index_right = 0;
		}
		index_right = index_right + cellRank * _maxNodePerCell;

		double locX_left = _locXAddr[index_left];
		double locY_left = _locYAddr[index_left];

		double locX_this = _locXAddr[index];
		double locY_this = _locYAddr[index];

		double locX_right = _locXAddr[index_right];
		double locY_right = _locYAddr[index_right];

	double sdelta = 0.000000000001;
//finding the tangent vector
		double Tx_lt = locX_this - locX_left;
		double Ty_lt = locY_this - locY_left;
		double S_lt = sqrt( Tx_lt * Tx_lt + Ty_lt * Ty_lt );
		if (S_lt < sdelta){
		 	Tx_lt = 0.0;
			Ty_lt = 0.0;}
		else {
 			Tx_lt = Tx_lt / S_lt;
			Ty_lt = Ty_lt / S_lt;}

		double Tx_tr = locX_right - locX_this;
		double Ty_tr = locY_right - locY_this;
		double S_tr = sqrt( Tx_tr * Tx_tr + Ty_tr * Ty_tr );
		if (S_tr < sdelta){
			Tx_tr = 0.0;
			Ty_tr = 0.0;}
		else {	
 			Tx_tr = Tx_tr / S_tr;
			Ty_tr = Ty_tr / S_tr;}

		double avgT_x = (Tx_lt + Tx_tr) / 2.0;
		double avgT_y = (Ty_lt + Ty_tr) / 2.0;
		double avgSize = sqrt( avgT_x*avgT_x + avgT_y*avgT_y );
		if (avgSize < sdelta){
			avgT_x = 0.0;
			avgT_y = 0.0;}
		else {
			avgT_x = avgT_x / avgSize;
			avgT_y = avgT_y / avgSize;}

//finding the Normal Vector
		double Nx = Tx_tr - Tx_lt;
		double Ny = Ty_tr - Ty_lt;
		double sizeN = sqrt( Nx*Nx + Ny*Ny );
		if (sizeN < sdelta){
			Nx = 0.0;
			Ny = 0.0;}
		else {
			Nx = Nx / sizeN;
			Ny = Ny / sizeN;}

//calculating the Tangent and Normal Tensions
		f_MI_M_T = f_MI_M_X * avgT_x + f_MI_M_Y * avgT_y;
		f_MI_M_N = f_MI_M_X * Nx     + f_MI_M_Y * Ny;

//finding the curvature at this node
		curvature = sizeN;

//calculating the Tangent and Normal Ext Forces
		extForceT = extForceX * avgT_x + extForceY * avgT_y;
		extForceN = extForceX * Nx     + extForceY * Ny;

                DistToRi=S_tr ; 
		return thrust::make_tuple(f_MI_M_T, f_MI_M_N, curvature, extForceT, extForceN,DistToRi);
	}
};


struct AddMembrBendDevice: public thrust::unary_function<BendData, CVec2> {
	uint _maxNodePerCell;
	bool* _isActiveAddr;
	double* _bendLeftXAddr;
	double* _bendLeftYAddr;
	double* _bendRightXAddr;
	double* _bendRightYAddr;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ AddMembrBendDevice(uint maxNodePerCell, bool* isActiveAddr,
			double* bendLeftXAddr, double* bendLeftYAddr,
			double* bendRightXAddr, double* bendRightYAddr) :
			_maxNodePerCell(maxNodePerCell), _isActiveAddr(isActiveAddr), _bendLeftXAddr(
					bendLeftXAddr), _bendLeftYAddr(bendLeftYAddr), _bendRightXAddr(
					bendRightXAddr), _bendRightYAddr(bendRightYAddr) {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ CVec2 operator()(const BendData &bData) const {
		uint activeMembrCount = thrust::get<0>(bData);
		uint cellRank = thrust::get<1>(bData);
		uint nodeRank = thrust::get<2>(bData);
		double oriVelX = thrust::get<3>(bData);
		double oriVelY = thrust::get<4>(bData);

		uint index = cellRank * _maxNodePerCell + nodeRank;
		if (_isActiveAddr[index] == false || nodeRank >= activeMembrCount) {
			return thrust::make_tuple(oriVelX, oriVelY);
		}

		int index_left = nodeRank - 1;
		if (index_left == -1) {
			index_left = activeMembrCount - 1;
		}
		index_left = index_left + cellRank * _maxNodePerCell;
		// apply bend force from left
		if (_isActiveAddr[index_left]) {
			oriVelX = oriVelX + _bendRightXAddr[index_left];
			oriVelY = oriVelY + _bendRightYAddr[index_left];
		}

		int index_right = nodeRank + 1;
		if (index_right == (int) activeMembrCount) {
			index_right = 0;
		}
		index_right = index_right + cellRank * _maxNodePerCell;
		// apply bend force from right
		if (_isActiveAddr[index_right]) {
			oriVelX = oriVelX + _bendLeftXAddr[index_right];
			oriVelY = oriVelY + _bendLeftYAddr[index_right];
		}
		return thrust::make_tuple(oriVelX, oriVelY);
	}
};



struct AddMembrVolDevice: public thrust::unary_function<UUUDDDD, CVec2> {
	uint _maxNodePerCell;
	bool* _isActiveAddr;
	double* _locXAddr;
	double* _locYAddr;

	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ AddMembrVolDevice(uint maxNodePerCell, bool* isActiveAddr,
			double* locXAddr, double* locYAddr) :
			_maxNodePerCell(maxNodePerCell), _isActiveAddr(isActiveAddr), _locXAddr(
					locXAddr), _locYAddr(locYAddr) {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ CVec2 operator()(const UUUDDDD &cData) const {
		uint activeMembrCount = thrust::get<0>(cData);
		uint cellRank = thrust::get<1>(cData);
		uint nodeRank = thrust::get<2>(cData);
		double initCellArea0 = thrust::get<3>(cData);
		double curCellArea = thrust::get<4>(cData);
		double oriVelX = thrust::get<5>(cData);
		double oriVelY = thrust::get<6>(cData);

		double kVol = 0; // volume preservation parameter 

		uint index = cellRank * _maxNodePerCell + nodeRank;
		if (_isActiveAddr[index] == false || nodeRank >= activeMembrCount) {
			return thrust::make_tuple(oriVelX, oriVelY);
		}
		// left node
		int index_left = nodeRank - 1;
		if (index_left == -1) {
			index_left = (int) activeMembrCount - 1;
		}
		index_left = index_left + cellRank * _maxNodePerCell;

		// right node 
		int index_right = nodeRank + 1;
		if (index_right == (int) activeMembrCount) {
			index_right = 0;
		}
		index_right = index_right + cellRank * _maxNodePerCell;
		// 
		if (_isActiveAddr[index_right] && _isActiveAddr[index_left]) { 
			oriVelX = oriVelX - kVol*(curCellArea-initCellArea0)*(_locYAddr[index_right] - _locYAddr[index_left]);
			oriVelY = oriVelY - kVol*(curCellArea-initCellArea0)*(_locXAddr[index_left]-_locXAddr[index_right]);
		}
		return thrust::make_tuple(oriVelX, oriVelY);
	}
};




/*
struct ApplyAdhCell: public thrust::unary_function<BUUIDD, CVec2> {
    double* _nodeLocXArrAddr;
    double* _nodeLocYArrAddr;
    double* _nodeGrowProAddr;
// comment prevents bad formatting issues of __host__ and __device__ in Nsight
    __host__ __device__
    ApplyAdhCell(double* nodeLocXArrAddr, double* nodeLocYArrAddr, double* nodeGrowProAddr) :
            _nodeLocXArrAddr(nodeLocXArrAddr), _nodeLocYArrAddr(nodeLocYArrAddr), _nodeGrowProAddr(nodeGrowProAddr) {
    }
    __device__
    CVec2 operator()(const BUUIDD& adhInput) const {
        bool isActive = thrust::get<0>(adhInput);
        uint nodeIndx = thrust::get<1>(adhInput);
		uint curActLevel = thrust::get<2>(adhInput);
		int adhIndx = thrust::get<3>(adhInput);
        double oriVelX = thrust::get<4>(adhInput);
        double oriVelY = thrust::get<5>(adhInput);
        double growProg = _nodeGrowProAddr[nodeIndx];
        double growProgNeigh = _nodeGrowProAddr[adhIndx];
        //bool adhSkipped = false;  
        double alpha = getMitoticAdhCoefCell(growProg, growProgNeigh);//to adjust the mitotic values of stiffness


        if (adhIndx == -1 || !isActive) {
            return thrust::make_tuple(oriVelX, oriVelY);
        } else {
            double locX = _nodeLocXArrAddr[nodeIndx];
            double locY = _nodeLocYArrAddr[nodeIndx];
            double adhLocX = _nodeLocXArrAddr[adhIndx];
            double adhLocY = _nodeLocYArrAddr[adhIndx];
            handleAdhesionForceCell_M(adhIndx, locX, locY, adhLocX, adhLocY,
                    oriVelX, oriVelY, alpha,curActLevel);
            return thrust::make_tuple(oriVelX, oriVelY);
        }
    }
};
*/










//Ali modidfied for cell pressure calculation
struct applySceCellDisc_MDevice: public thrust::unary_function<CellData, CVec4> {
	uint _maxNodePerCell;
	uint _maxMemNodePerCell;
	double* _locXAddr;
	double* _locYAddr;
	bool* _isActiveAddr;
	double _grthPrgrCriVal_M;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ applySceCellDisc_MDevice(uint maxNodePerCell,
			uint maxMemNodePerCell, double* locXAddr, double* locYAddr,
			bool* isActiveAddr, double grthPrgrCriVal_M) :
			_maxNodePerCell(maxNodePerCell), _maxMemNodePerCell(
					maxMemNodePerCell), _locXAddr(locXAddr), _locYAddr(
					locYAddr), _isActiveAddr(isActiveAddr), _grthPrgrCriVal_M(
					grthPrgrCriVal_M) {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ CVec4 operator()(const CellData &cData) const {
		uint activeMembrCount = thrust::get<0>(cData);
		uint activeIntnlCount = thrust::get<1>(cData);
		uint cellRank = thrust::get<2>(cData);
		uint nodeRank = thrust::get<3>(cData);
		double progress = thrust::get<4>(cData);
		double oriVelX = thrust::get<5>(cData);
		double oriVelY = thrust::get<6>(cData);
		uint index = cellRank * _maxNodePerCell + nodeRank;
                
		double F_MI_M_x=0 ; //AliA
		double F_MI_M_y=0 ; //AliA

		if (_isActiveAddr[index] == false) {
			return thrust::make_tuple(oriVelX, oriVelY,F_MI_M_x,F_MI_M_y); //AliE
		}
		uint intnlIndxMemBegin = cellRank * _maxNodePerCell;
		uint intnlIndxBegin = cellRank * _maxNodePerCell + _maxMemNodePerCell;
		uint intnlIndxEnd = intnlIndxBegin + activeIntnlCount;
		uint index_other;
		double nodeX = _locXAddr[index];
		double nodeY = _locYAddr[index];
		double nodeXOther, nodeYOther;
		// means membrane node // membrane node first
		//Because we want to compute the force on the membrane nodes we modify this function 
		if (nodeRank < _maxMemNodePerCell) {
			for (index_other = intnlIndxBegin; index_other < intnlIndxEnd;
					index_other++) {
				nodeXOther = _locXAddr[index_other];
				nodeYOther = _locYAddr[index_other]; 
                                /* Ali comment
				calAndAddIB_M(nodeX, nodeY, nodeXOther, nodeYOther, progress,
						oriVelX, oriVelY, _grthPrgrCriVal_M);
                                 */ 
				calAndAddIB_M2(nodeX, nodeY, nodeXOther, nodeYOther, progress,
						oriVelX, oriVelY,F_MI_M_x,F_MI_M_y, _grthPrgrCriVal_M); // membrane internal interactions

			}
		} else {
			// means internal node
			for (index_other = intnlIndxMemBegin; // interaction between cur internal node and membrane node
					index_other < intnlIndxMemBegin + activeMembrCount;
					index_other++) {
				nodeXOther = _locXAddr[index_other];
				nodeYOther = _locYAddr[index_other];
				calAndAddIB_M(nodeX, nodeY, nodeXOther, nodeYOther, progress, // internal-membrane nodes
						oriVelX, oriVelY, _grthPrgrCriVal_M);
			}
			for (index_other = intnlIndxBegin; index_other < intnlIndxEnd; // interaction between cur internal node and other internal node
					index_other++) {
				if (index_other == index) {
					continue;
				}
				nodeXOther = _locXAddr[index_other];
				nodeYOther = _locYAddr[index_other];
				calAndAddII_M(nodeX, nodeY, nodeXOther, nodeYOther, progress, // internal-internal interactions
						oriVelX, oriVelY, _grthPrgrCriVal_M);
			}
		}
		return thrust::make_tuple(oriVelX, oriVelY,F_MI_M_x,F_MI_M_y);
	}
};







struct addSceCellMyosinForce: public thrust::unary_function<UUUUDDDDD, CVec2> {
	uint _maxNodePerCell;
	uint _maxMemNodePerCell;
	double* _locXAddr;
	double* _locYAddr;
	bool* _isActiveAddr;
	double* _myosinLevelAddr;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ addSceCellMyosinForce(uint maxNodePerCell,
			uint maxMemNodePerCell, double* locXAddr, double* locYAddr,
			bool* isActiveAddr, double* myosinLevelAddr) :
			_maxNodePerCell(maxNodePerCell), _maxMemNodePerCell(
					maxMemNodePerCell), _locXAddr(locXAddr), _locYAddr(
					locYAddr), _isActiveAddr(isActiveAddr), _myosinLevelAddr(
					myosinLevelAddr) {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ CVec2 operator()(const UUUUDDDDD &cData) const {
		uint activeMembrCount = thrust::get<0>(cData);
		uint activeIntnlCount = thrust::get<1>(cData);
		uint cellRank = thrust::get<2>(cData);
		uint nodeRank = thrust::get<3>(cData);
		double  myosinLevel= thrust::get<4>(cData); // progress
		double oriVelX = thrust::get<5>(cData);
		double oriVelY = thrust::get<6>(cData);
		double cellCenterX = thrust::get<7>(cData);
		double cellCenterY = thrust::get<8>(cData);
		uint index = cellRank * _maxNodePerCell + nodeRank;
                
		// double totalMyosin = 0.0;
		double distNodes = 0.0;
		

		if (_isActiveAddr[index] == false) {
			return thrust::make_tuple(oriVelX, oriVelY); //AliE
		}
		


		// uint intnlIndxMemBegin = cellRank * _maxNodePerCell;
		uint intnlIndxBegin = cellRank * _maxNodePerCell + _maxMemNodePerCell;
		uint intnlIndxEnd = intnlIndxBegin + activeIntnlCount;
		uint index_other;
		double nodeX = _locXAddr[index];
		double nodeY = _locYAddr[index];
		double nodeXOther, nodeYOther;
		// double myosinOther;

		int index_left;
		int index_right;
		double leftPosX;
		double leftPosY;
		double rightPosX;
		double rightPosY;
		double lenRightLeft;

		double shortestDistantce = 100;
		double normX;
		double normY;
		double outwardChecker;
		// means membrane node
		//Because we want to compute the force on the membrane nodes we modify this function 
		if (nodeRank < _maxMemNodePerCell) {
			index_left = nodeRank - 1;
			if (index_left == -1) {
				index_left = activeMembrCount - 1;
			}
			index_left = index_left + cellRank * _maxNodePerCell;

			index_right = nodeRank + 1;
			if (index_right == (int) activeMembrCount) {
				index_right = 0;
			}
			index_right = index_right + cellRank * _maxNodePerCell;


			leftPosX = _locXAddr[index_left];
			leftPosY = _locYAddr[index_left];
			rightPosX = _locXAddr[index_right];
			rightPosY = _locYAddr[index_right];
			lenRightLeft = sqrt((rightPosX-leftPosX)*(rightPosX-leftPosX)+(rightPosY-leftPosY)*(rightPosY-leftPosY));
			for (index_other = intnlIndxBegin; index_other < intnlIndxEnd;
					index_other++) {
				nodeXOther = _locXAddr[index_other];
				nodeYOther = _locYAddr[index_other]; 
				distNodes = compDist2D(nodeX, nodeY, nodeXOther, nodeYOther);
				if (distNodes < shortestDistantce) { // JG040623
					// totalMyosin = _myosinLevelAddr[index_other];
					shortestDistantce = distNodes;
					}
				// myosinOther = _myosinLevelAddr[index_other];
				// totalMyosin += exp(-pow(distNodes,2.0)) * myosinOther;
			}
			// oriVelX += -2.0/3.0*(tanh((1.0 - totalMyosin)/0.1)+0.5) * (rightPosY-leftPosY)/lenRightLeft;
			// oriVelY += +2.0/3.0*(tanh((1.0 - totalMyosin)/0.1)+0.5) * (rightPosX-leftPosX)/lenRightLeft; // opposite sign, Apr 4

			// new tanh function,
			// oriVelX += -(tanh((1.0-totalMyosin)/2.0)+0.1)/(tanh(1.0/2.0)+0.1) * (rightPosY-leftPosY)/lenRightLeft;
			// oriVelY += +(tanh((1.0-totalMyosin)/2.0)+0.1)/(tanh(1.0/2.0)+0.1) * (rightPosX-leftPosX)/lenRightLeft; // opposite sign, Apr 4

			// new function 
			normX = -(rightPosY-leftPosY)/lenRightLeft;
			normY =  (rightPosX-leftPosX)/lenRightLeft;
			outwardChecker = (nodeX-cellCenterX)*normX + (nodeY-cellCenterY)*normY;
			if (outwardChecker<0){
				normX = -normX;
				normY = -normY;
			}
			oriVelX += 0; //0.25*(-2.0*tanh(((totalMyosin-8.0)/(0.1)))-2.0 * tanh(((totalMyosin-12.0)/(0.1)))) * normX;
			oriVelY += 0; //0.25*(-2.0*tanh(((totalMyosin-8.0)/(0.1)))-2.0 * tanh(((totalMyosin-12.0)/(0.1)))) * normY; // JG040723
		}
		return thrust::make_tuple(oriVelX, oriVelY); 
	}
};

// end of Mar 31









// aug 21, 2023
struct addSceCellSigForce: public thrust::unary_function<UUUUDDDDDi, CVec2> {
	uint _maxNodePerCell;
	uint _maxMemNodePerCell;
	double* _locXAddr;
	double* _locYAddr;
	bool* _isActiveAddr;
	SigptStateV2* _sigPtAddr;
	double _timeNow;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ addSceCellSigForce(uint maxNodePerCell,
			uint maxMemNodePerCell, double* locXAddr, double* locYAddr,
			bool* isActiveAddr, SigptStateV2* sigPtAddr, double timeNow) :
			_maxNodePerCell(maxNodePerCell), _maxMemNodePerCell(
					maxMemNodePerCell), _locXAddr(locXAddr), _locYAddr(
					locYAddr), _isActiveAddr(isActiveAddr), _sigPtAddr(
					sigPtAddr),_timeNow(timeNow) {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ CVec2 operator()(const UUUUDDDDDi &cDatai) const {
		uint activeMembrCount = thrust::get<0>(cDatai);
		uint activeIntnlCount = thrust::get<1>(cDatai);
		uint cellRank = thrust::get<2>(cDatai);
		uint nodeRank = thrust::get<3>(cDatai);
		double  myosinLevel= thrust::get<4>(cDatai); // progress
		double oriVelX = thrust::get<5>(cDatai);
		double oriVelY = thrust::get<6>(cDatai);
		double cellCenterX = thrust::get<7>(cDatai);
		double cellCenterY = thrust::get<8>(cDatai);
		int cellType = thrust::get<9>(cDatai);
		uint index = cellRank * _maxNodePerCell + nodeRank;
                
		
		if (_isActiveAddr[index] == false) {
			return thrust::make_tuple(oriVelX, oriVelY); //AliE
		}
		double nodeX = _locXAddr[index];
		double nodeY = _locYAddr[index];
		double sigCharForce = 0.03;
		double sigCharDist = 5.0;
		double sigCharAge = 100;

		if (cellType!=1 && nodeRank < _maxMemNodePerCell){
			uint sigPtNum = 30;
			for (uint i=0; i<sigPtNum; i++){
				if (_sigPtAddr[i].isActive==true && _sigPtAddr[i].folRank!=cellRank){
					double nodeXLeader = _locXAddr[_sigPtAddr[i].memIndex];
					double nodeYLeader = _locYAddr[_sigPtAddr[i].memIndex];
					double vecX = nodeXLeader - nodeX;
					double vecY = nodeYLeader - nodeY;
					double distSN = sqrt(vecX*vecX + vecY*vecY);
					
					if (distSN>0){
						vecX = vecX/distSN;
						vecY = vecY/distSN;
						oriVelX += sigCharForce*exp(-distSN/sigCharDist)*exp(-(_timeNow - _sigPtAddr[i].timeInit)/sigCharAge)*vecX; 
						oriVelY += sigCharForce*exp(-distSN/sigCharDist)*exp(-(_timeNow - _sigPtAddr[i].timeInit)/sigCharAge)*vecY; 
					}
				}
			}
		}
		return thrust::make_tuple(oriVelX, oriVelY); 
	}
};







struct updateMinToMDist: public thrust::unary_function<UUUUIDDD, CVec2> {
	uint _maxNodePerCell;
	uint _maxMemNodePerCell;
	double* _locXAddr;
	double* _locYAddr;
	bool* _isActiveAddr;
	int* _nodeAdhereIndexAddr;

__host__ __device__ updateMinToMDist(uint maxNodePerCell,
			uint maxMemNodePerCell, double* locXAddr, double* locYAddr,
			bool* isActiveAddr, int* nodeAdhereIndexAddr ) :
			_maxNodePerCell(maxNodePerCell), _maxMemNodePerCell(
					maxMemNodePerCell), _locXAddr(locXAddr), _locYAddr(
					locYAddr), _isActiveAddr(isActiveAddr),
					_nodeAdhereIndexAddr(nodeAdhereIndexAddr) {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ CVec2 operator()(const UUUUIDDD &cData) const {
		uint activeMembrCount = thrust::get<0>(cData);
		uint activeIntnlCount = thrust::get<1>(cData);
		uint cellRank = thrust::get<2>(cData);
		uint nodeRank = thrust::get<3>(cData);
		int cell_Type = thrust::get<4>(cData);
		double cellCenterX = thrust::get<5>(cData);
		double cellCenterY = thrust::get<6>(cData);
		double minToMDist = thrust::get<7>(cData);
		uint index = cellRank * _maxNodePerCell + nodeRank;
		uint nodeMembrIndex;
		double tempMinDist = 100.0;
		double nodeX = _locXAddr[index];
		double nodeY = _locYAddr[index];
		double nodeMembrX, nodeMembrY;
		double tempNodeX, tempNodeY;
		double nodeMemVecX, nodeMemVecY, lenNodeMem;
		double memCenterX, memCenterY, lenMemCenter;
		double projLen;
		double distNodes=200.0;
		minToMDist = 100.0; 

		if (cell_Type==1){ // means leader
			if (nodeRank>=_maxMemNodePerCell){ // means an internal node, note the greater than or equal to // did not check if the internal node is active or not
				for (uint nodeMembrRank=0; nodeMembrRank<_maxMemNodePerCell; nodeMembrRank++){
					nodeMembrIndex = cellRank * _maxNodePerCell + nodeMembrRank; // absolute index
					if (_isActiveAddr[nodeMembrIndex]){ //090324
						if (_nodeAdhereIndexAddr[nodeMembrIndex]>-1){
							nodeMembrX = _locXAddr[nodeMembrIndex];
							nodeMembrY = _locYAddr[nodeMembrIndex];
							distNodes = compDist2D(nodeX,nodeY,nodeMembrX,nodeMembrY);
							if (distNodes<tempMinDist){
							tempMinDist = distNodes;
							tempNodeX = nodeMembrX;
							tempNodeY = nodeMembrY; 
							}
						}
					}
				}
				// nodeMemVecX = tempNodeX - nodeX;
				// nodeMemVecY = tempNodeY - nodeY;
				memCenterX = tempNodeX - cellCenterX; // depending on choice of cell center !!!
				memCenterY = tempNodeY - cellCenterY;
				// lenNodeMem = sqrt(nodeMemVecX*nodeMemVecX + nodeMemVecY*nodeMemVecY);
				lenMemCenter = sqrt(memCenterX * memCenterX + memCenterY * memCenterY);
				// projLen = (nodeMemVecX*memCenterX + nodeMemVecY*memCenterY)/lenMemCenter; // projection along center-membrane connection
				minToMDist = tempMinDist; // if no contact with followers, minToDist is set to be 100.
				// minToMDistRaw = tempMinDist;
			}
		}
		return thrust::make_tuple(minToMDist, lenMemCenter); 
	}
};



/*
struct calFluxWeightsMyosinDevice: public thrust::unary_function<UUUUIDD, double> {
	uint _maxNodePerCell;
	uint _maxMemNodePerCell;
	uint _maxIntnlNodePerCell;
	double* _locXAddr;
	double* _locYAddr;
	bool* _isActiveAddr;
	int* _nodeAdhereIndexAddr;
	double* _myosinLevelAddr;
	double* _minToMDistAddr;
	double* _fluxWeightsAddr;


__host__ __device__ calFluxWeightsMyosinDevice(uint maxNodePerCell,
			uint maxMemNodePerCell, uint maxIntnlNodePerCell, double* locXAddr, double* locYAddr,
			bool* isActiveAddr, double* myosinLevelAddr, int* nodeAdhereIndexAddr, double* minToMDistAddr, double* fluxWeightsAddr) :
			_maxNodePerCell(maxNodePerCell), _maxMemNodePerCell(maxMemNodePerCell), _maxIntnlNodePerCell(maxIntnlNodePerCell),
			_locXAddr(locXAddr), _locYAddr(locYAddr), _isActiveAddr(isActiveAddr), _myosinLevelAddr(myosinLevelAddr),
			_nodeAdhereIndexAddr(nodeAdhereIndexAddr), _minToMDistAddr(minToMDistAddr), _fluxWeightsAddr(fluxWeightsAddr) {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ double operator()(const UUUUIDD &cData) const {
		uint activeMembrCount = thrust::get<0>(cData);
		uint activeIntnlCount = thrust::get<1>(cData);
		uint cellRank = thrust::get<2>(cData);
		uint nodeRank = thrust::get<3>(cData); // relative rank 
		int cell_Type = thrust::get<4>(cData);
		double minToMDist = thrust::get<5>(cData);
		double cenToMDist = thrust::get<6>(cData);

		uint index = cellRank * _maxNodePerCell + nodeRank; // absolute index
		uint nodeOtherIntlIndex;
		double nodeX = _locXAddr[index];
		double nodeY = _locYAddr[index];
		double nodeOtherX, nodeOtherY;
		double distNodes;
		double sumFlux = 0;
		double myosinMaxLevel=5;
		uint fluxIndex; // index for the fluxWeights matrix
		double dist_0=2;
		
		if (cell_Type==1){ // means leader
			if (nodeRank>=_maxMemNodePerCell && _isActiveAddr[index]==true){ // means internal node 
				// loop over other internal nodes in the leader 
				for (uint nodeOtherIntlRank=_maxMemNodePerCell; nodeOtherIntlRank<_maxMemNodePerCell+activeIntnlCount; nodeOtherIntlRank++) {
					nodeOtherIntlIndex = cellRank * _maxNodePerCell + nodeOtherIntlRank;
					if (nodeRank!=nodeOtherIntlRank) {
						nodeOtherX = _locXAddr[nodeOtherIntlIndex];
						nodeOtherY = _locYAddr[nodeOtherIntlIndex];
						distNodes = compDist2D(nodeX,nodeY,nodeOtherX,nodeOtherY);
						fluxIndex = (nodeRank-_maxMemNodePerCell)*_maxIntnlNodePerCell+(nodeOtherIntlRank-_maxMemNodePerCell); // nodeRank row, nodeOtherIntlRank column
						// if current node is farther compared with other node, and the myosin level of other node has not reached max yet
						if ((minToMDist>_minToMDistAddr[nodeOtherIntlIndex])  && cenToMDist>minToMDist  && (_myosinLevelAddr[nodeOtherIntlIndex]<myosinMaxLevel)){  // || minToMDist<0)
							_fluxWeightsAddr[fluxIndex] = exp(-distNodes/dist_0)/(fabs(minToMDist)+1); // flux from nodeIntlIndex1 to nodeOtherIntlRank
							sumFlux = sumFlux + _fluxWeightsAddr[fluxIndex]; // sum up flux weights in the nodeRank row
						} else {
							_fluxWeightsAddr[fluxIndex] = 0;
						}
					} else {
						fluxIndex = (nodeRank-_maxMemNodePerCell)*(_maxIntnlNodePerCell +1);
						_fluxWeightsAddr[fluxIndex] = 0;
					}
				}
				for (uint nodeOtherIntlRank=_maxMemNodePerCell; nodeOtherIntlRank<_maxMemNodePerCell+activeIntnlCount; nodeOtherIntlRank++) {
					fluxIndex = (nodeRank-_maxMemNodePerCell)*_maxIntnlNodePerCell+(nodeOtherIntlRank-_maxMemNodePerCell);
					if (bigEnough(sumFlux)){
						_fluxWeightsAddr[fluxIndex] = _fluxWeightsAddr[fluxIndex]/sumFlux;
					} else {
						_fluxWeightsAddr[fluxIndex] = 0;
					}
				}

			}
		}
		return minToMDist;
	}
};
*/


// orginally updateFluxWeightsVec
struct calFluxWeightsMyosinDevice: public thrust::unary_function<UUUUIDD, double> {
	uint _maxNodePerCell;
	uint _maxMemNodePerCell;
	uint _maxIntnlNodePerCell;
	double* _locXAddr;
	double* _locYAddr;
	bool* _isActiveAddr;
	int* _nodeAdhereIndexAddr;
	double* _myosinLevelAddr;
	double* _minToMDistAddr;
	double* _fluxWeightsAddr;


__host__ __device__ calFluxWeightsMyosinDevice(uint maxNodePerCell,
			uint maxMemNodePerCell, uint maxIntnlNodePerCell, double* locXAddr, double* locYAddr,
			bool* isActiveAddr, double* myosinLevelAddr, int* nodeAdhereIndexAddr, double* minToMDistAddr, double* fluxWeightsAddr) :
			_maxNodePerCell(maxNodePerCell), _maxMemNodePerCell(maxMemNodePerCell), _maxIntnlNodePerCell(maxIntnlNodePerCell),
			_locXAddr(locXAddr), _locYAddr(locYAddr), _isActiveAddr(isActiveAddr), _myosinLevelAddr(myosinLevelAddr),
			_nodeAdhereIndexAddr(nodeAdhereIndexAddr), _minToMDistAddr(minToMDistAddr), _fluxWeightsAddr(fluxWeightsAddr) {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ double operator()(const UUUUIDD &cData) const {
		uint activeMembrCount = thrust::get<0>(cData);
		uint activeIntnlCount = thrust::get<1>(cData);
		uint cellRank = thrust::get<2>(cData);
		uint nodeRank = thrust::get<3>(cData); // relative rank 
		int cell_Type = thrust::get<4>(cData);
		double minToMDist = thrust::get<5>(cData);
		double cenToMDist = thrust::get<6>(cData);

		uint index = cellRank * _maxNodePerCell + nodeRank; // absolute index
		uint nodeOtherIntlIndex;
		double nodeX = _locXAddr[index];
		double nodeY = _locYAddr[index];
		double nodeOtherX, nodeOtherY;
		double distNodes;
		double distThrd = 0.5;
		double distThrd2 = 0.5;
		double sumFlux = 0;
		double myosinMaxLevel=5;
		uint fluxIndex; // index for the fluxWeights matrix
		double dist_0=2;
		
		if (cell_Type==1){ // means leader
			if (nodeRank>=_maxMemNodePerCell && _isActiveAddr[index]==true){ // means internal node 
				// loop over other internal nodes in the leader 
				for (uint nodeOtherIntlRank=_maxMemNodePerCell; nodeOtherIntlRank<_maxMemNodePerCell+_maxIntnlNodePerCell; nodeOtherIntlRank++) {
					nodeOtherIntlIndex = cellRank * _maxNodePerCell + nodeOtherIntlRank;
					if (_isActiveAddr[nodeOtherIntlIndex]==true){
						if (nodeRank!=nodeOtherIntlRank) {
							nodeOtherX = _locXAddr[nodeOtherIntlIndex];
							nodeOtherY = _locYAddr[nodeOtherIntlIndex];
							distNodes = compDist2D(nodeX,nodeY,nodeOtherX,nodeOtherY); // flux from current node to the other node
							fluxIndex = (nodeRank-_maxMemNodePerCell)*_maxIntnlNodePerCell+(nodeOtherIntlRank-_maxMemNodePerCell); // nodeRank row, nodeOtherIntlRank column
							// if current node is farther compared with other node, and the myosin level of other node has not reached max yet
							if (distNodes<distThrd && (minToMDist>_minToMDistAddr[nodeOtherIntlIndex]) &&  _minToMDistAddr[nodeOtherIntlIndex]<distThrd2 && _myosinLevelAddr[nodeOtherIntlIndex]<myosinMaxLevel && _myosinLevelAddr[index]>0.0000001){ // && _myosinLevelAddr[nodeOtherIntlIndex]<myosinMaxLevel // minToMDist<distThrd2 &&
								_fluxWeightsAddr[fluxIndex] = 1;// omega0 // flux from nodeIntlIndex1 to nodeOtherIntlRank //value controls flux rate
								// sumFlux = sumFlux + _fluxWeightsAddr[fluxIndex]; // sum up flux weights in the nodeRank row
							} else {
								_fluxWeightsAddr[fluxIndex] = 0;
							}
						} else { 
							fluxIndex = (nodeRank-_maxMemNodePerCell)*(_maxIntnlNodePerCell +1);
							_fluxWeightsAddr[fluxIndex] = 0;
						}
					} else {
						fluxIndex = (nodeRank-_maxMemNodePerCell)*_maxIntnlNodePerCell+(nodeOtherIntlRank-_maxMemNodePerCell);
						_fluxWeightsAddr[fluxIndex] = 0;
					}
				}
				/*
				for (uint nodeOtherIntlRank=_maxMemNodePerCell; nodeOtherIntlRank<_maxMemNodePerCell+activeIntnlCount; nodeOtherIntlRank++) {
					fluxIndex = (nodeRank-_maxMemNodePerCell)*_maxIntnlNodePerCell+(nodeOtherIntlRank-_maxMemNodePerCell);
					if (bigEnough(sumFlux)){
						// _fluxWeightsAddr[fluxIndex] = _fluxWeightsAddr[fluxIndex];///sumFlux;
					} else {
						// _fluxWeightsAddr[fluxIndex] = _fluxWeightsAddr[fluxIndex];
					}
				}
				*/
			}
		}
		return minToMDist;
	}
};








/*
// update the myosin level based on diffusion of myosin 
// old name: updateCellMyosin
struct calSceCellMyosinDevice: public thrust::unary_function<UUDDUUDDDi, double> {
	uint _maxNodePerCell;
	uint _maxMemNodePerCell;
	uint _maxIntnlNodePerCell;
	double* _locXAddr;
	double* _locYAddr;
	bool* _isActiveAddr;
	double* _myosinLevelAddr;
	double _myosinDiffusionThreshold;
	int* _nodeAdhereIndexAddr;
	double* _fluxWeightsAddr;
	double _timeStep;
	double _timeNow;
	// double _grthPrgrCriVal_M;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ calSceCellMyosinDevice(uint maxNodePerCell,
			uint maxMemNodePerCell, uint maxIntnlNodePerCell, double* locXAddr, double* locYAddr,
			bool* isActiveAddr, double* myosinLevelAddr, double myosinDiffusionThreshold, int* nodeAdhereIndexAddr, 
			double* fluxWeightsAddr, double timeStep, double timeNow) :
			_maxNodePerCell(maxNodePerCell), _maxMemNodePerCell(maxMemNodePerCell), _maxIntnlNodePerCell(maxIntnlNodePerCell), _locXAddr(locXAddr), _locYAddr(
					locYAddr), _isActiveAddr(isActiveAddr), _myosinLevelAddr(myosinLevelAddr), _myosinDiffusionThreshold(myosinDiffusionThreshold),
					_nodeAdhereIndexAddr(nodeAdhereIndexAddr), _fluxWeightsAddr(fluxWeightsAddr), _timeStep(timeStep), _timeNow(timeNow) {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ double operator()(const UUDDUUDDDi &cData) const {
		uint activeMembrCount = thrust::get<0>(cData);
		uint activeIntnlCount = thrust::get<1>(cData);
		double Cell_CenterX = thrust::get<2>(cData);
        double Cell_CenterY = thrust::get<3>(cData);
		uint cellRank = thrust::get<4>(cData);
		uint nodeRank = thrust::get<5>(cData);
		double nodeMyosin = thrust::get<6>(cData);
		double cellAngle = thrust::get<7>(cData);
		double cellRadius = thrust::get<8>(cData);
		int cell_Type = thrust::get<9>(cData);
		uint index = cellRank * _maxNodePerCell + nodeRank;

		if (_isActiveAddr[index] == false) {
			// if there is a bug in the number of activeinternalnode ?
			// nodeMyosin = 10;
			return nodeMyosin; 
		}
		uint intnlIndxMemBegin = cellRank * _maxNodePerCell;
		uint intnlIndxBegin = cellRank * _maxNodePerCell + _maxMemNodePerCell;
		uint intnlIndxEnd = intnlIndxBegin + activeIntnlCount;
		// uint index_other;
		double nodeX = _locXAddr[index];
		double nodeY = _locYAddr[index];
		// double nodeXOther, nodeYOther;
		// double distNodes = 0.0;
		// double myosinOther;
		// double myosinDiffer;
		// double kDiff = 0.0;
		double pX = cos(cellAngle); 
		double pY = sin(cellAngle);
		// double baseMyosin=1;
		// if (cell_Type == 0){baseMyosin=1.0;} // follower 
		double myosinTarget = 0.0; // baseMyosin-((nodeX-Cell_CenterX)*pX + (nodeY-Cell_CenterY)*pY);
		double kmyo = 0.001; // rate of myosin approaching its target value
		double kdeg = 0.001; // was 0.02

		double nodeXTemp, nodeYTemp;
		double distNodes;
		double distThreshold = 1.5;
		double distThreshold2 = 4;
		double countMemNeighbor = 0;
		double minDist = 100;
		double distNodeCenter=0;
		double distThresholdM = 1;
		double minDistIntl = 10;
		double tempdistMI = 0;
		double tempNodeMyosin = 0;
		double myosinThrd = 0.29;
		uint count = 0;
		// if (_timeNow > 55800.0) {kDiff = 0.0;} 
		// means membrane node
		//Because we want to compute the force on the membrane nodes we modify this function 
		if (nodeRank < _maxMemNodePerCell) {
			// means membrane node 
			if (cell_Type == 1 ){ // means leader 
			
			/*
			for (uint tempNodeRank=intnlIndxBegin; tempNodeRank<intnlIndxEnd;tempNodeRank++){ // internal nodes 
				nodeXTemp = _locXAddr[tempNodeRank];
				nodeYTemp = _locYAddr[tempNodeRank];
				tempdistMI = compDist2D(nodeX,nodeY,nodeXTemp,nodeYTemp);
				if (tempdistMI<distThresholdM) {tempNodeMyosin += _myosinLevelAddr[tempNodeRank]; count +=1;}
			}
			if(count>0) {
				nodeMyosin = tempNodeMyosin/(count*1.0);
			} else{
				nodeMyosin = 0;
			}
			??? **
			* 
				for (uint tempNodeRank=intnlIndxBegin; tempNodeRank<intnlIndxEnd;tempNodeRank++){ // internal nodes 
				nodeXTemp = _locXAddr[tempNodeRank];
				nodeYTemp = _locYAddr[tempNodeRank];
				tempdistMI = compDist2D(nodeX,nodeY,nodeXTemp,nodeYTemp);
				if (tempdistMI<minDistIntl) {
					nodeMyosin = _myosinLevelAddr[tempNodeRank];
					minDistIntl = tempdistMI;}
				}
				if (nodeMyosin<myosinThrd){nodeMyosin=0;}
			} else { // membrane of follower cells
				nodeMyosin = 0;
			}
			
			return nodeMyosin;
		} else {
			// means internal node
			if (cell_Type == 1 ){ // means leader 
				/*
				myosinTarget = 1.0;
				double nearestAdhX, nearestAdhY, minDistAllM;
				for (uint tempNodeRank=intnlIndxMemBegin; tempNodeRank<intnlIndxMemBegin+activeMembrCount;tempNodeRank++){
					nodeXTemp = _locXAddr[tempNodeRank];
					nodeYTemp = _locYAddr[tempNodeRank];
					distNodes = compDist2D(nodeX,nodeY,nodeXTemp,nodeYTemp);
					if (_nodeAdhereIndexAddr[tempNodeRank] > -1) {
						// nodeXTemp = _locXAddr[tempNodeRank];
						// nodeYTemp = _locYAddr[tempNodeRank];
						if (distNodes<minDist) {minDist = distNodes; nearestAdhX = nodeXTemp; nearestAdhY=nodeYTemp;}
						// if (distNodes < distThreshold) {countMemNeighbor = countMemNeighbor + 1; }
					}

					if (distNodes<minDistAllM) {minDistAllM = distNodes;}
				}
				distNodeCenter = compDist2D(nodeX,nodeY,Cell_CenterX,Cell_CenterY);
				// double adhCenterVecX = Cell_CenterX - nearestAdhX;
				// double adhCenterVecY = Cell_CenterY - nearestAdhY;
				double crossP = (nearestAdhX-Cell_CenterX)*(nodeY-Cell_CenterY) - (nearestAdhY-Cell_CenterY)*(nodeX-Cell_CenterX);
				if (minDist<distThreshold) {myosinTarget = 5;}
				else if (minDist<distThreshold2&&minDistAllM<distThreshold && crossP>0) {myosinTarget = 0.01;} // change 2 to half radius
				else {myosinTarget = 1;}
				// myosinTarget = myosinTarget * pow(2,countMemNeighbor);
				// if (myosinTarget > 15) {myosinTarget = 15;}
            
			*/

			 //pow(2,countMemNeighbor);
			/*
			for (index_other = intnlIndxBegin; index_other < intnlIndxEnd; // interaction between cur internal node and other internal node
					index_other++) {
				if (index_other == index) {
					continue;
				}
				// nodeXOther = _locXAddr[index_other];
				// nodeYOther = _locYAddr[index_other];
				// myosinOther = _myosinLevelAddr[index_other];
				// distNodes = compDist2D(nodeX,nodeY,nodeXOther,nodeYOther);
				// myosinDiffer = kDiff * max((1.0-distNodes/_myosinDiffusionThreshold),0.0) * (myosinOther-nodeMyosin);
				// nodeMyosin = nodeMyosin + myosinDiffer * _timeStep;
				
			}
			??? **
			*?
			
			double deltaMyosin = 0.0;
			uint fluxIndex,otherMyosinIndex;
			for (uint tempNodeRank=0; tempNodeRank<_maxIntnlNodePerCell;tempNodeRank++){ // internal nodes of the current cell, index for the flux matrix
				fluxIndex = (nodeRank-_maxMemNodePerCell)*_maxIntnlNodePerCell+tempNodeRank; // nodeRank-_maxMembrNode row, tempNodeRank column
				deltaMyosin = deltaMyosin - nodeMyosin*_fluxWeightsAddr[fluxIndex];
			}

			for (uint tempNodeRank=0; tempNodeRank<_maxIntnlNodePerCell;tempNodeRank++){ // internal nodes of the current cell 
				fluxIndex = tempNodeRank*_maxIntnlNodePerCell+(nodeRank-_maxMemNodePerCell);
				otherMyosinIndex = cellRank * _maxNodePerCell + _maxMemNodePerCell + tempNodeRank;
				deltaMyosin = deltaMyosin + _myosinLevelAddr[otherMyosinIndex]*_fluxWeightsAddr[fluxIndex];
			}
			
			//if (index>=intnlIndxBegin && index < intnlIndxEnd){ // seems not neccessary, as the if condition gurantees it is an internal node 
				// nodeMyosin = nodeMyosin + _timeStep * (kmyo* (myosinTarget - nodeMyosin)- kdeg * nodeMyosin); // make sure it is positive
			//	fluxIndex = 1*_maxIntnlNodePerCell+(nodeRank-_maxMemNodePerCell); // row 2, different columns
			// if (_isActiveAddr[index]) // seems no need as it the inactive nodes has return value already 
			nodeMyosin = nodeMyosin + _timeStep * kmyo* deltaMyosin; // _fluxWeightsAddr[fluxIndex]; //
			//}
			}
			return nodeMyosin;
		}
	}
};

*/



struct calSceCellMyosinDevice: public thrust::unary_function<UUDDUUDDDi, double> {
	uint _maxNodePerCell;
	uint _maxMemNodePerCell;
	uint _maxIntnlNodePerCell;
	double* _locXAddr;
	double* _locYAddr;
	bool* _isActiveAddr;
	double* _myosinLevelAddr;
	double _myosinDiffusionThreshold;
	int* _nodeAdhereIndexAddr;
	double* _fluxWeightsAddr;
	double _timeStep;
	double _timeNow;
	// double _grthPrgrCriVal_M;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ calSceCellMyosinDevice(uint maxNodePerCell,
			uint maxMemNodePerCell, uint maxIntnlNodePerCell, double* locXAddr, double* locYAddr,
			bool* isActiveAddr, double* myosinLevelAddr, double myosinDiffusionThreshold, int* nodeAdhereIndexAddr, 
			double* fluxWeightsAddr, double timeStep, double timeNow) :
			_maxNodePerCell(maxNodePerCell), _maxMemNodePerCell(maxMemNodePerCell), _maxIntnlNodePerCell(maxIntnlNodePerCell), _locXAddr(locXAddr), _locYAddr(
					locYAddr), _isActiveAddr(isActiveAddr), _myosinLevelAddr(myosinLevelAddr), _myosinDiffusionThreshold(myosinDiffusionThreshold),
					_nodeAdhereIndexAddr(nodeAdhereIndexAddr), _fluxWeightsAddr(fluxWeightsAddr), _timeStep(timeStep), _timeNow(timeNow) {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ double operator()(const UUDDUUDDDi &cData) const {
		uint activeMembrCount = thrust::get<0>(cData);
		uint activeIntnlCount = thrust::get<1>(cData);
		double Cell_CenterX = thrust::get<2>(cData);
        double Cell_CenterY = thrust::get<3>(cData);
		uint cellRank = thrust::get<4>(cData);
		uint nodeRank = thrust::get<5>(cData);
		double nodeMyosin = thrust::get<6>(cData);
		double cellAngle = thrust::get<7>(cData);
		double cellRadius = thrust::get<8>(cData);
		int cell_Type = thrust::get<9>(cData);
		uint index = cellRank * _maxNodePerCell + nodeRank;

		if (_isActiveAddr[index] == false) {
			// if there is a bug in the number of activeinternalnode ?
			// nodeMyosin = 10;
			return nodeMyosin; 
		}
		uint intnlIndxMemBegin = cellRank * _maxNodePerCell;
		uint intnlIndxBegin = cellRank * _maxNodePerCell + _maxMemNodePerCell;
		uint intnlIndxEnd = intnlIndxBegin + activeIntnlCount;
		// uint index_other;
		double nodeX = _locXAddr[index];
		double nodeY = _locYAddr[index];
		// double nodeXOther, nodeYOther;
		// double distNodes = 0.0;
		// double myosinOther;
		// double myosinDiffer;
		// double kDiff = 0.0;
		double pX = cos(cellAngle); 
		double pY = sin(cellAngle);
		double baseMyosin=5;
		// if (cell_Type == 0){baseMyosin=1.0;} // follower 
		double myosinTarget = 0.0; //baseMyosin-((nodeX-Cell_CenterX)*pX + (nodeY-Cell_CenterY)*pY);
		double kmyo = 0.001; // rate of myosin approaching its target value
		double kdeg = 0.001; // was 0.02

		double nodeXTemp, nodeYTemp;
		double distNodes;
		double distThreshold = 1.5;
		double distThreshold2 = 4;
		double countMemNeighbor = 0;
		double minDist = 100;
		double distNodeCenter=0;
		double distThresholdM = 1;
		double minDistIntl = 10;
		double tempdistMI = 0;
		double tempNodeMyosin = 0;
		double myosinThrd = 0.29;
		uint count = 0;
		
		// if (_timeNow > 55800.0) {kDiff = 0.0;} 
		// means membrane node
		//Because we want to compute the force on the membrane nodes we modify this function 
		if (nodeRank < _maxMemNodePerCell) {
			// means membrane node 
			if (cell_Type == 1 ){ // means leader 
			
			/*
			for (uint tempNodeRank=intnlIndxBegin; tempNodeRank<intnlIndxEnd;tempNodeRank++){ // internal nodes 
				nodeXTemp = _locXAddr[tempNodeRank];
				nodeYTemp = _locYAddr[tempNodeRank];
				tempdistMI = compDist2D(nodeX,nodeY,nodeXTemp,nodeYTemp);
				if (tempdistMI<distThresholdM) {tempNodeMyosin += _myosinLevelAddr[tempNodeRank]; count +=1;}
			}
			if(count>0) {
				nodeMyosin = tempNodeMyosin/(count*1.0);
			} else{
				nodeMyosin = 0;
			}
			*/
				for (uint tempNodeRank=intnlIndxBegin; tempNodeRank<intnlIndxEnd;tempNodeRank++){ // internal nodes 
				if (_isActiveAddr[tempNodeRank]){ // should not change anything, as intnlIndxEnd only includes active internal node
					nodeXTemp = _locXAddr[tempNodeRank];
					nodeYTemp = _locYAddr[tempNodeRank];
					tempdistMI = compDist2D(nodeX,nodeY,nodeXTemp,nodeYTemp);
					if (tempdistMI<minDistIntl) { // chose closest internal node 
						nodeMyosin = _myosinLevelAddr[tempNodeRank];
						minDistIntl = tempdistMI;}
					}
				}
				// if (nodeMyosin<myosinThrd){nodeMyosin=0;} // 
			} else { // membrane of follower cells
				nodeMyosin = 0;
			}
			
			return nodeMyosin;
		} else {
			// means internal node
			if (cell_Type == 1 ){ // means leader 
				int ruleNum = 3;
				if (ruleNum<3){
					nodeMyosin = nodeMyosin + _timeStep * kmyo* (myosinTarget - nodeMyosin); // make sure it is positive
				} else if (ruleNum==3){
				/*
				myosinTarget = 1.0;
				double nearestAdhX, nearestAdhY, minDistAllM;
				for (uint tempNodeRank=intnlIndxMemBegin; tempNodeRank<intnlIndxMemBegin+activeMembrCount;tempNodeRank++){
					nodeXTemp = _locXAddr[tempNodeRank];
					nodeYTemp = _locYAddr[tempNodeRank];
					distNodes = compDist2D(nodeX,nodeY,nodeXTemp,nodeYTemp);
					// if (_nodeAdhereIndexAddr[tempNodeRank] > -1) {
					if (tempNodeRank<activeMembrCount/16)//mannual contact line
						// nodeXTemp = _locXAddr[tempNodeRank];
						// nodeYTemp = _locYAddr[tempNodeRank];
						if (distNodes<minDist) {minDist = distNodes; nearestAdhX = nodeXTemp; nearestAdhY=nodeYTemp;}
						// if (distNodes < distThreshold) {countMemNeighbor = countMemNeighbor + 1; }
					}

					if (distNodes<minDistAllM) {minDistAllM = distNodes;}
				}
				distNodeCenter = compDist2D(nodeX,nodeY,Cell_CenterX,Cell_CenterY);
				// double adhCenterVecX = Cell_CenterX - nearestAdhX;
				// double adhCenterVecY = Cell_CenterY - nearestAdhY;
				double crossP = (nearestAdhX-Cell_CenterX)*(nodeY-Cell_CenterY) - (nearestAdhY-Cell_CenterY)*(nodeX-Cell_CenterX);
				if (minDist<distThreshold) {myosinTarget = 5;}
				else if (minDist<distThreshold2&&minDistAllM<distThreshold && crossP>0) {myosinTarget = 0.01;} // change 2 to half radius
				else {myosinTarget = 1;}
				// myosinTarget = myosinTarget * pow(2,countMemNeighbor);
				// if (myosinTarget > 15) {myosinTarget = 15;}
            
					
			for (index_other = intnlIndxBegin; index_other < intnlIndxEnd; // interaction between cur internal node and other internal node
					index_other++) {
				if (index_other == index) {
					continue;
				}
				// nodeXOther = _locXAddr[index_other];
				// nodeYOther = _locYAddr[index_other];
				// myosinOther = _myosinLevelAddr[index_other];
				// distNodes = compDist2D(nodeX,nodeY,nodeXOther,nodeYOther);
				// myosinDiffer = kDiff * max((1.0-distNodes/_myosinDiffusionThreshold),0.0) * (myosinOther-nodeMyosin);
				// nodeMyosin = nodeMyosin + myosinDiffer * _timeStep;
				
			}
			*/
			
			
			double deltaMyosin = 0.0;
			uint fluxIndex,otherMyosinIndex;
			for (uint tempNodeRank=0; tempNodeRank<_maxIntnlNodePerCell;tempNodeRank++){ // internal nodes of the current cell, index for the flux matrix
				fluxIndex = (nodeRank-_maxMemNodePerCell)*_maxIntnlNodePerCell+tempNodeRank; // nodeRank-_maxMembrNode row, tempNodeRank column
				deltaMyosin = deltaMyosin - nodeMyosin*_fluxWeightsAddr[fluxIndex];
			}

			for (uint tempNodeRank=0; tempNodeRank<_maxIntnlNodePerCell;tempNodeRank++){ // internal nodes of the current cell 
				fluxIndex = tempNodeRank*_maxIntnlNodePerCell+(nodeRank-_maxMemNodePerCell);
				otherMyosinIndex = cellRank * _maxNodePerCell + _maxMemNodePerCell + tempNodeRank;
				deltaMyosin = deltaMyosin + _myosinLevelAddr[otherMyosinIndex]*_fluxWeightsAddr[fluxIndex];
			}
			
			//if (index>=intnlIndxBegin && index < intnlIndxEnd){ // seems not neccessary, as the if condition gurantees it is an internal node 
				// nodeMyosin = nodeMyosin + _timeStep * (kmyo* (myosinTarget - nodeMyosin)- kdeg * nodeMyosin); // make sure it is positive
			//	fluxIndex = 1*_maxIntnlNodePerCell+(nodeRank-_maxMemNodePerCell); // row 2, different columns
			// if (_isActiveAddr[index]) // seems no need as it the inactive nodes has return value already 
			nodeMyosin = nodeMyosin + _timeStep * kmyo* deltaMyosin; // _fluxWeightsAddr[fluxIndex]; //
			//}
				}
			} else {
				nodeMyosin = 0;
			}
			return nodeMyosin;
		}
	}
};




struct updateActivationLevelDevice: public thrust::unary_function<UUUI, double> {
	uint _activeCellCount;
	uint _maxMemNodePerCell;
	uint _maxNodePerCell;
	double* _locXAddr;
	double* _locYAddr;
	bool* _isActiveAddr;
	int* _nodeAdhereIndexAddr;
	uint* _actLevelAddr;
	int* _cellTypeAddr;
	// double _grthPrgrCriVal_M;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ updateActivationLevelDevice(
			uint activeCellCount, uint maxMemNodePerCell, uint maxNodePerCell, double* locXAddr, double* locYAddr, 
			bool* isActiveAddr, int* nodeAdhereIndexAddr, uint* actLevelAddr, int* cellTypeAddr) :
			_activeCellCount(activeCellCount), _maxMemNodePerCell(maxMemNodePerCell),_maxNodePerCell(maxNodePerCell), _locXAddr(locXAddr), _locYAddr(locYAddr), 
			_isActiveAddr(isActiveAddr), _nodeAdhereIndexAddr(nodeAdhereIndexAddr), _actLevelAddr(actLevelAddr), _cellTypeAddr(cellTypeAddr){
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ double operator()(const UUUI &cData) const {
		uint cellRank = thrust::get<0>(cData);
		uint curActMembrNodeCount = thrust::get<1>(cData);
		uint curActLevel = thrust::get<2>(cData);
		int cell_Type = thrust::get<3>(cData);


		uint intnlIndxMemBegin = cellRank * _maxNodePerCell;
		int otherNodeRank, otherCellRank;
		int count=0;
		if (cell_Type==1){
			curActLevel = 0;
			return curActLevel;
		}
			// no adhesion with other active cells before
		for (uint tempNodeRank=intnlIndxMemBegin; tempNodeRank<intnlIndxMemBegin+curActMembrNodeCount;tempNodeRank++){
			otherNodeRank = _nodeAdhereIndexAddr[tempNodeRank];
			if (otherNodeRank != -1){
				otherCellRank = otherNodeRank/_maxNodePerCell;
				if (_cellTypeAddr[otherCellRank]==1 || _actLevelAddr[otherCellRank]==1){curActLevel = 1; count=1; break;}
			}
			// if (_actLevelAddr[otherCellRank]>0){curActLevel = curActLevel+1;}
		}
		if (count == 0) {curActLevel=0;} // if not adhere to any cell, then deactivate it
		 // else {}: for future version, add an else condition such that we can update the activation level if a cell is isolated 
		return curActLevel;
	}
};






struct updateCellPolarDevice: public thrust::unary_function<UUUIIDDDD, double> {
	uint _seed;
	// uint64_t _seed;
	double _timeStep;
	double _timeNow;
	double* _cellFilopXAddr;
	double* _cellFilopYAddr;
	double* _cellFilopAngleAddr;
	bool* _cellFilopIsActiveAddr;
	double* _cellFilopBirthTimeAddr;
	uint _activeCellCount;
	double* _cellCenterXAddr;
	double* _cellCenterYAddr;
	double* _cellRadiusAddr;
	uint* _cellActiveFilopCountsAddr;
	uint _maxMemNodePerCell;
	uint _maxNodePerCell;
	double* _locXAddr;
	double* _locYAddr;
	bool* _isActiveAddr;
	int* _nodeAdhereIndexAddr;
	uint* _nodeActLevelAddr;
	double* _myosinLevelAddr;
	int* _cellTypeAddr;
	uint _leaderRank;
	bool _leaderExist;
	double* _cellPolarAngleAddr;
	uint* _cellActLevelAddr;
	// double _grthPrgrCriVal_M;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ updateCellPolarDevice(uint seed, double timeStep, double timeNow, 
			double* cellFilopXAddr, double* cellFilopYAddr, double* cellFilopAngleAddr, bool* cellFilopIsActiveAddr, double* cellFilopBirthTimeAddr,
			uint activeCellCount, double* cellCenterXAddr, double* cellCenterYAddr, double* cellRadiusAddr, uint* cellActiveFilopCountsAddr,
			uint maxMemNodePerCell, uint maxNodePerCell, double* locXAddr, double* locYAddr, bool* isActiveAddr, int* nodeAdhereIndexAddr, 
			uint* nodeActLevelAddr, double* myosinLevelAddr, int* cellTypeAddr, int leaderRank, bool leaderExist, double* cellPolarAngleAddr, uint* cellActLevelAddr) :
			_seed(seed), _timeStep(timeStep), _timeNow(timeNow),
			_cellFilopXAddr(cellFilopXAddr), _cellFilopYAddr(cellFilopYAddr), _cellFilopAngleAddr(cellFilopAngleAddr),
			_cellFilopIsActiveAddr(cellFilopIsActiveAddr), _cellFilopBirthTimeAddr(cellFilopBirthTimeAddr), _activeCellCount(activeCellCount), 
			_cellCenterXAddr(cellCenterXAddr), _cellCenterYAddr(cellCenterYAddr), _cellRadiusAddr(cellRadiusAddr), _cellActiveFilopCountsAddr(cellActiveFilopCountsAddr),
			_maxMemNodePerCell(maxMemNodePerCell),_maxNodePerCell(maxNodePerCell), _locXAddr(locXAddr), _locYAddr(locYAddr), _isActiveAddr(isActiveAddr), 
			_nodeAdhereIndexAddr(nodeAdhereIndexAddr), _nodeActLevelAddr(nodeActLevelAddr), _myosinLevelAddr(myosinLevelAddr), _cellTypeAddr(cellTypeAddr),
			_leaderRank(leaderRank), _leaderExist(leaderExist), _cellPolarAngleAddr(cellPolarAngleAddr), _cellActLevelAddr(cellActLevelAddr) {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ double operator()(const UUUIIDDDD &cData) const {
		uint cellRank = thrust::get<0>(cData);
		uint curActLevel = thrust::get<1>(cData);
		uint activeMembrCount = thrust::get<2>(cData);
		int cell_Type = thrust::get<3>(cData);
		int filopDirection = thrust::get<4>(cData);
		double cell_CenterX = thrust::get<5>(cData);
        double cell_CenterY = thrust::get<6>(cData);
		double cell_Radius = thrust::get<7>(cData);
		double cellAngle = thrust::get<8>(cData);

		
		uint maxFilopPerCell = 5;
		thrust::default_random_engine rng(cellRank+_seed);
		thrust::uniform_real_distribution<double> u01(0.0, 1.0);
		rng.discard(cellRank+100);
		double random01;
		double filopLength;
		double filopMaxLen = 0.5; // leader does not extend filopodium, to be implemented later
		double filopMaxLifeTime = 30.0;
		double filopAllX = 0.0;
		double filopAllY = 0.0;
		double growthTime;
		double randomBirth;

		// uint intnlIndxMemBegin = cellRank * _maxNodePerCell;
		uint filopIndxBegin = cellRank * maxFilopPerCell;
		uint filopIndxEnd = filopIndxBegin + maxFilopPerCell;
		uint intnlIndxMemBegin = cellRank * _maxNodePerCell;

		double filopTipX;
		double filopTipY;
		double distCells;
		// double pI = acos(-1.0);
		double randNoiseAngle = u01(rng)*2.0*PI;
		uint nodeIndex;
		double probNewFilop = 0.00001;
		// double largestCos1 = 0.02;
		// double largestCos2 = 0.01;
		double randomAngleRd;
		bool isvalidDirection=false;
		bool isTangDirection = false;
		if (curActLevel>0){probNewFilop = 0.01;filopMaxLifeTime=150;} // higher likely to form filopodia
		// if (_nodeActLevelAddr[intnlIndxMemBegin]>0){probNewFilop = 0.00002;} // activation level is associated with node ???

		if (_timeNow < 55800.0) {
			return cellAngle;
			}
		else{// if is a follower
			if (cell_Type==0){ 
			// if (_cellRadiusAddr[cellRank]<3.0){
			for (uint filopIndex=filopIndxBegin; filopIndex<filopIndxEnd; filopIndex++){
				// tempindex1 = 0;
				// tempindex2 = 0;
				// endMemActiveIndex = -1;
				if (_cellFilopIsActiveAddr[filopIndex] == true){
					// if this slot has a filopodia already
					growthTime = _timeNow-_cellFilopBirthTimeAddr[filopIndex]; 
					// valid direction means not adhered to another cell
					isvalidDirection = isValidFilopDirection(intnlIndxMemBegin, _maxMemNodePerCell, _isActiveAddr, _locXAddr, _locYAddr,
					 						cell_CenterX, cell_CenterY, _cellFilopAngleAddr[filopIndex], _nodeAdhereIndexAddr,activeMembrCount,_maxNodePerCell,_myosinLevelAddr);
					if (growthTime<filopMaxLifeTime && isvalidDirection){
					// isvalidDirection = true;
					// if (isvalidDirection) {
						// this filopodia is allowed to grow or retract
						filopLength = filopMaxLen*(1.0-cos(2.0*PI*growthTime/filopMaxLifeTime));
						_cellFilopXAddr[filopIndex] = filopLength*cos(_cellFilopAngleAddr[filopIndex]);
						_cellFilopYAddr[filopIndex] = filopLength*sin(_cellFilopAngleAddr[filopIndex]);
					} else{
						// this filopodia have reached its maximum life time, reset length and angle, active status
						_cellFilopIsActiveAddr[filopIndex] = false;
						_cellFilopAngleAddr[filopIndex] = (2.0*u01(rng)-1.0)*50*PI;//2.0*u01(rng)*PI;//(2.0*u01(rng)-1.0)*50*PI;//_cellFilopAngleAddr[filopIndex];
						_cellFilopXAddr[filopIndex] = 0.0; //50.0 * cos(_cellFilopAngleAddr[filopIndex]);
						_cellFilopYAddr[filopIndex] = 0.0; //50.0 * sin(_cellFilopAngleAddr[filopIndex]);
						// _cellFilopAngleAddr[filopIndex] = 0.0;
						_cellFilopBirthTimeAddr[filopIndex] = 0.0;
						_cellActiveFilopCountsAddr[cellRank] = _cellActiveFilopCountsAddr[cellRank] - 1;
					}
				} else {
					// else if this slot does not have an active filopodia
					// add condition that the direction does not across the adhesion boundary
					double tempB =  u01(rng);
					randomBirth = tempB/10.0;
					if (randomBirth<probNewFilop){ // probability of a new filopodia can grow 
						random01 = u01(rng);
						randomAngleRd = (2.0*random01-1.0)*50*PI;//2.0*random01*PI;//(2.0*random01-1.0)*50*PI;
						// isvalidDirection = isValidFilopDirection(intnlIndxMemBegin, _maxMemNodePerCell, _isActiveAddr, _locXAddr, _locYAddr,
					 	//					cell_CenterX, cell_CenterY, _cellFilopAngleAddr[filopIndex], _nodeAdhereIndexAddr,activeMembrCount,_maxNodePerCell,_myosinLevelAddr);
						isTangDirection = isTangFilopDirection(intnlIndxMemBegin, _isActiveAddr, _locXAddr, _locYAddr,cell_CenterX, cell_CenterY, 
																randomAngleRd, _nodeAdhereIndexAddr, activeMembrCount, _maxNodePerCell, _cellTypeAddr, _leaderRank, _myosinLevelAddr, filopDirection);
						// isTangDirection = true;
						if (isTangDirection){
						// randomAngle = randomAngle*2.0*PI;
							_cellFilopIsActiveAddr[filopIndex] = true; // find the largest one and use the fact that the membrane nodes are indexed? 
							_cellFilopAngleAddr[filopIndex] = (2.0*random01-1.0)*50*PI;//2.0*random01*PI;//(2.0*random01-1.0)*50*PI;
							_cellFilopXAddr[filopIndex] = 0.0; // zero length 
							_cellFilopYAddr[filopIndex] = 0.0;
							_cellFilopBirthTimeAddr[filopIndex] = _timeNow;
							_cellActiveFilopCountsAddr[cellRank] = _cellActiveFilopCountsAddr[cellRank] + 1;
						} else {
							_cellFilopIsActiveAddr[filopIndex] = false;
							_cellFilopAngleAddr[filopIndex] = (2.0*random01-1.0)*50*PI;//2.0*random01*PI; //(2.0*random01-1.0)*50*PI;
							_cellFilopXAddr[filopIndex] = 0.0;//100.0 * cos(6.28*randomAngle);
							_cellFilopYAddr[filopIndex] = 0.0;//100.0 * sin(6.28*randomAngle);
							_cellFilopBirthTimeAddr[filopIndex] = 0.0;//_timeNow;
							// _cellActiveFilopCountsAddr[cellRank] = _cellActiveFilopCountsAddr[cellRank] + 1;
						}
					} else {
						_cellFilopIsActiveAddr[filopIndex] = false;
						_cellFilopAngleAddr[filopIndex] = (2.0*u01(rng)-1.0)*50*PI;// 2.0*u01(rng)*PI; //(2.0*u01(rng)-1.0)*50*PI;
						_cellFilopXAddr[filopIndex] = 0.0;
						_cellFilopYAddr[filopIndex] = 0.0;
						_cellFilopBirthTimeAddr[filopIndex] = 0.0;
					}
				}
			}

			for (uint filopIndex=filopIndxBegin; filopIndex<filopIndxEnd; filopIndex++){
				if (_cellFilopIsActiveAddr[filopIndex] == true){// if there is already a filopodia
					filopTipX = cell_CenterX+cell_Radius*cos(_cellFilopAngleAddr[filopIndex])+_cellFilopXAddr[filopIndex];
					filopTipY = cell_CenterY+cell_Radius*sin(_cellFilopAngleAddr[filopIndex])+_cellFilopYAddr[filopIndex];
					for (uint cellNghbrIndex = 0; cellNghbrIndex<_activeCellCount; cellNghbrIndex++) { // location of the filopodia tip, 
						if (cellNghbrIndex!=cellRank){
							distCells = compDist2D(_cellCenterXAddr[cellNghbrIndex], _cellCenterYAddr[cellNghbrIndex], filopTipX, filopTipY);
							if (distCells<2.0*_cellRadiusAddr[cellNghbrIndex]) {
								int isAttached = 0;
								for (uint memNodeRank=0; memNodeRank<_maxMemNodePerCell;memNodeRank++){
									nodeIndex = cellRank*_maxNodePerCell + memNodeRank;
									if (_isActiveAddr[nodeIndex]&&_nodeAdhereIndexAddr[nodeIndex]>-1){
										uint adhCellRank = _nodeAdhereIndexAddr[nodeIndex]/_maxNodePerCell;
										if (adhCellRank == cellNghbrIndex) {isAttached=1;}
									}
								}
								if (isAttached==0){
									cellAngle = cellAngle + _timeStep*0.5*(_cellFilopYAddr[filopIndex]*cos(cellAngle)-_cellFilopXAddr[filopIndex]*sin(cellAngle));
								}
							}
						}
					// for (nodeIndex = 0; nodeIndex<maxActiveMembNode; nodeIndex++)

					}
					// add all filopodia together, include the filopodia which is just born, although its length is 0 
					filopAllX += _cellFilopXAddr[filopIndex];
					filopAllY += _cellFilopYAddr[filopIndex];
				} 
			}
			for (uint memNodeRank=0; memNodeRank<_maxMemNodePerCell;memNodeRank++){
				nodeIndex = cellRank*_maxNodePerCell + memNodeRank;
				if (_isActiveAddr[nodeIndex]&&_nodeAdhereIndexAddr[nodeIndex]>-1){
					uint adhCellRank = _nodeAdhereIndexAddr[nodeIndex]/_maxNodePerCell;
					if (_cellTypeAddr[adhCellRank]==0 ){  // neighbor is a follower and is active //&& _cellActLevelAddr[adhCellRank]==1
						// double neighCellAngle = _cellPolarAngleAddr[adhCellRank];
						// cellAngle = cellAngle + _timeStep*(sin(neighCellAngle)*cos(cellAngle)-cos(neighCellAngle)*sin(cellAngle));
						double adhCenterVecX = _cellCenterXAddr[adhCellRank] - cell_CenterX;
						double adhCenterVecY = _cellCenterYAddr[adhCellRank] - cell_CenterY;
						double neighCellAngle = atan2(adhCenterVecY,adhCenterVecX) + 3.14159/4.0;
						cellAngle = cellAngle + _timeStep*0.1*(sin(neighCellAngle)*cos(cellAngle)-cos(neighCellAngle)*sin(cellAngle));
					}
				}
			}
			if (_leaderExist){ // means a leader exists
				double leaderCurCenterXVec = _cellCenterXAddr[_leaderRank] - cell_CenterX;
				double leaderCurCenterYVec = _cellCenterYAddr[_leaderRank] - cell_CenterY;
				double leaderCurAngle =  atan2(leaderCurCenterYVec,leaderCurCenterXVec); // attraction to the leader
				cellAngle = cellAngle + _timeStep*0.5*(filopAllY*cos(cellAngle)-filopAllX*sin(cellAngle)+3*(sin(leaderCurAngle)*cos(cellAngle)-cos(leaderCurAngle)*sin(cellAngle)));
			}
				cellAngle = cellAngle + _timeStep*0.5*(filopAllY*cos(cellAngle)-filopAllX*sin(cellAngle));
			}
			else{ // means a leader
				int ruleNum = 3;
				uint tempCellInd = 20000; // initial index is a fake cell
				uint firstCellRank;
				double rateRep = 0.2;
				if (ruleNum == 1){
					// neighbor alignment 
					for (uint memNodeRank=0; memNodeRank<_maxMemNodePerCell;memNodeRank++){
					nodeIndex = cellRank*_maxNodePerCell + memNodeRank; // add calculation to compute the total index
					if (_isActiveAddr[nodeIndex]&&_nodeAdhereIndexAddr[nodeIndex]>-1){// _nodeAdhereIndexAddr is adhere to another cell
						// compute the index of the adhered cell 
						uint adhCellRank = _nodeAdhereIndexAddr[nodeIndex]/(int)_maxNodePerCell;
						if (memNodeRank==0){firstCellRank=adhCellRank;} // avoid adding angle twice
						if (adhCellRank!=tempCellInd && (memNodeRank==0 || adhCellRank!=firstCellRank)){
							double neighCellAngle = _cellPolarAngleAddr[adhCellRank];
							cellAngle = cellAngle + _timeStep*rateRep*(neighCellAngle);
							tempCellInd = adhCellRank;
						}
					}
				}
				} else if (ruleNum == 2){
				// CIL with direction opposite to contact line 
				double allPolar = 0;
				for (uint memNodeRank=0; memNodeRank<_maxMemNodePerCell;memNodeRank++){
					nodeIndex = cellRank*_maxNodePerCell + memNodeRank; // add calculation to compute the total index
					if (_isActiveAddr[nodeIndex]&&_nodeAdhereIndexAddr[nodeIndex]>-1){// _nodeAdhereIndexAddr is adhere to another cell
						allPolar = allPolar + sin(atan2(cell_CenterY - _locYAddr[nodeIndex],cell_CenterX - _locXAddr[nodeIndex])-cellAngle);
					}
				}
				cellAngle = cellAngle + _timeStep*rateRep*(allPolar); // +randNoiseAngle 
				} else if (ruleNum==3){
					// for the myosin redistribution case
					cellAngle = 0;
				}

			}
			return cellAngle;
		}
	}
};








struct updateCellPolarLeaderDevice: public thrust::unary_function<UUUIDDDD, double> {
	uint _seed;
	double _timeStep;
	double _timeNow;
	uint _activeCellCount;
	double* _cellCenterXAddr;
	double* _cellCenterYAddr;
	double* _cellRadiusAddr;
	uint* _cellActiveFilopCountsAddr;
	uint _maxMemNodePerCell;
	uint _maxNodePerCell;
	double* _locXAddr;
	double* _locYAddr;
	bool* _isActiveAddr;
	int* _nodeAdhereIndexAddr;
	uint* _nodeActLevelAddr;
	double* _myosinLevelAddr;
	int* _cellTypeAddr;
	uint _leaderRank;
	double* _cellPolarAngleAddr;
	uint* _cellActLevelAddr;
	double* _myosinWeightAddr;
	// double _grthPrgrCriVal_M;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ updateCellPolarLeaderDevice(uint seed, double timeStep, double timeNow, 
			uint activeCellCount, double* cellCenterXAddr, double* cellCenterYAddr, double* cellRadiusAddr, uint* cellActiveFilopCountsAddr,
			uint maxMemNodePerCell, uint maxNodePerCell, double* locXAddr, double* locYAddr, bool* isActiveAddr, int* nodeAdhereIndexAddr, 
			uint* nodeActLevelAddr, double* myosinLevelAddr, int* cellTypeAddr, int leaderRank, double* cellPolarAngleAddr, uint* cellActLevelAddr, double* myosinWeightAddr) :
			_seed(seed), _timeStep(timeStep), _timeNow(timeNow), _activeCellCount(activeCellCount), 
			_cellCenterXAddr(cellCenterXAddr), _cellCenterYAddr(cellCenterYAddr), _cellRadiusAddr(cellRadiusAddr), _cellActiveFilopCountsAddr(cellActiveFilopCountsAddr),
			_maxMemNodePerCell(maxMemNodePerCell),_maxNodePerCell(maxNodePerCell), _locXAddr(locXAddr), _locYAddr(locYAddr), _isActiveAddr(isActiveAddr), 
			_nodeAdhereIndexAddr(nodeAdhereIndexAddr), _nodeActLevelAddr(nodeActLevelAddr), _myosinLevelAddr(myosinLevelAddr), _cellTypeAddr(cellTypeAddr),
			_leaderRank(leaderRank), _cellPolarAngleAddr(cellPolarAngleAddr), _cellActLevelAddr(cellActLevelAddr), _myosinWeightAddr(myosinWeightAddr) {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ double operator()(const UUUIDDDD &cData) const {
		uint cellRank = thrust::get<0>(cData);
		uint curActLevel = thrust::get<1>(cData);
		uint activeMembrCount = thrust::get<2>(cData);
		int cell_Type = thrust::get<3>(cData);
		double cell_CenterX = thrust::get<4>(cData);
        double cell_CenterY = thrust::get<5>(cData);
		double cell_Radius = thrust::get<6>(cData);
		double cellAngle = thrust::get<7>(cData);

		// uint intnlIndxMemBegin = cellRank * _maxNodePerCell;
		int intnlIndxMemBegin = cellRank * _maxNodePerCell;
		int intnlIndxMemEnd = cellRank * _maxNodePerCell + activeMembrCount; // last element not included ss
		int nodeMembrIndex,initNodeRank;
		int curRank,nextRank,curIndex,nextIndex,stNode,endNode,stNodeRecord,endNodeRecord;
		int midNodeIndex,midNodeRank;
		int curLen, maxLen;
		int index_left,index_right;
		uint nodeIndex;
		double normX, normY;
		// double PI = acos(-1.0);
		thrust::default_random_engine rng(cellRank+_seed);
		thrust::uniform_real_distribution<double> u01(0, 1.0);
		rng.discard(cellRank+100);
		double twoPi = 2.0 * PI;

		double myosinThrd = 0.29; // need to be changed if initial nodeMyosin changes
		bool flag = false;
		if (_timeNow < 55800.0) {
			return cellAngle;
			}
		else{
			if (cell_Type==1){ 
				for (int memNodeRank=0; memNodeRank<_maxMemNodePerCell;memNodeRank++){
				nodeIndex = cellRank*_maxNodePerCell + memNodeRank;
				if (_isActiveAddr[nodeIndex]&&_nodeAdhereIndexAddr[nodeIndex]>-1){
						flag = true; // leader is attached to a follower, find the first node that is attached
						break;
					}
				}
				if (flag==false) {return cellAngle;}

				for (int memNodeRank=0; memNodeRank<activeMembrCount; memNodeRank++){
					nodeMembrIndex = cellRank * _maxNodePerCell + memNodeRank;
					if (_myosinLevelAddr[nodeMembrIndex]> myosinThrd){ // choose the first membrane node that has a higher myosin level
						initNodeRank = (int) memNodeRank;
						break;
					}
				}

				
				for (int rawRank=initNodeRank;rawRank<initNodeRank+(int) activeMembrCount;rawRank++){
					if(rawRank>= (int) activeMembrCount){
						curRank = rawRank - (int) activeMembrCount;
					} else {
						curRank = rawRank;
					}
					if(rawRank+1>= (int) activeMembrCount){
						nextRank = rawRank +1 - (int) activeMembrCount;
					} else {
						nextRank = rawRank + 1;
					} // [ 0 0 0 0 0 1 1 1 1 1 1 0 0 0 0 0 0 1 1 1]
					curIndex = cellRank * _maxNodePerCell + curRank;
					nextIndex = cellRank * _maxNodePerCell + nextRank;
					if (_isActiveAddr[curIndex]&&_isActiveAddr[nextIndex]){
					}
					else {
						_myosinLevelAddr[curIndex] = 200; // this was initialized
						_myosinLevelAddr[nextIndex] = 300;
					}

					if (_myosinLevelAddr[curIndex] >= myosinThrd && _myosinLevelAddr[nextIndex] >= myosinThrd){ // 0 0 
						// do nothing
							stNode = 0;
							endNode = 0;
						} else if (_myosinLevelAddr[curIndex] >= myosinThrd && _myosinLevelAddr[nextIndex] < myosinThrd){   // 0 1 
							stNode = nextRank;
							endNode = nextRank;
							curLen = 1+_myosinWeightAddr[nextIndex];
						} else if (_myosinLevelAddr[curIndex] < myosinThrd && _myosinLevelAddr[nextIndex] >= myosinThrd){  // 1 0
							endNode = curRank;
							if (maxLen<curLen) {
								stNodeRecord = stNode;
								endNodeRecord = endNode;
								maxLen = curLen;
							}
							curLen = 0;
						} else if(_myosinLevelAddr[curIndex] < myosinThrd && _myosinLevelAddr[nextIndex] < myosinThrd){  //1 1
							endNode = nextRank;
							curLen = curLen + 1+_myosinWeightAddr[nextIndex];
						}
				}
			// stNodeRank = stNodeRecord - (int) intnlIndxMemBegin;
			// endNodeRank = endNodeRecord - (int) intnlIndxMemBegin;
			if (endNodeRecord<stNodeRecord) {
				midNodeRank = (stNodeRecord+ endNodeRecord+ (int)activeMembrCount)/2; //(int)activeMembrCount-1; 
				if (midNodeRank>= (int) activeMembrCount) {midNodeRank=midNodeRank-(int)activeMembrCount;} //
			} else {
				midNodeRank = (int) (stNodeRecord+endNodeRecord)/2;
			}
			// _myosinLevelAddr[initNodeRank+cellRank * _maxNodePerCell] = 10; //
			// _myosinLevelAddr[stNodeRecord+cellRank * _maxNodePerCell] = 30;
			// _myosinLevelAddr[endNodeRecord+cellRank * _maxNodePerCell] = 50;
			// _myosinLevelAddr[midNodeRank+cellRank * _maxNodePerCell] = 100;
			// midNodeRank = midNodeIndex - (int)intnlIndxMemBegin;
			index_left = midNodeRank - 1;
			if (index_left == -1) {
				index_left = (int) activeMembrCount - 1;
			}
			index_left = index_left + cellRank * _maxNodePerCell;

			index_right = midNodeRank + 1;
			if (index_right == (int) activeMembrCount) {
				index_right = 0;
			}
			index_right = index_right + cellRank * _maxNodePerCell;
			// 
			normX = -(_locYAddr[index_left] - _locYAddr[index_right] ); // get normal vector pointing to the direction with lowest myosin level
			normY = _locXAddr[index_left] - _locXAddr[index_right]; 
			cellAngle =  atan2(normY,normX) + (2.0*u01(rng)-1.0)*twoPi/8; 

			double isInsideIntv;
			double kdecay = 0.1;
			for (int memNodeRank=0; memNodeRank<_maxMemNodePerCell;memNodeRank++){
				nodeIndex = cellRank*_maxNodePerCell + memNodeRank;
				if (_isActiveAddr[nodeIndex]){
					if (endNodeRecord<stNodeRecord){
						if (stNodeRecord<=memNodeRank || memNodeRank<=endNodeRecord){
							isInsideIntv = 1;}
						else {isInsideIntv = 0;}
							
					} else {
						if (memNodeRank>=stNodeRecord && memNodeRank<=endNodeRecord){
							isInsideIntv = 1;
						} else {
							isInsideIntv = 0;
						}
					}
					_myosinWeightAddr[nodeIndex] = _myosinWeightAddr[nodeIndex]+_timeStep*(-kdecay*_myosinWeightAddr[nodeIndex]+isInsideIntv);
					} // double check the above equation 
			}
			}
			return cellAngle;
		}
	}
};








struct updateSigPtState: public thrust::unary_function<UiDDDDiU, double> {
	double _timeStep;
	double _timeNow;
	double* _cellFilopXAddr;
	double* _cellFilopYAddr;
	double* _cellFilopAngleAddr;
	bool* _cellFilopIsActiveAddr;
	double* _cellFilopBirthTimeAddr;
	uint _activeCellCount;
	double* _cellCenterXAddr;
	double* _cellCenterYAddr;
	double* _cellRadiusAddr;
	uint _maxMemNodePerCell;
	uint _maxNodePerCell;
	double* _locXAddr;
	double* _locYAddr;
	bool* _isActiveAddr;
	int* _nodeAdhereIndexAddr;
	SigptState* _sigPtAddr;
	// double _grthPrgrCriVal_M;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ updateSigPtState(double timeStep, double timeNow, 
			uint activeCellCount, double* cellCenterXAddr, double* cellCenterYAddr, double* cellRadiusAddr,
			uint maxMemNodePerCell, uint maxNodePerCell, double* locXAddr, double* locYAddr, bool* isActiveAddr, int* nodeAdhereIndexAddr, SigptState* sigPtAddr) :
			_timeStep(timeStep), _timeNow(timeNow),_activeCellCount(activeCellCount), 
			_cellCenterXAddr(cellCenterXAddr), _cellCenterYAddr(cellCenterYAddr), _cellRadiusAddr(cellRadiusAddr), 
			_maxMemNodePerCell(maxMemNodePerCell),_maxNodePerCell(maxNodePerCell), _locXAddr(locXAddr), _locYAddr(locYAddr), _isActiveAddr(isActiveAddr), 
			_nodeAdhereIndexAddr(nodeAdhereIndexAddr), _sigPtAddr(sigPtAddr){
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ double operator()(const UiDDDDiU &cData) const {
		uint cellRank = thrust::get<0>(cData);
		double cell_CenterX = thrust::get<1>(cData);
        double cell_CenterY = thrust::get<2>(cData);
		double cell_Radius = thrust::get<3>(cData);
		double cellAngle = thrust::get<4>(cData);
		int cell_Type = thrust::get<5>(cData);
		uint activeMembrCount = thrust::get<6>(cData);
		uint intnlIndxMemBegin = cellRank * _maxNodePerCell;
		
		// uint nodeRank;
		double xRes, yRes;
		// nodeRank = cellRank*_maxNodePerCell + memNodeIndex; 
		for (uint i=0; i<100; i++){
			xRes = _sigPtAddr[i].locx;
			yRes = _sigPtAddr[i].locy;
			if (isInsideCell(xRes,yRes,intnlIndxMemBegin,activeMembrCount,_locXAddr,_locYAddr)){
						_sigPtAddr[i].ptState = cellRank;
					}
			if (i==0){
				_sigPtAddr[i].ptState = 10;
			}
		}
		return cellAngle;
	}
};








struct calCellRadius: public thrust::unary_function<UiDDU, double> {
	uint _maxNodePerCell;
	uint _maxMemNodePerCell;
	double* _locXAddr;
	double* _locYAddr;
	bool* _isActiveAddr;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ calCellRadius(uint maxNodePerCell, uint maxMemNodePerCell, 
			double* locXAddr, double* locYAddr, bool* isActiveAddr) :
			_maxNodePerCell(maxNodePerCell), _maxMemNodePerCell(maxMemNodePerCell), _locXAddr(locXAddr), _locYAddr(locYAddr), _isActiveAddr(isActiveAddr){
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ double operator()(const UiDDU &cData) const {
		uint cellRank = thrust::get<0>(cData);
		double cell_CenterX = thrust::get<1>(cData);
        double cell_CenterY = thrust::get<2>(cData);
		uint activeMembrCount = thrust::get<3>(cData);
	
		uint indexMemBegin = cellRank * _maxNodePerCell;
		double cellRadius=0.0;
		double distCenterMemb;
		uint nodeIndex;
		for (uint memNodeIndex=0; memNodeIndex<activeMembrCount;memNodeIndex++){
			nodeIndex = indexMemBegin+memNodeIndex;
			distCenterMemb = compDist2D(cell_CenterX,cell_CenterY,_locXAddr[nodeIndex],_locYAddr[nodeIndex]);
			if (distCenterMemb>cellRadius){
				cellRadius = distCenterMemb;
			}
		}
		return cellRadius;
		}
};








// four binding grids, not randomly permuted 
/*
struct calSubAdhForceDevice: public thrust::unary_function<UUDDUUDDD, CVec2> {
	uint _maxNodePerCell;
	uint _maxMemNodePerCell;
	double* _locXAddr;
	double* _locYAddr;
	bool* _isActiveAddr;
	double* _myosinLevelAddr;
	double _timeStep;
	double _timeNow;
	double* _subAdhLocXAddr; 
	double* _subAdhLocYAddr;
	bool* _subAdhIsBoundAddr;
	uint _maxSubSitePerNode; 
	uint _seed;
	// double _grthPrgrCriVal_M;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ calSubAdhForceDevice(uint maxNodePerCell,
			uint maxMemNodePerCell, double* locXAddr, double* locYAddr,
			bool* isActiveAddr, double* myosinLevelAddr, double timeStep, double timeNow,
			double* subAdhLocXAddr, double* subAdhLocYAddr, bool* subAdhIsBoundAddr, uint maxSubSitePerNode, uint seed) :
			_maxNodePerCell(maxNodePerCell), _maxMemNodePerCell(
					maxMemNodePerCell), _locXAddr(locXAddr), _locYAddr(
					locYAddr), _isActiveAddr(isActiveAddr), _myosinLevelAddr(myosinLevelAddr), 
					_timeStep(timeStep), _timeNow(timeNow), 
					_subAdhLocXAddr(subAdhLocXAddr), _subAdhLocYAddr(subAdhLocYAddr), _subAdhIsBoundAddr(subAdhIsBoundAddr), _maxSubSitePerNode(maxSubSitePerNode), _seed(seed)  {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ CVec2 operator()(const UUDDUUDDD &cData) const {
		uint activeMembrCount = thrust::get<0>(cData);
		uint activeIntnlCount = thrust::get<1>(cData);
		double Cell_CenterX = thrust::get<2>(cData);
        double Cell_CenterY = thrust::get<3>(cData);
		uint cellRank = thrust::get<4>(cData);
		uint nodeRank = thrust::get<5>(cData); // Is the node 
		double nodeMyosin = thrust::get<6>(cData);
		double velX = thrust::get<7>(cData);
		double velY = thrust::get<8>(cData);
		uint index = cellRank * _maxNodePerCell + nodeRank;
		// uint intnlIndxMemBegin = cellRank * _maxNodePerCell;

		double nodeX = _locXAddr[index];
		double nodeY = _locYAddr[index];
		
		double pX =  sqrt(2.0)/2.0; // to be added as cell property later, JG041123
        double pY = -sqrt(2.0)/2.0;
		// double projLen = (nodeX - Cell_CenterX)*pX + (nodeY - Cell_CenterY)*pY;
		double projLen;
		double tempCellRad = 1.25;
		double twoPi = 2.0*2.0*3.1415926535897932384626;
		double randAngle;
		double randLen;
		thrust::default_random_engine rng(_seed);
		// rng.discard(cellRank);
    	thrust::uniform_real_distribution<double> u01(0, 1.0);
		thrust::uniform_real_distribution<double> u0TwoPi(0, twoPi);


		// To be implemented: 
		// if this is an active membrane node, do the following (one binding and one unbinding per time step)
		// if sum(subAdhIsBound(nodeRank*10:nodeRank*10+9)) != 10 (means bound adhesion sites is less than 10)
		// find the cloest site, assign some probability r1 to combine (do not do the binding yet) 
		// if sum(subsAdhIsBound(...)) != 0 (there are some bound adhesion sites)
		// compute the distance between the cloest site and the current node, assign some probability r2 to detach
		// if r1>threshold, bind; find a empty spot in the 10-element vector, record the location and change element in IsBound vector to 1
		// if r2>threshold, unbind; reset the position to 0? change the element in IsBound vector to 0
		// Use an external distance function for later modifications 

		// Compute the adhesion force from the substrate force 

		// elseif this is not a membrane node, do nothing and return original velX, velY values

		uint bindSiteCounts = 0;
		double posBind = 0.0; // initialize probability for binding and unbinding 
		double posUnbind = 0.0; 
		double siteX;
		double siteY;
		uint siteIndex;
		double charUnbindDist = 0.1;
		double distNodeSite;
		double adhForceX = 0.0;
		double adhForceY = 0.0;
		double kAdh = 30.0;
		if (_timeNow > 55800.0 && _isActiveAddr[index] == true && (nodeRank < _maxMemNodePerCell)) {
			// starting of the index of the substrate site corresponding to this node is: index*10, 10 is the max subs sites
			for (int bindSiteIndex = 0; bindSiteIndex < _maxSubSitePerNode; // interaction between cur internal node and other internal node
					bindSiteIndex++) {
						bindSiteCounts += _subAdhIsBoundAddr[index*_maxSubSitePerNode + bindSiteIndex];
					}
			// Perform the unbinding and binding events by adding the location of the site to the siteXY vector 
			if (bindSiteCounts > 0){
				// means there are possibilities for unbinding, compute the unbinding probability 
				for (int bindSiteIndex = 0; bindSiteIndex < _maxSubSitePerNode; bindSiteIndex++){
					double randomN1 = u01(rng);
					siteIndex = index*_maxSubSitePerNode + bindSiteIndex; 
					if (_subAdhIsBoundAddr[siteIndex] == 1){
						siteX = _subAdhLocXAddr[siteIndex];
						siteY = _subAdhLocYAddr[siteIndex];
						distNodeSite = compDist2D(nodeX, nodeY, siteX, siteY);  
						if (randomN1 < (1.0-exp(-distNodeSite/charUnbindDist))){
							// _subAdhLocXAddr = ;
							// _subAdhLocYAddr = ;
							_subAdhIsBoundAddr[siteIndex] = false;
						}
					}
				}
			}
			if (bindSiteCounts < _maxSubSitePerNode){
				// compute possibility for binding to the closest site
				double randomN2;
				// compute the nearest site
				double nearestX = floor(nodeX*10.0)/10.0; // with 0.1 as grid increment, what is the function for rounding???
				double nearestY = floor(nodeY*10.0)/10.0; //
				double nearestOther4X[4] = {nearestX, nearestX+1.0/10.0, nearestX+1.0/10.0, nearestX}; // four neighbors
				double nearestOther4Y[4] = {nearestY, nearestY, nearestY+1.0/10.0, nearestY+1.0/10.0};

				bool siteAlreadyBound[4] = {false, false, false, false}; // four neighboring sites 
				// double nearest9Neigh[9];
				for (int bindSiteIndex = 0; bindSiteIndex < _maxSubSitePerNode; bindSiteIndex++){
					siteIndex = index*_maxSubSitePerNode + bindSiteIndex; 
					// check if the nearest site is already bound to the current node 
					for (int allNbrIndex = 0; allNbrIndex < 4; allNbrIndex++){
						if (_subAdhIsBoundAddr[siteIndex] == 1 && abs(nearestOther4X[allNbrIndex]-_subAdhLocXAddr[siteIndex])<0.00001 && abs(nearestOther4Y[allNbrIndex]-_subAdhLocYAddr[siteIndex])<0.00001) {
							siteAlreadyBound[allNbrIndex] = true;
							break;
							}
						}
					}
				// 0, 1, 2, 3 
				// construct a neighbor list 0 to 3 with permulated numbers 
				for (int allNbrIndex = 0; allNbrIndex < 4; allNbrIndex++){
					randomN2= u01(rng);
					projLen = (nearestOther4X[allNbrIndex] - Cell_CenterX)*pX + (nearestOther4Y[allNbrIndex] - Cell_CenterY)*pY;
					if (randomN2<1-exp(-projLen/tempCellRad) && siteAlreadyBound[allNbrIndex] == false && bindSiteCounts<_maxSubSitePerNode) { // all sites using the same prob 
						for (int bindSiteIndex = 0; bindSiteIndex < _maxSubSitePerNode; bindSiteIndex++){
							siteIndex = index*_maxSubSitePerNode + bindSiteIndex; 
							if (_subAdhIsBoundAddr[siteIndex] == 0){
								_subAdhLocXAddr[siteIndex] = nearestOther4X[allNbrIndex]; // add permutated arry and change the code to nearestOther4X[nbrListPerm[allNbrIndex]]
								_subAdhLocYAddr[siteIndex] = nearestOther4Y[allNbrIndex];
								_subAdhIsBoundAddr[siteIndex] = true;
								bindSiteCounts += 1; // changing this inside the if condition involving bindSiteCounts 
								break;
							}
						}
					}
				}
			}
			
			// Compute the updated force depending on updated distance for each binding site
			for (int bindSiteIndex = 0; bindSiteIndex < _maxSubSitePerNode; bindSiteIndex++){
				siteIndex = index*_maxSubSitePerNode + bindSiteIndex; 
				if (_subAdhIsBoundAddr[siteIndex] == 1){
						siteX = _subAdhLocXAddr[siteIndex];
						siteY = _subAdhLocYAddr[siteIndex];
						distNodeSite = compDist2D(nodeX, nodeY, siteX, siteY);  
						if (distNodeSite > 0.0){
							adhForceX = (siteX - nodeX)/distNodeSite;
							adhForceY = (siteY - nodeY)/distNodeSite;
							velX += kAdh * distNodeSite * adhForceX;
							velY += kAdh * distNodeSite * adhForceY;
						}
					}
				}
		return thrust::make_tuple(velX, velY);
	} else {
			return thrust::make_tuple(velX, velY); // determines return type !!!
		}
	}
};

*/




struct calSubAdhForceDevice: public thrust::unary_function<UIUDDUUDDD, CVec2> {
	uint _maxNodePerCell;
	uint _maxMemNodePerCell;
	double* _locXAddr;
	double* _locYAddr;
	bool* _isActiveAddr;
	double* _myosinLevelAddr;
	double _timeStep;
	double _timeNow;
	double* _subAdhLocXAddr; 
	double* _subAdhLocYAddr;
	bool* _subAdhIsBoundAddr;
	uint* _cellActLevelAddr; 
	uint _seed;
	uint* _nodeActLevelAddr;
	double* _cellPolarAngleAddr;
	// double _grthPrgrCriVal_M;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ calSubAdhForceDevice(uint maxNodePerCell,
			uint maxMemNodePerCell, double* locXAddr, double* locYAddr,
			bool* isActiveAddr, double* myosinLevelAddr, double timeStep, double timeNow,
			double* subAdhLocXAddr, double* subAdhLocYAddr, bool* subAdhIsBoundAddr, uint* cellActLevelAddr, 
			uint seed, uint* nodeActLevelAddr, double* cellPolarAngleAddr) :
			_maxNodePerCell(maxNodePerCell), _maxMemNodePerCell(
					maxMemNodePerCell), _locXAddr(locXAddr), _locYAddr(
					locYAddr), _isActiveAddr(isActiveAddr), _myosinLevelAddr(myosinLevelAddr), 
					_timeStep(timeStep), _timeNow(timeNow), 
					_subAdhLocXAddr(subAdhLocXAddr), _subAdhLocYAddr(subAdhLocYAddr), _subAdhIsBoundAddr(subAdhIsBoundAddr), 
					_cellActLevelAddr(cellActLevelAddr), _seed(seed), _nodeActLevelAddr(nodeActLevelAddr), _cellPolarAngleAddr(cellPolarAngleAddr)  {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ CVec2 operator()(const UIUDDUUDDD &cData) const {
		uint activeMembrCount = thrust::get<0>(cData); // change here 
		int cellType = thrust::get<1>(cData);	
		uint activeIntnlCount = thrust::get<2>(cData);
		double Cell_CenterX = thrust::get<3>(cData);
        double Cell_CenterY = thrust::get<4>(cData);
		uint cellRank = thrust::get<5>(cData);
		uint nodeRank = thrust::get<6>(cData); // Is the node
		double nodeMyosin = thrust::get<7>(cData);
		double velX = thrust::get<8>(cData);
		double velY = thrust::get<9>(cData);
		// double cellAngle = thrust::get<9>(cData);
		uint index = cellRank * _maxNodePerCell + nodeRank;// 
		// uint intnlIndxMemBegin = cellRank * _maxNodePerCell;
		double curCellAngle = _cellPolarAngleAddr[cellRank];

		uint intnlIndxBegin = cellRank * _maxNodePerCell + _maxMemNodePerCell;
		uint intnlIndxEnd = intnlIndxBegin + activeIntnlCount;

		double nodeX = _locXAddr[index];
		double nodeY = _locYAddr[index];
		
		int index_left, index_right;
		double normX, normY;
		// for testing only
		// uint index1 = cellRank * _maxNodePerCell + 3;
		// double nodeX1 = _locXAddr[index1];
		// double nodeY1 = _locYAddr[index1];
		// double distNode1;

		// double pX = cos(cellAngle); // to be added as cell property later, 
        // double pY = sin(cellAngle);
		// double projLen = (nodeX - Cell_CenterX)*pX + (nodeY - Cell_CenterY)*pY;
		// double projLen;
		double tempCellRad = 1.25;
		double twoPi = 2.0*acos(-1.0);
		double randAngle;
		double randLen;
		double lambda = 10; // parameter for the exponential distribution
		thrust::default_random_engine rng(_seed+index);
    	thrust::uniform_real_distribution<double> u01(0.0, 1.0);
		thrust::uniform_real_distribution<double> u0TwoPi(0.0, twoPi);
		rng.discard(index+_seed);

		uint bindSiteCounts = 0;
		// double posBind = 0.0; // initialize probability for binding and unbinding 
		// double posUnbind = 0.0; 
		double siteX;
		double siteY;
		uint siteIndex;
		double charUnbindDist = 0.175;
		double distNodeSite;
		double adhForceX = 0.0;
		double adhForceY = 0.0;
		double kAdh;
		double siteBindThreshold;
		double charMyosin = 20.0; // maybe this should be the same as the myosinTarget???
		
		double randomN1;
		double randomN2;
		double randomN3;
		double ngbrSiteX;
		double ngbrSiteY;
		double nodeMyosinThreshold = 5;
		double minDistIntl=100;
		double nodeXTemp, nodeYTemp, tempdistMI;
		double charMIntlDist=0.8;
		uint slotSubSitePerNode=10; // this is used in defining the size of the _subAdhIsBoundAddr variable
		uint maxSubSitePerNode;
		if (cellType != 1 && _nodeActLevelAddr[index]>0){kAdh = 2;}
		if (cellType == 1){kAdh=2;} // for leader
		if (cellType == 1){// including leader cell and cells adhere to it //_cellActLevelAddr[cellRank] == 1
			maxSubSitePerNode = 10;
		} else {
			maxSubSitePerNode = 10;
		}
		
		// if (_timeNow > 55800.0 && _isActiveAddr[index] == true && (nodeRank < _maxMemNodePerCell)) {
		if (_timeNow > 55800.0 && _isActiveAddr[index] == true) {
			if (nodeRank < _maxMemNodePerCell && cellType == 1){ // nodeMyosin >0 exclude follower cells
				for (uint tempNodeRank=intnlIndxBegin; tempNodeRank<intnlIndxEnd;tempNodeRank++){ // internal nodes 
					nodeXTemp = _locXAddr[tempNodeRank];
					nodeYTemp = _locYAddr[tempNodeRank];
					tempdistMI = compDist2D(nodeX,nodeY,nodeXTemp,nodeYTemp);
					if (tempdistMI<minDistIntl) {
						minDistIntl = tempdistMI;}// minimal distance to internal node
				}
			}
			// starting of the index of the substrate site corresponding to this node is: index*10, 10 is the max subs sites
			for (int bindSiteIndex = 0; bindSiteIndex < maxSubSitePerNode; 
					bindSiteIndex++) {
						bindSiteCounts += _subAdhIsBoundAddr[index*slotSubSitePerNode + bindSiteIndex];
					}
			// Perform the unbinding and binding events by adding the location of the site to the siteXY vector 
			if (bindSiteCounts > 0){
				// means there are possibilities for unbinding, compute the unbinding probability 
				for (int bindSiteIndex = 0; bindSiteIndex < maxSubSitePerNode; bindSiteIndex++){
					randomN1 = u01(rng);
					siteIndex = index*slotSubSitePerNode + bindSiteIndex; 
					if (_subAdhIsBoundAddr[siteIndex] == 1){
						siteX = _subAdhLocXAddr[siteIndex];
						siteY = _subAdhLocYAddr[siteIndex];
						distNodeSite = compDist2D(nodeX, nodeY, siteX, siteY);  
						if (randomN1 < (1.0-exp(-distNodeSite/charUnbindDist-nodeMyosin/charMyosin))){ // check: make sure node myosin level is > 0
							// _subAdhLocXAddr = ; // -minDistIntl/charMIntlDist
							// _subAdhLocYAddr = ;
							_subAdhIsBoundAddr[siteIndex] = false;
						}
					}
				}
			}
			if (bindSiteCounts < maxSubSitePerNode){
				// compute possibility for binding to an arbitrary site 
				// 
				for (int allNbrIndex = 0; allNbrIndex < (maxSubSitePerNode-bindSiteCounts); allNbrIndex++){
					randomN2= u01(rng);
					randomN3 = u01(rng);
					if (cellType == 1) {
						// randAngle = u01(rng)*twoPi;
						randAngle = curCellAngle;// + (2.0*u01(rng)-1.0)*twoPi/4.0;
						}
					else {
						randAngle = curCellAngle;// + (2.0*u01(rng)-1.0)*twoPi/4.0; // towards the direction of cell polarity 
					}
					randLen = -logf(1-randomN2)/lambda;
					/*
					if (nodeRank < _maxMemNodePerCell && cellType==1 && minDistIntl>0.8 && minDistIntl<0.7) { // the >0 condition is to avoid this force in follower cells //   && nodeMyosin < nodeMyosinThreshold
						// distNode1 = compDist2D(nodeX1, nodeY1, nodeX, nodeY);
						// if (distNode1>5&distNode1<8){
						// define the last active membrane node, double check here
						// if (minDistIntl>0.8){randomN3 = 1;}

						index_left = nodeRank - 1;
						if (index_left == -1) {
							index_left = activeMembrCount - 1;
						}
						index_left = index_left + cellRank * _maxNodePerCell;

						index_right = nodeRank + 1;
						if (index_right == (int) activeMembrCount) {
							index_right = 0;
						}
						index_right = index_right + cellRank * _maxNodePerCell;
						// double check above 
						normX = -(_locYAddr[index_left] - _locYAddr[index_right] );
						normY = _locXAddr[index_left] - _locXAddr[index_right]; // did not know the direction of the normal vector 
						randAngle =  atan2(normY,normX) + (2*u01(rng)-1)*twoPi/8; // double check here 
						// } 
					}
					*/
					ngbrSiteX = nodeX + randLen*cos(randAngle); // compute the X location of the neighboring site
					ngbrSiteY = nodeY + randLen*sin(randAngle);
					// projLen = (ngbrSiteX - Cell_CenterX)*pX + (ngbrSiteY - Cell_CenterY)*pY; 
					// if (nodeRank < _maxMemNodePerCell) {
						// siteBindThreshold = 1-exp(-projLen/tempCellRad);
					//} else{
						if (cellType==1){
							siteBindThreshold = 0.5; // means leader
						} else if (cellType==0){
							siteBindThreshold = 0.5; // means follower
						}
					// }
					if (randomN3<siteBindThreshold) { 
						for (int bindSiteIndex = 0; bindSiteIndex < maxSubSitePerNode; bindSiteIndex++){
							siteIndex = index*slotSubSitePerNode + bindSiteIndex; 
							if (_subAdhIsBoundAddr[siteIndex] == 0){
								_subAdhLocXAddr[siteIndex] = ngbrSiteX; 
								_subAdhLocYAddr[siteIndex] = ngbrSiteY;
								_subAdhIsBoundAddr[siteIndex] = true;
								break;
							}
						}
					}
				}
			}
			// Compute the updated force depending on updated distance for each binding site
			for (int bindSiteIndex = 0; bindSiteIndex < maxSubSitePerNode; bindSiteIndex++){
				siteIndex = index*slotSubSitePerNode + bindSiteIndex; 
				if (_subAdhIsBoundAddr[siteIndex] == 1){
						siteX = _subAdhLocXAddr[siteIndex];
						siteY = _subAdhLocYAddr[siteIndex];
						distNodeSite = compDist2D(nodeX, nodeY, siteX, siteY);  
						if (distNodeSite > 0.0){
							adhForceX = (siteX - nodeX)/distNodeSite;
							adhForceY = (siteY - nodeY)/distNodeSite;
							velX += kAdh * distNodeSite * adhForceX;
							velY += kAdh * distNodeSite * adhForceY;
						}
					}
				}
		return thrust::make_tuple(velX, velY);
	} else {
			return thrust::make_tuple(velX, velY); // determines return type !!!
		}
	}
};



// To do list:
// 1. adhesion to all internal nodes independent of P, random binding; check 
// 2. add force = k*grad(m)+ adh on all internal nodes (how to compute grad(m)?), 
// 3. add randomness to the removing and adding nodes function (keep the total number of internal nodes in some range), unbinding depending on myosin level 

struct initMyosinLevelDevice: public thrust::unary_function<UUDDUUi, double> {
	uint _maxNodePerCell;
	uint _maxMemNodePerCell;
	double* _locXAddr;
	double* _locYAddr;
	bool* _isActiveAddr;
	double* _myosinLevelAddr;

	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ initMyosinLevelDevice(uint maxNodePerCell,
			uint maxMemNodePerCell, double* locXAddr, double* locYAddr,
			bool* isActiveAddr, double* myosinLevelAddr) :
			_maxNodePerCell(maxNodePerCell), _maxMemNodePerCell(
					maxMemNodePerCell), _locXAddr(locXAddr), _locYAddr(
					locYAddr), _isActiveAddr(isActiveAddr), _myosinLevelAddr(myosinLevelAddr) {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ double operator()(const UUDDUUi &cData) const {
		uint activeMembrCount = thrust::get<0>(cData);
		uint activeIntnlCount = thrust::get<1>(cData);
		double Cell_CenterX = thrust::get<2>(cData);
		double Cell_CenterY = thrust::get<3>(cData);
		uint cellRank = thrust::get<4>(cData);
		uint nodeRank = thrust::get<5>(cData);
		int cellType = thrust::get<6>(cData);
		double nodeMyosin;
		uint index = cellRank * _maxNodePerCell + nodeRank;
        

		// uint intnlIndxMemBegin = cellRank * _maxNodePerCell;
		// uint intnlIndxBegin = cellRank * _maxNodePerCell + _maxMemNodePerCell;
		// uint intnlIndxEnd = intnlIndxBegin + activeIntnlCount;
		// uint index_other;
		double nodeX = _locXAddr[index];
		double nodeY = _locYAddr[index];
		// double nodeXOther, nodeYOther;
		// double baseMyosin = 0.0;
		// double gradientMyosin = 4.0;
		double pX = 1.0;
		double pY = 0.0;
		// double distNodes = 0.0;
		// double myosinOther;
		// double myosinDiffer;
		// double kDiff = 1.0;

		if (cellType == 0){
			if (nodeRank < _maxMemNodePerCell) {
				nodeMyosin = 0;
			} else {
				nodeMyosin = 0;
			}
		} else {
			if (nodeRank < _maxMemNodePerCell) {
				nodeMyosin =0;
			} else {
				if (_isActiveAddr[index] == false) {
					nodeMyosin = 0;
				} else {
					nodeMyosin = 0.3; // if this is changed, also change myosinThrd in 2 places 
				}
			}
		}
		// means membrane node
		//Because we want to compute the force on the membrane nodes we modify this function 
			
			// means internal node
			// assign initial value of myosin in the following block 
			// gradient is along to the potive direction of y-axis
			// nodeMyosin = baseMyosin + (nodeY-Cell_CenterY)*gradientMyosin; // dot prodect, 
			// nodeMyosin = -((nodeX-Cell_CenterX)*pX + (nodeY-Cell_CenterY)*pY) + baseMyosin;  
		
		return nodeMyosin;
	}
};



/**
 * Obtain growth speed and direction given node position.
 * @param _gridDimensionX number of grid points in x direction
 * @param _gridDimensionY number of grid points in y direction
 * @param _gridSpacing spacing of the chemical signal mesh.
 * @param _gridMagValue begin address of growth speed vector
 * @param _gridDirXCompValue begin address of growth direction x component vector
 * @param _gridDirYCompValue begin address of growth direction y component vector
 *
 * @param input1 x coordinate of node position
 * @param input2 y coordinate of node position
 *
 * @return output1 growth speed \n
 *         output2 x component of growth direction \n
 *         output3 y component of growth direction \n
 *
 */
struct LoadGridDataToNode: public thrust::unary_function<CVec2, CVec3> {
	uint _gridDimensionX;
	uint _gridDimensionY;
	double _gridSpacing;
	double* _gridMagValue;
	double* _gridDirXCompValue;
	double* _gridDirYCompValue;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight__host__ __device__
	__host__ __device__ LoadGridDataToNode(uint gridDimensionX,
			uint gridDimensionY, double gridSpacing, double* gridMagValue,
			double* gridDirXCompValue, double* gridDirYCompValue) :
			_gridDimensionX(gridDimensionX), _gridDimensionY(gridDimensionY), _gridSpacing(
					gridSpacing), _gridMagValue(gridMagValue), _gridDirXCompValue(
					gridDirXCompValue), _gridDirYCompValue(gridDirYCompValue) {
	}
	__host__ __device__ CVec3 operator()(const CVec2 &d2) const {
		double xCoord = thrust::get<0>(d2);
		double yCoord = thrust::get<1>(d2);
		uint gridLoc = (uint) (xCoord / _gridSpacing)
				+ (uint) (yCoord / _gridSpacing) * _gridDimensionX;
		double magRes = _gridMagValue[gridLoc];
		double xDirRes = _gridDirXCompValue[gridLoc];
		double yDirRes = _gridDirYCompValue[gridLoc];
		return thrust::make_tuple(magRes, xDirRes, yDirRes);
	}
};

/**
 * Obtain growth speed and direction given node position and chemical field.
 * @param _gridDimensionX number of grid points in x direction
 * @param _gridDimensionY number of grid points in y direction
 * @param _gridSpacing spacing of the chemical signal mesh.
 * @param _gridMagValue begin address of growth speed vector
 * @param _gridDirXCompValue begin address of growth direction x component vector
 * @param _gridDirYCompValue begin address of growth direction y component vector
 *
 * @param input1 x coordinate of node position
 * @param input2 y coordinate of node position
 * @param input3 type of the cell. Could be boundary, FNM or MX
 *
 * @return output1 growth speed \n
 *         output2 x component of growth direction \n
 *         output3 y component of growth direction \n
 *
 */
struct LoadChemDataToNode: public thrust::unary_function<CVec2Type, CVec3> {
	uint _gridDimensionX;
	uint _gridDimensionY;
	double _gridSpacing;
	double* _gridMagValue;
	double* _gridDirXCompValue;
	double* _gridDirYCompValue;
	uint _gridDimensionX2;
	uint _gridDimensionY2;
	double _gridSpacing2;
	double* _gridMagValue2;
	double* _gridDirXCompValue2;
	double* _gridDirYCompValue2;

	__host__ __device__ LoadChemDataToNode(uint gridDimensionX,
			uint gridDimensionY, double gridSpacing, double* gridMagValue,
			double* gridDirXCompValue, double* gridDirYCompValue,
			uint gridDimensionX2, uint gridDimensionY2, double gridSpacing2,
			double* gridMagValue2, double* gridDirXCompValue2,
			double* gridDirYCompValue2) :
			_gridDimensionX(gridDimensionX), _gridDimensionY(gridDimensionY), _gridSpacing(
					gridSpacing), _gridMagValue(gridMagValue), _gridDirXCompValue(
					gridDirXCompValue), _gridDirYCompValue(gridDirYCompValue), _gridDimensionX2(
					gridDimensionX2), _gridDimensionY2(gridDimensionY2), _gridSpacing2(
					gridSpacing2), _gridMagValue2(gridMagValue2), _gridDirXCompValue2(
					gridDirXCompValue2), _gridDirYCompValue2(gridDirYCompValue2) {
	}
	__host__ __device__
	// place holder for eclipse formatter
	CVec3 operator()(const CVec2Type &d2) const {
		double xCoord = thrust::get<0>(d2);
		double yCoord = thrust::get<1>(d2);
		SceNodeType type = thrust::get<2>(d2);
		uint gridLoc = (uint) (xCoord / _gridSpacing)
				+ (uint) (yCoord / _gridSpacing) * _gridDimensionX;
		if (type == FNM) {
			double magRes = _gridMagValue[gridLoc];
			double xDirRes = _gridDirXCompValue[gridLoc];
			double yDirRes = _gridDirYCompValue[gridLoc];
			return thrust::make_tuple(magRes, xDirRes, yDirRes);
		} else if (type == MX) {
			double magRes = _gridMagValue2[gridLoc];
			double xDirRes = _gridDirXCompValue2[gridLoc];
			double yDirRes = _gridDirYCompValue2[gridLoc];
			return thrust::make_tuple(magRes, xDirRes, yDirRes);
		} else {
			return thrust::make_tuple(0.0, 1.0, 0.0);
		}

	}
};

/**
 * One dimensional version of a*X plus Y.
 * @param input1 X
 * @param input2 Y
 *
 * @return output1 a*X+Y
 */
struct SaxpyFunctor: public thrust::binary_function<double, double, double> {
	double _dt;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight__host__ __device__
	__host__ __device__ SaxpyFunctor(double dt) :
			_dt(dt) {
	}
	__host__ __device__
	double operator()(const double &x, const double &y) {
		return _dt * x + y;
	}
};

struct MultiWithLimit: public thrust::unary_function<double, double> {
	double _k, _bound;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight__host__ __device__
	__host__ __device__ MultiWithLimit(double k, double bound) :
			_k(k), _bound(bound) {
	}
	__host__ __device__
	double operator()(const double &x) {
		if (x <= 0) {
			return 0;
		}
		double tmpRes = x * _k;
		if (tmpRes < _bound) {
			return tmpRes;
		} else {
			return _bound;
		}
	}
};

struct AdjustMembrGrow: public thrust::unary_function<UiDD, double> {
	/*
	 * Growth speed set to be constant when growth is necessary.
	 */
	double _constSpeed;

	/*
	 * Initial count of membrane elements
	 */
	uint _initMembrCt;

	/*
	 * Initial count of internal elements
	 */
	uint _initIntnlCt;

	__host__ __device__ //place holder for eclipse formatter
	AdjustMembrGrow(double constSpeed, uint initMembrCt, uint initIntnlCt) :
			_constSpeed(constSpeed), // place holder for eclipse formatter
			_initMembrCt(initMembrCt), // place holder for eclipse formatter
			_initIntnlCt(initIntnlCt) {
	}
	__device__
	double operator()(const UiUi &input) const {
		uint curActiveMembrNode = thrust::get<0>(input);
		uint curActiveIntnlNode = thrust::get<1>(input);

		// Force conversion of double to integer
		uint targetMembrCt = sqrt(double(curActiveIntnlNode) / _initIntnlCt)
				* _initMembrCt;

		if (curActiveMembrNode < targetMembrCt) {
			return _constSpeed;
		} else {
			return 0;
		}
	}
};




//Ali struct MemGrowFunc: public thrust::unary_function<DUi, BoolD> {
struct MemGrowFunc: public thrust::unary_function<UiDD, BoolD> {
	uint _bound;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight__host__ __device__
	__host__ __device__ MemGrowFunc(uint bound) :
			_bound(bound) {
	}
	//Ali __host__ __device__ BoolD operator()(const DUi& dui) {
	__host__ __device__ BoolD operator()(const UiDD& uidd) {
		//Ali double progress = thrust::get<0>(dui);
		uint   curActiveMembrNode = thrust::get<0>(uidd); //Ali
		double progress = thrust::get<1>(uidd); //Ali
        double LengthMax=thrust::get<2>(uidd); //Ali
		//Ali uint curActiveMembrNode = thrust::get<1>(dui);
		if (curActiveMembrNode < _bound && progress >= 1.0 && LengthMax>0.0975 ) {
			return thrust::make_tuple(true, 0);
		} else {
			return thrust::make_tuple(false, progress);
		}
	}
};

/**
 * One dimensional version of a*X plus Y, return one if result is larger than one.
 * @param input1 X
 * @param input2 Y
 *
 * @return output1 a*X+Y
 */
struct SaxpyFunctorWithMaxOfOne: public thrust::binary_function<double, double,
		double> {
	double _dt;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight__host__ __device__
	SaxpyFunctorWithMaxOfOne(double dt) :
			_dt(dt) {
	}
	__host__ __device__
	double operator()(const double &x, const double &y) {
		double result = _dt * x + y;
		if (result > 1.0) {
			return 1.0;
		} else {
			return result;
		}
	}
};

/**
 * Two dimensional version of a*X plus Y.
 * @param input1 x and y components of X
 * @param input2 x and y components of Y
 *
 * @return output1 x and y compoents of result
 */
struct SaxpyFunctorDim2: public thrust::binary_function<CVec2, CVec2, CVec2> {
	double _dt;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight__host__ __device__
	__host__ __device__ SaxpyFunctorDim2(double dt) :
			_dt(dt) {
	}
	__host__ __device__ CVec2 operator()(const CVec2 &vec1, const CVec2 &vec2) {
		double xRes = thrust::get<0>(vec1) * _dt + thrust::get<0>(vec2);
		double yRes = thrust::get<1>(vec1) * _dt + thrust::get<1>(vec2);
		return thrust::make_tuple(xRes, yRes);
	}
};

//Ali
struct SaxpyFunctorDim2_Damp: public thrust::binary_function<CVec2, CVec2, CVec2> {
        double _dt;
        double _DampCoef ; 
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight__host__ __device__
	__host__ __device__ SaxpyFunctorDim2_Damp(double dt, double Damp_Coef) :
                        _dt(dt), _DampCoef(Damp_Coef) 
                        {
	}
	__host__ __device__ CVec2 operator()(const CVec2 &vec1, const CVec2 &vec2) {
		double xRes = thrust::get<0>(vec1) * _dt/_DampCoef + thrust::get<0>(vec2);
		double yRes = thrust::get<1>(vec1) * _dt/_DampCoef + thrust::get<1>(vec2);
		return thrust::make_tuple(xRes, yRes);
	}
};


struct SaxpyFunctorDim2_BC_Damp: public thrust::binary_function<CVec3, CVec2, CVec2> {
        double _dt;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight__host__ __device__
	__host__ __device__ SaxpyFunctorDim2_BC_Damp(double dt) :
                        _dt(dt) 
                        {
	}
	__host__ __device__ CVec2 operator()(const CVec3 &vec1, const CVec2 &vec2) {
		double xRes = thrust::get<1>(vec1) * _dt/thrust::get<0>(vec1) + thrust::get<0>(vec2);
		double yRes = thrust::get<2>(vec1) * _dt/thrust::get<0>(vec1) + thrust::get<1>(vec2);
		return thrust::make_tuple(xRes, yRes);
	}
};



// define a new function and make sure every internal node is inside the corresponding cell after applying forces
/*
struct SaxpyFunctorDim2_BC_Damp: public thrust::binary_function<CVec5UUU, CVec2, CVec2> {
        double _dt;
		double* _locXAddr;
    	double* _locYAddr;
		bool* _isActiveAddr;
		uint _maxNodePerCell;
		uint _maxMemNodePerCell;
		int _timeStep;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight__host__ __device__
	__host__ __device__ SaxpyFunctorDim2_BC_Damp(double dt,double* locXAddr, double* locYAddr, bool* isActiveAddr,
						uint maxNodePerCell, uint maxMemNodePerCell, int timeStep) :
                        _dt(dt),_locXAddr(locXAddr), _locYAddr(locYAddr),_isActiveAddr(isActiveAddr),
						_maxNodePerCell(maxNodePerCell), _maxMemNodePerCell(maxMemNodePerCell), _timeStep(timeStep)
                        {
	}
	__device__ CVec2 operator()(const CVec5UUU &vec1, const CVec2 &vec2) {
		double xOld = thrust::get<0>(vec2);
		double yOld = thrust::get<1>(vec2);
		double centerX = thrust::get<3>(vec1);
		double centerY = thrust::get<4>(vec1);
		uint   cellRank = thrust::get<5>(vec1);
		uint   nodeRank = thrust::get<6>(vec1);
		uint activeMembrCount = thrust::get<7>(vec1);
		uint index = cellRank * _maxNodePerCell + nodeRank;
		uint intnlIndxMemBegin = cellRank * _maxNodePerCell;

		double xRes = thrust::get<1>(vec1) * _dt/thrust::get<0>(vec1) + xOld;
		double yRes = thrust::get<2>(vec1) * _dt/thrust::get<0>(vec1) + yOld;
		double rOffset = 0.00001;
        if (_isActiveAddr[index] == false) {
            return thrust::make_tuple(xOld, yOld); 
        }

		if (_timeStep%100==0){
			if (nodeRank < _maxMemNodePerCell){ // means membrane node
					return thrust::make_tuple(xRes, yRes);
			}else{ //means internal node
				if (isInsideCell(xRes,yRes,intnlIndxMemBegin,activeMembrCount,_locXAddr,_locYAddr)){
					return thrust::make_tuple(xRes, yRes);
				}
				else{
					double xResRad = centerX - xRes;
					double yResRad = centerY - yRes;
					double lenResRad = sqrt(xResRad*xResRad+yResRad*yResRad);
					double fcenter = 0.01;
					xRes = centerX+rOffset*(-xResRad)/lenResRad; //xRes + fcenter*xResRad/lenResRad;
					yRes = centerY+rOffset*(-yResRad)/lenResRad; //yRes + fcenter*yResRad/lenResRad;
					return thrust::make_tuple(xRes,yRes);
				}
			}
		}else{
			return thrust::make_tuple(xRes, yRes);
		}
	}
};
*/




/**


 * Point condition operater, decide if cell is ready to add a new point.
 * @param _threshold threshold value for difference of current progress and last checkpoint.
 * if difference is bigger than threshold then the cell is ready for adding a new node.
 * @param input1 growth progress 
 * @param input2 last check point 
 * @return output1 is the cell going to add one more node?
 * @return output2 updated check point value (change or unchanged)
 */
struct decideIsScheduleToGrow_MDevice: public thrust::unary_function<DDI, bool> {
	double _threshold;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight__host__ __device__
	__host__ __device__ decideIsScheduleToGrow_MDevice(double threshold) :
			_threshold(threshold) {
	}
	__host__ __device__
	bool operator()(const DDI &d2) const {
		double progress = thrust::get<0>(d2);
		double lastCheckPoint = thrust::get<1>(d2);
		int cell_Type = thrust::get<2>(d2);
		bool resBool = false;
		// cell_Type == 0, follower
		// cell_Type == 1, leader
		if (progress == 1.0 && lastCheckPoint < 1.0 && cell_Type == 0) {
			resBool = true; 
		}
		if (progress - lastCheckPoint >= _threshold && cell_Type == 0) {
			resBool = true; 
		}
		return resBool;
	}
};



//AAMIRI May5

struct isDelOp: public thrust::unary_function<UiDDBool, bool> {
	double _laserCenterX;
	double _laserCenterY;
	double _ablationRadius;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight__host__ __device__
	__host__ __device__ isDelOp(double laserCenterX, double laserCenterY, double ablationRadius) :
			_laserCenterX(laserCenterX), _laserCenterY(laserCenterY), _ablationRadius(ablationRadius) {
	}
	__host__ __device__
	bool operator()(const UiDDBool &d2) const {

		uint cellRank = thrust::get<0>(d2);
		double CellCenterX = thrust::get<1>(d2);
		double CellCenterY = thrust::get<2>(d2);
		bool resBool = thrust::get<3>(d2);

		if (resBool == true){
			return resBool;}

		double distToLaser = sqrt( (_laserCenterX-CellCenterX)*(_laserCenterX-CellCenterX) + (_laserCenterY-CellCenterY)*(_laserCenterY-CellCenterY) );
		if (distToLaser < _ablationRadius){
		    resBool = true;}

		return resBool;
	}
};


/**
 * return zero given a celltype
 */
struct GetZero: public thrust::unary_function<SceNodeType, double> {
	__host__ __device__
	double operator()(const SceNodeType &type) {
		return 0.0;
	}
};
/**
 * determines if cell type is boundary.
 */
struct IsBoundary: public thrust::unary_function<SceNodeType, bool> {
	__host__ __device__ uint operator()(const SceNodeType &type) {
		if (type == Boundary) {
			return true;
		} else {
			return false;
		}
	}
};

/**
 * Unary opterator for adding new node in the cell.
 * BoolUIDDUI consists of the following:
 *
 * (1) - (Bool,bool) is this cell scheduled to grow?
 *
 * (2) - (UI,unsigned integer) how many active nodes are there in this cell?
 * we need this input to decide where should we place the coordinate information of the new node
 *
 * (3) - (D, double) x coordinate of the cell center
 *
 * (4) - (D, double) y coordinate of the cell center
 * we need these two cell center coordinates because cuda only has a pseduo-random number generator,
 * so we need to obtain a good seed to generate a random number. Here we choose center position of the cell.
 *
 * (5) - (UI, unsigned integer) rank of the cell
 *
 * BoolUI consists of the following:
 *
 * (1) - (Bool,bool) if operation succeed, this will return 0. otherwise, return 1
 *
 * (2) - (UI,unsigned integer) how many active nodes are there in this cell? if operation succeed,
 * this will input active node count + 1. otherwise, return input active node count
 *
 * @param _maxNodeOfOneCell Maximum node count of a cell.
 * @param _addNodeDistance  While adding a node, we need to set a fixed distance as radius of the circle
 *        that we would like to add a point.
 * @param _minDistanceToOtherNode Minimum distance of the newly added point to any other node.
 *        If the distance of the newly added node is greater than this min distance,
 *        the add operation will fail and the method will change nothing.
 * @param _nodeIsActiveAddress pointer to the begining of vector nodeIsActive of SceNodes
 * @param _nodeXPosAddress pointer to the begining of vector nodeLocX of SceNodes
 * @param _nodeYPosAddress pointer to the begining of vector nodeLocY of SceNodes
 */
struct AddPtOp: thrust::unary_function<BoolUIDDUID, BoolUID> {
	uint _maxNodeOfOneCell;
	double _addNodeDistance;
	double _minDistanceToOtherNode;
	bool* _nodeIsActiveAddress;
	double* _nodeXPosAddress;
	double* _nodeYPosAddress;
	double _growThreshold;

	unsigned int m_seed;

	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ AddPtOp(uint maxNodeOfOneCell, double addNodeDistance,
			double minDistanceToOtherNode, bool* nodeIsActiveAddress,
			double* nodeXPosAddress, double* nodeYPosAddress, uint seed,
			double growThreshold) :
			_maxNodeOfOneCell(maxNodeOfOneCell), _addNodeDistance(
					addNodeDistance), _minDistanceToOtherNode(
					minDistanceToOtherNode), _nodeIsActiveAddress(
					nodeIsActiveAddress), _nodeXPosAddress(nodeXPosAddress), _nodeYPosAddress(
					nodeYPosAddress), _growThreshold(growThreshold), m_seed(
					seed) {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ BoolUID operator()(const BoolUIDDUID &biddi) {
		const double pI = acos(-1.0);
		bool isScheduledToGrow = thrust::get<0>(biddi);
		uint activeNodeCountOfThisCell = thrust::get<1>(biddi);
		double lastCheckPoint = thrust::get<5>(biddi);
		if (isScheduledToGrow == false) {
			return thrust::make_tuple(isScheduledToGrow,
					activeNodeCountOfThisCell, lastCheckPoint);
		}
		double cellCenterXCoord = thrust::get<2>(biddi);
		double cellCenterYCoord = thrust::get<3>(biddi);
		uint cellRank = thrust::get<4>(biddi);

		bool isSuccess = true;

		thrust::default_random_engine rng(cellRank+m_seed);

		// discard n numbers to avoid correlation
		rng.discard(cellRank+m_seed);

		thrust::uniform_real_distribution<double> u0TwoPi(0, 2.0 * pI);
		double randomAngle = u0TwoPi(rng);
		double xOffset = _addNodeDistance * cos(randomAngle);
		double yOffset = _addNodeDistance * sin(randomAngle);
		double xCoordNewPt = cellCenterXCoord + xOffset;
		double yCoordNewPt = cellCenterYCoord + yOffset;
		uint cellNodeStartPos = cellRank * _maxNodeOfOneCell;
		uint cellNodeEndPos = cellNodeStartPos + activeNodeCountOfThisCell;
		for (uint i = cellNodeStartPos; i < cellNodeEndPos; i++) {
			double distance = sqrt(
					(xCoordNewPt - _nodeXPosAddress[i])
							* (xCoordNewPt - _nodeXPosAddress[i])
							+ (yCoordNewPt - _nodeYPosAddress[i])
									* (yCoordNewPt - _nodeYPosAddress[i]));
			if (distance < _minDistanceToOtherNode) {
				isSuccess = false;
				break;
			}
		}

		if (isSuccess) {
			_nodeXPosAddress[cellNodeEndPos] = xCoordNewPt;
			_nodeYPosAddress[cellNodeEndPos] = yCoordNewPt;
			_nodeIsActiveAddress[cellNodeEndPos] = true;
			isScheduledToGrow = false;
			activeNodeCountOfThisCell = activeNodeCountOfThisCell + 1;
			lastCheckPoint = lastCheckPoint + _growThreshold;
			if (lastCheckPoint > 1.0) {
				lastCheckPoint = 1.0;
			}
		}
		return thrust::make_tuple(isScheduledToGrow, activeNodeCountOfThisCell,
				lastCheckPoint);
	}

};

struct addPointIfScheduledToGrow_MDevice: thrust::unary_function<BoolUIDDUID, DUi> {
	uint _seed;
	double _addNodeDistance;
	double _growThreshold;
	double* _nodeXPosAddress;
	double* _nodeYPosAddress;
	bool* _nodeIsActiveAddress;

	uint _maxNodePerCell;
    uint _maxMemNodePerCell;
	double* _myosinLevelAddr;

	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ addPointIfScheduledToGrow_MDevice(uint seed, double addNodeDistance,
			double growThreshold, double* nodeXPosAddress,
			double* nodeYPosAddress, bool* nodeIsActiveAddress, 
			uint maxNodePerCell, uint maxMemNodePerCell, double* myosinLevelAddr) :
			_seed(seed), _addNodeDistance(addNodeDistance), _growThreshold(
					growThreshold), _nodeXPosAddress(nodeXPosAddress), _nodeYPosAddress(
					nodeYPosAddress), _nodeIsActiveAddress(nodeIsActiveAddress), 
					_maxNodePerCell(maxNodePerCell), _maxMemNodePerCell(maxMemNodePerCell), _myosinLevelAddr(myosinLevelAddr) {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ DUi operator()(const BoolUIDDUID &biddi) {
		bool isScheduledToGrow = thrust::get<0>(biddi);
		uint activeIntlNodeThis = thrust::get<1>(biddi);
		double lastCheckPoint = thrust::get<5>(biddi);

		bool isFull = isAllIntnlFilled(activeIntlNodeThis);
		if (!isScheduledToGrow || isFull) {
			return thrust::make_tuple(lastCheckPoint, activeIntlNodeThis);
		}

		double cellCenterXCoord = thrust::get<2>(biddi);
		double cellCenterYCoord = thrust::get<3>(biddi);
		uint cellRank = thrust::get<4>(biddi);
		double randomAngle = obtainRandAngle(cellRank, _seed);
		double xOffset = _addNodeDistance * cos(randomAngle);
		double yOffset = _addNodeDistance * sin(randomAngle);
		double xCoordNewPt = cellCenterXCoord + xOffset;
		double yCoordNewPt = cellCenterYCoord + yOffset;

		uint cellNodeEndPos = obtainNewIntnlNodeIndex(cellRank,
				activeIntlNodeThis);
		_nodeXPosAddress[cellNodeEndPos] = xCoordNewPt;
		_nodeYPosAddress[cellNodeEndPos] = yCoordNewPt;
		_nodeIsActiveAddress[cellNodeEndPos] = true;
		// add myosin to new node here
		//
		uint intnlIndxBegin = cellRank * _maxNodePerCell + _maxMemNodePerCell;
        uint intnlIndxEnd = cellNodeEndPos; // double check this.  bcs of this index_intnl < intnlIndxEnd command, this is okay?
		uint index_intnl;
		double nodeXIntnl;
		double nodeYIntnl;
		double distNodes;
		// double shortestDistantce = 100;
		double distThreshold = 1;
		double nodeMyosin;
		// uint indexClosestNeighbr;
		double totalNeighbrMyosin;
		uint neighbrNodesCounts = 0;
		uint index_neighbr; // 
		const uint tempTotalNeighbr = 200; // double check this, JG040723, cannot use _maxNodePerCell as it is not known at compile time
		uint neighbrNodesIndex[tempTotalNeighbr]; 
        for (index_intnl = intnlIndxBegin; index_intnl < intnlIndxEnd;
                    index_intnl++) {
                nodeXIntnl = _nodeXPosAddress[index_intnl];
                nodeYIntnl = _nodeYPosAddress[index_intnl]; 
                distNodes = compDist2D(xCoordNewPt, yCoordNewPt, nodeXIntnl, nodeYIntnl);
                if (distNodes < distThreshold) { 
					neighbrNodesIndex[neighbrNodesCounts] = index_intnl;
					totalNeighbrMyosin += _myosinLevelAddr[index_intnl];
					neighbrNodesCounts += 1;
		   			// indexClosestNeighbr = index_intnl;
                    // shortestDistantce = distNodes;
                    }
            }
		nodeMyosin = 0.0; // totalNeighbrMyosin/(neighbrNodesCounts*1.0); // change here
		for (index_neighbr = 0; index_neighbr<neighbrNodesCounts;
				index_neighbr++) {
				_myosinLevelAddr[neighbrNodesIndex[index_neighbr]] = nodeMyosin;
				}
		// nodeMyosin = _myosinLevelAddr[indexClosestNeighbr]/2.0;
		// _myosinLevelAddr[indexClosestNeighbr] = nodeMyosin;
		// _myosinLevelAddr[cellNodeEndPos] = nodeMyosin;
		_myosinLevelAddr[cellNodeEndPos] = nodeMyosin;
		//
		activeIntlNodeThis = activeIntlNodeThis + 1;
		lastCheckPoint = lastCheckPoint + _growThreshold;
		if (lastCheckPoint > 1.0) {
			lastCheckPoint = 1.0;
		}

		return thrust::make_tuple(lastCheckPoint, activeIntlNodeThis);
	}

};







// JG041123
// add and delete one node
/*
struct AddDelPtDueToActinTemp: thrust::unary_function<BoolUIDDUID, DUi> {
	uint _seed;
	double _addNodeDistance;
	double _growThreshold;
	double* _nodeXPosAddress;
	double* _nodeYPosAddress;
	bool* _nodeIsActiveAddress;

	uint _maxNodePerCell;
    uint _maxMemNodePerCell;
	double* _myosinLevelAddr;
	double* _nodeVelXAddr;
	double* _nodeVelYAddr;

	int _timeStep;
	int* _adhIndxAddr;
	double _InitTimeStage;
	double _ddt;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ AddDelPtDueToActinTemp(uint seed, double addNodeDistance,
			double growThreshold, double* nodeXPosAddress,
			double* nodeYPosAddress, bool* nodeIsActiveAddress, 
			uint maxNodePerCell, uint maxMemNodePerCell, double* myosinLevelAddr,
			double* nodeVelXAddr, double* nodeVelYAddr, int timeStep, int* adhIndxAddr, double InitTimeStage, double ddt) :
			_seed(seed), _addNodeDistance(addNodeDistance), _growThreshold(
					growThreshold), _nodeXPosAddress(nodeXPosAddress), _nodeYPosAddress(
					nodeYPosAddress), _nodeIsActiveAddress(nodeIsActiveAddress), 
					_maxNodePerCell(maxNodePerCell), _maxMemNodePerCell(maxMemNodePerCell), 
					_myosinLevelAddr(myosinLevelAddr), _nodeVelXAddr(nodeVelXAddr), _nodeVelYAddr(nodeVelYAddr),
					_timeStep(timeStep), _adhIndxAddr(adhIndxAddr), _InitTimeStage(InitTimeStage), _ddt(ddt) {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight 
	__device__ DUi operator()(const BoolUIDDUID &biddi) {
		bool isScheduledToGrow = thrust::get<0>(biddi);
		uint activeIntnlNodeThis = thrust::get<1>(biddi);
		double lastCheckPoint = thrust::get<5>(biddi);

		bool isFull = isAllIntnlFilled(activeIntnlNodeThis);
		if (isFull) {
			return thrust::make_tuple(lastCheckPoint, activeIntnlNodeThis);
		}

		double cellCenterXCoord = thrust::get<2>(biddi);
		double cellCenterYCoord = thrust::get<3>(biddi);
		uint cellRank = thrust::get<4>(biddi);
		double randomAngle = obtainRandAngle(cellRank, _seed);
		// double xOffset = _addNodeDistance * cos(randomAngle);
		// double yOffset = _addNodeDistance * sin(randomAngle);
		// double xOffset = _addNodeDistance;
		// double yOffset = _addNodeDistance;

		uint cellNodeEndPos = obtainNewIntnlNodeIndex(cellRank,
		 		activeIntnlNodeThis);
		uint intnlIndxBegin = cellRank * _maxNodePerCell + _maxMemNodePerCell;
        uint intnlIndxEnd = cellNodeEndPos; 
		uint index_intnl;
		uint index_highstMyo;
		uint index_lowstMyo;
		double nodeMyosinOldL = 100;
		double nodeMyosinOldH = -100;
		double nodeMyosinCur;
		double nodeXAdd;
		double nodeYAdd;
		double nodeXDel;
		double nodeYDel;
		
		double delta = 0.000001;
		if (activeIntnlNodeThis>1 && _timeStep%10000==0 && _timeStep * _ddt >=_InitTimeStage){ // !isIntnlEmptied
			for (index_intnl = intnlIndxBegin; index_intnl < intnlIndxEnd;
                    index_intnl++) {
					nodeMyosinCur = _myosinLevelAddr[index_intnl];
					if (nodeMyosinCur<nodeMyosinOldL) { // lowest myosin location 
						nodeMyosinOldL = nodeMyosinCur;
						index_lowstMyo = index_intnl;
					}
					if (nodeMyosinCur>=nodeMyosinOldH){
						nodeMyosinOldH = nodeMyosinCur; // highest myosin location 
						index_highstMyo = index_intnl;
					}
					}

			nodeXDel = _nodeXPosAddress[index_highstMyo];
			nodeYDel = _nodeYPosAddress[index_highstMyo];

			nodeXAdd = _nodeXPosAddress[index_lowstMyo];
			nodeYAdd = _nodeYPosAddress[index_lowstMyo]; 

			double vectorDelAddX = nodeXAdd - nodeXDel;
			double vectorDelAddY = nodeYAdd - nodeYDel;
			double lenVectDelAdd = sqrt(vectorDelAddX*vectorDelAddX + vectorDelAddY*vectorDelAddY);
			if (lenVectDelAdd > 0.0){
				vectorDelAddX = vectorDelAddX/lenVectDelAdd;
				vectorDelAddY = vectorDelAddY/lenVectDelAdd;
			}
			double xCoordNewPt = nodeXAdd - _addNodeDistance * vectorDelAddX * cos(randomAngle);
			double yCoordNewPt = nodeYAdd - _addNodeDistance * vectorDelAddY * sin(randomAngle);

			uint cellNodeEndPos = obtainNewIntnlNodeIndex(cellRank,
				activeIntnlNodeThis);
			_nodeXPosAddress[cellNodeEndPos] = xCoordNewPt;
			_nodeYPosAddress[cellNodeEndPos] = yCoordNewPt;
			_nodeIsActiveAddress[cellNodeEndPos] = true;
			cellNodeEndPos = cellNodeEndPos + 1;
		
		for (int m=index_highstMyo; m<cellNodeEndPos; m++){
			_nodeXPosAddress[m] = _nodeXPosAddress[m+1];
			_nodeYPosAddress[m] = _nodeYPosAddress[m+1];
			// _adhIndxAddr[m] = _adhIndxAddr[m+1];
			_nodeIsActiveAddress[m] = _nodeIsActiveAddress[m+1];}

			_nodeXPosAddress[cellNodeEndPos-1] = 0.0 + delta;
			_nodeYPosAddress[cellNodeEndPos-1] = 0.0 + delta;
			_nodeIsActiveAddress[cellNodeEndPos-1] = false;
			// _adhIndxAddr[cellNodeEndPos-1] = -1;
			cellNodeEndPos = cellNodeEndPos - 1;
			}
		// _nodeXPosAddress[index_highstMyo] = xCoordNewPt;
		// _nodeYPosAddress[index_highstMyo] = yCoordNewPt;
		// _nodeVelXAddr[index_highstMyo] = 0.0;
		// _nodeVelYAddr[index_highstMyo] = 0.0;
		// _nodeIsActiveAddress[cellNodeEndPos] = true;

		// activeIntnlNodeThis = activeIntnlNodeThis;
		lastCheckPoint = lastCheckPoint + _growThreshold;
		if (lastCheckPoint > 1.0) {
			lastCheckPoint = 1.0;
		}

		
		return thrust::make_tuple(lastCheckPoint, activeIntnlNodeThis);
	}
};
*/


// add and delete multiple internal nodes
struct AddDelPtDueToActinTemp: thrust::unary_function<BoolUIDDUID, DUi> {
	uint _seed;
	double _addNodeDistance;
	double _growThreshold;
	double* _nodeXPosAddress;
	double* _nodeYPosAddress;
	bool* _nodeIsActiveAddress;

	uint _maxNodePerCell;
    uint _maxMemNodePerCell;
	double* _myosinLevelAddr;
	double* _nodeVelXAddr;
	double* _nodeVelYAddr;

	int _timeStep;
	int* _adhIndxAddr;
	double _InitTimeStage;
	double _ddt;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ AddDelPtDueToActinTemp(uint seed, double addNodeDistance,
			double growThreshold, double* nodeXPosAddress,
			double* nodeYPosAddress, bool* nodeIsActiveAddress, 
			uint maxNodePerCell, uint maxMemNodePerCell, double* myosinLevelAddr,
			double* nodeVelXAddr, double* nodeVelYAddr, int timeStep, int* adhIndxAddr, double InitTimeStage, double ddt) :
			_seed(seed), _addNodeDistance(addNodeDistance), _growThreshold(
					growThreshold), _nodeXPosAddress(nodeXPosAddress), _nodeYPosAddress(
					nodeYPosAddress), _nodeIsActiveAddress(nodeIsActiveAddress), 
					_maxNodePerCell(maxNodePerCell), _maxMemNodePerCell(maxMemNodePerCell), 
					_myosinLevelAddr(myosinLevelAddr), _nodeVelXAddr(nodeVelXAddr), _nodeVelYAddr(nodeVelYAddr),
					_timeStep(timeStep), _adhIndxAddr(adhIndxAddr), _InitTimeStage(InitTimeStage), _ddt(ddt) {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight 
	__device__ DUi operator()(const BoolUIDDUID &biddi) {
		bool isScheduledToGrow = thrust::get<0>(biddi);
		uint activeIntnlNodeThis = thrust::get<1>(biddi); // number of active internal node???
		double lastCheckPoint = thrust::get<5>(biddi);

		bool isFull = isAllIntnlFilled(activeIntnlNodeThis);
		if (isFull) {
			return thrust::make_tuple(lastCheckPoint, activeIntnlNodeThis);
		}

		double cellCenterXCoord = thrust::get<2>(biddi);
		double cellCenterYCoord = thrust::get<3>(biddi);
		uint cellRank = thrust::get<4>(biddi);
		double randomAngle = obtainRandAngle(cellRank, _seed);


		uint cellNodeEndPos = obtainNewIntnlNodeIndex(cellRank,
		 		activeIntnlNodeThis); // size of the vector until the last active node in the current cell (inclusive), index to add a new node?
		uint intnlIndxBegin = cellRank * _maxNodePerCell + _maxMemNodePerCell; // length of the vector before the first internal node, index of the first internal node 
        uint intnlIndxEnd = cellNodeEndPos; // index of the new node to be added. 
		uint minIntnlNode=60;
		uint index_intnl;
		// uint index_highstMyo;
		// uint index_lowstMyo;
		double nodeMyosinOldL = 100;
		double nodeMyosinOldH = -100;
		double nodeMyosinCur;
		double maxAddDelNum = 5; // must agree with the number in [] in the following 6 arrays 
		double nodeXAddAll[5]; 
		double nodeYAddAll[5];
		// double nodeXDelAll[5];
		// double nodeYDelAll[5];
		uint index_delAll[5];
		// uint index_addAll[5];

		uint nodeCountAdd = 0;
		uint nodeCountDel = 0;
		
		double delta = 0.000001;
		if (/*!isIntnlEmptied*/activeIntnlNodeThis>1 && _timeStep%10000==0 && _timeStep * _ddt >=_InitTimeStage){
			for (index_intnl = intnlIndxBegin; index_intnl < intnlIndxEnd;
                    index_intnl++) {
					nodeMyosinCur = _myosinLevelAddr[index_intnl];
					if (nodeMyosinCur<nodeMyosinOldL) { // lowest myosin location 
						nodeMyosinOldL = nodeMyosinCur;
						// index_lowstMyo = index_intnl;
					}
					if (nodeMyosinCur>=nodeMyosinOldH){
						nodeMyosinOldH = nodeMyosinCur; // highest myosin location 
						// index_highstMyo = index_intnl;
					}
				}
			//
			// record myosin level is about 105% of the lowest value
			// record myosin level is about 98% of the highest value 
			for (index_intnl = intnlIndxBegin; index_intnl < intnlIndxEnd;
                    index_intnl++) {
					nodeMyosinCur = _myosinLevelAddr[index_intnl];
				if (nodeMyosinCur<1.05*nodeMyosinOldL && nodeCountAdd<maxAddDelNum) { // to be changed // add 
					nodeXAddAll[nodeCountAdd] = _nodeXPosAddress[index_intnl];
					nodeYAddAll[nodeCountAdd] = _nodeYPosAddress[index_intnl]; 
					// index_addAll[nodeCountAdd] = index_intnl;
					nodeCountAdd +=1;
				}
				if (nodeMyosinCur>0.98*nodeMyosinOldH && nodeCountDel<maxAddDelNum) { // delete
					// nodeXDelAll[nodeCountDel] = _nodeXPosAddress[index_intnl]; // not used???
					// nodeYDelAll[nodeCountDel] = _nodeYPosAddress[index_intnl]; 
					index_delAll[nodeCountDel] = index_intnl;
					nodeCountDel +=1;
				}
			} 
			uint maxIntnlNodePerCell = _maxNodePerCell-_maxMemNodePerCell; // check this 
			uint tempIndexAddDel;
			uint index_delStart;
			// uint cellNodeEndPos = obtainNewIntnlNodeIndex(cellRank,
			//	activeIntnlNodeThis); // this is the index in the global vector ???
			// the following delete nodes 
			for (tempIndexAddDel=0; tempIndexAddDel < nodeCountDel; tempIndexAddDel++){
				if (cellNodeEndPos-intnlIndxBegin>minIntnlNode){
					index_delStart = index_delAll[tempIndexAddDel]-tempIndexAddDel; //check this, following the order? 
					// add a condition to check if there is active internal node
					for (int m=index_delStart; m<cellNodeEndPos; m++){ // cellNodeEndPos-1 
						_nodeXPosAddress[m] = _nodeXPosAddress[m+1];
						_nodeYPosAddress[m] = _nodeYPosAddress[m+1];
						// _adhIndxAddr[m] = _adhIndxAddr[m+1];
						_nodeIsActiveAddress[m] = _nodeIsActiveAddress[m+1];
						}

					_nodeXPosAddress[cellNodeEndPos-1] = 0.0 + delta;
					_nodeYPosAddress[cellNodeEndPos-1] = 0.0 + delta;
					_nodeIsActiveAddress[cellNodeEndPos-1] = false;
					// _adhIndxAddr[cellNodeEndPos-1] = -1;
					cellNodeEndPos = cellNodeEndPos - 1;
					activeIntnlNodeThis = activeIntnlNodeThis-1;
				}
			}
			// the following add nodes
			double xCoordNewPt;
			double yCoordNewPt;
			for (tempIndexAddDel=0; tempIndexAddDel < nodeCountAdd; tempIndexAddDel++){
				xCoordNewPt = nodeXAddAll[tempIndexAddDel] - _addNodeDistance * cos(randomAngle); // same for all new node 
				yCoordNewPt = nodeYAddAll[tempIndexAddDel] - _addNodeDistance * sin(randomAngle); 
				if (cellNodeEndPos-intnlIndxBegin < maxIntnlNodePerCell) { // if the number of internal node is less than the maximum internal node // double check this 
					_nodeXPosAddress[cellNodeEndPos] = xCoordNewPt;
					_nodeYPosAddress[cellNodeEndPos] = yCoordNewPt;
					_nodeIsActiveAddress[cellNodeEndPos] = true;
					cellNodeEndPos = cellNodeEndPos + 1;
					activeIntnlNodeThis = activeIntnlNodeThis + 1;
				}
			}
		}

		lastCheckPoint = lastCheckPoint + _growThreshold;
		if (lastCheckPoint > 1.0) {
			lastCheckPoint = 1.0;
		}
		return thrust::make_tuple(lastCheckPoint, activeIntnlNodeThis);
	}
};






//AAMIRI
struct DelPtOp_M: thrust::unary_function<BoolUIDDUIUIBoolD, UiUiBD> {
	uint _seed;
	int _timeStep;
	double _growThreshold;
	double* _nodeXPosAddress;
	double* _nodeYPosAddress;
	bool* _nodeIsActiveAddress;
	int* _adhIndxAddr;
	
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ DelPtOp_M(uint seed, int timeStep,
			int* adhIndxAddr, double* nodeXPosAddress,
			double* nodeYPosAddress, bool* nodeIsActiveAddress) :
			_seed(seed), _timeStep(timeStep), _adhIndxAddr(
					adhIndxAddr), _nodeXPosAddress(nodeXPosAddress),
					 _nodeYPosAddress(nodeYPosAddress), _nodeIsActiveAddress(nodeIsActiveAddress) {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__device__ UiUiBD operator()(const BoolUIDDUIUIBoolD &biddi) {
		bool isScheduledToShrink = thrust::get<0>(biddi);
		uint activeIntnlNodeThis = thrust::get<1>(biddi);
		uint cellRank = thrust::get<4>(biddi);
		uint activeMembrNodeThis = thrust::get<5>(biddi);

		//bool isFull = isAllIntnlFilled(activeIntnlNodeThis);
		bool isIntnlEmptied = isAllIntnlEmptied(activeIntnlNodeThis);//AAMIRI
		bool isMembrEmptied = isAllMembrEmptied(activeMembrNodeThis);


		bool isCellActive = thrust::get<6>(biddi);
		double growthSpeed = thrust::get<7>(biddi);

		if (!isScheduledToShrink || (isIntnlEmptied && isMembrEmptied) /*|| cellRank != 0*/ ) {
			return thrust::make_tuple(activeMembrNodeThis, activeIntnlNodeThis, isCellActive, growthSpeed);
		}
		
		double cellCenterXCoord = thrust::get<2>(biddi);
		double cellCenterYCoord = thrust::get<3>(biddi);
		int randMembID = obtainRemovingMembrNodeID(cellRank, activeMembrNodeThis, _seed);
		int membEndNode = obtainMembEndNode(cellRank, activeMembrNodeThis);

		uint cellNodeEndPos = obtainLastIntnlNodeIndex(cellRank,
				activeIntnlNodeThis);

		double delta = 0.000001;
		if (/*!isIntnlEmptied*/activeIntnlNodeThis>1 && _timeStep%80==0){
		_nodeXPosAddress[cellNodeEndPos-1] = 0.0 + delta;
		_nodeYPosAddress[cellNodeEndPos-1] = 0.0 + delta;
		_nodeIsActiveAddress[cellNodeEndPos-1] = false;
		_adhIndxAddr[cellNodeEndPos-1] = -1;
	//	_membrIntnlIndex[cellNodeEndPos-1] = -1;
		activeIntnlNodeThis = activeIntnlNodeThis - 1;
		}

		if (/*!isMembrEmptied*/activeMembrNodeThis>2 && _timeStep%50==0){
		    for (int m=randMembID; m<membEndNode; m++){
			_nodeXPosAddress[m] = _nodeXPosAddress[m+1];
			_nodeYPosAddress[m] = _nodeYPosAddress[m+1];
			_adhIndxAddr[m] = _adhIndxAddr[m+1];
			_nodeIsActiveAddress[m] = _nodeIsActiveAddress[m+1];}
		
/*			_nodeXPosAddress[randMembID] = _nodeXPosAddress[membEndNode];
			_nodeYPosAddress[randMembID] = _nodeYPosAddress[membEndNode];
			_adhIndxAddr[randMembID] = _adhIndxAddr[membEndNode];
			_nodeIsActiveAddress[randMembID] = _nodeIsActiveAddress[membEndNode];*/

		_nodeXPosAddress[membEndNode] = 0.0 + delta;
		_nodeYPosAddress[membEndNode] = 0.0 + delta;
		_nodeIsActiveAddress[membEndNode] = false;
		_adhIndxAddr[membEndNode] = -1;
	//	_membrIntnlIndex[membEndNode] = -1;
		activeMembrNodeThis = activeMembrNodeThis - 1;
		}

		if (activeMembrNodeThis == 2){
		_nodeXPosAddress[membEndNode] = 0.0 + delta;
		_nodeYPosAddress[membEndNode] = 0.0 + delta;
		_nodeXPosAddress[membEndNode - 1] = 0.0 + delta;
		_nodeYPosAddress[membEndNode - 1] = 0.0 + delta;

		_adhIndxAddr[membEndNode] = -1;
		_adhIndxAddr[membEndNode - 1] = -1;

		}

		if (activeIntnlNodeThis == 1){
		_nodeXPosAddress[cellNodeEndPos-1] = 0.0 + delta;
		_nodeYPosAddress[cellNodeEndPos-1] = 0.0 + delta;
		_adhIndxAddr[cellNodeEndPos-1] = -1; 	
		}

		if ( (activeIntnlNodeThis+activeMembrNodeThis) == 2 ){
		isCellActive = false;
		growthSpeed = 0.0;
		}


		return thrust::make_tuple(activeMembrNodeThis, activeIntnlNodeThis, isCellActive, growthSpeed);
	}

};

/**
 * Compute the target length of a cell given growth progress.
 * @param _cellInitLength initial length of a cell. (when growth progress = 0)
 * @param _cellFinalLength final length of a cell. (when growth progress =1)
 * @param input1 progress cell growth progress.
 * @return cell expected length
 */
struct CompuTarLen: thrust::unary_function<double, double> {
	double _cellInitLength, _cellFinalLength;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ CompuTarLen(double initLen, double finalLen) :
			_cellInitLength(initLen), _cellFinalLength(finalLen) {
	}
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__
	double operator()(const double &progress) {
		return _cellInitLength + progress * (_cellFinalLength - _cellInitLength);
	}
};

/**
 * Compute the distance of a node to its corresponding center, return 0 if node is inactive.
 * @param input1 x component of center position of cell center
 * @param input2 y component of center position of cell center
 * @param input3 x component of cell growth direction
 * @param input4 y component of cell growth direction
 * @param input5 x component of node location
 * @param input6 y component of node location
 * @param input7 flag for node activeness
 */
struct CompuDist: thrust::unary_function<CVec6Bool, double> {
	__host__ __device__
	double operator()(const CVec6Bool &vec6b) {
		double centerXPos = thrust::get<0>(vec6b);
		double centerYPos = thrust::get<1>(vec6b);
		double growthXDir = thrust::get<2>(vec6b);
		double growthYDir = thrust::get<3>(vec6b);
		double nodeXPos = thrust::get<4>(vec6b);
		double nodeYPos = thrust::get<5>(vec6b);
		bool nodeIsActive = thrust::get<6>(vec6b);
		if (!nodeIsActive) {
			// All nodes that are inactive will be omitted.
			// I choose 0 because 0 will not be either maximum or minimum
			return 0;
		} else {
			double dirModule = sqrt(
					growthXDir * growthXDir + growthYDir * growthYDir);
			return ((nodeXPos - centerXPos) * (growthXDir) / dirModule
					+ (nodeYPos - centerYPos) * growthYDir / dirModule);
		}
	}
};

/**
 * Compute difference of cell expected length and current length.
 * @param input1 expected length of the cell
 * @param input2 minimum distance of nodes of the cell to its corresponding center along growth direction
 * @param input3 maximum distance of nodes of the cell to its corresponding center along growth direction
 * @return difference of expected and current length.
 */
struct CompuDiff: thrust::unary_function<CVec3, double> {
	__host__ __device__
	double operator()(const CVec3 &vec3) {
		double expectedLen = thrust::get<0>(vec3);
		// minimum distance of node to its corresponding center along growth direction
		double minDistance = thrust::get<1>(vec3);
		double maxDistance = thrust::get<2>(vec3);
		return (expectedLen - (maxDistance - minDistance));
	}
};

/**
 * Apply stretch force to all cell nodes.
 * @param _elongationCoefficient elongationForce = _elongationCoefficient*distInElongationDirection
 * 			* elongateDirection;
 * @param input1 distToCenterAlongGrowDir distance of a node to the corresponding cell center along growth direction
 * @param input2 lengthDifference length difference of the expected length of a cell and currentl length of the same cell.
 * @param input3 x component of growth direction.
 * @param input4 y component of growth direction.
 * @param input5 x direction of original velocity
 * @param input6 y direction of original velocity
 */
struct ApplyStretchForce: thrust::unary_function<CVec6, CVec2> {
	double _elongationCoefficient;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ ApplyStretchForce(double elongationCoefficient) :
			_elongationCoefficient(elongationCoefficient) {
	}
	__host__ __device__ CVec2 operator()(const CVec6 &vec6) {
		double distToCenterAlongGrowDir = thrust::get<0>(vec6);
		// minimum distance of node to its corresponding center along growth direction
		double lengthDifference = thrust::get<1>(vec6);
		double growthXDir = thrust::get<2>(vec6);
		double growthYDir = thrust::get<3>(vec6);
		double originalVelX = thrust::get<4>(vec6);
		double originalVelY = thrust::get<5>(vec6);
		double xRes = lengthDifference * _elongationCoefficient
				* distToCenterAlongGrowDir * growthXDir;
		xRes = xRes + originalVelX;
		double yRes = lengthDifference * _elongationCoefficient
				* distToCenterAlongGrowDir * growthYDir;
		yRes = yRes + originalVelY;
		return thrust::make_tuple(xRes, yRes);
	}
};

struct ApplyStretchForce_M: thrust::unary_function<CVec6UI, CVec2> {
	double _elongationCoefficient;
	double _typeThreshold;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ ApplyStretchForce_M(double elongationCoefficient,
			uint threshold) :
			_elongationCoefficient(elongationCoefficient), _typeThreshold(
					threshold) {
	}
	__host__ __device__ CVec2 operator()(const CVec6UI &vec6ui) {
		double distToCenterAlongGrowDir = thrust::get<0>(vec6ui);
		// minimum distance of node to its corresponding center along growth direction
		double lengthDifference = thrust::get<1>(vec6ui);
		double growthXDir = thrust::get<2>(vec6ui);
		double growthYDir = thrust::get<3>(vec6ui);
		double originalVelX = thrust::get<4>(vec6ui);
		double originalVelY = thrust::get<5>(vec6ui);
		uint nodeRank = thrust::get<6>(vec6ui);
		if (nodeRank < _typeThreshold) {
			return thrust::make_tuple(originalVelX, originalVelY);
		} else {
			double xRes = lengthDifference * _elongationCoefficient
					* distToCenterAlongGrowDir * growthXDir;
			xRes = xRes + originalVelX;
			double yRes = lengthDifference * _elongationCoefficient
					* distToCenterAlongGrowDir * growthYDir;
			yRes = yRes + originalVelY;
			return thrust::make_tuple(xRes, yRes);
		}

	}
};

struct ApplyChemoVel: thrust::unary_function<CVec5, CVec2> {
	double _chemoCoefficient;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ ApplyChemoVel(double chemoCoefficient) :
			_chemoCoefficient(chemoCoefficient) {
	}
	__host__ __device__ CVec2 operator()(const CVec5 &vec5) {
		double growthSpeed = thrust::get<0>(vec5);
		double growthXDir = thrust::get<1>(vec5);
		double growthYDir = thrust::get<2>(vec5);
		double originalVelX = thrust::get<3>(vec5);
		double originalVelY = thrust::get<4>(vec5);
		double xRes = growthSpeed * _chemoCoefficient * growthXDir;
		xRes = xRes + originalVelX;
		double yRes = growthSpeed * _chemoCoefficient * growthYDir;
		yRes = yRes + originalVelY;
		return thrust::make_tuple(xRes, yRes);
	}
};

/**
 * compute the left shifted global position of a node.
 * @param _shiftLeftOffset number of spaces the node should left shift \n
 * @param input original global position of a node \n
 * @return output shifted global position of a node.\n
 */
struct LeftShiftFunctor: thrust::unary_function<uint, uint> {
	uint _shiftLeftOffset;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ LeftShiftFunctor(uint maxNodeOfOneCell) :
			_shiftLeftOffset(maxNodeOfOneCell / 2) {
	}
	__host__ __device__ uint operator()(const uint &position) {
		uint result;
		if (position < _shiftLeftOffset) {
			// could be 0, because these region will actually never be used
			result = 0;
		} else {
			result = position - _shiftLeftOffset;
		}
		return result;
	}
};

/**
 * decide if a node, given by its global rank, is on the right side of a cell.
 * @param _maxNodeCountPerCell maximum number of nodes per cell \n
 * @param _halfMaxNode half of maximum number of nodes per cell \n
 * @param nodeGlobalRank global rank of a node \n
 * @return IsRightSide : true if is on the left side.\n
 */
struct IsRightSide: thrust::unary_function<uint, bool> {
	uint _maxNodeCountPerCell;
	uint _halfMaxNode;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ IsRightSide(uint maxNodeOfOneCell) :
			_maxNodeCountPerCell(maxNodeOfOneCell), _halfMaxNode(
					maxNodeOfOneCell / 2) {
	}
	__host__ __device__
	bool operator()(const uint &position) {
		if (position % _maxNodeCountPerCell < _halfMaxNode) {
			return false;
		} else {
			return true;
		}
	}
};

/**
 * decide if a node, given by its global rank, is on the left side of a cell.
 * @param _maxNodeCountPerCell maximum number of nodes per cell \n
 * @param _halfMaxNode half of maximum number of nodes per cell \n
 * @param nodeGlobalRank global rank of a node \n
 * @return IsLeftSide : true if is on the left side.\n
 */
struct IsLeftSide: thrust::unary_function<uint, bool> {
	uint _maxNodeCountPerCell;
	uint _halfMaxNode;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ IsLeftSide(uint maxNodeOfOneCell) :
			_maxNodeCountPerCell(maxNodeOfOneCell), _halfMaxNode(
					maxNodeOfOneCell / 2) {
	}
	__host__ __device__
	bool operator()(const uint &position) {
		if (position % _maxNodeCountPerCell < _halfMaxNode) {
			return true;
		} else {
			return false;
		}
	}
};

/**
 * Given rank of a node inside a cell and rank of the cell, get the global rank of the node.
 * @param _maxNodeCountPerCell maximum number of nodes of a cell
 * @param vec first input: rank of a node inside a cell. \n
 * second input: rank of the cell \n
 * @return nodePosition global rank of a node
 */
struct CompuPos: thrust::unary_function<Tuint2, uint> {
	uint _maxNodeCountPerCell;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ CompuPos(uint maxNodeOfOneCell) :
			_maxNodeCountPerCell(maxNodeOfOneCell) {
	}
	__host__ __device__ uint operator()(const Tuint2 &vec) {
		uint rankInCell = thrust::get<0>(vec) % _maxNodeCountPerCell;
		uint cellRank = thrust::get<1>(vec);
		return (cellRank * _maxNodeCountPerCell + rankInCell);
	}
};

/**
 * struct for decide if a cell is ready to divide.
 * @param _isDivideCriticalRatio If the length difference to expected length
 *     is less than this critical ratio and growth progress is equal or bigger than 1.0
 *     it means the cell is ready to divide.
 * @param vec first input : length difference of current length and expected length \n
 * second input: expected length \n
 * thrid input: growth progress. should be 0.0 to 1.0. \n
 * @return isGoingToDivide result that indicates whether a cell is ready to divide.
 */
struct CompuIsDivide: thrust::unary_function<CVec3Int, BoolD> {
	double _isDivideCriticalRatio;
	uint _maxNodePerCell;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ CompuIsDivide(double isDivideCriticalRatio,
			uint maxNodePerCell) :
			_isDivideCriticalRatio(isDivideCriticalRatio), _maxNodePerCell(
					maxNodePerCell) {
	}
	__host__ __device__ BoolD operator()(const CVec3Int &vec) {
		double lengthDifference = thrust::get<0>(vec);
		double expectedLength = thrust::get<1>(vec);
		double currentLength = expectedLength - lengthDifference;
		double growthProgress = thrust::get<2>(vec);
		uint nodeCount = thrust::get<3>(vec);
		if (currentLength / expectedLength > _isDivideCriticalRatio
				&& growthProgress >= 1.0 && nodeCount == _maxNodePerCell) {
			return thrust::make_tuple(true, 0.0);
		} else {
			return thrust::make_tuple(false, growthProgress);
		}
	}
};

struct CompuIsDivide_M: thrust::unary_function<DUi, bool> {
	uint _maxIntnlNodePerFollower;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ CompuIsDivide_M(uint maxIntnlNodePerFollower) :
			_maxIntnlNodePerFollower(maxIntnlNodePerFollower) {
	}
	__host__ __device__
	bool operator()(const DUi &vec) {
		double growthProgress = thrust::get<0>(vec);
		uint nodeCount = thrust::get<1>(vec);
		if (growthProgress >= 1.0 && nodeCount >= _maxIntnlNodePerFollower) { // should add another condition that cell_Type == 0, means follower
			return true;
		} else {
			return false;
		}
	}
};

//A&A
struct CompuIsEnteringMitotic_M: thrust::unary_function<CVec2, bool> {
	double _grthprgCriVal ; 
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ CompuIsEnteringMitotic_M(double grthprgCriVal_M) :
			_grthprgCriVal(grthprgCriVal_M) {
	}
	__host__ __device__
	bool operator()(const CVec2 &vec) {
		double growthProgress = thrust::get<0>(vec);
		double growthProgressOld = thrust::get<1>(vec);
		
		if (growthProgress >= _grthprgCriVal &&  growthProgressOld <= _grthprgCriVal) {
			return true;
		} else {
			return false;
		}
	}
};

//AAMIRI
/*
struct CompuIsRemoving_M: thrust::unary_function<DUi, bool> {
	uint _maxIntnlNodePerCell;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ CompuIsDivide_M(uint maxIntnlNodePerCell) :
			_maxIntnlNodePerCell(maxIntnlNodePerCell) {
	}
	__host__ __device__
	bool operator()(const DUi &vec) {
		double growthProgress = thrust::get<0>(vec);
		uint nodeCount = thrust::get<1>(vec);
		if ( nodeCount == _maxIntnlNodePerCell - _maxIntnlNodePerCell) {
			return true;
		} else {
			return false;
		}
	}
};
*/

/**
 * Functor for modify veolcities of all nodes given node type and isActive
 * @param input1: take velocity , type and isActive info of node
 * @return output: modified velocity.
 */
struct VelocityModifier: public thrust::unary_function<Vel2DActiveTypeRank,
		CVec2> {
	uint beginPosOfProfileNodes;
	uint currentActiveProfileNodes;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ VelocityModifier(uint beginPos,
			uint currentProfileNodeCount) {
		beginPosOfProfileNodes = beginPos;
		currentActiveProfileNodes = currentProfileNodeCount;
	}
	__host__ __device__
	// place holder for eclipse formatter
	CVec2 operator()(const Vel2DActiveTypeRank &nodeInfo) {
		double velX = thrust::get<0>(nodeInfo);
		double velY = thrust::get<1>(nodeInfo);
		bool isActive = thrust::get<2>(nodeInfo);
		SceNodeType type = thrust::get<3>(nodeInfo);
		uint nodeRank = thrust::get<4>(nodeInfo);
		// boundary nodes should not move. Also, inactive nodes should not move.
		if (type == Boundary || type == Cart || isActive == false) {
			return thrust::make_tuple(0.0, 0.0);
		}
		// The end nodes of the profile should be fixed.
		if (type == Profile
				&& (nodeRank == beginPosOfProfileNodes
						|| nodeRank
								== (beginPosOfProfileNodes
										+ currentActiveProfileNodes - 1))) {
			return thrust::make_tuple(0.0, 0.0);
		}
		return thrust::make_tuple(velX, velY);
	}
};

struct ForceZero: public thrust::unary_function<CVec2, CVec2> {
	__host__ __device__ CVec2 operator()(const CVec2 &oriData) {
		return thrust::make_tuple(0.0, 0.0);
	}
};

struct AdjustGrowth: public thrust::unary_function<UiDD, BoolDD> {
	uint _halfMax;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ AdjustGrowth(uint halfMax) :
			_halfMax(halfMax) {
	}
	__host__ __device__ BoolDD operator()(const UiDD &growData) {
		uint curIntnlNodeCount = thrust::get<0>(growData);
		double curProgress = thrust::get<1>(growData);
		double lastPoint = thrust::get<2>(growData);
		if (curIntnlNodeCount <= _halfMax) {
			curProgress = 0;
			lastPoint = 0;
		}
		return thrust::make_tuple(false, curProgress, lastPoint);
	}
};

struct AssignRandIfNotInit: public thrust::unary_function<CVec3BoolInt,
		CVec3Bool> {
	double _lowerLimit, _upperLimit;
	uint _currentCellCount, _randAux;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ AssignRandIfNotInit(double low, double high,
			uint currentCellCount, uint randAux) :
			_lowerLimit(low), _upperLimit(high), _currentCellCount(
					currentCellCount), _randAux(randAux) {
	}
	__host__ __device__ CVec3Bool operator()(const CVec3BoolInt &inputInfo) {
		uint preSeed = _currentCellCount / _randAux;
		double currentDirX = thrust::get<1>(inputInfo);
		double currentDirY = thrust::get<2>(inputInfo);
		bool isInitBefore = thrust::get<3>(inputInfo);
		uint seed = thrust::get<4>(inputInfo) + preSeed;
		thrust::default_random_engine rng;
		thrust::uniform_real_distribution<double> dist(_lowerLimit,
				_upperLimit);

		if (isInitBefore) {
			rng.discard(seed);
			double randomNum = dist(rng);
			return thrust::make_tuple(randomNum, currentDirX, currentDirY, true);
		} else {
			rng.discard(seed);
			double randomNum1 = dist(rng);
			thrust::uniform_real_distribution<double> dist2(0, 2 * PI);
			rng.discard(seed);
			double randomNum2 = dist2(rng);
			double xDir = cos(randomNum2);
			double yDir = sin(randomNum2);
			return thrust::make_tuple(randomNum1, xDir, yDir, true);
		}
	}
};
//Ali modified
struct RandomizeGrow_M: public thrust::unary_function<CVec3BoolInt, CVec3Bool> {
	double _CntrTisuX,_CntrTisuY ; 
	double _RTisu ; 
	double _lowerLimit, _upperLimit;
	uint _seed;
	thrust::default_random_engine rng;
	thrust::uniform_real_distribution<double> dist;
//	thrust::uniform_real_distribution<double> dist2;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ RandomizeGrow_M(double CntrTisuX, double CntrTisuY,double RTisu,double low, double high, uint seed) :
			_CntrTisuX(CntrTisuX),_CntrTisuY(CntrTisuY),_RTisu(RTisu),_lowerLimit(low), _upperLimit(high), _seed(seed),dist(_lowerLimit,_upperLimit)  {
	}
	__host__ __device__ CVec3Bool operator()(const CVec3BoolInt &inputInfo) {
		double curSpeed = thrust::get<0>(inputInfo);
		double centerX = thrust::get<1>(inputInfo);
		double centerY = thrust::get<2>(inputInfo);
		bool isInitBefore = thrust::get<3>(inputInfo);
		if (isInitBefore) {
			return thrust::make_tuple(curSpeed,centerX,centerY,true);
		} else {
			uint cellRank = thrust::get<4>(inputInfo);
			uint seedNew = _seed + cellRank;
			rng.discard(seedNew);
			double distance=sqrt( (centerX-_CntrTisuX)*(centerX-_CntrTisuX)+(centerY-_CntrTisuY)*(centerY-_CntrTisuY)) ; 
			//double randomNum1 =1.89*exp(-distance/_RTisu)*dist(rng);
			double randomNum1 =dist(rng);
			//rng.discard(seedNew);
//			double randomNum2 = dist2(rng);
//			double xDir = cos(randomNum2);
//			double yDir = sin(randomNum2);
			return thrust::make_tuple(randomNum1,centerX,centerY,true);
		}
	}
};

// division 
struct DppGrowRegulator: public thrust::unary_function<DDDDi, double> {
	double _dt ;
	double _mitoticCheckPoint ; 


	__host__ __device__ DppGrowRegulator(double dt, double mitoticCheckPoint) :
			_dt(dt),_mitoticCheckPoint(mitoticCheckPoint)  {
	}
	__host__ __device__ double  operator()(const DDDDi &dDDD) {
		double dpp = thrust::get<0>(dDDD);
		double dpp_Old = thrust::get<1>(dDDD);
		double progress = thrust::get<2>(dDDD);
		double speed = thrust::get<3>(dDDD);
		int cell_Type = thrust::get<4>(dDDD);
		double progressNew ; 
		// double smallValue = 0.000001 ;

		if (cell_Type == 0){// followers
			// progressNew=progress+speed*_dt ;} 
			progressNew = 0.1;}
		else {
			progressNew=0.1; // leader cell never divides 
		}
		return progressNew;	// Alireza : bypass if conditions means 2way coupling temporal model
		/*
		if ((progress <= _mitoticCheckPoint) && (progressNew>_mitoticCheckPoint)) {
			if (dpp/(dpp_Old+smallValue)>1.5) {
				return (progressNew) ; 
			}
			else{
				return (progress); 
			}
		}
		else {

				return (progressNew) ; 
        
		}
		*/
	}
};


struct AssignFixedGrowth: public thrust::unary_function<CVec3BoolInt, CVec3> {
	double _speed;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ AssignFixedGrowth(double fixedSpeed) :
			_speed(fixedSpeed) {
	}
	__host__ __device__ CVec3 operator()(const CVec3BoolInt &inputInfo) {
		return thrust::make_tuple(_speed, 1, 0);
	}
};

struct AddMemNode: public thrust::unary_function<Tuuudd, uint> {
	uint _maxNodePerCell;
	bool* _isActiveAddr;
	double* _xPosAddr, *_yPosAddr;
	int* _adhIndxAddr;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ AddMemNode(uint maxNodePerCell, bool* isActiveAddr,
			double* xPosAddr, double* yPosAddr, int* adhIndxAddr) :
			_maxNodePerCell(maxNodePerCell), _isActiveAddr(isActiveAddr), _xPosAddr(
					xPosAddr), _yPosAddr(yPosAddr), _adhIndxAddr(adhIndxAddr) {
	}
	__device__ uint operator()(const Tuuudd &oriData) {
		uint cellRank = thrust::get<0>(oriData);
		uint insertIndx = thrust::get<1>(oriData) + 1;
		uint curActCount = thrust::get<2>(oriData);
		double insertX = thrust::get<3>(oriData);
		double insertY = thrust::get<4>(oriData);
		uint globalIndxEnd = cellRank * _maxNodePerCell + curActCount;
		uint globalIndexInsert = cellRank * _maxNodePerCell + insertIndx;
		for (uint i = globalIndxEnd; i >= globalIndexInsert; i--) {
			_isActiveAddr[i] = _isActiveAddr[i - 1];
			_xPosAddr[i] = _xPosAddr[i - 1];
			_yPosAddr[i] = _yPosAddr[i - 1];
			//_adhIndxAddr[i] = _adhIndxAddr[i - 1];
			_adhIndxAddr[i] = -1;
		}
		_isActiveAddr[globalIndexInsert] = true;
		_xPosAddr[globalIndexInsert] = insertX;
		_yPosAddr[globalIndexInsert] = insertY;
		_adhIndxAddr[globalIndexInsert] = -1;
		return (curActCount + 1);
	}
};

struct CalTriArea: public thrust::unary_function<Tuuudd, double> {
	uint _maxNodePerCell;
	bool* _isActiveAddr;
	double* _locXAddr;
	double* _locYAddr;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ CalTriArea(uint maxNodePerCell, bool* isActiveAddr,
			double* locXAddr, double* locYAddr) :
			_maxNodePerCell(maxNodePerCell), _isActiveAddr(isActiveAddr), _locXAddr(
					locXAddr), _locYAddr(locYAddr) {
	}
	__device__
	double operator()(const Tuuudd &inputData) const {
		uint activeMembrCount = thrust::get<0>(inputData);
		uint cellRank = thrust::get<1>(inputData);
		uint nodeRank = thrust::get<2>(inputData);
		double centerX = thrust::get<3>(inputData);
		double centerY = thrust::get<4>(inputData);
		uint index = cellRank * _maxNodePerCell + nodeRank;

		if (_isActiveAddr[index] == false || nodeRank >= activeMembrCount ) {
			return 0.0;
		} else {
			int index_right = nodeRank + 1;
			if (index_right == (int) activeMembrCount) {
				index_right = 0;
			}
			index_right = index_right + cellRank * _maxNodePerCell;
			// apply tension force from right
			if (_isActiveAddr[index_right]) {
				double posX = _locXAddr[index];
				double posY = _locYAddr[index];
				double vec1X = _locXAddr[index_right] - posX;
				double vec1Y = _locYAddr[index_right] - posY;
				double vec2X = centerX - posX;
				double vec2Y = centerY - posY;
				double result = fabs(vec1X * vec2Y - vec2X * vec1Y) / 2.0;
				return result;
			} else {
				return 0.0;
			}
		}
	}
};



struct CalTriAreaV1: public thrust::unary_function<Tuuudd, double> {
	uint _maxNodePerCell;
	bool* _isActiveAddr;
	double* _locXAddr;
	double* _locYAddr;
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__ CalTriAreaV1(uint maxNodePerCell, bool* isActiveAddr,
			double* locXAddr, double* locYAddr) :
			_maxNodePerCell(maxNodePerCell), _isActiveAddr(isActiveAddr), _locXAddr(
					locXAddr), _locYAddr(locYAddr) {
	}
	__device__
	double operator()(const Tuuudd &inputData) const {
		uint activeMembrCount = thrust::get<0>(inputData);
		uint cellRank = thrust::get<1>(inputData);
		uint nodeRank = thrust::get<2>(inputData);
		double centerX = thrust::get<3>(inputData);
		double centerY = thrust::get<4>(inputData);
		uint index = cellRank * _maxNodePerCell + nodeRank;

		if (_isActiveAddr[index] == false || nodeRank >= activeMembrCount ) {
			return 0.0;
		} else {
			int index_right = nodeRank + 1;
			if (index_right == (int) activeMembrCount) {
				index_right = 0;
			}
			index_right = index_right + cellRank * _maxNodePerCell;
			// apply tension force from right
			if (_isActiveAddr[index_right]) {
				double posX = _locXAddr[index];
				double posY = _locYAddr[index];
				double pos1X = _locXAddr[index_right];
				double pos1Y = _locYAddr[index_right];
				double result = (posX * pos1Y - pos1X * posY) / 2.0;
				return result;
			} else {
				return 0.0;
			}
		}
	}
};



struct BC_Tissue_Damp: public thrust::unary_function<CVec3,CVec2> {
	double _TMinX, _TMaxX,_TMinY,_TMaxY,_Damp_Coef ; 
        int    _NumActCells ; 
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__  BC_Tissue_Damp(double InputFunctor1, double InputFunctor2, double InputFunctor3
                                          , double InputFunctor4, double InputFunctor5, int InputFunctor6
			) :
			_TMinX(InputFunctor1), _TMaxX(InputFunctor2)
                       ,_TMinY(InputFunctor3), _TMaxY(InputFunctor4), _Damp_Coef(InputFunctor5),_NumActCells(InputFunctor6) {
	}
	__host__ __device__ CVec2 operator()(const CVec3 &inputInfo) {
		double CenterCellX = thrust::get<0>(inputInfo);
		double CenterCellY = thrust::get<1>(inputInfo);
                double TCenterX= 0.5*(_TMinX+_TMaxX);
                double TCenterY= 0.5*(_TMinY+_TMaxY); 
                double TRadius=0.5*( 0.5*(_TMaxX-_TMinX) +0.5*(_TMaxY-_TMinY)) ; 
                
                if (_NumActCells>50) {                  

                double Dist=sqrt (
                            (CenterCellX-TCenterX)*(CenterCellX-TCenterX)+
                           (CenterCellY-TCenterY)*(CenterCellY-TCenterY)) ; 
                double Damp=_Damp_Coef;// + max(0.0,Dist/TRadius-0.8)*1.0/(1.0-0.8)*0*_Damp_Coef     ;              
                        
			return thrust::make_tuple(CenterCellX,Damp);
		} 
                else { 
                        return thrust::make_tuple(CenterCellX,_Damp_Coef);
                     }

}
}; 


/**struct BC_Tissue_Damp: public thrust::unary_function<CVec2,CVec2> {
	double _Damp_Coef ; 
	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
	__host__ __device__  BC_Tissue_Damp(double InputFunctor1
			) :
			 _Damp_Coef(InputFunctor1) {
	}
	__host__ __device__ CVec2 operator()(const CVec2 &inputInfo) {
		double CenterCellX = thrust::get<0>(inputInfo);
		double CenterCellY = thrust::get<1>(inputInfo);
         //       double TCenterX= 0.5*(_TMinX+_TMaxX);
           //     double TCenterY= 0.5*(_TMinY+_TMaxY); 
             //   double TRadius=0.5*( 0.5*(_TMaxX-_TMinX) +0.5*(_TMaxY-_TMinY)) ; 

               // double Dist=sqrt (
                 //           (CenterCellX-TCenterX)*(CenterCellX-TCenterX)+
                   //        (CenterCellY-TCenterY)*(CenterCellY-TCenterY)) ; 
                //double Damp=_Damp_Coef + max(0.0,Dist/TRadius-0.8)*1.0/(1.0-0.8)*200*_Damp_Coef     ;              
                        
			return thrust::make_tuple(CenterCellX,CenterCellY);
		}
}; 

**/

struct calAbs: public thrust::unary_function<double, double> {
	uint _totalNodeCountForActiveCells; 
__host__ __device__ calAbs (uint totalNodeCountForActiveCells) : 
	_totalNodeCountForActiveCells(totalNodeCountForActiveCells) {
	}

__host__ __device__
double operator()(const double &inputData) const {
	return fabs(inputData);
	}
};




struct CalPerim: public thrust::unary_function<Tuuudd, double> {
 	uint _maxNodePerCell;
 	bool* _isActiveAddr;
 	double* _locXAddr;
 	double* _locYAddr;
 	// comment prevents bad formatting issues of __host__ and __device__ in Nsight
 	__host__ __device__ CalPerim(uint maxNodePerCell, bool* isActiveAddr,
 			double* locXAddr, double* locYAddr) :
 			_maxNodePerCell(maxNodePerCell), _isActiveAddr(isActiveAddr), _locXAddr(
 					locXAddr), _locYAddr(locYAddr) {
 	}
 	__device__
 	double operator()(const Tuuudd &inputData) const {
 		uint activeMembrCount = thrust::get<0>(inputData);
 		uint cellRank = thrust::get<1>(inputData);
 		uint nodeRank = thrust::get<2>(inputData);
 		double centerX = thrust::get<3>(inputData);
 		double centerY = thrust::get<4>(inputData);
 		uint index = cellRank * _maxNodePerCell + nodeRank;
 
 		if (_isActiveAddr[index] == false || nodeRank >= activeMembrCount ) {
 			return 0.0;
 		} else {
 			int index_right = nodeRank + 1;
 			if (index_right == (int) activeMembrCount) {
 				index_right = 0;
 			}
 			index_right = index_right + cellRank * _maxNodePerCell;
 			// apply tension force from right
 			if (_isActiveAddr[index_right]) {
 				double posX = _locXAddr[index];
 				double posY = _locYAddr[index];
 				double vec1X = _locXAddr[index_right] - posX;
 				double vec1Y = _locYAddr[index_right] - posY;
 				//double vec2X = centerX - posX;
 				//double vec2Y = centerY - posY;
 				double result = sqrt(vec1X * vec1X + vec1Y * vec1Y);
 				return result;
 			} else {
 				return 0.0;
 			}
 		}
 	}
 };
 

struct CellInfoVecs {
	/**
	 * @param growthProgress is a vector of size maxCellCount.
	 * In each cell, \n
	 * progress == 0 means recently divided
	 * progress == 1 means ready to divide
	 */
	thrust::device_vector<double> growthProgress; //Ali
	thrust::device_vector<double> Cell_Time;//Ali
	thrust::device_vector<double> Cell_Damp;//Ali
	thrust::device_vector<double> cell_Dpp;//Ali
	thrust::device_vector<double> cell_DppOld;//Ali
	thrust::device_vector<double> cell_Tkv;//Alireza
	thrust::device_vector<double> cell_DppTkv;//Alireza
	thrust::device_vector<double> cell_pMad;//Alireza
    thrust::device_vector<double> cell_pMadOld;//Alireza
	thrust::device_vector<int> cell_Type ;//Alireza

	thrust::device_vector<double> growthProgressOld;  //A&A
//Ali
        
//Ali
	thrust::device_vector<double> expectedLength;
//thrust::device_vector<double> currentLength;
	thrust::device_vector<double> lengthDifference;
// array to hold temp value computed in growth phase.
// obtained by smallest value of vector product of (a cell node to the cell center)
// and (growth direction). This will be a negative value
	thrust::device_vector<double> smallestDistance;
// biggest value of vector product of (a cell node to the cell center)
// and (growth direction). This will be a positive value
	thrust::device_vector<double> biggestDistance;
	thrust::device_vector<uint> activeNodeCountOfThisCell;
	thrust::device_vector<double> lastCheckPoint;
	thrust::device_vector<bool> isDividing;
	thrust::device_vector<bool> isEnteringMitotic; //A&A

	//thrust::device_vector<bool> isRemoving;//AAMIRI

// This cell type array should be initialized together with the host class.
	thrust::device_vector<SceNodeType> cellTypes;
	thrust::device_vector<bool> isScheduledToGrow;
	thrust::device_vector<bool> isScheduledToShrink;//AAMIRI
	thrust::device_vector<bool> isCellActive;//AAMIRI
	thrust::device_vector<double> centerCoordX;
	thrust::device_vector<double> centerCoordY;
	thrust::device_vector<double> centerCoordZ;

	thrust::device_vector<double> HertwigXdir; //A&A
	thrust::device_vector<double> HertwigYdir; //A&A


	thrust::device_vector<bool> isRandGrowInited;
	thrust::device_vector<double> growthSpeed;
	thrust::device_vector<double> growthXDir;
	thrust::device_vector<double> growthYDir;
// some memory for holding intermediate values instead of dynamically allocating.
	thrust::device_vector<uint> cellRanksTmpStorage;

	thrust::device_vector<uint> activeMembrNodeCounts;
	thrust::device_vector<uint> activeIntnlNodeCounts;

	thrust::device_vector<double> membrGrowProgress;
	thrust::device_vector<double> membrGrowSpeed;
	thrust::device_vector<double> maxTenRiVec;
	thrust::device_vector<double> maxDistToRiVec;  //Ali 
	thrust::device_vector<double> maxTenRiMidXVec;
	thrust::device_vector<double> maxTenRiMidYVec;
	thrust::device_vector<double> aveTension;
	thrust::device_vector<uint> maxTenIndxVec;
	thrust::device_vector<bool> isMembrAddingNode;

	thrust::device_vector<double> cellAreaVec;
	thrust::device_vector<double> cellAreaInit;
    thrust::device_vector<double> cellPerimVec;//AAMIRI

	thrust::device_vector<double> cellFilopX;
	thrust::device_vector<double> cellFilopY;
	thrust::device_vector<double> cellFilopAngle;
	thrust::device_vector<bool> cellFilopIsActive;
	thrust::device_vector<double> cellFilopBirthTime;
	thrust::device_vector<uint> activeCellFilopCounts;
	thrust::device_vector<int> filopDirection;

	thrust::device_vector<double> cellPolarX;
	thrust::device_vector<double> cellPolarY;
	thrust::device_vector<double> cellPolarAngle;
	thrust::device_vector<double> cellRadius;

	thrust::device_vector<uint> activationLevel;
	
};

struct CellNodeInfoVecs {
// some memory for holding intermediate values instead of dynamically allocating.
	thrust::device_vector<uint> cellRanks;
	thrust::device_vector<double> activeXPoss;
	thrust::device_vector<double> activeYPoss;
	thrust::device_vector<double> activeZPoss;

// temp value for holding a node direction to its corresponding center
	thrust::device_vector<double> distToCenterAlongGrowDir;
};

struct CellGrowthAuxData {
	double prolifDecay;

	double randomGrowthSpeedMin_Ori;
	double randomGrowthSpeedMax_Ori;

	double randomGrowthSpeedMin;
	double randomGrowthSpeedMax;

	double grthPrgrCriVal_M_Ori;
	double grthProgrEndCPU;
// we need help from this parameter to generate better quality pseduo-random numbers.
	uint randGenAuxPara;

	double fixedGrowthSpeed;

	double* growthFactorMagAddress;
	double* growthFactorDirXAddress;
	double* growthFactorDirYAddress;

// obtain pointer address for second region
	double* growthFactorMagAddress2;
	double* growthFactorDirXAddress2;
	double* growthFactorDirYAddress2;

	uint totalNodeCountForActiveCells;

	bool* nodeIsActiveAddress;
	double* nodeXPosAddress;
	double* nodeYPosAddress;

	int* adhIndxAddr;
};

struct InfoForSignal {    //Ali

	double sCenterX; 
	double sCenterY; 
	double sCenterZ; 
	
}; 

struct CellDivAuxData {
// ************************ these parameters are used for cell division *************************
// sum all bool values which indicate whether the cell is going to divide.
// toBeDivideCount is the total number of cells going to divide.
        uint toEnterMitoticCount ; //A&A
	uint toBeDivideCount;
	uint nodeStorageCount;

	//uint toBeRemovedCount;//AAMIRI

	thrust::device_vector<bool> tmpIsActiveHold1;
	thrust::device_vector<double> tmpDistToCenter1;
	thrust::device_vector<uint> tmpCellRankHold1;
	thrust::device_vector<double> tmpXValueHold1;
	thrust::device_vector<double> tmpYValueHold1;
	thrust::device_vector<double> tmpZValueHold1;

	thrust::device_vector<bool> tmpIsActiveHold2;
	thrust::device_vector<double> tmpDistToCenter2;
	thrust::device_vector<double> tmpXValueHold2;
	thrust::device_vector<double> tmpYValueHold2;
	thrust::device_vector<double> tmpZValueHold2;

	thrust::device_vector<SceNodeType> tmpCellTypes;
// ************************ these parameters are used for cell division *************************

	thrust::device_vector<uint> tmpCellRank_M;
	thrust::device_vector<double> tmpDivDirX_M;
	thrust::device_vector<double> tmpDivDirY_M;
	thrust::device_vector<double> tmpCenterPosX_M;
	thrust::device_vector<double> tmpCenterPosY_M;

	thrust::device_vector<bool> tmpIsActive_M;
	thrust::device_vector<double> tmpNodePosX_M;
	thrust::device_vector<double> tmpNodePosY_M;

	thrust::device_vector<bool> tmpIsActiveHost_M;
	thrust::device_vector<double> tmpNodePosXHost_M;
	thrust::device_vector<double> tmpNodePosYHost_M;

	thrust::device_vector<bool> tmpIsActive1_M;
	thrust::device_vector<double> tmpXPos1_M;
	thrust::device_vector<double> tmpYPos1_M;

	thrust::device_vector<bool> tmpIsActive2_M;
	thrust::device_vector<double> tmpXPos2_M;
	thrust::device_vector<double> tmpYPos2_M;

	thrust::device_vector<double> tmpHertwigXdir;  //A&A
	thrust::device_vector<double> tmpHertwigYdir;  //A&A

	std::vector<CVector> tmp1IntnlVec, tmp2IntnlVec;
	std::vector<CVector> tmp1VecMem, tmp2VecMem;
	std::vector<uint> tmp1MemActiveCounts, tmp1InternalActiveCounts;
	std::vector<uint> tmp2MemActiveCounts, tmp2InternalActiveCounts;
};

struct MembrPara {
	double membrStiffCPU;
	double membrStiff_Mitotic;
	double membrEquLenCPU;
	double membrGrowCoeff_Ori;
	double membrGrowLimit_Ori;
	double membrGrowCoeff;
	double membrGrowLimit;
	double membrBendCoeff;
	double membrBendCoeff_Mitotic;
	double membrBendCoeffLeader;
	double adjustLimit;
	double adjustCoeff;

	double growthConst_N;
	uint initMembrCt_N;
	uint initIntnlCt_N;
	void initFromConfig();
        double F_Ext_Incline ; 
};

/**
 * Modified implementation of subcellular element.
 * Important component to process cell growth and division.
 * Boundary, epithilum layer and ECM are also treated as cells but computed seperately.
 * @param beginPosOfBdry   represents begining position of boundary.
 * @param maxNodeOfBdry    represents maximum number of nodes of boundary.
 * @param beginPosOfEpi    represents begining position of epithilum layer.
 * @param maxNodeOfEpi     represents maximum number of nodes of epithilum layer.
 * @param maxNodeOfECM     represents maximum number of nodes per ECM
 * @param beginPosOfECM    represents begining position of ECM.
 * @param maxECMCount      represents maximum number of ECM.
 * @param maxNodeOfOneCell represents maximum number of nodes per cell
 * @param beginPosOfCells  represents begining position of cells.
 * @param maxCellCount     represents maximum number of cells.
 * @param isDivideCrticalRatio If the current cell length divide
 * CellFinalLength is larger than this ratio and the cell growth progress
 *  is complete then we set cell ready to divide
 */
class SceCells {
	SceNodes* nodes;

	Signal  signal;

	NodeAllocPara allocPara;
	SceMiscPara miscPara;
	SceBioPara bioPara;
	CellInfoVecs cellInfoVecs;
	CellNodeInfoVecs cellNodeInfoVecs;
	CellGrowthAuxData growthAuxData;
	CellDivAuxData divAuxData;
	ControlPara controlPara;
	NodeAllocPara_M allocPara_m; // Defined in Cell class
	MembrPara membrPara;
	// vector<SigptState> sigPtVec;

// in this class, there are a lot of arrays that store information for each cell
// this counting iterator is used for all these arrays indicating the begining.
	thrust::counting_iterator<uint> countingBegin;
	thrust::constant_iterator<uint> initIntnlNodeCount;
	thrust::constant_iterator<double> initGrowthProgress;

	uint totalNodeCountForActiveCells;
	bool firstTimeReadDpp ; 
	double  lastTimeExchange ; 
	int currentActiveCellCountOld ;  //Ali  
	double dt;
        double Damp_Coef ;   //Ali
        double InitTimeStage ;  //A & A 
        double Tisu_MinX ; //Ali 
        double Tisu_MaxX ;  //Ali
        double Tisu_MinY ;  //Ali
        double Tisu_MaxY ; //Ali
        double Tisu_R ; //Ali  
	double centerShiftRatio;
	double shrinkRatio;
	double memNewSpacing;
	double curTime;
        vector<double>  dppLevels ; 
        vector<double>  dppLevels_Cell ; 
        vector<double>  dppLevels_CellOld ; 

	void readMiscPara();
	void readBioPara();
    void exchSignal() ; 
	void copyInitActiveNodeCount(
			std::vector<uint>& numOfInitActiveNodesOfCells);
	void copyInitActiveNodeCount_M(std::vector<uint>& initMembrActiveNodeCounts,
			std::vector<uint>& initIntnlActiveNodeCounts,
			std::vector<double> &initGrowProgVec,
			std::vector<double> &initCellRadii);

	void initCellInfoVecs();
	void initCellNodeInfoVecs();
	void initGrowthAuxData();
	void initGrowthAuxData_M();
	void initMyosinLevel();
	void initCellArea();
	

	void initialize(SceNodes* nodesInput);
	void initialize_M(SceNodes* nodesInput, std::vector<double> &initCellRadii);

	void distributeBdryIsActiveInfo();
	void distributeProfileIsActiveInfo();
	void distributeECMIsActiveInfo();
	void distributeCellIsActiveInfo();

	void distributeCellGrowthProgress();

	void distributeIsCellRank();

	void allComponentsMove();

	void growAtRandom(double d_t);

	void growAlongX(bool isAddPt, double d_t);
	void growWithStress(double d_t);

	void randomizeGrowth();
	void setGrowthDirXAxis();

	void computeCenterPos();
    void BC_Imp_M() ; 
	void test_SigPt(std::vector<SigptState>& sigPtVec);
	void divide2DSimplified();

	/**
	 * Use the growth magnitude and dt to update growthProgress.
	 */
	void updateGrowthProgress();

	/**
	 * Decide if the cells are going to add a node or not.
	 * Use lastCheckPoint and growthProgress to decide whether add point or not
	 */
	void decideIsScheduleToGrow();

	/**
	 * Calculate target length of cell given the cell growth progress.
	 * length is along the growth direction.
	 */
	void computeCellTargetLength();

	/**
	 * Compute distance of each node to its corresponding cell center.
	 * The distantce could be either positive or negative, depending on the pre-defined
	 * growth direction.
	 */
	void computeDistToCellCenter();

	/**
	 * For nodes of each cell, find the maximum and minimum distance to the center.
	 * We will then calculate the current length of a cell along its growth direction
	 * using max and min distance to the center.
	 */
	void findMinAndMaxDistToCenter();

	/**
	 * Compute the difference for cells between their expected length and current length.
	 */
	void computeLenDiffExpCur();

	/**
	 * Use the difference that just computed and growthXDir&growthYDir
	 * to apply stretching force (velocity) on nodes of all cells
	 */
	void stretchCellGivenLenDiff();

	/**
	 * This is just an attempt. Cells move according to chemicals.
	 */
	void cellChemotaxis();

	/**
	 * Adjust the velocities of nodes.
	 * For example, velocity of boundary nodes must be zero.
	 */
	void adjustNodeVel();

	/**
	 * Move nodes according to the velocity we just adjusted.
	 */
	void moveNodes();

	/**
	 * Add a point to a cell if it is scheduled to grow.
	 * This step does not guarantee success ; If adding new point failed, it will not change
	 * isScheduleToGrow and activeNodeCount;
	 */
	void addPointIfScheduledToGrow();

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

	bool decideIfGoingToDivide();

	void copyCellsPreDivision();

	void sortNodesAccordingToDist();

	void copyLeftAndRightToSeperateArrays();

	void transformIsActiveArrayOfBothArrays();

	void addSecondArrayToCellArray();

	void copyFirstArrayToPreviousPos();

	void updateActiveCellCount();

	void markIsDivideFalse();

	/**
	 * This is different from the default set function.
	 */
	void setCellTypes(thrust::device_vector<SceNodeType> cellTypesInput) {
		thrust::copy(cellTypesInput.begin(), cellTypesInput.end(),
				cellInfoVecs.cellTypes.begin());
	}

	void distributeIsActiveInfo();

	void applyMemForce_M();

	void updateActivationLevel();

	// void applyMembrAdhCell_M();

// 	void copyExtForcesCell_M();

	void applySceCellDisc_M();

	// void updateCellAdhIndex();

	void updateCellPolar();

	void calFluxWeightsMyosin(); 

	void calSceCellMyosin();

	void updateCellPolarLeader();

	void applySceCellMyosin();

	void applySigForce(std::vector<SigptStateV2>& sigPtVecV2);

	void calSubAdhForce();

	void computeCenterPos_M();

	void computeCellRadius();

	void growAtRandom_M(double dt);

	void divide2D_M();

	void distributeCellGrowthProgress_M();

	void distributeCellActivationLevel_M();

	void distributeCell_Type_M();

	void allComponentsMove_M();

	void randomizeGrowth_M();

	void updateGrowthProgress_M();

	void decideIsScheduleToGrow_M();

	void decideIsScheduleToShrink_M();//AAMIRI

	void computeCellTargetLength_M();

	void computeDistToCellCenter_M();

	void findMinAndMaxDistToCenter_M();

	void computeLenDiffExpCur_M();

	void stretchCellGivenLenDiff_M();

	void addPointIfScheduledToGrow_M();

	void addPointDueToActin(); 

	void delPointIfScheduledToGrow_M();//AAMIRI

	void findTangentAndNormal_M();//AAMIRI

	/**
	 * It is possible for cells to have node number that is less than expect after division.
	 * This adjustment is necessary for cells to grow normally.
	 */
	void adjustGrowthInfo_M();

	void copyCellsPreDivision_M();
	void copyCellsEnterMitotic(); //A&A
        void findHertwigAxis(); //A&A 
	void createTwoNewCellArr_M();
	void copyFirstCellArr_M();
	void copySecondCellArr_M();
	void updateActiveCellCount_M();
	void markIsDivideFalse_M();

	//void removeCellArr_M();//AAMIRI
	//void updateActiveCellCountAfterRemoval_M();//AAMIRI

	void adjustNodeVel_M();
	void moveNodes_M();
	void moveNodes_BC_M();

	void readMiscPara_M();
	void initCellInfoVecs_M();
	void initCellNodeInfoVecs_M();

	void handleMembrGrowth_M();

	void copyToGPUConstMem();

	void myDebugFunction();
	void divDebug();
	void membrDebug();

	void calMembrGrowSpeed_M();
	/**
	 * if cell is under compression, its area will not be less than expected.
	 * growth speed is adjusted by sqrt(area) and circumference ratio.
	 */
	void adjustMembrGrowSpeed_M();
	void decideIfAddMembrNode_M();
	void addMembrNodes_M();

	bool tmpDebug;

	bool decideIfGoingToDivide_M();
    bool decideIfAnyCellEnteringMitotic();//A&A 
//	bool decideIfGoingToRemove_M();//AAMIRI

	void assembleVecForTwoCells(uint i);
	void shiftIntnlNodesByCellCenter(CVector cell1Center, CVector cell2Center);
	void processMemVec(std::vector<VecVal>& tmp1, std::vector<VecVal>& tmp2);
	void obtainMembrAndIntnlNodes(uint i, vector<CVector>& membrNodes,
			vector<CVector>& intnlNodes);
	CVector obtainCenter(uint i);
	CVector calDivDir_MajorAxis(CVector oldCenter, vector<CVector>& membrNodes,
			double& lenAlongMajorAxis);

	double calLengthAlongHertwigAxis(CVector divDir, CVector oldCenter, vector<CVector>& membrNodes); //A&A

	void obtainTwoNewCenters(CVector& oldCenter, CVector& divDir,
			double lenAlongHertwigAxis, CVector& centerNew1, CVector& centerNew2); //A& A  modified
	void prepareTmpVec(uint i, CVector divDir, CVector oldCenter,
			std::vector<VecVal>& tmp1, std::vector<VecVal>& tmp2);

	void calCellArea();
        void calCellPerim();//AAMIRI
public:
	SceCells();

	SceCells(SceNodes* nodesInput,
			std::vector<uint> &numOfInitActiveNodesOfCells,
			std::vector<SceNodeType> &cellTypes);

	SceCells(SceNodes* nodesInput,
			std::vector<uint> &numOfInitActiveMembrNodeCounts,
			std::vector<uint> &numOfInitActiveIntnlNodeCounts,
			std::vector<double> &initGrowProgVec, 
			std::vector<double> &initCellRadii, double InitTimeStage);

	void runAllCellLevelLogicsDisc(double dt);

    // void runAllCellLogicsDisc_M(double dt);
	// void runAllCellLogicsDisc_M(double dt, double Damp_Coef, double InitTimeStage, std::vector<SigptStateV2>& sigPtVecV2);
	void runAllCellLogicsDisc_M(double dt, double Damp_Coef, double InitTimeStage);  //, std::vector<double>& fluxWeightsVec

	void runStretchTest(double dt);

	std::vector<CVector> getAllCellCenters();
	//void getAllCellCenters();
	std::vector<double> getGrowthProgressVec();

	void runAblationTest(AblationEvent &ablEvent);

	const NodeAllocPara& getAllocPara() const {
		return allocPara;
	}

	void setAllocPara(const NodeAllocPara& allocPara) {
		this->allocPara = allocPara;
	}

	AniRawData obtainAniRawData(AnimationCriteria& aniCri);

	AniRawData obtainAniRawDataGivenCellColor(vector<double>& cellColors,
			AnimationCriteria& aniCri,vector<double>& cellsPerimeter, vector<double> & cellsDppLevel);

	VtkAnimationData outputVtkData(AniRawData& rawAniData,
			AnimationCriteria& aniCri);


	CellsStatsData outputPolyCountData();

	const NodeAllocPara_M& getAllocParaM() const {
		return allocPara_m;
	}

	void setAllocParaM(const NodeAllocPara_M& allocPara_M) {
		allocPara_m = allocPara_M;
	} // not used??? not called 

	bool aniDebug;
};

#endif /* SCECELLS_H_ */
