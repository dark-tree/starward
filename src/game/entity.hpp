#pragma once

#include <external.hpp>
#include <rendering.hpp>

class Level;

class Entity {

	protected:

		float r, g, b, a;
		float angle = 0;

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

		bool shouldRemove() const;
		virtual bool shouldCollide(Entity* entity);
		virtual void onDamage(Level& level, int damage, Entity* damager);

		virtual bool isCausedByPlayer();
		virtual Entity* getParent();

		virtual gls::Sprite sprite(gls::TileSet& tileset);
		virtual void draw(Level& level, gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& writer);
		virtual void tick(Level& level);

};

class BulletEntity : public Entity {

	private:

		Entity* parent;
		float velocity;

	public:

		BulletEntity(float velocity, double x, double y, Entity* except);

		bool isCausedByPlayer() override;
		Entity* getParent() override;
		void tick(Level& level) override;

};

class BlowEntity : public Entity {

	public:

		BlowEntity(double x, double y);

		bool shouldCollide(Entity* entity) override;

		gls::Sprite sprite(gls::TileSet& tileset) override;
		void tick(Level& level) override;

};

class PlayerEntity : public Entity {

	private:

		int lives = 2;
		int invulnerable = 0;
		float cooldown = 0;
		float tilt;

	public:

		PlayerEntity();

		bool shouldCollide(Entity* entity) override;

		bool isCausedByPlayer() override;
		void onDamage(Level& level, int damage, Entity* damager);
		gls::Sprite sprite(gls::TileSet& tileset) override;
		void draw(Level& level, gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& writer);
		void tick(Level& level) override;

};

class TileEntity : public Entity {

	private:

		double fx;
		double fy;
		uint8_t tile;

	public:

		TileEntity(double x, double y, uint8_t tile, int tx, int ty);

		gls::Sprite sprite(gls::TileSet& tileset) override;
		void tick(Level& level) override;

};

class ExtraLiveEntity : public Entity {

	public:

		ExtraLiveEntity(double x, double y);

		void onDamage(Level& level, int damage, Entity* damager) override;
		gls::Sprite sprite(gls::TileSet& tileset) override;
		void tick(Level& level) override;

};

class SweeperAlienEntity : public Entity {

	private:

		int evolution; // 0, 1, 2
		float bump = 0;
		int health = 2;
		int count = 0;
		float facing = 1;
		float cooldown = 1;

	public:

		SweeperAlienEntity(double x, double y, int evolution);

		void onDamage(Level& level, int damage, Entity* damager) override;
		gls::Sprite sprite(gls::TileSet& tileset) override;
		void tick(Level& level) override;

};