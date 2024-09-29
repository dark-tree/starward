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