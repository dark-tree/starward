#pragma once
#include "alien.hpp"

class TeslaAlienEntity : public AlienEntity {

	public:

		enum Side {
			LEFT, RIGHT
		};

		TeslaAlienEntity(double x, double y, int evolution, Side side);

		bool checkPlacement(Level& level) override;

		void tick(Level& level) override;
		void draw(Level& level, TileSet& tileset, BufferWriter<Vert4f4b>& writer) override;

};
