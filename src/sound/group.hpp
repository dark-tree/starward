#pragma once

#include <external.hpp>
#include "buffer.hpp"

class SoundGroup {

	private:

		std::string name;
		std::vector<SoundBuffer> buffers;

	public:

		SoundGroup() = default;
		SoundGroup(const std::string& name);

		/// Load new sounds into the group
		void emplace(const std::string& path);

		/// Pick any sound, at random, from the group
		const SoundBuffer& pick() const;

};