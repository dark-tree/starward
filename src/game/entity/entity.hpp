#pragma once

#include "external.hpp"
#include "rendering.hpp"
#include "game/color.hpp"
#include "game/level/box.hpp"
#include "render/renderer.hpp"

class Level;
class Segment;

class Entity : public std::enable_shared_from_this<Entity> {

	protected:

		float angle = 0;

		bool visible = false;
		bool dead = false;
		long age = 0;
		Box collider;
		float size;

		void emitEntityQuad(Level& level, BufferWriter<Vert4f4b>& writer, Sprite sprite, float size, float angle, Color color) const;
		void emitBoxWireframe(Box box, RenderLayer& layer, float width, Color color) const;

	public:

		float x;
		float y;

	public:

		Entity(float size, float x, float y);
		virtual ~Entity();
		void clamp();

		float getAngle() const;

		bool shouldRemove() const;
		std::shared_ptr<Entity> self();
		Box getBoxCollider() const;

		bool isDead() const;

	public:

		/// Invoked to check for collision between this and the given entity
		virtual bool shouldCollide(Entity* entity);

		/// Used when something deals damage to the entity
		virtual void onDamage(Level& level, int damage, NULLABLE Entity* damager);

		/// invoked every tick to update the entity state
		virtual void tick(Level& level);

		/// Invoked every frame to draw the entity into the given buffer
		virtual void draw(Level& level, Renderer& renderer) = 0;

		/// Invoked every frame in debug mode to draw colliders and extra info
		virtual void debugDraw(Level& level, Renderer& renderer);

		/// Invoked for entities when added to the world
		virtual void onSpawned(const Level& level, NULLABLE Segment* segment);

		virtual bool checkPlacement(Level& level);
		virtual bool isCausedByPlayer();
		virtual std::shared_ptr<Entity> getParent();

};



