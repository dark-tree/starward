#pragma once

#include "entity.hpp"

class PlayerEntity;

class PowerUpEntity : public Entity {

	public:

		enum Type {
			LIVE = 0,
			DOUBLE_BARREL = 1,
			SHIELD = 2,
			NITRO = 3,
			STAN = 4,
			PIERCE = 5,
		};

		static Type randomPick();

	private:

		Type type;

	public:

		PowerUpEntity(double x, double y, Type type);

		bool checkPlacement(Level& level) override;

		void applyEffect(Level& level, PlayerEntity* player);
		void onDamage(Level& level, int damage, Entity* damager) override;
		void tick(Level& level) override;
		void draw(Level& level, Renderer& renderer) override;

};
