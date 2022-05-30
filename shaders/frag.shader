#version 330 core

out vec4 FragColor;
in vec2 TextureOut;

uniform sampler2D Texture1;

void main() {
    FragColor = vec4(texture(Texture1, TextureOut).rgb, 1);
}
