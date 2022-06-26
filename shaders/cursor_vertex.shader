#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexColor;

uniform mat4 OrthoMatrix;
out vec3 Color;

void main()
{
    gl_Position = OrthoMatrix * vec4(VertexPosition, 1.);
    Color = VertexColor;
}
