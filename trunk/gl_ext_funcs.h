/*
   Copyright (C) 2009 Serge "ftrvxmtrx" Ziryukin

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General
   Public License along with this library; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301 USA
*/

#ifndef _GL_EXT_FUNCS_H
#define _GL_EXT_FUNCS_H

#ifndef GLEXTPRIVATE
#define GLEXTPRIVATE extern
#define GLEXTNOFUNCS
#endif /* !GLEXTPRIVATE */

/* gl_arb_texture_compression */
GLEXTPRIVATE void (*eglCompressedTexImage3DARB)    (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data);
GLEXTPRIVATE void (*eglCompressedTexImage2DARB)    (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
GLEXTPRIVATE void (*eglCompressedTexImage1DARB)    (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data);
GLEXTPRIVATE void (*eglCompressedTexSubImage3DARB) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data);
GLEXTPRIVATE void (*eglCompressedTexSubImage2DARB) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
GLEXTPRIVATE void (*eglCompressedTexSubImage1DARB) (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data);
GLEXTPRIVATE void (*eglGetCompressedTexImageARB)   (GLenum target, GLint level, GLvoid *img);

/* gl_ext_polygon_offset */
GLEXTPRIVATE void (*eglPolygonOffsetEXT) (GLfloat factor, GLfloat bias);

/* gl_arb_vertex_shader */
GLEXTPRIVATE void  (*eglBindAttribLocationARB) (GLhandleARB programObj, GLuint index, const GLcharARB *name);
GLEXTPRIVATE void  (*eglGetActiveAttribARB)    (GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei *length, GLint *size, GLenum *type, GLcharARB *name);
GLEXTPRIVATE GLint (*eglGetAttribLocationARB)  (GLhandleARB programObj, const GLcharARB *name);

/* gl_arb_vertex_program */
GLEXTPRIVATE void      (*eglVertexAttrib1dARB) (GLuint index, GLdouble x);
GLEXTPRIVATE void      (*eglVertexAttrib1dvARB) (GLuint index, const GLdouble *v);
GLEXTPRIVATE void      (*eglVertexAttrib1fARB) (GLuint index, GLfloat x);
GLEXTPRIVATE void      (*eglVertexAttrib1fvARB) (GLuint index, const GLfloat *v);
GLEXTPRIVATE void      (*eglVertexAttrib1sARB) (GLuint index, GLshort x);
GLEXTPRIVATE void      (*eglVertexAttrib1svARB) (GLuint index, const GLshort *v);
GLEXTPRIVATE void      (*eglVertexAttrib2dARB) (GLuint index, GLdouble x, GLdouble y);
GLEXTPRIVATE void      (*eglVertexAttrib2dvARB) (GLuint index, const GLdouble *v);
GLEXTPRIVATE void      (*eglVertexAttrib2fARB) (GLuint index, GLfloat x, GLfloat y);
GLEXTPRIVATE void      (*eglVertexAttrib2fvARB) (GLuint index, const GLfloat *v);
GLEXTPRIVATE void      (*eglVertexAttrib2sARB) (GLuint index, GLshort x, GLshort y);
GLEXTPRIVATE void      (*eglVertexAttrib2svARB) (GLuint index, const GLshort *v);
GLEXTPRIVATE void      (*eglVertexAttrib3dARB) (GLuint index, GLdouble x, GLdouble y, GLdouble z);
GLEXTPRIVATE void      (*eglVertexAttrib3dvARB) (GLuint index, const GLdouble *v);
GLEXTPRIVATE void      (*eglVertexAttrib3fARB) (GLuint index, GLfloat x, GLfloat y, GLfloat z);
GLEXTPRIVATE void      (*eglVertexAttrib3fvARB) (GLuint index, const GLfloat *v);
GLEXTPRIVATE void      (*eglVertexAttrib3sARB) (GLuint index, GLshort x, GLshort y, GLshort z);
GLEXTPRIVATE void      (*eglVertexAttrib3svARB) (GLuint index, const GLshort *v);
GLEXTPRIVATE void      (*eglVertexAttrib4NbvARB) (GLuint index, const GLbyte *v);
GLEXTPRIVATE void      (*eglVertexAttrib4NivARB) (GLuint index, const GLint *v);
GLEXTPRIVATE void      (*eglVertexAttrib4NsvARB) (GLuint index, const GLshort *v);
GLEXTPRIVATE void      (*eglVertexAttrib4NubARB) (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
GLEXTPRIVATE void      (*eglVertexAttrib4NubvARB) (GLuint index, const GLubyte *v);
GLEXTPRIVATE void      (*eglVertexAttrib4NuivARB) (GLuint index, const GLuint *v);
GLEXTPRIVATE void      (*eglVertexAttrib4NusvARB) (GLuint index, const GLushort *v);
GLEXTPRIVATE void      (*eglVertexAttrib4bvARB) (GLuint index, const GLbyte *v);
GLEXTPRIVATE void      (*eglVertexAttrib4dARB) (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
GLEXTPRIVATE void      (*eglVertexAttrib4dvARB) (GLuint index, const GLdouble *v);
GLEXTPRIVATE void      (*eglVertexAttrib4fARB) (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
GLEXTPRIVATE void      (*eglVertexAttrib4fvARB) (GLuint index, const GLfloat *v);
GLEXTPRIVATE void      (*eglVertexAttrib4ivARB) (GLuint index, const GLint *v);
GLEXTPRIVATE void      (*eglVertexAttrib4sARB) (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
GLEXTPRIVATE void      (*eglVertexAttrib4svARB) (GLuint index, const GLshort *v);
GLEXTPRIVATE void      (*eglVertexAttrib4ubvARB) (GLuint index, const GLubyte *v);
GLEXTPRIVATE void      (*eglVertexAttrib4uivARB) (GLuint index, const GLuint *v);
GLEXTPRIVATE void      (*eglVertexAttrib4usvARB) (GLuint index, const GLushort *v);
GLEXTPRIVATE void      (*eglVertexAttribPointerARB) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
GLEXTPRIVATE void      (*eglEnableVertexAttribArrayARB) (GLuint index);
GLEXTPRIVATE void      (*eglDisableVertexAttribArrayARB) (GLuint index);
GLEXTPRIVATE void      (*eglProgramStringARB) (GLenum target, GLenum format, GLsizei len, const GLvoid *string);
GLEXTPRIVATE void      (*eglBindProgramARB) (GLenum target, GLuint program);
GLEXTPRIVATE void      (*eglDeleteProgramsARB) (GLsizei n, const GLuint *programs);
GLEXTPRIVATE void      (*eglGenProgramsARB) (GLsizei n, GLuint *programs);
GLEXTPRIVATE void      (*eglProgramEnvParameter4dARB) (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
GLEXTPRIVATE void      (*eglProgramEnvParameter4dvARB) (GLenum target, GLuint index, const GLdouble *params);
GLEXTPRIVATE void      (*eglProgramEnvParameter4fARB) (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
GLEXTPRIVATE void      (*eglProgramEnvParameter4fvARB) (GLenum target, GLuint index, const GLfloat *params);
GLEXTPRIVATE void      (*eglProgramLocalParameter4dARB) (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
GLEXTPRIVATE void      (*eglProgramLocalParameter4dvARB) (GLenum target, GLuint index, const GLdouble *params);
GLEXTPRIVATE void      (*eglProgramLocalParameter4fARB) (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
GLEXTPRIVATE void      (*eglProgramLocalParameter4fvARB) (GLenum target, GLuint index, const GLfloat *params);
GLEXTPRIVATE void      (*eglGetProgramEnvParameterdvARB) (GLenum target, GLuint index, GLdouble *params);
GLEXTPRIVATE void      (*eglGetProgramEnvParameterfvARB) (GLenum target, GLuint index, GLfloat *params);
GLEXTPRIVATE void      (*eglGetProgramLocalParameterdvARB) (GLenum target, GLuint index, GLdouble *params);
GLEXTPRIVATE void      (*eglGetProgramLocalParameterfvARB) (GLenum target, GLuint index, GLfloat *params);
GLEXTPRIVATE void      (*eglGetProgramivARB) (GLenum target, GLenum pname, GLint *params);
GLEXTPRIVATE void      (*eglGetProgramStringARB) (GLenum target, GLenum pname, GLvoid *string);
GLEXTPRIVATE void      (*eglGetVertexAttribdvARB) (GLuint index, GLenum pname, GLdouble *params);
GLEXTPRIVATE void      (*eglGetVertexAttribfvARB) (GLuint index, GLenum pname, GLfloat *params);
GLEXTPRIVATE void      (*eglGetVertexAttribivARB) (GLuint index, GLenum pname, GLint *params);
GLEXTPRIVATE void      (*eglGetVertexAttribPointervARB) (GLuint index, GLenum pname, GLvoid* *pointer);
GLEXTPRIVATE GLboolean (*eglIsProgramARB) (GLuint program);

/* glx_sgi_swap_control */
GLEXTPRIVATE int (*eglXSwapIntervalSGI) (int);

/* gl_ext_texture3d */
GLEXTPRIVATE void      (*eglTexImage3DEXT) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
GLEXTPRIVATE void      (*eglTexSubImage3DEXT) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels);

/* gl_ext_fog_coord */
GLEXTPRIVATE void (*eglFogCoordfEXT) (GLfloat coord);
GLEXTPRIVATE void (*eglFogCoordfvEXT) (const GLfloat *coord);
GLEXTPRIVATE void (*eglFogCoorddEXT) (GLdouble coord);
GLEXTPRIVATE void (*eglFogCoorddvEXT) (const GLdouble *coord);
GLEXTPRIVATE void (*eglFogCoordPointerEXT) (GLenum type, GLsizei stride, const GLvoid *pointer);

/* gl_arb_point_parameters */
GLEXTPRIVATE void (*eglPointParameterfARB) (GLenum pname, GLfloat param);
GLEXTPRIVATE void (*eglPointParameterfvARB) (GLenum pname, const GLfloat *params);

/* gl_arb_occlusion_query */
GLEXTPRIVATE void      (*eglGenQueriesARB) (GLsizei n, GLuint *ids);
GLEXTPRIVATE void      (*eglDeleteQueriesARB) (GLsizei n, const GLuint *ids);
GLEXTPRIVATE GLboolean (*eglIsQueryARB) (GLuint id);
GLEXTPRIVATE void      (*eglBeginQueryARB) (GLenum target, GLuint id);
GLEXTPRIVATE void      (*eglEndQueryARB) (GLenum target);
GLEXTPRIVATE void      (*eglGetQueryivARB) (GLenum target, GLenum pname, GLint *params);
GLEXTPRIVATE void      (*eglGetQueryObjectivARB) (GLuint id, GLenum pname, GLint *params);
GLEXTPRIVATE void      (*eglGetQueryObjectuivARB) (GLuint id, GLenum pname, GLuint *params);

/* gl_ext_blend_func_separate */
GLEXTPRIVATE void (*eglBlendFuncSeparateEXT) (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);

/* gl_ext_blend_minmax */
GLEXTPRIVATE void (*eglBlendEquationEXT) (GLenum mode);

#ifndef GLEXTNOFUNCS

static lib_func_t gl_arb_texture_compression_funcs[] =
{
    { "glCompressedTexImage3DARB",    (void **)&eglCompressedTexImage3DARB    },
    { "glCompressedTexImage2DARB",    (void **)&eglCompressedTexImage2DARB    },
    { "glCompressedTexImage1DARB",    (void **)&eglCompressedTexImage1DARB    },
    { "glCompressedTexSubImage3DARB", (void **)&eglCompressedTexSubImage3DARB },
    { "glCompressedTexSubImage2DARB", (void **)&eglCompressedTexSubImage2DARB },
    { "glCompressedTexSubImage1DARB", (void **)&eglCompressedTexSubImage1DARB },
    { "glGetCompressedTexImageARB",   (void **)&eglGetCompressedTexImageARB   },
    { NULL, NULL }
};

static lib_func_t gl_ext_polygon_offset_funcs[] =
{
    { "glPolygonOffsetEXT", (void **)&eglPolygonOffsetEXT },
    { NULL, NULL }
};

static lib_func_t gl_arb_vertex_shader_funcs[] =
{
    { "glBindAttribLocationARB", (void **)&eglBindAttribLocationARB },
    { "glGetActiveAttribARB", (void **)&eglGetActiveAttribARB       },
    { "glGetAttribLocationARB", (void **)&eglGetAttribLocationARB   },
    { NULL, NULL }
};

static lib_func_t gl_arb_vertex_program_funcs[] =
{
    { "glVertexAttrib1dARB",             (void **)&eglVertexAttrib1dARB             },
    { "glVertexAttrib1dvARB",            (void **)&eglVertexAttrib1dvARB            },
    { "glVertexAttrib1fARB",             (void **)&eglVertexAttrib1fARB             },
    { "glVertexAttrib1fvARB",            (void **)&eglVertexAttrib1fvARB            },
    { "glVertexAttrib1sARB",             (void **)&eglVertexAttrib1sARB             },
    { "glVertexAttrib1svARB",            (void **)&eglVertexAttrib1svARB            },
    { "glVertexAttrib2dARB",             (void **)&eglVertexAttrib2dARB             },
    { "glVertexAttrib2dvARB",            (void **)&eglVertexAttrib2dvARB            },
    { "glVertexAttrib2fARB",             (void **)&eglVertexAttrib2fARB             },
    { "glVertexAttrib2fvARB",            (void **)&eglVertexAttrib2fvARB            },
    { "glVertexAttrib2sARB",             (void **)&eglVertexAttrib2sARB             },
    { "glVertexAttrib2svARB",            (void **)&eglVertexAttrib2svARB            },
    { "glVertexAttrib3dARB",             (void **)&eglVertexAttrib3dARB             },
    { "glVertexAttrib3dvARB",            (void **)&eglVertexAttrib3dvARB            },
    { "glVertexAttrib3fARB",             (void **)&eglVertexAttrib3fARB             },
    { "glVertexAttrib3fvARB",            (void **)&eglVertexAttrib3fvARB            },
    { "glVertexAttrib3sARB",             (void **)&eglVertexAttrib3sARB             },
    { "glVertexAttrib3svARB",            (void **)&eglVertexAttrib3svARB            },
    { "glVertexAttrib4NbvARB",           (void **)&eglVertexAttrib4NbvARB           },
    { "glVertexAttrib4NivARB",           (void **)&eglVertexAttrib4NivARB           },
    { "glVertexAttrib4NsvARB",           (void **)&eglVertexAttrib4NsvARB           },
    { "glVertexAttrib4NubARB",           (void **)&eglVertexAttrib4NubARB           },
    { "glVertexAttrib4NubvARB",          (void **)&eglVertexAttrib4NubvARB          },
    { "glVertexAttrib4NuivARB",          (void **)&eglVertexAttrib4NuivARB          },
    { "glVertexAttrib4NusvARB",          (void **)&eglVertexAttrib4NusvARB          },
    { "glVertexAttrib4bvARB",            (void **)&eglVertexAttrib4bvARB            },
    { "glVertexAttrib4dARB",             (void **)&eglVertexAttrib4dARB             },
    { "glVertexAttrib4dvARB",            (void **)&eglVertexAttrib4dvARB            },
    { "glVertexAttrib4fARB",             (void **)&eglVertexAttrib4fARB             },
    { "glVertexAttrib4fvARB",            (void **)&eglVertexAttrib4fvARB            },
    { "glVertexAttrib4ivARB",            (void **)&eglVertexAttrib4ivARB            },
    { "glVertexAttrib4sARB",             (void **)&eglVertexAttrib4sARB             },
    { "glVertexAttrib4svARB",            (void **)&eglVertexAttrib4svARB            },
    { "glVertexAttrib4ubvARB",           (void **)&eglVertexAttrib4ubvARB           },
    { "glVertexAttrib4uivARB",           (void **)&eglVertexAttrib4uivARB           },
    { "glVertexAttrib4usvARB",           (void **)&eglVertexAttrib4usvARB           },
    { "glVertexAttribPointerARB",        (void **)&eglVertexAttribPointerARB        },
    { "glEnableVertexAttribArrayARB",    (void **)&eglEnableVertexAttribArrayARB    },
    { "glDisableVertexAttribArrayARB",   (void **)&eglDisableVertexAttribArrayARB   },
    { "glProgramStringARB",              (void **)&eglProgramStringARB              },
    { "glBindProgramARB",                (void **)&eglBindProgramARB                },
    { "glDeleteProgramsARB",             (void **)&eglDeleteProgramsARB             },
    { "glGenProgramsARB",                (void **)&eglGenProgramsARB                },
    { "glProgramEnvParameter4dARB",      (void **)&eglProgramEnvParameter4dARB      },
    { "glProgramEnvParameter4dvARB",     (void **)&eglProgramEnvParameter4dvARB     },
    { "glProgramEnvParameter4fARB",      (void **)&eglProgramEnvParameter4fARB      },
    { "glProgramEnvParameter4fvARB",     (void **)&eglProgramEnvParameter4fvARB     },
    { "glProgramLocalParameter4dARB",    (void **)&eglProgramLocalParameter4dARB    },
    { "glProgramLocalParameter4dvARB",   (void **)&eglProgramLocalParameter4dvARB   },
    { "glProgramLocalParameter4fARB",    (void **)&eglProgramLocalParameter4fARB    },
    { "glProgramLocalParameter4fvARB",   (void **)&eglProgramLocalParameter4fvARB   },
    { "glGetProgramEnvParameterdvARB",   (void **)&eglGetProgramEnvParameterdvARB   },
    { "glGetProgramEnvParameterfvARB",   (void **)&eglGetProgramEnvParameterfvARB   },
    { "glGetProgramLocalParameterdvARB", (void **)&eglGetProgramLocalParameterdvARB },
    { "glGetProgramLocalParameterfvARB", (void **)&eglGetProgramLocalParameterfvARB },
    { "glGetProgramivARB",               (void **)&eglGetProgramivARB               },
    { "glGetProgramStringARB",           (void **)&eglGetProgramStringARB           },
    { "glGetVertexAttribdvARB",          (void **)&eglGetVertexAttribdvARB          },
    { "glGetVertexAttribfvARB",          (void **)&eglGetVertexAttribfvARB          },
    { "glGetVertexAttribivARB",          (void **)&eglGetVertexAttribivARB          },
    { "glGetVertexAttribPointervARB",    (void **)&eglGetVertexAttribPointervARB    },
    { "glIsProgramARB",                  (void **)&eglIsProgramARB                  },
    { NULL, NULL }
};

static lib_func_t glx_sgi_swap_control_funcs[] =
{
    { "glXSwapIntervalSGI", (void **)&eglXSwapIntervalSGI },
    { NULL, NULL }
};

static lib_func_t gl_ext_texture3d_funcs[] =
{
    { "glTexImage3DEXT",    (void **)&eglTexImage3DEXT    },
    { "glTexSubImage3DEXT", (void **)&eglTexSubImage3DEXT },
    { NULL, NULL }
};

static lib_func_t gl_ext_fog_coord_funcs[] =
{
    { "glFogCoordfEXT",       (void **)&eglFogCoordfEXT       },
    { "glFogCoordfvEXT",      (void **)&eglFogCoordfvEXT      },
    { "glFogCoorddEXT",       (void **)&eglFogCoorddEXT       },
    { "glFogCoorddvEXT",      (void **)&eglFogCoorddvEXT      },
    { "glFogCoordPointerEXT", (void **)&eglFogCoordPointerEXT },
    { NULL, NULL }
};

static lib_func_t gl_arb_point_parameters_funcs[] =
{
    { "glPointParameterfARB",  (void **)&eglPointParameterfARB  },
    { "glPointParameterfvARB", (void **)&eglPointParameterfvARB },
    { NULL, NULL }
};

static lib_func_t gl_arb_occlusion_query_funcs[] =
{
    { "glGenQueriesARB",        (void **)&eglGenQueriesARB        },
    { "glDeleteQueriesARB",     (void **)&eglDeleteQueriesARB     },
    { "glIsQueryARB",           (void **)&eglIsQueryARB           },
    { "glBeginQueryARB",        (void **)&eglBeginQueryARB        },
    { "glEndQueryARB",          (void **)&eglEndQueryARB          },
    { "glGetQueryivARB",        (void **)&eglGetQueryivARB        },
    { "glGetQueryObjectivARB",  (void **)&eglGetQueryObjectivARB  },
    { "glGetQueryObjectuivARB", (void **)&eglGetQueryObjectuivARB },
    { NULL, NULL }
};

static lib_func_t gl_ext_blend_func_separate_funcs[] =
{
    { "glBlendFuncSeparateEXT", (void **)&eglBlendFuncSeparateEXT },
    { NULL, NULL }
};

static lib_func_t gl_ext_blend_minmax_funcs[] =
{
    { "glBlendEquationEXT", (void **)&eglBlendEquationEXT },
    { NULL, NULL }
};

#endif /* !GLEXTNOFUNCS */

#endif /* !_GL_EXT_FUNCS_H */
