#version 430 core

layout(location = 0) in dvec3 position; 
layout(location = 1) in dvec4 color;

out vec4 fragColor;

uniform mat4 rotMat, projMat, sizePosMat;

const mat4 offset = mat4(
    1.0, 0.0, 0.0, 0.0, 
    0.0, 1.0, 0.0, 0.0, 
    0.0, 0.0, 1.0, 0.0, 
    0.0, 0.0, -3.0, 1.0 
);

void main() {
	mat4 transform = sizePosMat * projMat * offset * rotMat;
    gl_Position = transform * vec4(position, 1.0); 
	fragColor = vec4(color);
}
