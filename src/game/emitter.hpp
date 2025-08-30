#pragma once

#include "external.hpp"
#include "rendering.hpp"

struct RenderLayer;

enum struct TextMode {
	LEFT   = 0,
	CENTER = 1,
	RIGHT  = 2,
};

void emitSpriteQuad(BufferWriter<VertLevel>& writer, float tx, float ty, float sx, float sy, float angle, const Sprite& s, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void emitLineQuad(BufferWriter<VertLevel>& writer, float x1, float y1, float x2, float y2, float width, const Sprite& s, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void emitTextQuads(RenderLayer& layer, float x, float y, float spacing, float size, uint8_t r, uint8_t g, uint8_t b, uint8_t a, const std::string& str, TextMode mode);
void emitTileQuad(BufferWriter<VertLevel>& writer, Sprite s, int x, int y, float ox, float oy, uint8_t r, uint8_t g, uint8_t b, uint8_t a);