#pragma once

#include <external.hpp>
#include <rendering.hpp>

#include "segment.hpp"
#include "entity.hpp"

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

	private:

		static constexpr int segment_width = 128;
		static constexpr int segment_height = 32;

		double base_speed = 0.75;
		double scroll = 0;
		float skip = 0;

		std::array<Segment<segment_width, segment_height>, 4> segments;

		std::vector<Entity*> pending;
		std::vector<std::unique_ptr<Entity>> entities;

	public:

		double getSkip() const;
		double getScroll() const;

		glm::ivec2 toTilePos(int x, int y);
		glm::vec2 toEntityPos(int x, int y);

		void addEntity(Entity* entity);
		void tick();
		void draw(gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& buffer);
		void set(int x, int y, uint8_t tile);
		uint8_t get(int x, int y);
		Collision checkCollision(Entity* self);

};
