#version 440 

layout (local_size_x = 0) in;
layout(std430, binding = 0) buffer MyBuffer
{
	float val[];
};

uniform float beta;

void main(void)
{
    val[gl_WorkGroupID.x] = pow(val[gl_WorkGroupID.x],2);
}

