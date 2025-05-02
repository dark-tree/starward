#pragma once

#include "entity.hpp"

class BulletEntity : public Entity {

	private:

		std::shared_ptr<Entity> parent;
		float velocity;

		bool isTileProtected(Level& level, glm::ivec2 pos, int tx, int ty);

	public:

		BulletEntity(float velocity, double x, double y, const std::shared_ptr<Entity>& except, float angle = deg(180));

		bool isCausedByPlayer() override;
		std::shared_ptr<Entity> getParent() override;
		void tick(Level& level) override;
		void draw(Level& level, TileSet& tileset, BufferWriter<Vert4f4b>& writer) override;

};