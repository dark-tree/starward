#pragma once

#include "external.hpp"

class Segment;

class TerrainMacher {

	private:

		Segment& segment;
		int x, y;

	public:

		TerrainMacher(Segment& segment, int x, int y);

		/// Check if the next tile is air
		bool peekIsAir() const;

		/// Accepts a single tile
		bool acceptTile(bool expectSolid, int ox, int oy);

		/// Accepts a range of tiles in a line, both min and max are inclusive
		bool acceptMany(bool expectSolid, int ox, int oy, int min, int max);

		/// Accept one (or many) tiles to the right
		bool acceptRight(bool solid, int min_in = 1, int max_ex = 1);

		/// Accept one (or many) tiles down
		bool acceptDown(bool solid, int min_in = 1, int max_ex = 1);

		/// Get current tile position
		glm::ivec2 here() const;

};
