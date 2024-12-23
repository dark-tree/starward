#pragma once

#include <external.hpp>
#include <rendering.hpp>

#include "segment.hpp"
#include "entity.hpp"

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

	const Type type;
	Entity* const entity = nullptr;
	const int x, y;

	Collision()
	: type(MISS), entity(nullptr), x(0), y(0) {}

	Collision(int x, int y)
	: type(TILE), entity(nullptr), x(x), y(y) {}

	Collision(Entity* entity)
	: type(ENTITY), entity(entity), x(0), y(0) {}
};

class Level {

	public:

		static constexpr int segment_width = 128;
		static constexpr int segment_height = 32;

		using LevelSegment = Segment<segment_width, segment_height>;

	private:

		GameState state = GameState::BEGIN;
		int score = 0;
		double base_speed = 0.75;
		double scroll = 0;
		float skip = 0;
		int age = 0;

		std::array<LevelSegment, 4> segments;

		std::vector<Entity*> pending;
		std::vector<std::shared_ptr<Entity>> entities;
		std::shared_ptr<PlayerEntity> player;

	public:

		double getSkip() const;
		double getScroll() const;
		double getSpeed() const;

		glm::ivec2 toTilePos(int x, int y);
		glm::vec2 toEntityPos(int x, int y);

		Entity* randomAlien(int margin, LevelSegment& segment);
		std::shared_ptr<PlayerEntity> getPlayer();

		void addScore(int points);
		void addEntity(Entity* entity);
		void tick();
		void draw(gls::TileSet& font8x8, gls::BufferWriter<gls::Vert4f4b>& text_writer, gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& game_writer);
		void set(int x, int y, uint8_t tile);
		uint8_t get(int x, int y);
		Collision checkCollision(Entity* self);
		void setState(GameState state);

};
