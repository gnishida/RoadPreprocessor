#pragma once

#include <qvector2d.h>
#include <vector>

class RoadEdge {
public:
	bool oneWay;
	unsigned int lanes;
	unsigned int type;
	std::vector<QVector2D> polyLine;

public:
	RoadEdge(bool oneWay, int numLanes, int typeRoad);
	~RoadEdge();
	
	int getNumLanes();
	float getLength();
	int getType();
	std::vector<QVector2D>& getPolyLine();

	void addPoint(const QVector2D &pt);
	float getWidth();

	void startFrom(QVector2D start);
	bool containsPoint(const QVector2D &pos);
};

