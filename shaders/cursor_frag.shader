#version 330 core

in  vec4 Color;
in  vec2 UV;
out vec4 FragColor;

void main()
{
    FragColor = vec4(Color.xyz, 1.);

    vec2 Uv = UV * 2 - 1;
    
    // TODO : eliminate this branches if possible
    if(Color.w > 0.9)
    {
        if(abs(Uv.x) > 0.9 || abs(Uv.y) > 0.95)
        {
            FragColor = vec4(Color.xyz, 1);
        }
        else
        {
            FragColor = vec4(0,0,0,0);
        }
    }
}
