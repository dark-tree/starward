#pragma once

#include <external.hpp>
#include <rendering.hpp>

enum struct TextMode {
	LEFT,
	RIGHT,
};

void emitSpriteQuad(gls::BufferWriter<gls::Vert4f4b>& writer, float tx, float ty, float sx, float sy, float angle, const gls::Sprite& s, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void emitLineQuad(gls::BufferWriter<gls::Vert4f4b>& writer, float x1, float y1, float x2, float y2, float width, const gls::Sprite& s, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void emitTextQuads(gls::BufferWriter<gls::Vert4f4b>& text_writer, float x, float y, float spacing, float size, gls::TileSet& font, uint8_t r, uint8_t g, uint8_t b, uint8_t a, const std::string& str, TextMode mode);