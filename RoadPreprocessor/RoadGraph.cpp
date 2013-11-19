#include "RoadGraph.h"
#include "Util.h"
#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>

#define SQR(x)		((x) * (x))

using namespace std;

RoadGraph::RoadGraph() {
}

RoadVertexDesc RoadGraph::addVertex(RoadVertex* vertex) {
	RoadVertexDesc desc = boost::add_vertex(graph);
	graph[desc] = vertex;

	return desc;
}


/**
 * Change the specified vertex as a point along the edge between two vertices that are connected this vertex.
 *
 * @param desc		the specified vertex
 * @return			true if the vertex is changed / false otherwise.
 */
bool RoadGraph::reduceVertex(RoadVertexDesc desc) {
	int count = 0;
	RoadVertexDesc vd[2];
	RoadEdgeDesc ed[2];
	RoadEdge* edges[2];

	RoadOutEdgeIter ei, ei_end;
	for (boost::tie(ei, ei_end) = out_edges(desc, graph); ei != ei_end; ++ei, ++count) {
		vd[count] = boost::target(*ei, graph);
		ed[count] = *ei;
		edges[count] = graph[*ei];
	}

	if (edges[0]->type != edges[1]->type) return false;
	if (edges[0]->oneWay != edges[1]->oneWay) return false;
	if (edges[0]->lanes != edges[1]->lanes) return false;

	RoadEdge* new_edge = new RoadEdge(edges[0]->oneWay, edges[0]->lanes, edges[0]->type);
	edges[0]->startFrom(graph[vd[0]]->getPt());
	edges[1]->startFrom(graph[desc]->getPt());
	
	for (int i = 0; i < edges[0]->getPolyLine().size(); i++) {
		new_edge->addPoint(edges[0]->getPolyLine()[i]);
	}
	for (int i = 1; i < edges[1]->getPolyLine().size(); i++) {
		new_edge->addPoint(edges[1]->getPolyLine()[i]);
	}
	addEdge(vd[0], vd[1], new_edge);

	boost::remove_edge(vd[0], desc, graph);
	boost::remove_edge(vd[1], desc, graph);
	delete edges[0];
	if (edges[1] != edges[0]) delete edges[1];

	RoadVertex* vertex = graph[desc];
	boost::remove_vertex(desc, graph);
	delete vertex;

	return true;
}

RoadEdgeDesc RoadGraph::addEdge(RoadVertexDesc v1, RoadVertexDesc v2, RoadEdge* edge) {
	std::pair<RoadEdgeDesc, bool> edge_pair = boost::add_edge(v1, v2, graph);
	graph[edge_pair.first] = edge;

	return edge_pair.first;
}

bool RoadGraph::isRedundant(RoadVertexDesc desc, const QVector3D &dir, float threshold) {
	RoadOutEdgeIter ei, ei_end;
	for (boost::tie(ei, ei_end) = out_edges(desc, graph); ei != ei_end; ++ei) {
		RoadVertexDesc tgt = boost::target(*ei, graph);
		QVector3D d = graph[tgt]->getPt() - graph[desc]->getPt();

		if (fabs(atan2f(dir.y(), dir.x()) - atan2f(d.y(), d.x())) < threshold) return true;
	}

	return false;
}

void RoadGraph::clear() {
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(graph); vi != vend; ++vi) {
		RoadVertex* v = graph[*vi];
		delete v;
	}

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(graph); ei != eend; ++ei) {
		RoadEdge* edge = graph[*ei];
		delete edge;
	}

	graph.clear();
}

void RoadGraph::reduce() {
	// reduce the graph by removing the vertices which have two outing edges.
	RoadVertexIter vi, vend;
	bool deleted = false;
	do {
		deleted = false;

		for (boost::tie(vi, vend) = boost::vertices(graph); vi != vend; ++vi) {
			RoadVertex* v = graph[*vi];

			if (boost::degree(*vi, graph) == 2) {
				if (reduceVertex(*vi)) {
					deleted = true;
					break;
				}
			}
		}
	} while (deleted);

	// 独立している頂点を削除
	do {
		deleted = false;

		for (boost::tie(vi, vend) = boost::vertices(graph); vi != vend; ++vi) {
			RoadVertex* v = graph[*vi];

			if (boost::degree(*vi, graph) == 0) {
				boost::remove_vertex(*vi, graph);
				deleted = true;
				break;
			}
		}
	} while (deleted);
}

void RoadGraph::save(FILE* fp) {
	int nVertices = boost::num_vertices(graph);
	fwrite(&nVertices, sizeof(int), 1, fp);

	// 各頂点につき、ID、X座標、Y座標を出力する
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(graph); vi != vend; ++vi) {
		RoadVertex* v = graph[*vi];
	
		RoadVertexDesc desc = *vi;
		float x = v->getPt().x();
		float y = v->getPt().y();
		fwrite(&desc, sizeof(RoadVertexDesc), 1, fp);
		fwrite(&x, sizeof(float), 1, fp);
		fwrite(&y, sizeof(float), 1, fp);
	}

	int nEdges = boost::num_edges(graph);
	fwrite(&nEdges, sizeof(int), 1, fp);

	// 各エッジにつき、２つの頂点の各ID、道路タイプ、レーン数、一方通行か、ポリラインを構成するポイント数、各ポイントのX座標とY座標を出力する
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(graph); ei != eend; ++ei) {
		RoadEdge* edge = graph[*ei];

		RoadVertexDesc src = boost::source(*ei, graph);
		RoadVertexDesc tgt = boost::target(*ei, graph);

		fwrite(&src, sizeof(RoadVertexDesc), 1, fp);
		fwrite(&tgt, sizeof(RoadVertexDesc), 1, fp);
		
		unsigned int type = edge->getType();
		fwrite(&type, sizeof(unsigned int), 1, fp);

		unsigned int lanes = edge->getNumLanes();
		fwrite(&lanes, sizeof(unsigned int), 1, fp);

		unsigned int oneWay;
		if (edge->oneWay) {
			oneWay = 1;
		} else {
			oneWay = 0;
		}
		fwrite(&oneWay, sizeof(unsigned int), 1, fp);

		int nPoints = edge->getPolyLine().size();
		fwrite(&nPoints, sizeof(int), 1, fp);

		for (int i = 0; i < edge->getPolyLine().size(); i++) {
			float x = edge->getPolyLine()[i].x();
			float y = edge->getPolyLine()[i].y();
			fwrite(&x, sizeof(float), 1, fp);
			fwrite(&y, sizeof(float), 1, fp);
		}
	}
}

// 立体交差を解消する
void RoadGraph::solveCrossover() {
	while (true) {
		if (!solveOneCrossover()) break;
	}
}

/**
 * 立体交差を一つ見つけて解消する。一つも立体交差がない場合は、falseを返却する。
 */
bool RoadGraph::solveOneCrossover() {
	RoadEdgeIter a_ei, a_ei_end;
	RoadEdgeIter b_ei, b_ei_end;

	for (boost::tie(a_ei, a_ei_end) = boost::edges(graph); a_ei != a_ei_end; ++a_ei) {
		RoadEdge* edgeA = graph[*a_ei];

		for (b_ei = a_ei; b_ei != a_ei_end; ++b_ei) {
			if (b_ei == a_ei) continue;

			RoadEdge* edgeB = graph[*b_ei];

			for (int i = 0; i < edgeA->getPolyLine().size() - 1; i++) {
				QVector2D a0 = edgeA->getPolyLine()[i];
				QVector2D a1 = edgeA->getPolyLine()[i + 1];

				for (int j = 0; j < edgeB->getPolyLine().size() - 1; j++) {
					QVector2D b0 = edgeB->getPolyLine()[j];
					QVector2D b1 = edgeB->getPolyLine()[j + 1];

					QVector2D intPt;
					float ta0a1, tb0b1;
					if (Util::segmentSegmentIntersectXY(a0, a1, b0, b1, &ta0a1, &tb0b1, true, intPt)) {
						int i2;
						if ((edgeA->getPolyLine()[i] - intPt).length() < 0.1f) {
							i2 = i;
						} else if ((edgeA->getPolyLine()[i + 1] - intPt).length() < 0.1f) {
							i2 = i + 1;
						} else {
							edgeA->getPolyLine().insert(edgeA->getPolyLine().begin() + (i + 1), intPt);
							i2 = i + 1;
						}

						int j2;
						if ((edgeB->getPolyLine()[j] - intPt).length() < 0.1f) {
							j2 = i;
						} else if ((edgeB->getPolyLine()[j + 1] - intPt).length() < 0.1f) {
							j2 = j + 1;
						} else {
							edgeB->getPolyLine().insert(edgeB->getPolyLine().begin() + (j + 1), intPt);
							j2 = j + 1;
						}

						RoadVertexDesc new_v = splitEdgeAt(*a_ei, i2);
						splitEdgeAt(*b_ei, j2, new_v);
						return true;
					}
				}
			}
		}		
	}
	
	return false;
}

RoadVertexDesc RoadGraph::splitEdgeAt(RoadEdgeDesc desc, int index) {
	RoadEdge* edge = graph[desc];
	QVector2D pos = edge->getPolyLine()[index];
	RoadVertexDesc src = boost::source(desc, graph);
	RoadVertexDesc tgt = boost::target(desc, graph);

	if ((graph[src]->getPt() - pos).length() == 0.0f) return src;
	if ((graph[tgt]->getPt() - pos).length() == 0.0f) return tgt;

	// add a new vertex at the specified point on the edge
	RoadVertex* vertex = new RoadVertex(pos);
	RoadVertexDesc v = boost::add_vertex(graph);
	graph[v] = vertex;

	// add the first edge
	RoadEdge* edge1 = new RoadEdge(edge->oneWay, edge->lanes, edge->type);
	if ((edge->getPolyLine()[0] - graph[src]->getPt()).length() < (edge->getPolyLine()[0] - graph[tgt]->getPt()).length()) {
		//for (int i = 0; i <= index; i++) {
		for (int i = index; i >= 0; i--) {
			edge1->addPoint(edge->getPolyLine()[i]);
		}

	} else {
		for (int i = index; i < edge->getPolyLine().size(); i++) {
			edge1->addPoint(edge->getPolyLine()[i]);
		}
	}
	std::pair<RoadEdgeDesc, bool> edge_pair1 = boost::add_edge(src, v, graph);
	graph[edge_pair1.first] = edge1;

	// add the second edge
	RoadEdge* edge2 = new RoadEdge(edge->oneWay, edge->lanes, edge->type);
	if ((edge->getPolyLine()[0] - graph[src]->getPt()).length() < (edge->getPolyLine()[0] - graph[tgt]->getPt()).length()) {
		for (int i = index; i < edge->getPolyLine().size(); i++) {
			edge2->addPoint(edge->getPolyLine()[i]);
		}
	} else {
		for (int i = index; i >= 0; i--) {
			edge2->addPoint(edge->getPolyLine()[i]);
		}
	}
	std::pair<RoadEdgeDesc, bool> edge_pair2 = boost::add_edge(v, tgt, graph);
	graph[edge_pair2.first] = edge2;

	// remove the original edge
	boost::remove_edge(desc, graph);

	return v;
}

/**
 * エッジを分割する。分割点のノードIDは、すでにvとして登録済みなので、新たにノードを作成する必要はない。
 */
RoadVertexDesc RoadGraph::splitEdgeAt(RoadEdgeDesc desc, int index, RoadVertexDesc v) {
	RoadEdge* edge = graph[desc];
	QVector2D pos = edge->getPolyLine()[index];
	RoadVertexDesc src = boost::source(desc, graph);
	RoadVertexDesc tgt = boost::target(desc, graph);

	// add the first edge
	RoadEdge* edge1 = new RoadEdge(edge->oneWay, edge->lanes, edge->type);
	if ((edge->getPolyLine()[0] - graph[src]->getPt()).length() < (edge->getPolyLine()[0] - graph[tgt]->getPt()).length()) {
		//for (int i = 0; i <= index; i++) {
		for (int i = index; i >= 0; i--) {
			edge1->addPoint(edge->getPolyLine()[i]);
		}

	} else {
		for (int i = index; i < edge->getPolyLine().size(); i++) {
			edge1->addPoint(edge->getPolyLine()[i]);
		}
	}
	std::pair<RoadEdgeDesc, bool> edge_pair1 = boost::add_edge(src, v, graph);
	graph[edge_pair1.first] = edge1;

	// add the second edge
	RoadEdge* edge2 = new RoadEdge(edge->oneWay, edge->lanes, edge->type);
	if ((edge->getPolyLine()[0] - graph[src]->getPt()).length() < (edge->getPolyLine()[0] - graph[tgt]->getPt()).length()) {
		for (int i = index; i < edge->getPolyLine().size(); i++) {
			edge2->addPoint(edge->getPolyLine()[i]);
		}
	} else {
		for (int i = index; i >= 0; i--) {
			edge2->addPoint(edge->getPolyLine()[i]);
		}
	}
	std::pair<RoadEdgeDesc, bool> edge_pair2 = boost::add_edge(v, tgt, graph);
	graph[edge_pair2.first] = edge2;

	// remove the original edge
	boost::remove_edge(desc, graph);

	return v;
}

void RoadGraph::check() {
	// 頂点が近すぎないことをチェック
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(graph); vi != vend; ++vi) {
		RoadVertex* v = graph[*vi];
	
		RoadVertexIter vi2, vend2;
		for (boost::tie(vi2, vend2) = boost::vertices(graph); vi2 != vend2; ++vi2) {
			if (*vi == *vi2) continue;
			RoadVertex* v2 = graph[*vi2];

			if ((v->getPt() - v2->getPt()).length() < 5.0f) {
				int k = 0;
			}
		}
	}

	// エッジの方向が近すぎないことをチェック
	for (boost::tie(vi, vend) = boost::vertices(graph); vi != vend; ++vi) {
		RoadVertex* v = graph[*vi];

		if (fabs(v->getPt().x() + 970.5) < 0.11f && fabs(v->getPt().y() + 922.76) < 0.11f) {
 			int kkk = 0;
		}
	
		RoadOutEdgeIter ei, ei_end;
		int cnt;
		for (boost::tie(ei, ei_end) = out_edges(*vi, graph), cnt = 0; ei != ei_end; ++ei, ++cnt) {
			RoadEdge* edge = graph[*ei];
			RoadVertexDesc tgt = boost::target(*ei, graph);
			QVector3D d = graph[tgt]->getPt() - v->getPt();

			int cnt2;
			RoadOutEdgeIter ei2, ei_end2;
			for (boost::tie(ei2, ei_end2) = out_edges(*vi, graph), cnt2 = 0; ei2 != ei_end2; ++ei2, ++cnt2) {
				if (cnt == cnt2) continue;
				RoadVertexDesc tgt2 = boost::target(*ei2, graph);
				QVector3D d2 = graph[tgt2]->getPt() - v->getPt();
				if (fabs(atan2f(d.y(), d.x()) - atan2f(d2.y(), d2.x())) < 0.01f) {
					int kkk = 0;
				}
			}
		}
	}
}