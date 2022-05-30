#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 TextureCoord;

out vec2 TextureOut;

void main() {
    gl_Position = vec4(VertexPosition, 1);
    TextureOut  = TextureCoord;
}
