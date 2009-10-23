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

#define GLEXTPRIVATE
#include "gl_private.h"

#define GL_ADD_EXT(name, cvar, init)                                    \
    ext_##cvar = gl_add_extension(name, &cvar, #cvar, cvar##_funcs, init)

#define GL_ADD_EXT_VAR(cvar)                                   \
    cvar_t *cvar;                                              \
    bool ext_##cvar = false

#define GL_ADD_EXT_VAR2(cvar)                                   \
    GL_ADD_EXT_VAR(cvar);                                       \
    static libgl_func_t *cvar##_funcs = NULL

static const char *extensions;

GL_ADD_EXT_VAR2(gl_nv_fog_distance);
GL_ADD_EXT_VAR(gl_ext_fog_coord);
GL_ADD_EXT_VAR2(gl_ext_texture_lod_bias);
GL_ADD_EXT_VAR2(gl_ext_texture_filter_anisotropic);
GL_ADD_EXT_VAR(gl_arb_texture_compression);
GL_ADD_EXT_VAR2(gl_ext_texture_compression_latc);
GL_ADD_EXT_VAR2(gl_ext_texture_compression_rgtc);
GL_ADD_EXT_VAR2(gl_sgis_generate_mipmap);
GL_ADD_EXT_VAR(gl_ext_polygon_offset);
GL_ADD_EXT_VAR(gl_arb_vertex_shader);
GL_ADD_EXT_VAR(gl_arb_vertex_program);
GL_ADD_EXT_VAR(glx_sgi_swap_control);
GL_ADD_EXT_VAR(gl_ext_texture3d);
GL_ADD_EXT_VAR2(gl_arb_texture_cube_map);
GL_ADD_EXT_VAR2(gl_arb_texture_env_add);
GL_ADD_EXT_VAR2(gl_arb_texture_env_combine);
GL_ADD_EXT_VAR2(gl_arb_texture_env_dot3);
GL_ADD_EXT_VAR2(gl_arb_texture_non_power_of_two);
GL_ADD_EXT_VAR(gl_arb_point_parameters);
GL_ADD_EXT_VAR2(gl_arb_point_sprite);
GL_ADD_EXT_VAR(gl_arb_occlusion_query);
GL_ADD_EXT_VAR2(gl_ext_clip_volume_hint);
GL_ADD_EXT_VAR(gl_ext_blend_func_separate);
GL_ADD_EXT_VAR2(gl_ext_blend_logic_op);
GL_ADD_EXT_VAR(gl_ext_blend_minmax);
GL_ADD_EXT_VAR2(gl_ext_blend_subtract);
GL_ADD_EXT_VAR2(gl_arb_depth_texture);

int gl_lod_bias_max;
GLfloat gl_anisotropy_max;
int gl_compressed_tex_num_formats;
int *gl_compressed_tex_formats;
int gl_max_texture_size;
int gl_max_texture_units;
int gl_texture3d_size_max;
int gl_texture_cube_map_size_max;
int gl_viewport_max[2];
int gl_max_clip_planes;

/*
=================
gl_add_extension
=================
*/
static bool gl_add_extension (GNUC_UNUSED const char   *name,
                              GNUC_UNUSED cvar_t      **cvar,
                              GNUC_UNUSED const char   *cvar_name,
                              GNUC_UNUSED libgl_func_t *funcs,
                              GNUC_UNUSED bool        (*init_func) (bool ok))
{
#if 0
    const char *s;

    *cvar = cvar_get(cvar_name, "1", 0);

    if (NULL == *cvar || NULL == extensions)
        goto no;

    for (s = strstr(extensions, name); NULL != s ;s = strstr(extensions, name))
        if (' ' == s[strlen(s)] || '\0' == s[strlen(s)])
            break;

    if (NULL == s || (NULL != funcs && lib_get_funcs(libgl, funcs, 0)))
        goto no;

    sys_printf("+%s%s\n", name, !(*cvar)->i ? " (disabled)" : "");

    if (NULL != init_func)
        if (!init_func(true))
            return false;

    return true;

no:
    if (NULL != init_func)
        init_func(false);
#endif
    return false;
}

/*
=================
nvfog_init
=================
*/
static bool nvfog_init (bool ok)
{
    if (!ok)
        return true;

    return true;
}

/*
=================
lod_bias_get_max
=================
*/
static bool lod_bias_get_max (bool ok)
{
    if (!ok)
        return true;

#ifdef GL_MAX_TEXTURE_LOD_BIAS_EXT
    glGetIntegerv(GL_MAX_TEXTURE_LOD_BIAS_EXT, &gl_lod_bias_max);
    GLERROR();
#endif

    return true;
}

/*
=================
aniso_get_max
=================
*/
static bool aniso_get_max (bool ok)
{
    if (!ok)
        return true;

#ifdef GL_EXT_texture_filter_anisotropic
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &gl_anisotropy_max);
    GLERROR();
#endif

    return true;
}

/*
=================
compressed_init
=================
*/
static bool compressed_init (bool ok)
{
    if (!ok)
        return true;

#ifdef GL_ARB_texture_compression
    glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB, &gl_compressed_tex_num_formats);
    GLERROR();

    gl_compressed_tex_formats = mem_alloc_static(gl_compressed_tex_num_formats * sizeof(int));

    if (NULL == gl_compressed_tex_formats)
    {
        sys_printf("failed to allocate memory for compressed format enums\n");
        return false;
    }

    glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS_ARB, gl_compressed_tex_formats);
    GLERROR();
#endif

    return true;
}

/*
=================
gen_mipmap_init
=================
*/
static bool gen_mipmap_init (bool ok)
{
    if (!ok)
        return true;

#ifdef GL_SGIS_generate_mipmap
    glHint(GL_GENERATE_MIPMAP_HINT_SGIS, GL_NICEST);
    GLERROR();
#endif

    return true;
}

/*
=================
tex3d_init
=================
*/
static bool tex3d_init (bool ok)
{
    if (!ok)
        return true;

#ifdef GL_EXT_texture3D
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE_EXT, &gl_texture3d_size_max);
    GLERROR();
#endif

    return true;
}

/*
=================
texcubemap_init
=================
*/
static bool texcubemap_init (bool ok)
{
    if (!ok)
        return true;

#ifdef GL_ARB_texture_cube_map
    glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB, &gl_texture_cube_map_size_max);
    GLERROR();
#endif

    return true;
}

/*
=================
clipvol_init
=================
*/
static bool clipvol_init (bool ok)
{
    if (!ok)
        return true;

#ifdef GL_EXT_clip_volume_hint
    glHint(GL_CLIP_VOLUME_CLIPPING_HINT_EXT, GL_NICEST);
    GLERROR();
#endif

    return true;
}

/*
=================
gl_init_extensions
=================
*/
void gl_init_extensions (const char *ext)
{
    extensions = ext;

    if (NULL == extensions || NULL == mempool)
        return;

    GL_ADD_EXT("GL_ARB_depth_texture",              gl_arb_depth_texture,              NULL);
    GL_ADD_EXT("GL_ARB_occlusion_query",            gl_arb_occlusion_query,            NULL);
    GL_ADD_EXT("GL_ARB_point_parameters",           gl_arb_point_parameters,           NULL);
    GL_ADD_EXT("GL_ARB_point_sprite",               gl_arb_point_sprite,               NULL);
    GL_ADD_EXT("GL_ARB_texture_compression",        gl_arb_texture_compression,        &compressed_init);
    GL_ADD_EXT("GL_ARB_texture_cube_map",           gl_arb_texture_cube_map,           &texcubemap_init);
    GL_ADD_EXT("GL_ARB_texture_env_add",            gl_arb_texture_env_add,            NULL);
    GL_ADD_EXT("GL_ARB_texture_env_combine",        gl_arb_texture_env_combine,        NULL);
    GL_ADD_EXT("GL_ARB_texture_env_dot3",           gl_arb_texture_env_dot3,           NULL);
    GL_ADD_EXT("GL_ARB_texture_non_power_of_two",   gl_arb_texture_non_power_of_two,   NULL);
    GL_ADD_EXT("GL_ARB_vertex_program",             gl_arb_vertex_program,             NULL);
    GL_ADD_EXT("GL_ARB_vertex_shader",              gl_arb_vertex_shader,              NULL);
    GL_ADD_EXT("GL_EXT_blend_func_separate",        gl_ext_blend_func_separate,        NULL);
    GL_ADD_EXT("GL_EXT_blend_logic_op",             gl_ext_blend_logic_op,             NULL);
    GL_ADD_EXT("GL_EXT_blend_minmax",               gl_ext_blend_minmax,               NULL);
    GL_ADD_EXT("GL_EXT_blend_subtract",             gl_ext_blend_subtract,             NULL);
    GL_ADD_EXT("GL_EXT_clip_volume_hint",           gl_ext_clip_volume_hint,           &clipvol_init);
    GL_ADD_EXT("GL_EXT_fog_coord",                  gl_ext_fog_coord,                  NULL);
    GL_ADD_EXT("GL_EXT_polygon_offset",             gl_ext_polygon_offset,             NULL);
    GL_ADD_EXT("GL_EXT_texture_compression_latc",   gl_ext_texture_compression_latc,   NULL);
    GL_ADD_EXT("GL_EXT_texture_compression_rgtc",   gl_ext_texture_compression_rgtc,   NULL);
    GL_ADD_EXT("GL_EXT_texture_filter_anisotropic", gl_ext_texture_filter_anisotropic, &aniso_get_max);
    GL_ADD_EXT("GL_EXT_texture_lod_bias",           gl_ext_texture_lod_bias,           &lod_bias_get_max);
    GL_ADD_EXT("GL_EXT_texture3D",                  gl_ext_texture3d,                  &tex3d_init);
    GL_ADD_EXT("GL_NV_fog_distance",                gl_nv_fog_distance,                &nvfog_init);
    GL_ADD_EXT("GL_SGIS_generate_mipmap",           gl_sgis_generate_mipmap,           &gen_mipmap_init);
    GL_ADD_EXT("GLX_SGI_swap_control",              glx_sgi_swap_control,              NULL);

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &gl_max_texture_size);
    GLERROR();
    sys_printf("opengl max texture size: %i\n", gl_max_texture_size);

    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &gl_max_texture_units);
    GLERROR();
    sys_printf("opengl max texture units: %i\n", gl_max_texture_units);

    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, gl_viewport_max);
    GLERROR();
    sys_printf("opengl max viewport: %ix%i\n", gl_viewport_max[0], gl_viewport_max[1]);

    glGetIntegerv(GL_MAX_CLIP_PLANES, &gl_max_clip_planes);
    GLERROR();
    sys_printf("opengl max clip planes: %i\n", gl_max_clip_planes);
}
