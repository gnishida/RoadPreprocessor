﻿#include <vector>
#include <iostream>
#include "common/Util.h"
#include "road/GraphUtil.h"
#include "road/RoadVertex.h"
#include "road/RoadEdge.h"
#include "common/BBox.h"
#include "OSMRoadsParser.h"

OSMRoadsParser::OSMRoadsParser(RoadGraph *roads, const QVector2D &lonlat, const BBox &range) {
	this->roads = roads;
	this->centerLonLat = lonlat;
	this->centerPos = Util::projLatLonToMeter(lonlat, lonlat);
	this->range = range;

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
	unsigned long long id = atts.value("id").toULongLong();
	QVector2D pos = Util::projLatLonToMeter(atts.value("lon").toDouble(), atts.value("lat").toDouble(), centerLonLat) - centerPos;

	// ignore the node outside the area
	if (!range.contains(pos)) return;

	idToActualId.insert(id, id);

	// add a vertex
	vertices.insert(id, RoadVertex(pos));
}

void OSMRoadsParser::handleWay(const QXmlAttributes &atts) {
	way.way_id = atts.value("id").toULongLong();

	way.isStreet = false;
	way.oneWay = false;
	way.link = false;
	way.roundabout = false;
	way.bridge = false;
	way.lanes = 1;
	way.type = RoadEdge::TYPE_STREET;
	way.nds.clear();
}

void OSMRoadsParser::handleNd(const QXmlAttributes &atts) {
	QString hoge = atts.value("ref");
	unsigned long long ref = atts.value("ref").toULongLong();
	way.nds.push_back(ref);
}

void OSMRoadsParser::handleTag(const QXmlAttributes &atts) {
	QString key = atts.value("k");
	if (key == "highway") {
		QString value = atts.value("v");
		way.isStreet = true;
		if (value=="motorway" || value=="motorway_link" || value=="trunk") {
			way.type = RoadEdge::TYPE_HIGHWAY;
		} else if (value == "trunk_link") {
			way.type = RoadEdge::TYPE_HIGHWAY;
			way.link = true;
		} else if (value=="primary") {
			way.type = RoadEdge::TYPE_BOULEVARD;
		} else if (value=="primary_link") {
			way.type = RoadEdge::TYPE_BOULEVARD;
			way.link = true;
		} else if (value=="secondary") {
			way.type = RoadEdge::TYPE_AVENUE;
		} else if (value=="secondary_link") {
			way.type = RoadEdge::TYPE_AVENUE;
			way.link = true;
		} else if (value=="tertiary") {
			way.type = RoadEdge::TYPE_AVENUE;
		} else if (value=="tertiary_link") {
			way.type = RoadEdge::TYPE_AVENUE;
			way.link = true;
		} else if (value=="residential" || value=="living_street" || value=="unclassified") {
			way.type = RoadEdge::TYPE_STREET;
		} else {
			way.type = RoadEdge::TYPE_OTHERS;
		}
	} else if (key == "sidewalk") {
	} else if (key == "junction") {
		QString value = atts.value("v");
		if (value == "roundabout") {
			way.roundabout = true;
		}
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

	for (int k = 0; k < way.nds.size() - 1; k++) {
		unsigned long long id = way.nds[k];
		unsigned long long next = way.nds[k + 1];

		// check if both end points are already registered
		if (!idToActualId.contains(id)) continue;
		if (!idToActualId.contains(next)) continue;

		RoadVertexDesc sourceDesc;
		if (idToDesc.contains(id)) {		// obtain the vertex desc
			sourceDesc = idToDesc[id];
		} else {							// add a vertex
			RoadVertexPtr v = RoadVertexPtr(new RoadVertex(vertices[id].getPt()));
			sourceDesc = GraphUtil::addVertex(*roads, v);
			idToDesc.insert(id, sourceDesc);
		}

		RoadVertexDesc destDesc;
		if (idToDesc.contains(next)) {		// obtain the vertex desc
			destDesc = idToDesc[next];
		} else {							// add a vertex
			RoadVertexPtr v = RoadVertexPtr(new RoadVertex(vertices[next].getPt()));
			destDesc = GraphUtil::addVertex(*roads, v);
			idToDesc.insert(next, destDesc);
		}

		// add a road segment
		GraphUtil::addEdge(*roads, sourceDesc, destDesc, way.type, way.lanes, way.oneWay, way.link, way.roundabout);
	}
}