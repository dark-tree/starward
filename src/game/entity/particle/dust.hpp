#pragma once

#include "game/entity/entity.hpp"

class DustEntity : public Entity {

	private:

		const int lifetime;
		Color color;

		float fx;
		float fy;
		float rotation;
		float scalar;

	public:

		DustEntity(double x, double y, float fx, float fy, float angle, float rotation, float jitter, int lifetime, Color color);

		bool shouldCollide(Entity* entity) override;
		void tick(Level& level) override;
		void draw(Level& level, TileSet& tileset, BufferWriter<Vert4f4b>& writer) override;

};