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

	public:

		SweeperAlienEntity(double x, double y, int evolution);

		bool checkPlacement(Level& level) override;

		void onDamage(Level& level, int damage, Entity* damager) override;
		gls::Sprite sprite(gls::TileSet& tileset) override;
		void tick(Level& level) override;

};