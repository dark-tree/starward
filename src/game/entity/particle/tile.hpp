#pragma once

#include "game/entity/entity.hpp"

class TileEntity : public Entity {

	private:

		const float max_age = 10;

		double fx;
		double fy;
		uint8_t tile;

	public:

		TileEntity(double x, double y, uint8_t tile, int tx, int ty);

		void tick(Level& level) override;
		void draw(Level& level, Renderer& renderer) override;

};