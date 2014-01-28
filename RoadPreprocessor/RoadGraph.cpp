#include "RoadGraph.h"
#include "Util.h"
#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>

#define SQR(x)		((x) * (x))

using namespace std;

RoadGraph::RoadGraph() {
}

void RoadGraph::clear() {
	graph.clear();
}
