#pragma once

#include "entity.hpp"

class BlowEntity : public Entity {

	public:

		BlowEntity(double x, double y);

		bool shouldCollide(Entity* entity) override;

		void tick(Level& level) override;
		void draw(Level& level, gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& writer) override;

};