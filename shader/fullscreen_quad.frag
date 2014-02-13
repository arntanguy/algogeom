#version 330 core

out vec4 out_Color;
in vec2 UV;

uniform sampler2D Texture; //Image to be projected

void main(void)
{
    out_Color = texture(Texture,UV);
    //out_Color = vec4(UV, 0., 1.);
    //out_Color = vec4(1., 0., 0., 1.);
}
