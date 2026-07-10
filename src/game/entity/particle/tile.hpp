#pragma once

#include "game/entity/entity.hpp"

class TileEntity : public Entity {

	private:

		const float max_age = 10;
		uint8_t tile;

	public:

		TileEntity(uint8_t tile, int tx, int ty);

		bool shouldCollide(Entity* entity) override;
		void tick(Level& level) override;
		void draw(Level& level, Renderer& renderer) override;

};