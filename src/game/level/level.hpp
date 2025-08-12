#pragma once

#include <external.hpp>
#include <rendering.hpp>

#include "segment.hpp"
#include "game/entity/entity.hpp"
#include "game/entity/player.hpp"
#include "biome.hpp"
#include "box.hpp"

enum struct GameState {
	BEGIN,
	PLAYING,
	DEAD,
};

struct Collision {

	enum Type {
		MISS,
		TILE,
		ENTITY
	};

	Type type;
	Entity* entity = nullptr;
	int x, y;

	Collision()
	: type(MISS), entity(nullptr), x(0), y(0) {}

	Collision(int x, int y)
	: type(TILE), entity(nullptr), x(x), y(y) {}

	Collision(Entity* entity)
	: type(ENTITY), entity(entity), x(0), y(0) {}

};

class Level {

	public:

		// read by Game class to reload game state next tick
		bool reload = false;

		static constexpr int segment_width = 128;
		static constexpr int segment_height = 32;

		using LevelSegment = Segment<segment_width, segment_height>;

	private:

		BiomeManager& manager;

		GameState state = GameState::BEGIN;
		float aliveness = 1.0f;
		float linear_aliveness = 1.0f;
		int score = 0;
		int hi = 0;
		float base_speed = 0.8;
		float scroll = 0;
		float tar = 0.0f;
		float skip = 0;
		float biome_speed = 0;
		int age = 0;
		int total = 0;
		int play_count = 0;

		int timer = 0;
		int konami = 0;
		bool debug = false;

		std::array<LevelSegment, 4> segments;

		std::vector<std::shared_ptr<Entity>> pending {};
		std::vector<std::shared_ptr<Entity>> entities {};
		std::shared_ptr<PlayerEntity> player {};

	public:

		Level(BiomeManager& manager);
		void loadHighScore();
		void spawnInitial();
		void loadPlayCount();

		float getSkip() const;
		float getScroll() const;
		float getSpeed() const;
		float getLinearAliveness() const;
		bool isDebug() const;

		glm::vec2 toTilePos(int x, int y) const;
		glm::vec2 toEntityPos(int x, int y) const;

		Entity* randomAlien(int margin, LevelSegment& segment);
		std::shared_ptr<PlayerEntity> getPlayer();

		template<typename T>
		std::shared_ptr<T> addEntity(T* entity) {
			return std::static_pointer_cast<T>(pending.emplace_back(entity));
		}

		void addSlowness(float tar);
		void addScore(int points);
		void tick();
		void draw(TileSet& font8x8, BufferWriter<Vert4f4b>& text_writer, TileSet& tileset, BufferWriter<Vert4f4b>& game_writer);
		void setTile(int x, int y, uint8_t tile);
		uint8_t getTile(int x, int y) const;

		Collision checkTileCollision(const Box& collider) const;
		Collision checkEntityCollision(Entity* self) const;
		Collision checkCollision(const Box& collider);

		Collision checkCollision(Entity* self);
		void setState(GameState state);

		std::vector<std::shared_ptr<Entity>>& getEntities();

};
