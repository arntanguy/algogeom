#version 430

// 1024 is the minimum size all devices have to comply to, as defined in opengl standard
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

//layout(rgba32f, binding=0) uniform image2D north_hemisphere;
//layout(rgba32f, binding=1) uniform image2D south_hemisphere;

layout(std430, binding=0) buffer MyBuffer
{
	float normal[];
};


void main(void)
{
                                                                        
    const uint indice = gl_WorkGroupID.x * gl_WorkGroupSize.x + gl_LocalInvocationID.x;
    const uvec3 texturePos = gl_WorkGroupID * gl_WorkGroupSize + gl_LocalInvocationID;

    memoryBarrierShared();
    normal[0] = 42;
    memoryBarrierShared();
    normal[1] = 42;

    //memoryBarrierShared();
    //vec4 color = imageLoad(north_hemisphere, ivec2(texturePos.xy));
    //memoryBarrierShared();
    //imageStore(north_hemisphere, ivec2(texturePos.xy), vec4(n, 1.)); 
    //imageStore(south_hemisphere, ivec2(texturePos.xy), vec4(n, 1.)); 
//	if(normal.z<0) {
//        imageStore(north_hemisphere, ivec2(gl_LocalInvocationID.x, gl_LocalInvocationID.y), vec4(normal, 1.)); 
//		//imageAtomicAdd(south_hemisphere, ivec2(floor((beta+1f)/(1f-normal.z)*normal.xy)), 1u);
//    } else {
//	    //imageAtomicAdd(north_hemisphere, floor((beta+1f)/(1f+normal.z)*normal.xy), 1u);	
//        imageStore(south_hemisphere, ivec2(0, 0), vec4(normal, 1.)); 
//    }
}
