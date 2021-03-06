///////////////////////////////////////////////////////////////////////////
// Copyright 2011 Evan Mezeske.
//
// This file is part of Digbuild.
// 
// Digbuild is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.
// 
// Digbuild is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Digbuild.  If not, see <http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////////

#version 130
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DArray material_texture_array;
uniform sampler2DArray material_specular_map_array;
uniform sampler2DArray material_bump_map_array;
uniform float fog_distance;

varying vec3 tangent_sun_direction;
varying vec3 tangent_camera_direction;
varying vec3 base_lighting;
varying vec3 sun_lighting;
varying vec3 texture_coordinates;
varying float fog_depth;

void main()
{
    vec4 texture_color = texture2DArray( material_texture_array, texture_coordinates );

    // The bump map uses the 'z' coordinate to represent height, while Digbuild uses the 'y'
    // coordinate; thus the bump map needs to be swizzled a bit.
    vec3 bump_direction = texture2DArray( material_bump_map_array, texture_coordinates ).xzy;
    // The bump map 'x' and 'z' coordinates are packed into the range [0,1] so they need to be
    // massaged into their real range of [-1,1].  The 'y' coordinate is not packed this way.
    bump_direction.x = bump_direction.x * 2.0 - 1.0;
    bump_direction.z = bump_direction.z * 2.0 - 1.0;
    bump_direction = normalize( bump_direction );

    float bump_factor = clamp( dot( tangent_sun_direction, bump_direction ), 0.0, 1.0 );
    vec3 sun_bump = 0.50 * bump_factor * sun_lighting;

    vec3 reflected_sun_direction = reflect( -tangent_sun_direction, bump_direction );
    float sun_specularity = max( dot( tangent_camera_direction, reflected_sun_direction ), 0.0 );
    float material_specularity = texture2DArray( material_specular_map_array, texture_coordinates ).r;
    vec3 sun_specular = material_specularity * pow( sun_specularity, 16 ) * sun_lighting;

    vec3 color = texture_color.rgb * ( base_lighting + sun_bump ) + sun_specular;
    // FIXME: Making the distant geometry fade out via alpha looks nice, but it will
    //        do really weird stuff if, e.g., a player is inside a really tall house --
    //        the sky will show through the roof!
    float fog_factor = clamp( ( fog_distance - fog_depth ) * 0.20, 0.0, 1.0 );
    gl_FragColor = vec4( color, fog_factor * texture_color.a );
}
