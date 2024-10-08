
#include "emitter.hpp"

void emitSpriteQuad(gls::BufferWriter<gls::Vert4f4b>& writer, float tx, float ty, float sx, float sy, float angle, const gls::Sprite& s, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
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
