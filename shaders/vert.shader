#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 TextureCoord;

out vec2 TextureOut;

uniform mat4 OrthoMatrix;
uniform mat4 ModelMatrix;

void main() {
    gl_Position = OrthoMatrix * ModelMatrix * vec4(VertexPosition, 1);
    TextureOut  = TextureCoord;
}
