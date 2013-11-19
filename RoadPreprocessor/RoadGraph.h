#pragma once

#include "RoadVertex.h"
#include "RoadEdge.h"
#include "BBox2D.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/graph_utility.hpp>

using namespace boost;


typedef adjacency_list<vecS, vecS, undirectedS, RoadVertex*, RoadEdge*> BGLGraph;
typedef graph_traits<BGLGraph>::vertex_descriptor RoadVertexDesc;
typedef graph_traits<BGLGraph>::edge_descriptor RoadEdgeDesc;
typedef graph_traits<BGLGraph>::vertex_iterator RoadVertexIter;
typedef graph_traits<BGLGraph>::edge_iterator RoadEdgeIter;
typedef graph_traits<BGLGraph>::out_edge_iterator RoadOutEdgeIter;
typedef graph_traits<BGLGraph>::in_edge_iterator RoadInEdgeIter;

typedef struct {
	float location[3];
	float tex[2];
	float normal[3];
	float color[4];
	char padding[16];
} Vertex;

class RoadStyle;

class RoadGraph {
public:
	BGLGraph graph;

public:
	RoadGraph();

	RoadVertexDesc addVertex(RoadVertex* vertex);
	bool reduceVertex(RoadVertexDesc desc);
	RoadEdgeDesc addEdge(RoadVertexDesc v1, RoadVertexDesc v2, RoadEdge* edge);
	bool isRedundant(RoadVertexDesc desc, const QVector3D &dir, float threshold);

	void clear();
	void reduce();
	void save(FILE* fp);
	void solveCrossover();
	bool solveOneCrossover();
	RoadVertexDesc splitEdgeAt(RoadEdgeDesc desc, int index);
	RoadVertexDesc splitEdgeAt(RoadEdgeDesc desc, int index, RoadVertexDesc v);
	void check();
};

