#version 430

// 1024 is the minimum size all devices have to comply to, as defined in opengl standard
layout(local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding=0) uniform image2D north_hemisphere;
layout(rgba32f, binding=1) uniform image2D south_hemisphere;

layout(std430, binding=2) buffer MyBuffer
{
	vec3 normal[];
} NormalBuffer;

const float beta = 1.;

void main(void)
{
                                                                        
    const uint indice = gl_WorkGroupID.x * gl_WorkGroupSize.x + gl_LocalInvocationID.x;
    vec3 normal = NormalBuffer.normal[indice];
    const ivec2 texturePos = ivec2(floor((beta+1f)/(1f-normal.z)*normal.xy));

    vec3 val = NormalBuffer.normal[indice];
    barrier();
    memoryBarrierBuffer();
    val *= 2.;
    barrier();
    memoryBarrierBuffer();
    NormalBuffer.normal[indice] = val;//vec3(indice, 0., 0.);
    //barrier();
    //memoryBarrierBuffer();
    //
    memoryBarrierBuffer();
    barrier();
    memoryBarrierBuffer();
    barrier();

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
