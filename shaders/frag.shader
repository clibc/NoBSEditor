#version 330 core

out vec4 FragColor;
in vec2 TextureOut;
in vec3 FontColor;

uniform sampler2D Texture;

void main()
{
    vec4 TexColor = texture(Texture, TextureOut);
    FragColor = vec4(FontColor, TexColor.x);
}
