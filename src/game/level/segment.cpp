
#include "segment.hpp"

#include "tile.hpp"
#include "render/renderer.hpp"

int global_segment_id;

/*
 * Segment
 */

void Segment::resetTerrainGeneratr() {
	global_segment_id = 0;
}

double Segment::size() {
	return SW / (double) width;
}

int Segment::next() {
	return global_segment_id ++;
}

void Segment::drawTile(RenderLayer& layer, int tile, int x, int y, int width, float scroll, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	const float unit = SW / width;

	const float tx = x * unit;
	const float ty = y * unit + scroll;

	const float ex = tx + unit;
	const float ey = ty + unit;

	Sprite s = getTileSprite(*layer.tileset, tile);

	layer.writer->push({tx, ty, s.min_u, s.min_v, r, g, b, a});
	layer.writer->push({ex, ty, s.max_u, s.min_v, r, g, b, a});
	layer.writer->push({ex, ey, s.max_u, s.max_v, r, g, b, a});
	layer.writer->push({ex, ey, s.max_u, s.max_v, r, g, b, a});
	layer.writer->push({tx, ey, s.min_u, s.max_v, r, g, b, a});
	layer.writer->push({tx, ty, s.min_u, s.min_v, r, g, b, a});
}

void Segment::fill(int tile) {
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			at(x, y) = tile;
		}
	}
}


void Segment::generate(float low, float high) {
	memset(tiles, 0, width * height);
	int sample = index - SEGMENT_START_OFFSET;

	float slope = 16.0f;
	float effect = (slope - std::max(0.0f, slope - sample)) / slope;

	if (sample < 0) {
		fill(0);
		goto skip_terrain;
	}

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			float n1 = 0.5 * (glm::perlin(glm::vec2{x * 0.05f / 2, (y + sample * height) * 0.05f / 2}) / 2 + 0.5);
			float n2 = 0.25 * (glm::perlin(glm::vec2{x * 0.16f / 2, (y + sample * height) * 0.16f / 2}) / 2 + 0.5);
			float n3 = 0.125 * (glm::perlin(glm::vec2{x * 0.2f, (y + sample * height) * 0.2f}) / 2 + 0.5);

			if (n2 < effect) {
				at(x, y) = (((n1 + n2 + n3) < (0.2 * effect) && n1 > 0.1) ? 1 : 0);
			}

			float nc = n1 + n2 + n3;

			if (nc < high && nc > low) {
				float vein = glm::perlin(glm::vec2{x * 0.05, (y + sample * height) * 0.1f});
				float ore = (glm::perlin(glm::vec2{x * 0.1, (y + sample * height) * 0.1f}) / 2 + 0.5);

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

	skip_terrain:
	printf("Segment %d generated, low=%f, high=%f\n", sample, low, high);
}

void Segment::generateString(int x, int y, const std::string& text, int tile, int scale) {

	for (char c : text) {
		for (int ox = 0; ox < 8; ox ++) {
			for (int oy = 0; oy < 8; oy ++) {
				if (sampleFontBitmap(font8x8_basic, c, ox, oy)) {
					for (int i = 0; i < scale; i ++) {
						for (int j = 0; j < scale; j ++) {
							int tx = ox + x + i;
							int ty = oy + y + j;

							if (isLocalTile(tx, ty)) {
								at(tx, ty) = tile;
							}
						}
					}
				}
			}
		}

		x += 8 * scale;
	}

}

Segment::Segment() {
	index = next();
	generate(0, 0.25);
}

glm::ivec2 Segment::getRandomSpawnPos(int margin) {
	int x = randomInt(margin, width - margin * 2);
	int y = randomInt(0, height - 1);

	return {x, y + index * height};
}

bool Segment::contains(int y) const {
	return (y >= index * height) && (y < (index + 1) * height);
}

bool Segment::isLocalTile(int x, int y) {
	return !(x < 0 || x >= width || y < 0 || y >= height);
}

uint8_t& Segment::at(int sx, int sy) {
	return tiles[sx + sy * width];
}

const uint8_t& Segment::at(int sx, int sy) const {
	return tiles[sx + sy * width];
}

int Segment::getStartY() const {
	return index * height;
}

int Segment::getEndY() const {
	return getStartY() + height;
}

uint8_t& Segment::atWorldPos(int sx, int sy) {
	return at(sx, sy - getStartY());
}

const uint8_t& Segment::atWorldPos(int sx, int sy) const {
	return at(sx, sy - getStartY());
}

bool Segment::tick(double scroll, glm::vec2 terrain) {
	double scaled = height * size();

	if ((index + 1) * scaled + scroll < 0) {
		index = next();
		generate(terrain.r, terrain.g);
		return true;
	}

	return false;
}

void Segment::draw(RenderLayer& layer, double scroll, bool debug) {
	float unit = height *  size();

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			uint8_t tile = at(x, y);

			if (tile) {
				drawTile(layer, tile, x, y, width, scroll + index * unit, 255, 255, 255, 255);
			}
		}

		if (debug) {
			drawTile(layer, 1, x, 0, width, scroll + index * unit, 0, 255, 0, 50);
		}
	}
}
