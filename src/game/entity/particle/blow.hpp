#pragma once

#include "game/entity/entity.hpp"

class BlowEntity : public Entity {

	public:

		BlowEntity(double x, double y);

		bool shouldCollide(Entity* entity) override;

		void tick(Level& level) override;
		void draw(Level& level, Renderer& renderer) override;

};