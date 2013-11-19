#pragma once

#include <vector>
#include <qstring.h>

class RoadEdge {
public:
	unsigned int id;
	std::vector<unsigned int> nds;
	//QString highway;
	unsigned int type;		// 0: sidewalk / 1: street / 2: avenue / 3: highway
	unsigned int lanes;

public:
	RoadEdge();
};

