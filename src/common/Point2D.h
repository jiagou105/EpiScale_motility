/*
 * Point2D.h
 *
 *  Created on: Aug 20, 2014
 *      Author: wsun2
 */

#ifndef POINT2D_H_
#define POINT2D_H_

namespace GEOMETRY {

class Point2D {
	bool isOnBdry;
	bool isLeader;
	double x;
	double y;
	double rad;
public:
	Point2D();
	Point2D(double inputX, double inputY, double inputRad);

	virtual ~Point2D();

	double getX() const {
		return x;
	}

	void setX(double x) {
		this->x = x;
	}

	double getY() const {
		return y;
	}

	void setY(double y) {
		this->y = y;
	}
		
	double getRad() const {
		return rad;
	}

	void setRad(double rad) {
		this->rad = rad;
	}

	bool isIsOnBdry() const {
		return isOnBdry;
	}

	void setIsOnBdry(bool isOnBdry) {
		this->isOnBdry = isOnBdry;
	}
	bool isIsLeader() const {
		return isLeader;
	}

	void setIsLeader(bool isOnBdry) {
		this->isLeader = isLeader;
	}
    void Assign_M2(double inputx1, double inputy1, double inputRad) ; 
                 

};

} /* namespace GEOMETRY */
#endif /* POINT2D_H_ */
