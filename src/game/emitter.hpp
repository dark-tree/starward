#pragma once

#include <external.hpp>
#include <rendering.hpp>

void emitSpriteQuad(gls::BufferWriter<gls::Vert4f4b>& writer, float tx, float ty, float sx, float sy, float angle, const gls::Sprite& s, uint8_t r, uint8_t g, uint8_t b, uint8_t a);