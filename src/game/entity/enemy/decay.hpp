#pragma once
#include "alien.hpp"

class DecayEntity;

class DecaySharedState : public std::enable_shared_from_this<DecaySharedState> {

		std::unordered_map<glm::ivec2, std::weak_ptr<DecayEntity>> parts;

	public:

		~DecaySharedState();

		std::shared_ptr<DecayEntity> createPart(float x, float y);
		std::shared_ptr<DecayEntity> getPart(float x, float y);

};

class DecayEntity : public AlienEntity {

	int base;
	int row_1, row_2;
	int timer = 0;
	std::shared_ptr<DecaySharedState> shared;

	Color cb = Color::red();

	public:

		DecayEntity(float x, float y, const std::shared_ptr<DecaySharedState>& shared);

		bool checkPlacement(Level& level) override;
		// void onDamage(Level& level, int damage, Entity* damager) override;

		void tick(Level& level) override;
		void draw(Level& level, TileSet& tileset, BufferWriter<Vert4f4b>& writer) override;

};
