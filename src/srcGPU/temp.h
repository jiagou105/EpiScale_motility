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