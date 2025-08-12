
#include "emitter.hpp"

#include "level/level.hpp"

void emitSpriteQuad(BufferWriter<Vert4f4b>& writer, float tx, float ty, float sx, float sy, float angle, const Sprite& s, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	double quarter = M_PI / 2;
	double start = M_PI / 4 + M_PI / 2 + angle;

	auto getUnitPoint = [=] (int index) {
		return glm::vec2 {sin(start + quarter * index) * sx, cos(start + quarter * index) * sy};
	};

	glm::vec2 v0 = getUnitPoint(0);
	glm::vec2 v1 = getUnitPoint(1);
	glm::vec2 v2 = getUnitPoint(2);
	glm::vec2 v3 = getUnitPoint(3);

	writer.push({tx + v0.x, v0.y + ty, s.min_u, s.min_v, r, g, b, a});
	writer.push({tx + v1.x, v1.y + ty, s.max_u, s.min_v, r, g, b, a});
	writer.push({tx + v2.x, v2.y + ty, s.max_u, s.max_v, r, g, b, a});

	writer.push({tx + v2.x, v2.y + ty, s.max_u, s.max_v, r, g, b, a});
	writer.push({tx + v3.x, v3.y + ty, s.min_u, s.max_v, r, g, b, a});
	writer.push({tx + v0.x, v0.y + ty, s.min_u, s.min_v, r, g, b, a});
}

void emitLineQuad(BufferWriter<Vert4f4b>& writer, float x1, float y1, float x2, float y2, float width, const Sprite& s, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {

	float dx = y1 - y2;
	float dy = x2 - x1;
	float dl = sqrt(dx * dx + dy * dy);

	dx /= dl;
	dy /= dl;

	dx *= width;
	dy *= width;

	writer.push({x1 + dx, y1 + dy, s.min_u, s.min_v, r, g, b, a});
	writer.push({x2 - dx, y2 - dy, s.max_u, s.max_v, r, g, b, a});
	writer.push({x2 + dx, y2 + dy, s.min_u, s.max_v, r, g, b, a});

	writer.push({x1 + dx, y1 + dy, s.min_u, s.min_v, r, g, b, a});
	writer.push({x1 - dx, y1 - dy, s.max_u, s.min_v, r, g, b, a});
	writer.push({x2 - dx, y2 - dy, s.max_u, s.max_v, r, g, b, a});
}

void emitTextQuads(BufferWriter<Vert4f4b>& text_writer, float x, float y, float spacing, float size, TileSet& font, uint8_t r, uint8_t g, uint8_t b, uint8_t a, const std::string& str, TextMode mode) {
	int len = str.length();

	if (mode == TextMode::RIGHT) {
		for (int i = 0; i < len; i ++) {
			emitSpriteQuad(text_writer, x - i * spacing, y, -size, size, 0, font.sprite(str[len - i - 1]), r, g, b, a);
		}

		return;
	}

	if (mode == TextMode::LEFT) {
		for (int i = 0; i < len; i ++) {
			emitSpriteQuad(text_writer, x + i * spacing, y, -size, size, 0, font.sprite(str[i]), r, g, b, a);
		}

		return;
	}
}

void emitTileQuad(BufferWriter<Vert4f4b>& writer, Sprite s, int x, int y, float ox, float oy, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	const float unit = SW / Level::segment_width;

	const float tx = x * unit + ox;
	const float ty = y * unit + oy;

	const float ex = tx + unit;
	const float ey = ty + unit;

	writer.push({tx, ty, s.min_u, s.min_v, r, g, b, a});
	writer.push({ex, ty, s.max_u, s.min_v, r, g, b, a});
	writer.push({ex, ey, s.max_u, s.max_v, r, g, b, a});
	writer.push({ex, ey, s.max_u, s.max_v, r, g, b, a});
	writer.push({tx, ey, s.min_u, s.max_v, r, g, b, a});
	writer.push({tx, ty, s.min_u, s.min_v, r, g, b, a});
}