#pragma once

#include "alien.hpp"

class TurretAlienEntity : public AlienEntity {

	private:

		int barrel = 1;
		float target = -deg(180); // desired rotation
		float head = -deg(180); // current rotation

		void shoot(Level& level, float speed, float radius, float angle);

	public:

		TurretAlienEntity(double x, double y, int evolution);

		bool checkPlacement(Level& level) override;

		void tick(Level& level) override;
		void draw(Level& level, TileSet& tileset, BufferWriter<Vert4f4b>& writer) override;
		void onSpawned(const Level& level, Segment* segment) override;

};