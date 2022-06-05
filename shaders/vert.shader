#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in int CharacterIndex;

out vec2 TextureOut;

uniform mat4 OrthoMatrix;
uniform vec2[94*4] TextureLookupTable;

void main() {
    gl_Position = OrthoMatrix * vec4(VertexPosition, 1);

    //TextureOut  = TextureLookupTable[CharacterIndex * 4 + VertexIndex];
    TextureOut  = TextureLookupTable[CharacterIndex];
}
