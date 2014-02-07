#pragma once

#include <QVector2D>
#include <vector>
#include <boost/shared_ptr.hpp>

class RoadEdge {
public:
	unsigned int type;
	unsigned int lanes;
	bool oneWay;
	bool link;
	bool roundabout;
	std::vector<QVector2D> polyLine;
	float importance;

	bool valid;			// if this edge is valid
	bool seed;			// if this edge is used as a seed of a forest
	int group;			// to which tree in the forest this edge belongs to
	bool fullyPaired;	// if this edge has a corresponding edge

public:
	RoadEdge(int type, int lanes, bool oneWay = false, bool link = false, bool roundabout = false);
	~RoadEdge();
	
	float getLength();

	void addPoint(const QVector2D &pt);
};

typedef boost::shared_ptr<RoadEdge> RoadEdgePtr;