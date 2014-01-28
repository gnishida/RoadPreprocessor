#pragma once

#include "MapRange.h"
#include "BBox2D.h"
#include <qstring.h>
#include <vector>

class RoadGraph;

//class RoadEdge;

class RoadPreprocessor {
public:
	RoadPreprocessor();

	void preprocess(QString filename, const QVector2D &lonlat, const BBox2D &range);

private:
	void writeGSM(FILE* fp, RoadGraph *roads);
	//void writeXML(MapRange &ranges, QMap<unsigned int, RoadNode*> *nodes, std::vector<RoadEdge*> *edges);
};

