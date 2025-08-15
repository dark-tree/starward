#pragma once
#include "alien.hpp"

class TeslaAlienEntity : public AlienEntity {

	private:

		static bool spawnAt(Level& level, int lx, int rx, int y, int evolution);
		void generateFoundation(Segment& segment, glm::ivec2 pos, int x, bool flip);

	public:

		enum Side {
			LEFT = 0,
			RIGHT = 1
		};

		const Side side;

		TeslaAlienEntity(double x, double y, int evolution, Side side);

		bool checkPlacement(Level& level) override;

		void tick(Level& level) override;
		void draw(Level& level, Renderer& renderer) override;
		void onSpawned(const Level& level, Segment* segment) override;

		static bool spawn(Level& level, Segment& segment, int evolution);

};
