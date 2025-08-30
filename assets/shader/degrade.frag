#version 300 es

#ifdef GL_FRAGMENT_PRECISION_HIGH
    precision highp float;
#else
    precision mediump float;
#endif

uniform sampler2D uSampler;
uniform float uTime;
uniform float uAliveness;
uniform vec2 uResolution;

vec2 getResolution() {
#ifdef GL_FRAGMENT_PRECISION_HIGH
    return uResolution;
#else
    return uResolution * 0.5f;
#endif
}

in vec2 vTex;

out vec4 fColor;

vec2 curve(vec2 uv) {
    uv = (uv - 0.5) * 2.0;
    uv *= 1.1;
    uv.x *= 1.0 + pow((abs(uv.y) / 5.0), 2.0);
    uv.y *= 1.0 + pow((abs(uv.x) / 4.0), 2.0);
    uv  = (uv / 2.0) + 0.5;
    uv =  uv *0.92 + 0.04;

    return uv;
}

vec3 sampleInput(vec2 resolution, vec2 pos) {
    const float pixelation = 2.0f;

    vec2 pixelPos = (pos * resolution);
    vec2 sourcePixel = round(pixelPos / pixelation) * pixelation;
    vec2 normalizedPos = sourcePixel / resolution;

    return texture(uSampler, normalizedPos).rgb;
}

void main() {
    vec2 res = getResolution();
    vec2 uv = curve(vTex.xy);

    vec3 col;
    float x = sin(0.3*uTime+uv.y*21.0)*sin(0.7*uTime+uv.y*29.0)*sin(0.3+0.33*uTime+uv.y*31.0)*0.0017;

    col.r = sampleInput(res, vec2(x+uv.x+0.001,uv.y+0.001)).x+0.05;
    col.g = sampleInput(res, vec2(x+uv.x+0.000,uv.y-0.002)).y+0.05;
    col.b = sampleInput(res, vec2(x+uv.x-0.002,uv.y+0.000)).z+0.05;
    col.r += 0.08*sampleInput(res, 0.75*vec2(x+0.025, -0.027)+vec2(uv.x+0.001,uv.y+0.001)).x;
    col.g += 0.05*sampleInput(res, 0.75*vec2(x-0.022, -0.02)+vec2(uv.x+0.000,uv.y-0.002)).y;
    col.b += 0.08*sampleInput(res, 0.75*vec2(x-0.02, -0.018)+vec2(uv.x-0.002,uv.y+0.000)).z;

    col = clamp(col*0.6+0.4*col*col*1.0,0.0,1.0);
    col = mix(col, vec3(0.3, 1.0, 0.5), 0.20);

    float vig = (0.0 + 1.0*16.0*uv.x*uv.y*(1.0-uv.x)*(1.0-uv.y));
    col *= vec3(pow(vig,0.3));

    col *= vec3(0.95,1.05,0.95);
    col *= 2.8;

    float scans = clamp( 0.35+0.35*sin(3.5*uTime+vTex.y * res.y *1.5), 0.0, 1.0);

    float s = pow(scans,1.7);
    col = col*vec3( 0.4+0.7*s) ;

    col *= 1.0+0.01*sin(110.0*uTime);

    if (uv.x < 0.0 || uv.x > 1.0) col *= 0.0;
    if (uv.y < 0.0 || uv.y > 1.0) col *= 0.0;

    col *= 1.0 - 0.65 * vec3(clamp((mod(vTex.x * res.x, 2.0) - 1.0) * 2.0, 0.0, 1.0));
    fColor = vec4(col, 1.0) * (0.9f + uAliveness * 0.1f);
}