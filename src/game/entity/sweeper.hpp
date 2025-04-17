#pragma once

#include "entity.hpp"

class SweeperAlienEntity : public Entity {

	private:

		int evolution; // 0, 1, 2
		float bump = 0;
		int health = 2;
		int count = 0;
		float facing = 1;
		float cooldown = 1;
		int buried = 0;
		bool attacked = false;
		int flash = 0;

	public:

		SweeperAlienEntity(double x, double y, int evolution);

		bool checkPlacement(Level& level) override;

		void onDamage(Level& level, int damage, Entity* damager) override;
		void tick(Level& level) override;
		void draw(Level& level, gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& writer) override;
};