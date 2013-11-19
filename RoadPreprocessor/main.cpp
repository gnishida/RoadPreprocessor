#include "MapRange.h"
#include "BBox2D.h"
#include "RoadPreprocessor.h"
#include "RoadGraph.h"
#include <vector>
#include <iostream>
#include <qvector2d.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstring.h>
#include <qstringlist.h>

using namespace std;

/**
 * argv[1]	the OSM file name
 * argv[2]	the file name that contains the ranges of the latitude and longitude
 */
int main(int argc, char** argv) {
	if (argc < 7) {
		cerr << "Usage: RoadPreprocessor <OSM file> <longitude> <latitude> <width> <height> <road type>" << endl;
		cerr << "Road type is specified in the following way:" << endl;
		cerr << "  1 : streets" << endl;
		cerr << "  2 : avenues" << endl;
		cerr << "  4 : highways" << endl;
		cerr << "You can add the numbers to specified the multiple types of roads." << endl;
		cerr << "For instance, 3 means both streets and avenues." << endl;
		return 1;
	}

	QFile file(argv[1]);
	if (!file.open(QIODevice::ReadOnly)) {
		cerr << "File is not accessible: " << argv[1] << endl;
		return 1;
	}

	float longitude = atof(argv[2]);
	float latitude = atof(argv[3]);
	int width = atoi(argv[4]);
	int height = atoi(argv[5]);
	int roadType = atoi(argv[6]);

	RoadPreprocessor rp;
	rp.preprocess(argv[1], QVector2D(longitude, latitude), BBox2D(QVector2D(-width/2, -height/2), QVector2D(width/2, height/2)), roadType);

	return 0;
}