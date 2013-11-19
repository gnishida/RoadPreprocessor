#pragma once

#include <qvector2d.h>

class BBox2D {
private:
	QVector2D corner0;
	QVector2D corner1;

public:
	BBox2D();
	BBox2D(const QVector2D &corner0, const QVector2D &corner1);

	void addPoint(const QVector2D &p);
	bool contains(const QVector2D &p);

	QVector2D& getCorner0();
	QVector2D& getCorner1();
};

