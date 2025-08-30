#pragma once

#include "external.hpp"

struct VertLevel {

	float x, y;
	float u, v;
	uint8_t r, g, b, a;

	VertLevel(float x, float y, float u, float v, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	: x(x), y(y), u(u), v(v), r(r), g(g), b(b), a(a) {}

};

struct VertBlit {

	float x, y;

	VertBlit(float x, float y)
	: x(x), y(y) {}

};

static_assert (sizeof(VertLevel) == 4 * sizeof(float) + 4 * sizeof(uint8_t), "VertLevel is not of the correct size!");
static_assert (sizeof(VertBlit) == 2 * sizeof(float), "VertBlit is not of the correct size!");
