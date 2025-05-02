#pragma once

#include "alien.hpp"

class BulletEntity;

class FighterAlienEntity : public AlienEntity {

	private:

		float vx = 0, vy = 0;
		float px = 0, py = 0;

		int count = 0;
		int avoiding = 0;

		bool escape = false;
		bool underhung = false;
		bool down = false;

		void forEachDanger(Level& level, const std::function<void(BulletEntity*, float, float)>& callback);
		bool tickMovement(Level& level);

	public:

		FighterAlienEntity(double x, double y, int evolution);

		bool checkPlacement(Level& level) override;

		void onKilled(Level& level) override;
		void tick(Level& level) override;
		void draw(Level& level, TileSet& tileset, BufferWriter<Vert4f4b>& writer) override;
		void debugDraw(Level& level, TileSet& tileset, BufferWriter<Vert4f4b>& writer) override;

};