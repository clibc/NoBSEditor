#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec4 VertexColor;
layout (location = 2) in vec2 VertexUV;

uniform mat4 OrthoMatrix;
out vec4 Color;
out vec2 UV;

void main()
{
    gl_Position = OrthoMatrix * vec4(VertexPosition, 1.);
    Color = VertexColor;
    UV = VertexUV;
}
