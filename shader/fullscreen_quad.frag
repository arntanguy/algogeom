#version 430 core

out vec4 out_Color;
in vec2 UV;

//layout(r32ui) uniform uimage2D Texture;
uniform sampler2D Texture; //Image to be projected

void main(void)
{
    //vec4 color = imageLoad(Texture,ivec2(UV*100));
    vec4 color = texture(Texture,UV);
    out_Color = color;
    //vec4(color.x, 0., 0., 1.);
    //out_Color = vec4(UV, 0., 1.);
    //out_Color = vec4(1., 0., 0., 1.);
}
