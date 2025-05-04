#pragma once

#include "entity.hpp"

class PlayerEntity : public Entity {

	private:

		bool first_input = true;
		int lives = 2;
		int invulnerable = 0;
		float cooldown = 0;
		float tilt = 0;
		int ammo = 32;
		Box bumper;

		/// get the left (side == -1) or right (side == +1) bumper box
		Box getBoxBumper(int side) const;

		void onUserInput(Level& level);

	public:

		// bonuses
		int double_barrel_ticks = 0;

	public:

		PlayerEntity();

		bool shouldCollide(Entity* entity) override;

		bool isCausedByPlayer() override;
		void onDamage(Level& level, int damage, Entity* damager) override;
		void tick(Level& level) override;
		void draw(Level& level, TileSet& tileset, BufferWriter<Vert4f4b>& writer) override;
		void debugDraw(Level& level, TileSet& tileset, BufferWriter<Vert4f4b>& writer) override;

};