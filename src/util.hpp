#pragma once

// TODO: make this also show something to the user
// compilers really hate me making my own printf-like function
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
template <typename... Args>
[[noreturn]] inline void fault(const char* format, Args... args) {
	printf(format, args...);
	platform_exit(-1);
}
#pragma clang diagnostic pop
#pragma GCC diagnostic pop

inline int randomInt(int min, int max) {
	static std::random_device device;
	static std::mt19937 generator(device());

	std::uniform_int_distribution<std::mt19937::result_type> distribution(min, max);
	return distribution(generator);
}

inline std::string readFile(const std::string& path) {
	if (std::ifstream source_file {path, std::ios::binary}; source_file) {
		return std::string(std::istreambuf_iterator<char>{source_file}, {});
	}

	fault("Unable to read file: '%s'!\n", path.c_str());
	return {};
}

constexpr float deg(float degrees) {
	return degrees * (M_PI / 180);
}

constexpr float slerp(float a, float b, float speed) {
	// normalize angles to range [0, 2 PI)
	a = fmod(a + 2 * M_PI, 2 * M_PI);
	b = fmod(b + 2 * M_PI, 2 * M_PI);

	// find the shortest arc direction
	float delta = b - a;
	if (delta > M_PI) {
		delta -= 2 * M_PI;
	} else if (delta < -M_PI) {
		delta += 2 * M_PI;
	}

	// clamp the movement by speed
	if (fabs(delta) <= speed) {
		return b; // Can reach the target within this step
	}

	// move toward the target by speed
	return a + (delta > 0 ? speed : -speed);
}

inline void randomBuffer(int* buffer, int size) {
	for (int i = 0; i < size; ++i) {
		buffer[i] = i;
	}

	// Perform Fisher-Yates shuffle
	for (int i = size - 1; i > 0; i --) {
		std::swap(buffer[i], buffer[randomInt(0, i)]);
	}
}

inline std::vector<glm::ivec2> trace(const glm::ivec2 start, const glm::ivec2 end) {
	std::vector<glm::ivec2> points;

	int dx = abs(end.x - start.x);
	int dy = abs(end.y - start.y);
	int sx = (start.x < end.x) ? 1 : -1;
	int sy = (start.y < end.y) ? 1 : -1;
	int err = dx - dy;

	glm::ivec2 current = start;
	while (true) {
		points.push_back(current);
		if (current == end) break;

		int e2 = 2 * err;
		if (e2 > -dy) {
			err -= dy;
			current.x += sx;
		}
		if (e2 < dx) {
			err += dx;
			current.y += sy;
		}
	}

	return points;
}
