#pragma once

#include <qstring.h>
#include <qvector2d.h>

class RoadNode {
public:
	unsigned int id;
	QString version;
	QString timestamp;
	QString uid;
	QString user;
	QString changeset;

	QVector2D pos;

public:
	RoadNode();
};

