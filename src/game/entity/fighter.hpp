#pragma once

#include "entity.hpp"

class BulletEntity;

class FighterAlienEntity : public Entity {

	private:

		float vx = 0, vy = 0;
		float px = 0, py = 0;

		int count = 0;
		int avoiding = 0;
		int evolution; // 0, 1, 2
		float cooldown = 1;

		bool escape = false;
		bool underhung = false;
		bool down = false;

		void forEachDanger(Level& level, const std::function<void(BulletEntity*, float, float)>& callback);
		bool tickMovement(Level& level);

	public:

		FighterAlienEntity(double x, double y, int evolution);

		bool checkPlacement(Level& level) override;

		void onDamage(Level& level, int damage, Entity* damager) override;
		gls::Sprite sprite(gls::TileSet& tileset) override;
		void tick(Level& level) override;
		void draw(Level& level, gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& writer) override;

};