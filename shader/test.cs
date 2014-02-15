#version 440 

layout (local_size_x = 10) in;
layout(std430, binding = 0) buffer MyBuffer
{
	float val[];
};

layout (binding=1, r32f) uniform writeonly image2D destTex;
uniform float beta;

void main(void)
{
    val[gl_WorkGroupID.x] = pow(val[gl_WorkGroupID.x],2);
    ivec2 storePos = ivec2(gl_WorkGroupID.x, 1.);
    const vec4 color = vec4(1.0, 0.0, 0.0, 0.0);
    imageStore(destTex, storePos, color);
}

