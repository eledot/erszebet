/*
   Copyright (C) 2009 Serge "ftrvxmtrx" Ziryukin

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General
   Public License along with this library; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301 USA
*/

#ifndef _GL_EXT_H
#define _GL_EXT_H

#define GL_ADD_EXT_PROTO(cvar)                  \
    extern cvar_t *cvar;                        \
    extern bool ext_##cvar

GL_ADD_EXT_PROTO(gl_nv_fog_distance);
GL_ADD_EXT_PROTO(gl_ext_fog_coord);
GL_ADD_EXT_PROTO(gl_ext_texture_lod_bias);
GL_ADD_EXT_PROTO(gl_ext_texture_filter_anisotropic);
GL_ADD_EXT_PROTO(gl_arb_texture_compression);
GL_ADD_EXT_PROTO(gl_ext_texture_compression_latc);
GL_ADD_EXT_PROTO(gl_ext_texture_compression_rgtc);
GL_ADD_EXT_PROTO(gl_sgis_generate_mipmap);
GL_ADD_EXT_PROTO(gl_ext_polygon_offset);
GL_ADD_EXT_PROTO(gl_arb_vertex_shader);
GL_ADD_EXT_PROTO(gl_arb_vertex_program);
GL_ADD_EXT_PROTO(glx_sgi_swap_control);
GL_ADD_EXT_PROTO(gl_ext_texture3d);
GL_ADD_EXT_PROTO(gl_arb_texture_cube_map);
GL_ADD_EXT_PROTO(gl_arb_texture_env_add);
GL_ADD_EXT_PROTO(gl_arb_texture_env_combine);
GL_ADD_EXT_PROTO(gl_arb_texture_env_dot3);
GL_ADD_EXT_PROTO(gl_arb_texture_non_power_of_two);
GL_ADD_EXT_PROTO(gl_arb_point_parameters);
GL_ADD_EXT_PROTO(gl_arb_point_sprite);
GL_ADD_EXT_PROTO(gl_arb_occlusion_query);
GL_ADD_EXT_PROTO(gl_ext_clip_volume_hint);
GL_ADD_EXT_PROTO(gl_ext_blend_func_separate);
GL_ADD_EXT_PROTO(gl_ext_blend_logic_op);
GL_ADD_EXT_PROTO(gl_ext_blend_minmax);
GL_ADD_EXT_PROTO(gl_ext_blend_subtract);
GL_ADD_EXT_PROTO(gl_arb_depth_texture);

extern int gl_lod_bias_max;
extern GLfloat gl_anisotropy_max;
extern int gl_compressed_tex_num_formats;
extern int *gl_compressed_tex_formats;
extern int gl_max_texture_size;
extern int gl_max_texture_units;
extern int gl_texture3d_size_max;
extern int gl_texture_cube_map_size_max;
extern int gl_viewport_max[2];
extern int gl_max_clip_planes;

void gl_init_extensions (const char *ext);

#endif /* !_GL_EXT_H */
