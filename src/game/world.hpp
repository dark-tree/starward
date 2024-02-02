#pragma once

#include <external.hpp>
#include <util/field.hpp>

class World {

	private:

		Field<uint16_t> tiles;

	public:

		const uint32_t width, height;

		World(uint32_t width, uint32_t height)
		: tiles(width, height), width(width), height(height) {}

	public:

		void set(int x, int y, uint16_t tid) {
			get(x, y) = tid;
		}

		uint16_t& get(int x, int y) {
			return tiles.at(x, y);
		}

};
