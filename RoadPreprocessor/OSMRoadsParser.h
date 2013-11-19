#pragma once

#include "MapRange.h"
#include "RoadGraph.h"
#include <QtXml/qxml.h>
#include <qstring.h>
#include <qvector3d.h>

class RoadNode;
class RoadEdge;

typedef struct {
	QString parentNodeName;
	uint way_id;
	bool isStreet;
	bool oneWay;
	bool bridge;
	uint lanes;
	uint type;
	std::vector<uint> nds;
} Way;

class OSMRoadsParser : public QXmlDefaultHandler {
private:
	// temporary data for parsing ways
	Way way;

	//QVector3D centerLatLon;
	QVector2D centerLonLat;
	QVector2D centerPos;
	BBox2D range;
	int roadType;

	RoadGraph* roads;

	/** temporary node list */
	QMap<uint, RoadVertexDesc> idToDesc;
	QMap<uint, uint> idToActualId;
	QMap<uint, RoadVertex> vertices;

public:
	/** node list to be output to XML file */
	QMap<uint, RoadNode*> nodes;

	/** edge list to be output to XML file */
	std::vector<RoadEdge*> edges;

public:
	OSMRoadsParser(RoadGraph *roads, const QVector2D &lonlat, const BBox2D &range, int roadType);

	bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts);
	bool characters(const QString &ch_in);
	bool endElement(const QString&, const QString& localName, const QString& qName);

private:
	void handleNode(const QXmlAttributes &atts);
	void handleWay(const QXmlAttributes &atts);
	void handleNd(const QXmlAttributes &atts);
	void handleTag(const QXmlAttributes &atts);
	void createRoadEdge();
};

