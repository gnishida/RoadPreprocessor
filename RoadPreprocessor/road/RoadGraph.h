#pragma once

#include <stdio.h>
#include <QVector2D>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/shared_ptr.hpp>
#include "RoadVertex.h"
#include "RoadEdge.h"

using namespace boost;

typedef adjacency_list<vecS, vecS, undirectedS, RoadVertexPtr, RoadEdgePtr> BGLGraph;
typedef graph_traits<BGLGraph>::vertex_descriptor RoadVertexDesc;
typedef graph_traits<BGLGraph>::edge_descriptor RoadEdgeDesc;
typedef graph_traits<BGLGraph>::vertex_iterator RoadVertexIter;
typedef graph_traits<BGLGraph>::edge_iterator RoadEdgeIter;
typedef graph_traits<BGLGraph>::out_edge_iterator RoadOutEdgeIter;
typedef graph_traits<BGLGraph>::in_edge_iterator RoadInEdgeIter;

class RoadGraph {
public:
	BGLGraph graph;

public:
	RoadGraph();
	~RoadGraph();

	void clear();
};

typedef boost::shared_ptr<RoadGraph> RoadGraphPtr;
