#include "box.hpp"

glm::vec2 Box::begin() const {
	return {x, y};
}

glm::vec2 Box::end() const {
	return {x + w, y + h};
}

Box Box::withOffset(float ox, float oy) const {
	return {x + ox, y + oy, w, h};
}

bool Box::intersects(const Box& other) const {

	const auto tb = begin();
	const auto te = end();

	const auto ob = other.begin();
	const auto oe = other.end();

	if (te.x < ob.x || te.y < ob.y) return false;
	if (tb.x > oe.x || tb.y > oe.y) return false;

	return true;

}
