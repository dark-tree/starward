#pragma once

#include "external.hpp"
#include "matcher.hpp"
#include "rendering.hpp"
#include "tile.hpp"

extern int global_segment_id;

struct TeslaPlacement {
	int lx, rx;
	int y;
};

class Segment {

	public:

		static constexpr int width = 128;
		static constexpr int height = 32;

		int index = 0;
		uint8_t tiles[width * height];

		double size() {
			return SW / (double) width;
		}

		int next() {
			return global_segment_id ++;
		}

		void drawTile(BufferWriter<Vert4f4b>& writer, Sprite s, int x, int y, int width, float scroll, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
			const float unit = SW / width;

			const float tx = x * unit;
			const float ty = y * unit + scroll;

			const float ex = tx + unit;
			const float ey = ty + unit;

			writer.push({tx, ty, s.min_u, s.min_v, r, g, b, a});
			writer.push({ex, ty, s.max_u, s.min_v, r, g, b, a});
			writer.push({ex, ey, s.max_u, s.max_v, r, g, b, a});
			writer.push({ex, ey, s.max_u, s.max_v, r, g, b, a});
			writer.push({tx, ey, s.min_u, s.max_v, r, g, b, a});
			writer.push({tx, ty, s.min_u, s.min_v, r, g, b, a});
		}

		void generate(float low, float high) {
			memset(tiles, 0, width * height);

			float slope = 16.0f;
			float effect = (slope - std::max(0.0f, slope - index)) / slope;

			for (int x = 0; x < width; x ++) {
				for (int y = 0; y < height; y ++) {

					float n1 = 0.5 * (glm::perlin(glm::vec2 {x * 0.05f / 2, (y + index * height) * 0.05f / 2}) / 2 + 0.5);
					float n2 = 0.25 * (glm::perlin(glm::vec2 {x * 0.16f / 2, (y + index * height) * 0.16f / 2}) / 2 + 0.5);
					float n3 = 0.125 * (glm::perlin(glm::vec2 {x * 0.2f, (y + index * height) * 0.2f}) / 2 + 0.5);

					if (n2 < effect) {
						at(x, y) = (((n1 + n2 + n3) < (0.2 * effect) && n1 > 0.1) ? 1 : 0);
					}

					float nc = n1 + n2 + n3;

					if (nc < high && nc > low) {
						float vein = glm::perlin(glm::vec2 {x * 0.05, (y + index * height) * 0.1f});
						float ore = (glm::perlin(glm::vec2 {x * 0.1, (y + index * height) * 0.1f}) / 2 + 0.5);

						if (vein < 0.2f && vein > 0.0f) {
							at(x, y) = 2;
							continue;
						}

						if (ore < 0.2f) {
							at(x, y) = 3;
							continue;
						}

						at(x, y) = 1;
					}


				}
			}

			printf("Segment %d generated, low=%f, high=%f\n", index, low, high);
		}

	public:

		Segment() {
			index = next();
			generate(0, 0.25);
		}

		glm::ivec2 getRandomPos(int margin) {
			int x = randomInt(margin, width - margin);
			int y = randomInt(0, height - 1);

			return {x, y + index * height};
		}

		glm::ivec2 getRandomTurretPos(int margin) {

			int column = 0;
			int buffer[width];
			randomBuffer(buffer, width);

			while (column < width) {
				int match = 0;
				bool table[7] = {true, true, true, true, false, false, false};

				int x = buffer[column ++];
				if (x < margin) continue;
				if (x > width - margin) break;

				for (int y = 0; y < height; y++) {
					bool air = (at(x, y) == 0);

					if (air != table[match++]) {
						match = 0;
					}

					if (match == 7) {
						return {x, y + index * height - 4};
					}
				}
			}

			printf("Failed to find turret placement spot!\n");
			return {0, 0};
		}

		void generateTeslaTowerFoundation(glm::ivec2 pos, int x, bool flip) {

			const int count = 7;
			int heights[count]  = {2, 3, 3, 2, 2, 3, 3};
			int material[count] = {2, 2, 2, 0, 0, 0, 0};

			for (int i = 0; i < count; i ++) {

				int j = flip ? count - i - 1 : i;
				int h = heights[j];
				int m = material[j];

				for (int y = -h; y <= h; y ++) {
					int tx = pos.x + x;
					int ty = pos.y + y;

					if (tx < 0 || tx >= width || ty < 0 || ty >= height) {
						continue;
					}

					at(tx, ty) = m;
				}

				x ++;
			}
		}

		TeslaPlacement getRandomTeslaPos(int margin) {

			int rows[height];
			randomBuffer(rows, height);

			for (int row = 0; row < height; row ++) {

				int cols[width];
				randomBuffer(cols, width);

				for (int col = 0; col < width; col ++) {

					TerrainMacher matcher {*this, cols[col], rows[row]};

					if (!matcher.acceptRight(true, 5, 20)) {
						continue;
					}

					glm::ivec2 left = matcher.here();

					if (!matcher.acceptRight(false, 15, 100)) {
						continue;
					}

					glm::ivec2 right = matcher.here();

					if (!matcher.acceptRight(true, 5, 20)) {
						continue;
					}

					generateTeslaTowerFoundation(left, -4, false);
					generateTeslaTowerFoundation(right, -2, true);

					return {left.x, right.x, left.y + this->index * height};
				}

			}

			printf("Failed to find left tesla tower placement spot!\n");
			return {0, 0, 0};
		}

		bool contains(int y) const {
			return (y >= index * height) && (y < (index + 1) * height);
		}

		uint8_t& at(int sx, int sy) {
			return tiles[sx + sy * width];
		}

		const uint8_t& at(int sx, int sy) const {
			return tiles[sx + sy * width];
		}

		uint8_t& atWorldPos(int sx, int sy) {
			return at(sx, sy - index * height);
		}

		const uint8_t& atWorldPos(int sx, int sy) const {
			return at(sx, sy - index * height);
		}

		bool tick(double scroll, glm::vec2 terrain) {
			double scaled = height * size();

			if ((index + 1) * scaled + scroll < 0) {
				index = next();
				generate(terrain.r, terrain.g);
				return true;
			}

			return false;
		}

		void draw(double scroll, TileSet& tileset, BufferWriter<Vert4f4b>& writer, bool debug) {
			float unit = size();

			for (int x = 0; x < width; x++) {
				for (int y = 0; y < height; y++) {
					uint8_t tile = at(x, y);

					if (tile) {
						drawTile(writer, getTileSprite(tileset, tile), x, y, width, scroll + index * height * unit, 255, 255, 255, 255);
					}
				}

				if (debug) {
					drawTile(writer, tileset.sprite(0, 0), x, 0, width, scroll + index * height * unit, 0, 255, 0, 50);
				}
			}
		}

};