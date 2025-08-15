#pragma once

#include "alien.hpp"

class SweeperAlienEntity : public AlienEntity {

	protected:

		float bump = 0;
		int count = 0;
		float facing = 1;
		int buried = 0;

	public:

		SweeperAlienEntity(double x, double y, int evolution);

		bool checkPlacement(Level& level) override;

		void onDamaged(Level& level) override;
		void onDamage(Level& level, int damage, Entity* damager) override;

		void tick(Level& level) override;
		void draw(Level& level, Renderer& renderer) override;

		virtual void tickShooting(Level& level);
		virtual void tickMovement();
};