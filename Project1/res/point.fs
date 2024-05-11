#version 430 core

in vec4 fragColor;

out vec4 color;

void main() {
    float r = 0.5 * 3;
    float dist = length(gl_PointCoord - vec2(0.5, 0.5));
    if (dist < r) {
        color = fragColor;
    } else {
        discard;
    }
}
