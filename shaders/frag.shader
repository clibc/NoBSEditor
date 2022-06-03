#version 330 core

out vec4 FragColor;
in vec2 TextureOut;

uniform sampler2D Texture1;

void main() {
    vec4 TexColor = texture(Texture1, TextureOut);
    float Alpha = TexColor.g;
    vec4 White = vec4(1);
    vec4 Bacground = vec4(1,0,0,1);
    FragColor = mix(Bacground, White, Alpha);
}
