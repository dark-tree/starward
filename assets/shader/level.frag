#version 300 es

precision mediump float;

uniform sampler2D uSampler;
uniform float uAliveness;

in vec2 vTex;
in vec4 vCol;

out vec4 fColor;

void main() {
    fColor = vCol.rgba * texture(uSampler, vTex).rgba * (0.7f + uAliveness * 0.3f);
}