#version 440 

layout (local_size_x = 1) in;

layout(std430, binding = 2) buffer MyBuffer
{
	float val[];
};

void main(void)
{
    val[gl_WorkGroupID.x] = pow(val[gl_WorkGroupID.x],2);
}

