#pragma once

#include <external.hpp>

struct Color {

	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;

	Color withAlpha(uint8_t a) const {
		return {r, g, b, a};
	}

	static constexpr Color of(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
		return {r, g, b, a};
	}

	static constexpr Color white() {
		return of(255, 255, 255);
	}

	static constexpr Color red(bool bright = false) {
		const uint8_t other = bright ? 200 : 50;
		return of(255, other, other);
	}

	static constexpr Color blue(bool bright = false) {
		const uint8_t other = bright ? 200 : 50;
		return of(other, other, 255);
	}

};