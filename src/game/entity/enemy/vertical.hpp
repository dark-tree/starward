#pragma once

#include "alien.hpp"
#include "sweeper.hpp"

class VerticalAlienEntity : public SweeperAlienEntity {

	private:

		bool active;
		Box trigger;

		Box getBoxTrigger() const;

	public:

		VerticalAlienEntity(double x, double y, int evolution);

		void tick(Level& level) override;
		void draw(Level& level, Renderer& renderer) override;
		void debugDraw(Level& level, Renderer& renderer) override;

		void tickMovement() override;
		void tickShooting(Level& level) override;

};

