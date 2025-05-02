#include "group.hpp"

#include "system.hpp"

/*
 * SoundGroup
 */

SoundGroup::SoundGroup(const std::string& name)
: name(name) {}

void SoundGroup::emplace(const std::string& path) {
	buffers.emplace_back(std::make_unique<SoundBuffer>(path.c_str()));
}

const SoundBuffer& SoundGroup::pick() const {
	if (buffers.empty()) {
		fault("Can't pick from empty group '%s'!\n", name.c_str());
	}

	return *buffers.at(randomInt(0, buffers.size() - 1)).get();
}