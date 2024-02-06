#pragma once

// TODO: make this also show something to the user
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
template <typename... Args>
[[noreturn]] void fault(const char* format, Args... args) {
	printf(format, args...);
	emscripten_force_exit(-1);
}
#pragma clang diagnostic pop
