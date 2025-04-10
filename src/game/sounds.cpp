
#include "sounds.hpp"
#include "util.hpp"

/*
 * Sounds
 */


void Sounds::load() {

	static std::unordered_map<std::string, SoundGroup> groups;
	const std::regex regexpr("(\\w+)(-\\d+)?$");

	for (const auto& entry : std::filesystem::directory_iterator("assets/sound")) {
		std::string fullpath = entry.path().generic_string();
		std::string filename = entry.path().stem().generic_string();

		std::smatch match;
		if (std::regex_match(filename, match, regexpr)) {
			auto group = match[1].str();

			groups[group].emplace(fullpath);
			printf("Loading sound '%s' from group '%s'\n", fullpath.c_str(), group.c_str());
		}
	}

	Sounds::beep = std::move(groups.at("beep"));
	Sounds::blow = std::move(groups.at("blow"));
	Sounds::coin = std::move(groups.at("coin"));
	Sounds::damage = std::move(groups.at("damage"));
	Sounds::death = std::move(groups.at("death"));
	Sounds::empty = std::move(groups.at("empty"));
	Sounds::soft = std::move(groups.at("soft"));

}

SoundGroup Sounds::beep;
SoundGroup Sounds::blow;
SoundGroup Sounds::coin;
SoundGroup Sounds::damage;
SoundGroup Sounds::death;
SoundGroup Sounds::empty;
SoundGroup Sounds::soft;
