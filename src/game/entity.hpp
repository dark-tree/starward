#pragma once

#include <external.hpp>
#include <rendering.hpp>

class Level;

class Entity {

	protected:

		float r, g, b, a;

		bool dead = false;
		long age = 0;

	public:

		const double size;
		double x;
		double y;

	public:

		Entity(double size, double x, double y);
		virtual ~Entity();

		bool shouldRemove() const;
		virtual bool checkCollision(float x, float y, float size);

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

		void tick(Level& level) override;

};

class BlowEntity : public Entity {

	public:

		BlowEntity(double x, double y);

		bool checkCollision(float x, float y, float size) override;

		gls::Sprite sprite(gls::TileSet& tileset) override;
		void tick(Level& level) override;

};

class PlayerEntity : public Entity {

	private:

		float cooldown = 0;

	public:

		PlayerEntity();

		gls::Sprite sprite(gls::TileSet& tileset) override;
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

class SweeperAlienEntity : public Entity {

	private:

		float facing = 1;
		float cooldown = 1;

	public:

		SweeperAlienEntity(double x, double y);

		gls::Sprite sprite(gls::TileSet& tileset) override;
		void tick(Level& level) override;

};