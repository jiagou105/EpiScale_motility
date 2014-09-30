#ifndef SimulationDomainGPU_H_
#define SimulationDomainGPU_H_

#include "SceNodes.h"
#include "SceCells.h"
#include "Cartilage.h"
//#include "CellInitHelper.h"
#include "commonData.h"

#include <sstream>
#include <iomanip>
#include <fstream>
#include <string>

class StabPara {
public:
	int outputFrameCount;
	int totalIterCount;
	double bdrySpacingRatio;
	double dt;
	std::string outputAniName;
};

/**
 * This class is responsible for domain-wise highest level logic, e.g. output animation.
 */
class SimulationDomainGPU {
	/**
	 * Variable that contains information for nodes.
	 * Handles node level interaction logic.
	 */
	SceNodes nodes;

	/**
	 * Variable that contains information for cells.
	 * Handles cell level logics like growth and division.
	 */
	SceCells cells;

	/**
	 * Cartilage is another important components in the model.
	 */
	Cartilage cartilage;

	/**
	 * Growth map that controls the growth for cells.
	 */
	GrowthDistriMap growthMap;

	/**
	 * Growth map that controls the growth for cells.
	 */
	GrowthDistriMap growthMap2;

	/**
	 * memory related parameters.
	 */
	SceMemPara memPara;

	/**
	 * domain related parameters.
	 */
	SceDomainPara domainPara;

	/**
	 * chemical related parameters.
	 */
	SceChemPara chemPara;

	/**
	 * parameters used for stabilize the initial cell positions
	 */
	StabPara stabPara;

	/**
	 * reads memory related parameters.
	 */
	void readMemPara();

	/**
	 * reads domain related parameters.
	 */
	void readDomainPara();

	/**
	 * reads chemical related parameters.
	 */
	void readChemPara();

	/**
	 * reads all parameters by calling all other reading methods.
	 */
	void readAllParameters();

	/**
	 * initializes growth maps.
	 */
	void initializeGrowthMap();

	/**
	 * Initializes data vectors by given vectors.
	 * This function was written in the past and may not be very robust.
	 */
	void initialCellsOfFiveTypes(std::vector<SceNodeType> &cellTypes,
			std::vector<uint> &numOfInitActiveNodesOfCells,
			std::vector<double> &initBdryCellNodePosX,
			std::vector<double> &initBdryCellNodePosY,
			std::vector<double> &initProfileNodePosX,
			std::vector<double> &initProfileNodePosY,
			std::vector<double> &initECMNodePosX,
			std::vector<double> &initECMNodePosY,
			std::vector<double> &initFNMCellNodePosX,
			std::vector<double> &initFNMCellNodePosY,
			std::vector<double> &initMXCellNodePosX,
			std::vector<double> &initMXCellNodePosY);

	/**
	 * Initializes data vectors by given vectors.
	 * improved from the previous version.
	 */
	void initializeNodes(std::vector<SceNodeType> &cellTypes,
			std::vector<uint> &numOfInitActiveNodesOfCells,
			std::vector<CVector> &initBdryNodeVec,
			std::vector<CVector> &initProfileNodeVec,
			std::vector<CVector> &initCartNodeVec,
			std::vector<CVector> &initECMNodeVec,
			std::vector<CVector> &initFNMNodeVec,
			std::vector<CVector> &initMXNodeVec);

public:
	/**
	 * Default constructor.
	 * Reads all the configuration values from file.
	 */
	SimulationDomainGPU();

	/**
	 * Domain initialization.
	 * Assigns values to the data fields in simulation domain.
	 * @param initData initial data set for simulation domain
	 */
	void initialize(SimulationInitData &initData);

	/**
	 * Domain initialization.
	 * Assigns values to the data fields in simulation domain.
	 * @param initData initial data set for simulation domain
	 */
	void initialize_v2(SimulationInitData_V2 &initData);

	/**
	 * This method is used for producing more or less evenly distributed cell center positions.
	 * Because CGAL mesh cannot guarantee that mesh vertices are relatively evenly distributed,
	 * we would need this extra step in order to stabilize the cell center positions
	 * so that cells can be allocated evenly in our simulation domain.
	 */
	std::vector<CVector> stablizeCellCenters(SimulationInitData &initData);

	/**
	 * Checks if all data fields are valid.
	 * This methods only loosely checks the data validity.
	 */
	void checkIfAllDataFieldsValid();

	/**
	 * Run one step of simulation in the domain.
	 * Contains cell level logics and node level logics.
	 * @param dt timestep
	 */
	void runAllLogic(double dt);

	/**
	 * Method that animates the domain to VTK format.
	 * @param scriptNameBase name of the vtk animation series.
	 * @param rank frame sequence in the vtk animation series.
	 * @param aniCri criteria for outputing animation.
	 */
	void outputVtkFilesWithColor(std::string scriptNameBase, int rank,
			AnimationCriteria aniCri);
};

#endif