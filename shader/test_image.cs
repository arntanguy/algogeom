#version 440 

layout (local_size_x = 16, local_size_y = 16) in;

layout(std430, binding = 0) buffer MyBuffer
{
	float val[];
};

uniform float roll;
layout(rgba32f) uniform image2D destTex;

// In order to write to a texture, we have to introduce it as image2D.
// local_size_x/y/z layout variables define the work group size.
// gl_GlobalInvocationID is a uvec3 variable giving the global ID of the thread,
// gl_LocalInvocationID is the local index within the work group, and
// gl_WorkGroupID is the work group's index
void main(void)
{
    //val[gl_WorkGroupID.x] = pow(val[gl_WorkGroupID.x],2);
        ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
        float localCoef = length(vec2(ivec2(gl_LocalInvocationID.xy)-8)/8.0);
        float globalCoef = sin(float(gl_WorkGroupID.x+gl_WorkGroupID.y)*0.1 + roll)*0.5;
        imageStore(destTex, storePos, vec4(1.0-globalCoef*localCoef, 0.0, 0.0, 0.0));
}

