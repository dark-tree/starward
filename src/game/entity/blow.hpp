#pragma once

#include "entity.hpp"

class BlowEntity : public Entity {

	public:

		BlowEntity(double x, double y);

		bool shouldCollide(Entity* entity) override;

		gls::Sprite sprite(gls::TileSet& tileset) override;
		void tick(Level& level) override;

};