#version 300 es

uniform mat4 uMatrix;

in vec2 iPos;

out vec2 vTex;

void main() {
    vTex = iPos;
    gl_Position = uMatrix * vec4(iPos.xy, 1.0, 1.0);
}