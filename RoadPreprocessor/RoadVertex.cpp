#include "RoadVertex.h"

RoadVertex::RoadVertex() {
	this->pt = QVector2D(0.0f, 0.0f);
}

RoadVertex::RoadVertex(const QVector2D &pt) {
	this->pt = pt;
}

const QVector2D& RoadVertex::getPt() const {
	return pt;
}