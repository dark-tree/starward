#version 300 es

precision mediump float;

uniform sampler2D uSampler;

in vec2 vTex;
in vec4 vCol;

out vec4 fColor;

void main() {
    fColor = vCol.rgba * texture(uSampler, vTex).rgba;
}