/******************************************************************************
*     Copyright (C) 2014 TANGUY Arnaud arn.tanguy@gmail.com                   *
*                                                                             *
* This program is free software; you can redistribute it and/or modify        *
* it under the terms of the GNU General Public License as published by        *
* the Free Software Foundation; either version 2 of the License, or           *
* (at your option) any later version.                                         *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                *
* GNU General Public License for more details.                                *
*                                                                             *
* You should have received a copy of the GNU General Public License along     *
* with this program; if not, write to the Free Software Foundation, Inc.,     *
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                 *
 ******************************************************************************/


/**
 * Use float4 because float3 would be padded to float4 anyway, so avoid confusion
 */
void kernel gauss_sphere(volatile global float4* normal, global int* north_hemisphere, global int* south_hemisphere)
{
    // Crashes if one of the normals is null!
    //
    // XXX
    private const float d = 1.;
    private const float p = 200.;
    private const float h = d+p;
    // Half width (of image)
    private const float w2 = 1+p/d;
    //printf("w2: %f\n", w2);

    private int index = get_global_id(0);
    private float4 n = normalize(normal[index]);


    //printf("n.x %f\n" , n.x);
    //printf("n.y %f\n" , n.y);
    //printf("n.z %f\n" , n.z);

    // C is above for the south hemisphere,
    // beneath for the north one
    float4 C;
    if(n.y < 0)
        C = d*normalize((float4)(0., 1., 0., 0.));
    else
        C = d*((float4)(0., -1., 0., 0.));

    //printf("c.x %f\n" , C.x);
    //printf("c.y %f\n" , C.y);
    //printf("c.z %f\n" , C.z);
    // We took the unit sphere, so n is directly the position on the sphere
    float4 direction = n-C;
    //printf("direction.x %f\n" , direction.x);
    //printf("direction.y %f\n" , direction.y);
    //printf("direction.z %f\n" , direction.z);

    // cos(vertical, direction)
    float dot_p = fabs(dot(direction, (float4)(0.f, 1.f, 0.f, 0.f)));
    //printf("dot: %f\n", dot_p);

    // Trigo to compute length of vector from C to plane
    float l = h/dot_p;
    //printf("l: %f\n", l);
    //printf("n: %f\n", n);
    // Project onto plane
    float4 point = C + l * direction;
    //printf("point.x %f\n" , point.x);
    //printf("point.y %f\n" , point.y);
    //printf("point.z %f\n" , point.z);

    // Convert from plane coordinates to image coordinates
    int2 coord = convert_int2((float2)(w2, w2) + point.xz);
    //printf("coord.x %i\n" , coord.x);
    //printf("coord.y %i\n" , coord.y);
    // Convert to accumulator array index
    int c = coord.x + 2*w2*coord.y;

    //printf("\n\n");
    // Accumulate value
    if(n.y < 0) {
        // Increment value
        atomic_inc(&north_hemisphere[c]);
    } else {
        atomic_inc(&south_hemisphere[c]);
    }

    //printf("normals %f", normal[index].x);

    //if(n.z < 0) {
    //    // Coordinate in image
    //    private int2 coord = convert_int2(((beta+1.f)/(1.f-n.z) * n.xy));
    //    // Coordinate in array
    //    private int c = coord.x + coord.y*width;
    //    printf("\ncoord.x %i\n", coord.x);
    //    printf("\ncoord.y %i\n", coord.y);
    //    printf("\nc %i\n", c);
    //    // Increment value
    //    atomic_inc(&north_hemisphere[c]);
    //    printf("\nnorth hemisphere %i\n", north_hemisphere[c]);
    //} else {
    //    // Coordinate in image
    //    private int2 coord = convert_int2(((beta+1.f)/(1.f-n.z) * n.xy));
    //    // Coordinate in array
    //    private int c = coord.x + coord.y*width;
    //    printf("\ncoord.x %i\n", coord.x);
    //    printf("\ncoord.y %i\n", coord.y);
    //    printf("\nc %i\n", c);
    //    // Increment value
    //    atomic_inc(&south_hemisphere[c]);
    //}
}


//void kernel gauss_sphere(volatile global float4* normal, global int* north_hemisphere, global int* south_hemisphere)
//{
//    // XXX
//    private const float beta = 90;
//    private const int width = 400;
//
//    private int index = get_global_id(0);
//
//    //printf("normals %f", normal[index].x);
//    private float4 n= normalize(normal[index]);
//
//    if(n.z < 0) {
//        // Coordinate in image
//        private int2 coord = convert_int2(((beta+1.f)/(1.f-n.z) * n.xy));
//        // Coordinate in array
//        private int c = coord.x + coord.y*width;
//        printf("\ncoord.x %i\n", coord.x);
//        printf("\ncoord.y %i\n", coord.y);
//        printf("\nc %i\n", c);
//        // Increment value
//        atomic_inc(&north_hemisphere[c]);
//        printf("\nnorth hemisphere %i\n", north_hemisphere[c]);
//    } else {
//        // Coordinate in image
//        private int2 coord = convert_int2(((beta+1.f)/(1.f-n.z) * n.xy));
//        // Coordinate in array
//        private int c = coord.x + coord.y*width;
//        printf("\ncoord.x %i\n", coord.x);
//        printf("\ncoord.y %i\n", coord.y);
//        printf("\nc %i\n", c);
//        // Increment value
//        atomic_inc(&south_hemisphere[c]);
//    }
//}
