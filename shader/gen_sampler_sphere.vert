#version 430
uniform r32ui image2D hemisphere_sud;
uniform r32ui sampler2D hemisphere_nord;

layout(std430, binding = 0) buffer MyBuffer
{
	vec3 normal[];
};

uniform float beta;

void main(void)
{
                                                                        
	vec3 normal = MyBuffer.normal[gl_WorkGroupID * gl_WorkGroupSize + gl_LocalInvocationID];
	if(normal.z<0)
		imageAtomicAdd(hemisphere_sud, floor((beta+1f)/(1f-normal.z)*normal.xy), 1);
	else
		imageAtomicAdd(hemisphere_nord, floor((beta+1f)/(1f+normal.z)*normal.xy), 1);	
}
