#version 330 core

layout (location = 0) in int VertexIndex;

out vec2 TextureOut;

uniform mat4 OrthoMatrix;
uniform mat4 ModelMatrix;
uniform vec3[4]    VertexLookupTable;
uniform vec2[94*4] TextureLookupTable;
uniform int CharacterIndex;

void main() {
    gl_Position = OrthoMatrix * ModelMatrix * vec4(VertexLookupTable[VertexIndex], 1);

    TextureOut  = TextureLookupTable[CharacterIndex * 4 + VertexIndex];
}
