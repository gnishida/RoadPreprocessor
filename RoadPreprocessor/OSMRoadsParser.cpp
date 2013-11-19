#include "OSMRoadsParser.h"
#include "RoadVertex.h"
#include "RoadEdge.h"
#include "BBox2D.h"
#include "Util.h"
#include <vector>
#include <iostream>

#define	THRESHOLD_CLOSE		5.0f

OSMRoadsParser::OSMRoadsParser(RoadGraph *roads, const QVector2D &lonlat, const BBox2D &range, int roadType) {
	this->roads = roads;
	this->centerLonLat = lonlat;
	this->centerPos = Util::projLatLonToMeter(lonlat, lonlat);
	this->range = range;
	this->roadType = roadType;

	way.parentNodeName = "osm";
}

bool OSMRoadsParser::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts) {
	if (localName == "node") {
		handleNode(atts);
	} else if (localName == "way") {
		way.parentNodeName = "way";
		handleWay(atts);
	} else if (localName == "nd") {
		if (way.parentNodeName == "way") {
			handleNd(atts);
		}
	} else if (localName == "tag") {
		if (way.parentNodeName == "way") {
			handleTag(atts);
		}
	}

	return true;
}

bool OSMRoadsParser::characters(const QString &ch_in) {
	return true;
}

bool OSMRoadsParser::endElement(const QString& namespaceURI, const QString& localName, const QString& qName) {
	if (localName == "way") {
		way.parentNodeName = "osm";

		createRoadEdge();
	}

	return true;
}

void OSMRoadsParser::handleNode(const QXmlAttributes &atts) {
	uint id = atts.value("id").toUInt();
	QVector2D pos = Util::projLatLonToMeter(atts.value("lon").toFloat(), atts.value("lat").toFloat(), centerLonLat) - centerPos;

	// マップの範囲がなら、無視する
	if (!range.contains(pos)) return;

	// 既に読み込み済みの頂点と近すぎないか、チェック
	bool duplicated = false;
	QMap<uint, RoadVertex>::iterator it;
	for (it = vertices.begin(); it != vertices.end(); it++) {
		if ((it.value().getPt() - pos).lengthSquared() < THRESHOLD_CLOSE * THRESHOLD_CLOSE) {
			idToActualId.insert(id, it.key());
			duplicated = true;
			break;
		}
	}

	if (!duplicated) {
		// 頂点ID -> 実際の頂点ID 変換テーブルに追加
		idToActualId.insert(id, id);

		// 頂点リストに追加
		vertices.insert(id, RoadVertex(pos));
	}
}

void OSMRoadsParser::handleWay(const QXmlAttributes &atts) {
	way.way_id = atts.value("id").toUInt();

	way.isStreet = false;
	way.oneWay = false;
	way.bridge = false;
	way.lanes = 1;
	way.type = 1;
	way.nds.clear();
}

void OSMRoadsParser::handleNd(const QXmlAttributes &atts) {
	uint ref = atts.value("ref").toUInt();
	way.nds.push_back(ref);
}

void OSMRoadsParser::handleTag(const QXmlAttributes &atts) {
	QString key = atts.value("k");
	if (key == "highway") {
		QString value = atts.value("v");
		way.isStreet = true;
		if (value=="motorway"||value=="motorway_link"||value=="trunk" || value == "trunk_link") {
			way.type = 3;
		} else if (value=="primary" || value=="primary_link") {
			way.type = 2;
		} else if (value=="secondary"||value=="secondary_link") {
			way.type = 2;
		} else if (value=="tertiary"||value=="tertiary_link") {
			way.type = 1;
		} else if (value=="residential"||value=="living_street"||value=="unclassified") {
			way.type = 1;
		} else {
			way.type = 0;
		}
	} else if (key == "sidewalk") {
	} else if (key == "bridge") {
	} else if (key == "bridge_number") {
	} else if (key == "oneway") {
		QString value = atts.value("v");
		if (value == "yes") {
			way.oneWay = true;
		}
	} else if (key == "lanes") {
		way.lanes = atts.value("v").toUInt();
	} else if (key == "name") {
	} else if (key == "maxspeed") {
	} else if (key == "layer") {
	} else {
	}
}

void OSMRoadsParser::createRoadEdge() {
	if (!way.isStreet || way.type == 0) return;

	// 指定された道路タイプ以外は、棄却する
	if (!((int)powf(2, (way.type - 1)) & roadType)) return;

	for (int k = 0; k < way.nds.size() - 1; k++) {
		uint id = way.nds[k];
		uint next = way.nds[k + 1];

		// 対象となる道路セグメントの両端の頂点がリストに登録済みであること！
		if (!idToActualId.contains(id)) continue;
		if (!idToActualId.contains(next)) continue;

		RoadVertexDesc sourceDesc;
		if (idToDesc.contains(idToActualId[id])) {		// 既にBGLに登録済みなら、BGLから該当頂点のdescを取得
			sourceDesc = idToDesc[idToActualId[id]];
		} else {										// 未登録なら、BGLに該当頂点を追加
			RoadVertex* v = new RoadVertex(vertices[idToActualId[id]].getPt());
			sourceDesc = roads->addVertex(v);
			idToDesc.insert(idToActualId[id], sourceDesc);
		}

		RoadVertexDesc destDesc;
		if (idToDesc.contains(idToActualId[next])) {	// 既にBGLに登録済みなら、BGLから該当頂点のdescを取得
			destDesc = idToDesc[idToActualId[next]];
		} else {										// 未登録なら、BGLに該当頂点を追加
			RoadVertex* v = new RoadVertex(vertices[idToActualId[next]].getPt());
			destDesc = roads->addVertex(v);
			idToDesc.insert(idToActualId[next], destDesc);
		}

		// 道路セグメントの両端が同一頂点なら、無視
		if (sourceDesc == destDesc) continue;

		// 当該２頂点の間に既にエッジがある場合は、無視


		// 道路セグメントの方向が、両端頂点から出ている他の道路の方向と近すぎる場合は、無視
		if (roads->isRedundant(sourceDesc, vertices[idToActualId[next]].getPt() - vertices[idToActualId[id]].getPt(), 0.01f)) {
			continue;
		}
		if (roads->isRedundant(destDesc, vertices[idToActualId[id]].getPt() - vertices[idToActualId[next]].getPt(), 0.01f)) {
			continue;
		}

		// 対象となる道路セグメントの両端の頂点を使った道路セグメントが既にBGLに登録済みかどうか、チェック
		bool duplicated = false;
		RoadEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
			if (boost::source(*ei, roads->graph) == sourceDesc && boost::target(*ei, roads->graph) == destDesc) {
				duplicated = true;
				break;
			}
			if (boost::source(*ei, roads->graph) == destDesc && boost::target(*ei, roads->graph) == sourceDesc) {
				duplicated = true;
				break;
			}
		}

		if (!duplicated) {
			// 道路セグメントをBGLに追加
			RoadEdge* e = new RoadEdge(way.oneWay, way.lanes, way.type);
			e->addPoint(vertices[idToActualId[id]].getPt());
			e->addPoint(vertices[idToActualId[next]].getPt());
			roads->addEdge(sourceDesc, destDesc, e);
		}
	}
}