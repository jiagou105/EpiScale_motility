/*
 * Point2D.cpp
 *
 *  Created on: Aug 20, 2014
 *      Author: wsun2
 */

#include "Point2D.h"

namespace GEOMETRY {

Point2D::Point2D() {
	isLeader = false;
	isOnBdry = false;
	x = 0.0;
	y = 0.0;
	rad = 0.0;
}

Point2D::~Point2D() {

}

void Point2D::Assign_M2(double inputx1, double inputy1, double inputRad) {
	isLeader = false;
	isOnBdry = false;
	x = inputx1;
	y = inputy1;
	rad = inputRad;
}


GEOMETRY::Point2D::Point2D(double inputX, double inputY, double inputRad) {
	isLeader = false;
	isOnBdry = false;
	x = inputX;
	y = inputY;
	rad = inputRad;
}


} /* namespace GEOMETRY */
