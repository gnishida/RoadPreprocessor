#pragma once

#include "BBox2D.h"
#include <qvector2d.h>
#include <vector>

class MapRange {
private:
	std::vector<BBox2D> ranges;

public:
	MapRange();

	void addBox(const BBox2D &box);
	bool contains(const QVector2D &p);
	int size() const;
	BBox2D& get(int index);
};

