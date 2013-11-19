#include "RoadEdge.h"
#include "Util.h"

RoadEdge::RoadEdge(bool oneWay, int lanes, int type) {
	this->oneWay = oneWay;
	this->lanes = lanes;
	this->type = type;
}

RoadEdge::~RoadEdge() {
}

int RoadEdge::getNumLanes() {
	return lanes;
}

float RoadEdge::getLength() {
	float length = 0.0f;
	for (int i = 0; i < polyLine.size() - 1; i++) {
		length += (polyLine[i + 1] - polyLine[i]).length();
	}
	return length;
}

int RoadEdge::getType() {
	return type;
}

std::vector<QVector2D>& RoadEdge::getPolyLine() {
	return polyLine;
}

/**
 * Add a point to the polyline of the road segment.
 *
 * @param pt		new point to be added
 */
void RoadEdge::addPoint(const QVector2D &pt) {
	polyLine.push_back(pt);
}

float RoadEdge::getWidth() {
	return lanes * 2 * 3.5f;
}

/**
 * Order the poly line of the road segment by starting from the point that is close to the specified point.
 *
 * @param start			the specified point to start the poly line
 */
void RoadEdge::startFrom(QVector2D start) {
	if (polyLine.size() == 0) return;

	if ((polyLine[0] - start).lengthSquared() > (polyLine[polyLine.size() - 1] - start).lengthSquared()) {
		std::reverse(polyLine.begin(), polyLine.end());
	}
}

/**
 * Check whether the point resides in this road segment.
 * Return true if so, false otherwise.
 *
 * @param pos		the point
 * @return			true if the point is inside the road segment, false otherwise
 */
bool RoadEdge::containsPoint(const QVector2D &pos) {
	for (int i = 0; i < polyLine.size() - 1; i++) {
		QVector2D p0 = polyLine[i];
		QVector2D p1 = polyLine[i + 1];
		if (Util::pointSegmentDistanceXY(p0, p1, pos) <= getWidth()) return true;
	}

	return false;
}