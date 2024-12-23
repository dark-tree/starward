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

class BulletEntity : public Entity {

	private:

		std::shared_ptr<Entity> parent;
		float velocity;

	public:

		BulletEntity(float velocity, double x, double y, const std::shared_ptr<Entity>& except, float angle = deg(180));

		bool isCausedByPlayer() override;
		std::shared_ptr<Entity> getParent() override;
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

		// bonuses
		int double_barrel_ticks = 0;

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

class PowerUpEntity : public Entity {

	public:

		enum Type {
			LIVE = 0,
			DOUBLE_BARREL = 1,
		};

		static Type randomPick();

	private:

		Type type;

	public:

		PowerUpEntity(double x, double y, Type type);

		bool checkPlacement(Level& level) override;

		void applyEffect(Level& level, PlayerEntity* player);
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
		int buried = 0;

	public:

		SweeperAlienEntity(double x, double y, int evolution);

		bool checkPlacement(Level& level) override;

		void onDamage(Level& level, int damage, Entity* damager) override;
		gls::Sprite sprite(gls::TileSet& tileset) override;
		void tick(Level& level) override;

};

class TurretAlienEntity : public Entity {

	private:

		int evolution; // 0, 1, 2
		float cooldown = 1;

		int barrel = 1;
		float target = -deg(180); // desired rotation
		float head = -deg(180); // current rotation

		void shoot(Level& level, float speed, float radius, float angle);

	public:

		TurretAlienEntity(double x, double y, int evolution);

		bool checkPlacement(Level& level) override;

		void onDamage(Level& level, int damage, Entity* damager) override;
		gls::Sprite sprite(gls::TileSet& tileset) override;
		void tick(Level& level) override;
		void draw(Level& level, gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& writer) override;

};