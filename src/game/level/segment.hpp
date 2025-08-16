#pragma once

#include "external.hpp"
#include "rendering.hpp"

// number of special segments to prepend before normal terrain generation
#define SEGMENT_START_OFFSET 3

struct RenderLayer;
extern int global_segment_id;

class Segment {

	public:

		static void resetTerrainGeneratr();

		static constexpr int width = 128;
		static constexpr int height = 32;

		int index = 0;
		uint8_t tiles[width * height];

		double size();

		int next();

		void drawTile(RenderLayer& layer, int tile, int x, int y, int width, float scroll, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

		void fill(int tile);
		void generate(float low, float high);
		void generateString(int x, int y, const std::string& text, int tile, int scale);

	public:

		Segment();

		glm::ivec2 getRandomSpawnPos(int margin);

		bool contains(int y) const;

		bool isLocalTile(int x, int y);

		uint8_t& at(int sx, int sy);

		const uint8_t& at(int sx, int sy) const;

		int getStartY() const;
		int getEndY() const;

		uint8_t& atWorldPos(int sx, int sy);

		const uint8_t& atWorldPos(int sx, int sy) const;

		bool tick(double scroll, glm::vec2 terrain);

		void draw(RenderLayer& layer, double scroll, bool debug);

};