
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

	beep = std::move(groups.at("beep"));
	blow = std::move(groups.at("blow"));
	coin = std::move(groups.at("coin"));
	damage = std::move(groups.at("damage"));
	death = std::move(groups.at("death"));
	empty = std::move(groups.at("empty"));
	soft = std::move(groups.at("soft"));

}
