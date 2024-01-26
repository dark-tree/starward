#pragma once

#include <external.hpp>

class World {

	private:

		uint16_t* tiles;

	public:

		const uint32_t width, height;

		World(uint32_t width, uint32_t height)
		: width(width), height(height) {
			tiles = (uint16_t*) calloc(width * height, sizeof(uint16_t));
		}

		~World() {
			free(tiles);
		}

	public:

		void set(int x, int y, uint16_t tid) {
			get(x, y) = tid;
		}

		uint16_t& get(int x, int y) {
			if (x < 0 || y < 0 || x > width || y > height) {
				fault("Reqested position (%d, %d) is out of bounds, world is of size (%u, %u)\n", x, y, width, height);
			}

			return tiles[y * width + x];
		}

};
