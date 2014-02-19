#version 440 

layout (local_size_x = 1) in;

layout(std430, binding = 2) buffer MyBuffer
{
	vec3 val[];
};

void main(void)
{
    val[gl_WorkGroupID.x] = val[gl_WorkGroupID.x]*float(gl_WorkGroupID.x);
}

