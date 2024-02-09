#pragma once

#include <external.hpp>
#include <util.hpp>

template <typename T>
class Field {

	private:

		T* tiles = nullptr;

	public:

		const uint32_t width, height;

		Field(uint32_t width, uint32_t height)
		: width(width), height(height) {
			tiles = (T*) calloc(width * height, sizeof(T));
		}

		~Field() {
			free(tiles);
		}

		Field(const Field& other)
		: Field(other.width, other.height) {
			memcpy(tiles, other.tiles, width * height * sizeof(T));
		}

	public:

		T& at(int x, int y) {
			if (x < 0 || y < 0 || x > width || y > height) {
				fault("Reqested element (%d, %d) is out of bounds (%u, %u)\n", x, y, width, height);
			}

			return tiles[y * width + x];
		}

};
