#pragma once

#include "entity.hpp"

class TileEntity : public Entity {

	private:

		double fx;
		double fy;
		uint8_t tile;

	public:

		TileEntity(double x, double y, uint8_t tile, int tx, int ty);

		gls::Sprite sprite(gls::TileSet& tileset) override;
		void tick(Level& level) override;

};