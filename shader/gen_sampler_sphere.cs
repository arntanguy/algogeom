#version 430

// 1024 is the minimum size all devices have to comply to, as defined in opengl standard
layout(local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding=0) uniform image2D north_hemisphere;
layout(rgba32f, binding=1) uniform image2D south_hemisphere;

layout(std430, binding=2) buffer MyBuffer
{
	vec4 normal[];
} NormalBuffer;

uniform float beta;

void main(void)
{
                                                                        
    const uint index = gl_WorkGroupID.x * gl_WorkGroupSize.x + gl_LocalInvocationID.x;
    const vec4 n = normalize(NormalBuffer.normal[index]);
    NormalBuffer.normal[index].xy =  vec2(floor((beta+1f)/(1f-n.z)*n.xy)); 

    vec4 val = imageLoad(north_hemisphere, ivec2(64, 64));
    //imageStore(north_hemisphere, ivec2( 64, 64), vec4(42f, 0., 0., 1.)); 
    //val = imageLoad(north_hemisphere, ivec2(64, 64));
    NormalBuffer.normal[index].w = val.x+1f; 

    //memoryBarrierImage();
    //imageStore(north_hemisphere, ivec2(texturePos.xy), vec4(normal, 1.)); 
    
//	if(normal.z<0) {
//        imageStore(north_hemisphere, ivec2(gl_LocalInvocationID.x, gl_LocalInvocationID.y), vec4(normal, 1.)); 
//		//imageAtomicAdd(south_hemisphere, ivec2(floor((beta+1f)/(1f-normal.z)*normal.xy)), 1u);
//    } else {
//	    //imageAtomicAdd(north_hemisphere, floor((beta+1f)/(1f+normal.z)*normal.xy), 1u);	
//        imageStore(south_hemisphere, ivec2(0, 0), vec4(normal, 1.)); 
//    }
}
