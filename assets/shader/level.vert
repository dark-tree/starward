#version 300 es
uniform mat4 uMatrix;

in vec2 iPos;
in vec2 iTex;
in vec4 iCol;

out vec2 vTex;
out vec4 vCol;

void main() {
    gl_Position = uMatrix * vec4(iPos.xy, 1.0, 1.0);
    vTex = iTex;
    vCol = iCol;
}