#pragma once

#include <external.hpp>
#include <rendering.hpp>

class Level;

class Entity : public std::enable_shared_from_this<Entity> {

	protected:

		float r, g, b, a;
		float angle = 0;

		bool visible = false;
		bool dead = false;
		long age = 0;

	public:

		const int tile_radius;
		const double size;
		double x;
		double y;

	public:

		Entity(int radius, double size, double x, double y);
		virtual ~Entity();

		void move(Level& level, float x, float y);
		void clamp();
		virtual bool checkPlacement(Level& level);

		bool shouldRemove() const;
		virtual bool shouldCollide(Entity* entity);
		virtual void onDamage(Level& level, int damage, Entity* damager);

		virtual bool isCausedByPlayer();
		virtual std::shared_ptr<Entity> getParent();

		virtual gls::Sprite sprite(gls::TileSet& tileset);
		virtual void draw(Level& level, gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& writer);
		virtual void tick(Level& level);

		std::shared_ptr<Entity> self();

};



