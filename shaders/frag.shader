#version 330 core

out vec4 FragColor;
in vec2 TextureOut;

uniform sampler2D Texture;

void main() {
    vec4 TexColor = texture(Texture, TextureOut);
    FragColor = vec4(vec3(1), TexColor.x);
}
