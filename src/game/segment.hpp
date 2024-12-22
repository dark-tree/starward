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

		void generate() {
			memset(tiles, 0, W * H);

			float slope = 16.0f;
			float effect = (slope - std::max(0.0f, slope - index)) / slope;

			int count = 3;

			for (int x = 0; x < W; x ++) {
				for (int y = 0; y < H; y ++) {

					float n1 = 0.5 * (glm::perlin(glm::vec2 {x * 0.05f / 2, (y + index * H) * 0.05f / 2}) / 2 + 0.5);
					float n2 = 0.25 * (glm::perlin(glm::vec2 {x * 0.16f / 2, (y + index * H) * 0.16f / 2}) / 2 + 0.5);
					float n3 = 0.125 * (glm::perlin(glm::vec2 {x * 0.2f, (y + index * H) * 0.2f}) / 2 + 0.5);

					if (n2 < effect) {
						at(x, y) = (((n1 + n2 + n3) < (0.2 * effect) && n1 > 0.1) ? 1 : 0);
					}

					float nc = n1 + n2 + n3;

					float low = 0.0;
					float high = 0.25; // 0/0.25 0/0.33 0.1/0.4 0.25/0.4 0.4/0.5 0.45/0.8

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

			for (int i = 0; i < 100; i ++) {
				int x = randomInt(2, W - 2);
				int y = randomInt(2, H - 2);

				uint8_t& tile = at(x, y);

				if (tile == 1) {
//					if (generateOre(x, y)) {
//						count --;
//					}
				}

				if (count == 0) {
					break;
				}
			}
		}

	public:

		Segment() {
			index = next();
			generate();
		}

		glm::ivec2 getRandomPos() {
			int x = randomInt(0, W);
			int y = randomInt(0, H);

			return {x, y + index * H};
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

		bool tick(double scroll) {
			double height = H * size();

			if ((index + 1) * height + scroll < 0) {
				index = next();
				generate();
				printf("Segment %d loaded\n", index);
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