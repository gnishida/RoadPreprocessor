#include "BBox2D.h"
#include <limits>

BBox2D::BBox2D() {
	corner0.setX(std::numeric_limits<float>::max());
	corner0.setY(std::numeric_limits<float>::max());

	corner1.setX(-std::numeric_limits<float>::max());
	corner1.setY(-std::numeric_limits<float>::max());
}

BBox2D::BBox2D(const QVector2D &corner0, const QVector2D &corner1) {
	this->corner0 = corner0;
	this->corner1 = corner1;
}

void BBox2D::addPoint(const QVector2D &p) {
	if (p.x() < corner0.x()) {
		corner0.setX(p.x());
	}
	if (p.y() < corner0.y()) {
		corner0.setY(p.y());
	}

	if (p.x() > corner1.x()) {
		corner1.setX(p.x());
	}
	if (p.y() > corner1.y()) {
		corner1.setY(p.y());
	}
}

bool BBox2D::contains(const QVector2D &p) {
	if (p.x() < corner0.x()) return false;
	if (p.y() < corner0.y()) return false;

	if (p.x() > corner1.x()) return false;
	if (p.y() > corner1.y()) return false;

	return true;
}

QVector2D& BBox2D::getCorner0() {
	return corner0;
}

QVector2D& BBox2D::getCorner1() {
	return corner1;
}