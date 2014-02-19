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
    // XXX
    private const float beta = 90;
    private const int width = 400;

    private int index = get_global_id(0);

    //printf("normals %f", normal[index].x);
    private float4 n= normalize(normal[index]);

    if(n.z < 0) {
        // Coordinate in image
        private int2 coord = convert_int2(((beta+1.f)/(1.f-n.z) * n.xy));
        // Coordinate in array
        private int c = coord.x + coord.y*width;
        printf("\ncoord.x %i\n", coord.x);
        printf("\ncoord.y %i\n", coord.y);
        printf("\nc %i\n", c);
        // Increment value
        atomic_inc(&north_hemisphere[c]);
        printf("\nnorth hemisphere %i\n", north_hemisphere[c]);
    } else {
        // Coordinate in image
        private int2 coord = convert_int2(((beta+1.f)/(1.f-n.z) * n.xy));
        // Coordinate in array
        private int c = coord.x + coord.y*width;
        printf("\ncoord.x %i\n", coord.x);
        printf("\ncoord.y %i\n", coord.y);
        printf("\nc %i\n", c);
        // Increment value
        atomic_inc(&south_hemisphere[c]);
    }
}
