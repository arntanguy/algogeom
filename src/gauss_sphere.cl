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
void kernel gauss_sphere(volatile global float4* normal, global int* north_hemisphere, global int* south_hemisphere, const float alpha_g, const float beta_g)
{
    private const float beta = beta_g;
    private const float alpha = alpha_g; 
    private const int rows = 2*(1+(int)ceil(beta/alpha)) + 1;
    
    private int index = get_global_id(0);
    private float tmp;
    private int2 coord;
    private float4 n = normal[index];
    // Compute projection for the south hemisphere
    if(n.z<0) {
        tmp = (beta+alpha)/(alpha-n.z);
        coord = convert_int2((float2)(floor(tmp*n.xy)))+ (int2)(rows/2, rows/2);
        private int tex = coord.y * rows + coord.x;
        // Atomic operation to increment the value for the current normal.
        // This ensures that no other normal is currently being accumulated at this position.
        atomic_inc(&south_hemisphere[tex]);
    } 
    // North hemisphere
    else {
        tmp = (beta+alpha)/(alpha+n.z);
        coord = convert_int2((float2)(floor(tmp*n.xy))) + (int2)(rows/2, rows/2);
        private int tex = coord.y * rows + coord.x;
        atomic_inc(&north_hemisphere[tex]);
    }
}
