#pragma once

#include "entity.hpp"

class TurretAlienEntity : public Entity {

	private:

		int evolution; // 0, 1, 2
		float cooldown = 1;

		int barrel = 1;
		float target = -deg(180); // desired rotation
		float head = -deg(180); // current rotation

		void shoot(Level& level, float speed, float radius, float angle);

	public:

		TurretAlienEntity(double x, double y, int evolution);

		bool checkPlacement(Level& level) override;

		void onDamage(Level& level, int damage, Entity* damager) override;
		void tick(Level& level) override;
		void draw(Level& level, gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& writer) override;

};