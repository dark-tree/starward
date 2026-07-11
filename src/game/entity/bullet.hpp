#pragma once

#include "entity.hpp"

struct BulletConfig {
	bool charged = false;
	bool piercing = false;
	bool boring = false;
	bool targeting = false;
};

class BulletEntity : public Entity {

	private:

		int cooldown = 0;
		BulletConfig config;
		Color color;

		int bored_tiles = 0;
		int time;
		std::shared_ptr<Entity> parent;
		std::shared_ptr<Entity> target;
		float velocity;

		bool isTileProtected(Level& level, glm::ivec2 pos, int tx, int ty);

		int eraseTilesAround(Level& level, int radius);
		bool lockOntoTarget(Level& level);

	public:

		BulletEntity(float velocity, double x, double y, const std::shared_ptr<Entity>& except, float angle, BulletConfig config = {.charged = false, .piercing = false});

		bool isCharged() const;
		BulletConfig getConfig() const;

		bool isCausedByPlayer() override;
		std::shared_ptr<Entity> getParent() override;
		void tick(Level& level) override;
		void draw(Level& level, Renderer& renderer) override;
		bool shouldAutoTarget() override;

};