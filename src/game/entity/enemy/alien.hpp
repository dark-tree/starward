#pragma once

#include "game/entity/entity.hpp"

class AlienEntity : public Entity {

	private:

		bool attacked = false;

	protected:

		int damage_ticks = 0;
		int health = 1;
		int evolution;
		float cooldown = 1;

		AlienEntity(float x, float y, int evolution);

		/// Spawn death/disintegrate particle cloud
		void spawnParticles(Level& level, int min, int max, float vx = 0, float vy = 0);

		/// Check if this entity was hit by the player before
		bool wasAttacked() const;

	public:

		void onDamage(Level& level, int damage, Entity* damager) override;
		void tick(Level& level) override;

		/// Called when damage is dealt, no matter is the entity was killed or not
		virtual void onDamaged(Level& level);

		/// Called when the entity is killed
		virtual void onKilled(Level& level);

		/// Get the number of health points
		int getHealth() const;

		/// Basic spawner, used by most aliens
		template<typename T, typename L = Level, typename S = Segment>
		static bool spawn(L& level, S& segment, int evolution) {
			glm::ivec2 tile = segment.getRandomSpawnPos(1);
			glm::vec2 pos = L::toEntityPos(tile.x, tile.y);

			return level.trySpawn(new T {pos.x, pos.y, evolution});
		}

};
