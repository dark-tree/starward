#pragma once

#include <external.hpp>
#include <rendering.hpp>
#include "tile.hpp"

template <size_t W, size_t H>
class Segment {

	public:

		int index = 0;
		uint8_t tiles[W * H];

		double size() {
			return SW / (double) W;
		}

		int next() {
			static int global = 0;
			return global ++;
		}

		void drawTile(gls::BufferWriter<gls::Vert4f4b>& writer, gls::Sprite s, int x, int y, int width, float scroll) {
			const float unit = SW / width;

			const float tx = x * unit;
			const float ty = y * unit + scroll;

			const float ex = tx + unit;
			const float ey = ty + unit;

			writer.push({tx, ty, s.min_u, s.min_v, 255, 255, 255, 255});
			writer.push({ex, ty, s.max_u, s.min_v, 255, 255, 255, 255});
			writer.push({ex, ey, s.max_u, s.max_v, 255, 255, 255, 255});
			writer.push({ex, ey, s.max_u, s.max_v, 255, 255, 255, 255});
			writer.push({tx, ey, s.min_u, s.max_v, 255, 255, 255, 255});
			writer.push({tx, ty, s.min_u, s.min_v, 255, 255, 255, 255});
		}

		void generate(float low, float high) {
			memset(tiles, 0, W * H);

			float slope = 16.0f;
			float effect = (slope - std::max(0.0f, slope - index)) / slope;

			for (int x = 0; x < W; x ++) {
				for (int y = 0; y < H; y ++) {

					float n1 = 0.5 * (glm::perlin(glm::vec2 {x * 0.05f / 2, (y + index * H) * 0.05f / 2}) / 2 + 0.5);
					float n2 = 0.25 * (glm::perlin(glm::vec2 {x * 0.16f / 2, (y + index * H) * 0.16f / 2}) / 2 + 0.5);
					float n3 = 0.125 * (glm::perlin(glm::vec2 {x * 0.2f, (y + index * H) * 0.2f}) / 2 + 0.5);

					if (n2 < effect) {
						at(x, y) = (((n1 + n2 + n3) < (0.2 * effect) && n1 > 0.1) ? 1 : 0);
					}

					float nc = n1 + n2 + n3;

					if (nc < high && nc > low) {
						float vein = glm::perlin(glm::vec2 {x * 0.05, (y + index * H) * 0.1f});
						float ore = (glm::perlin(glm::vec2 {x * 0.1, (y + index * H) * 0.1f}) / 2 + 0.5);

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
			int x = randomInt(margin, W - margin);
			int y = randomInt(0, H - 1);

			return {x, y + index * H};
		}

		glm::ivec2 getRandomTurretPos(int margin) {

			int column = 0;
			int buffer[W];
			randomBuffer(buffer, W);

			while (column < W) {
				int match = 0;
				bool table[7] = {true, true, true, true, false, false, false};

				int x = buffer[column ++];
				if (x < margin) continue;
				if (x > W - margin) break;

				for (int y = 0; y < H; y++) {
					bool air = (at(x, y) == 0);

					if (air != table[match++]) {
						match = 0;
					}

					if (match == 7) {
						return {x, y + index * H - 4};
					}
				}
			}

			printf("Failed to find turret placement spot!\n");
			return {0, 0};
		}

		bool contains(int y) {
			return (y >= index * H) && (y < (index + 1) * H);
		}

		uint8_t& at(int sx, int sy) {
			return tiles[sx + sy * W];
		}

		uint8_t& atWorldPos(int sx, int sy) {
			return at(sx, sy - index * H);
		}

		bool tick(double scroll, glm::vec2 terrain) {
			double height = H * size();

			if ((index + 1) * height + scroll < 0) {
				index = next();
				generate(terrain.r, terrain.g);
				return true;
			}

			return false;
		}

		void draw(double scroll, gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& writer) {
			float width = size();

			for (int x = 0; x < W; x++) {
				for (int y = 0; y < H; y++) {
					uint8_t tile = at(x, y);

					if (tile) {
						drawTile(writer, getTileSprite(tileset, tile), x, y, W, scroll + index * H * width);
					}
				}
			}
		}

};