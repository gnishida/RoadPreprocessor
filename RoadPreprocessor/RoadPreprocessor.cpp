#include "RoadPreprocessor.h"
#include "OSMRoadsParser.h"
#include "RoadGraph.h"
#include "RoadVertex.h"
#include "RoadEdge.h"
#include <qfile.h>
#include <qdom.h>
#include <iostream>

RoadPreprocessor::RoadPreprocessor() {
}

void RoadPreprocessor::preprocess(QString filename, const QVector2D &lonlat, const BBox2D &range) {
	RoadGraph roads;

	OSMRoadsParser parser(&roads, lonlat, range);

	// read OSM file
	QXmlSimpleReader reader;
	reader.setContentHandler(&parser);
	QFile file(filename);
	QXmlInputSource source(&file);
	reader.parse(source);

	// degreeが2の頂点は、全てエッジの一部にしちゃう。
	roads.reduce();

	// 立体交差を削除する
	//roads.solveCrossover();

	//roads.check();

	// write OSM file
	QString output = filename.split("/").last().split(".").at(0) + ".gsm";
	FILE* fp = fopen(output.toUtf8().data(), "wb");
	roads.save(fp);
}

