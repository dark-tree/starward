#pragma once

#include "alien.hpp"

class PlayerEntity;

class MineAlienEntity : public AlienEntity {

	private:

		bool led = false;
		int timer = 0;
		float distance;

	public:

		MineAlienEntity(float x, float y);

		bool checkPlacement(Level& level) override;
		void onKilled(Level& level) override;

		void tickExplode(Level& level);
		void tick(Level& level) override;
		void draw(Level& level, TileSet& tileset, BufferWriter<Vert4f4b>& writer) override;
};
