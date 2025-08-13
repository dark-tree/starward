#include "matcher.hpp"
#include "segment.hpp"

/*
 * TerrainMacher
 */

TerrainMacher::TerrainMacher(Segment& segment, int x, int y)
	: segment(segment), x(x), y(y) {
}

bool TerrainMacher::peekIsAir() const {
	return segment.at(x, y) == 0;
}

bool TerrainMacher::acceptTile(bool expectSolid, int ox, int oy) {

	if (x >= Segment::width || y >= Segment::height) {
		return false;
	}

	const bool isAir = peekIsAir();

	if (expectSolid != isAir) {
		x += ox;
		y += oy;

		return true;
	}

	return false;
}

bool TerrainMacher::acceptMany(bool expectSolid, int ox, int oy, int min, int max) {
	int sum = 0;

	while (sum < max && acceptTile(expectSolid, ox, oy)) {
		sum ++;
	}

	return sum >= min;
}

bool TerrainMacher::acceptRight(bool solid, int min_in, int max_ex) {
	return acceptMany(solid, 1, 0, min_in, max_ex);
}

bool TerrainMacher::acceptDown(bool solid, int min_in, int max_ex) {
	return acceptMany(solid, 0, -1, min_in, max_ex);
}

glm::ivec2 TerrainMacher::here() const {
	return {x, y};
}