#pragma once

#include <external.hpp>

struct Box {

	float x;
	float y;
	float w;
	float h;

	glm::vec2 begin() const;
	glm::vec2 end() const;

	Box withOffset(float ox, float oy) const;

	bool intersects(const Box& other) const;

};


