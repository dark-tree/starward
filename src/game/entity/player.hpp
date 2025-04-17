#pragma once

#include "entity.hpp"

class PlayerEntity : public Entity {

	private:

		int lives = 2;
		int invulnerable = 0;
		float cooldown = 0;
		float tilt = 0;
		int ammo = 32;

	public:

		// bonuses
		int double_barrel_ticks = 0;

	public:

		PlayerEntity();

		bool shouldCollide(Entity* entity) override;

		bool isCausedByPlayer() override;
		void onDamage(Level& level, int damage, Entity* damager) override;
		void tick(Level& level) override;
		void draw(Level& level, gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& writer) override;

};