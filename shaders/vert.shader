#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexColor;
layout (location = 2) in int CharacterIndex;

out vec2 TextureOut;
out vec3 FontColor;

uniform mat4 OrthoMatrix;
uniform vec2[94*4] TextureLookupTable;

void main()
{
    gl_Position = OrthoMatrix * vec4(VertexPosition, 1);
    TextureOut  = TextureLookupTable[CharacterIndex];
    FontColor = VertexColor;
}
