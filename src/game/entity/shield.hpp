#pragma once

#include "entity.hpp"

class PlayerEntity;

class ShieldEntity : public Entity {

	private:

		bool damaged = false;
		int power = 0;
		std::shared_ptr<PlayerEntity> player;

		void updatePosition();

	public:

		ShieldEntity(const std::shared_ptr<PlayerEntity>& player);

		bool shouldCollide(Entity* entity) override;
		void onDamage(Level& level, int damage, Entity* damager) override;
		void tick(Level& level) override;

		void draw(Level& level, TileSet& tileset, BufferWriter<Vert4f4b>& writer) override;

		void repower();

};
