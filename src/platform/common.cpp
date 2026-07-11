
/*
 * Shared components of the platform abstraction interface
 */

#include "platform.hpp"

int platform::get_startup_param() {
	std::string data = get_startup_string();

	if (data.empty()) {
		return 0;
	}

	try {
		return std::stoi(data);
	} catch (...) {
		return 0;
	}
}