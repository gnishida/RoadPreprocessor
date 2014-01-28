#include "RoadEdge.h"
#include "Util.h"

RoadEdge::RoadEdge(int type, int lanes, bool oneWay) {
	this->type = type;
	this->lanes = lanes;
	this->oneWay = oneWay;

	this->valid = true;
	this->importance = 0.0f;
	this->group = 0;
	this->seed = false;
	this->fullyPaired = false;
}

RoadEdge::~RoadEdge() {
}

float RoadEdge::getLength() {
	float length = 0.0f;
	for (int i = 0; i < polyLine.size() - 1; i++) {
		length += (polyLine[i + 1] - polyLine[i]).length();
	}
	return length;
}

/**
 * Add a point to the polyline of the road segment.
 *
 * @param pt		new point to be added
 */
void RoadEdge::addPoint(const QVector2D &pt) {
	polyLine.push_back(pt);
}
