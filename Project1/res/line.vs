#version 430 core

layout(location = 0) in dvec3 position; 
layout(location = 1) in dvec4 color;

out vec4 fragColor;

uniform mat4 rotMat, postRotMat, projMat, preRotMat, offsetScaleMat;

void main() {
	mat4 transform = offsetScaleMat * projMat * postRotMat * rotMat * preRotMat;
    gl_Position = transform * vec4(position, 1.0); 
	fragColor = vec4(color);
}
