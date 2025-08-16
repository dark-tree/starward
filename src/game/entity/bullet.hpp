#pragma once

#include "entity.hpp"

struct BulletConfig {
	bool charged = false;
	bool piercing = false;
};

class BulletEntity : public Entity {

	private:

		int cooldown = 0;
		BulletConfig config;
		Color color;

		int time;
		std::shared_ptr<Entity> parent;
		float velocity;

		bool isTileProtected(Level& level, glm::ivec2 pos, int tx, int ty);

	public:

		BulletEntity(float velocity, double x, double y, const std::shared_ptr<Entity>& except, float angle, BulletConfig config = {.charged = false, .piercing = false});

		bool isCharged() const;

		bool isCausedByPlayer() override;
		std::shared_ptr<Entity> getParent() override;
		void tick(Level& level) override;
		void draw(Level& level, Renderer& renderer) override;

};