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

/**
 * @file gl_funcs.h
 *
 * @brief OpenGL funcs header
 *
 * @author ftrvxmtrx
 */

#ifndef _GL_FUNCS_H
#define _GL_FUNCS_H

#ifndef GL_FALSE
#error gl.h not included
#endif

#ifndef GLPRIVATE
#define GLPRIVATE extern
#define GLNOFUNCS
#endif /* !GLPRIVATE */

GLPRIVATE void      (*eglClearIndex)        (GLfloat c);
GLPRIVATE void      (*eglClearColor)        (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
GLPRIVATE void      (*eglClear)             (GLbitfield mask);
GLPRIVATE void      (*eglIndexMask)         (GLuint mask);
GLPRIVATE void      (*eglColorMask)         (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
GLPRIVATE void      (*eglAlphaFunc)         (GLenum func, GLclampf ref);
GLPRIVATE void      (*eglBlendFunc)         (GLenum sfactor, GLenum dfactor);
GLPRIVATE void      (*eglLogicOp)           (GLenum opcode);
GLPRIVATE void      (*eglCullFace)          (GLenum mode);
GLPRIVATE void      (*eglFrontFace)         (GLenum mode);
GLPRIVATE void      (*eglPointSize)         (GLfloat size);
GLPRIVATE void      (*eglLineWidth)         (GLfloat width);
GLPRIVATE void      (*eglLineStipple)       (GLint factor, GLushort pattern);
GLPRIVATE void      (*eglPolygonMode)       (GLenum face, GLenum mode);
GLPRIVATE void      (*eglPolygonOffset)     (GLfloat factor, GLfloat units);
GLPRIVATE void      (*eglPolygonStipple)    (const GLubyte *mask);
GLPRIVATE void      (*eglGetPolygonStipple) (GLubyte *mask);
GLPRIVATE void      (*eglEdgeFlag)          (GLboolean flag);
GLPRIVATE void      (*eglEdgeFlagv)         (const GLboolean *flag);
GLPRIVATE void      (*eglScissor)           (GLint x, GLint y, GLsizei width, GLsizei height);
GLPRIVATE void      (*eglClipPlane)         (GLenum plane, const GLdouble *equation);
GLPRIVATE void      (*eglGetClipPlane)      (GLenum plane, GLdouble *equation);
GLPRIVATE void      (*eglDrawBuffer)        (GLenum mode);
GLPRIVATE void      (*eglReadBuffer)        (GLenum mode);
GLPRIVATE void      (*eglEnable_)           (GLenum cap);
GLPRIVATE void      (*eglDisable_)          (GLenum cap);
GLPRIVATE GLboolean (*eglIsEnabled)         (GLenum cap);
GLPRIVATE void      (*eglGetBooleanv)       (GLenum pname, GLboolean *params);
GLPRIVATE void      (*eglGetDoublev)        (GLenum pname, GLdouble *params);
GLPRIVATE void      (*eglGetFloatv)         (GLenum pname, GLfloat *params);
GLPRIVATE void      (*eglGetIntegerv)       (GLenum pname, GLint *params);
GLPRIVATE void      (*eglPushAttrib)        (GLbitfield mask);
GLPRIVATE void      (*eglPopAttrib)         (void);
GLPRIVATE GLint     (*eglRenderMode)        (GLenum mode);

GLPRIVATE GLenum          (*eglGetError)  (void);
GLPRIVATE const GLubyte * (*eglGetString) (GLenum name);

GLPRIVATE void      (*eglFinish)       (void);
GLPRIVATE void      (*eglFlush)        (void);
GLPRIVATE void      (*eglHint)         (GLenum target, GLenum mode);
GLPRIVATE void      (*eglClearDepth)   (GLclampd depth);
GLPRIVATE void      (*eglDepthFunc)    (GLenum func);
GLPRIVATE void      (*eglDepthMask_)   (GLboolean flag);
GLPRIVATE void      (*eglDepthRange)   (GLclampd near_val, GLclampd far_val);
GLPRIVATE void      (*eglClearAccum)   (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
GLPRIVATE void      (*eglAccum)        (GLenum op, GLfloat value);
GLPRIVATE void      (*eglMatrixMode)   (GLenum mode);
GLPRIVATE void      (*eglOrtho)        (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);
GLPRIVATE void      (*eglFrustum)      (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);
GLPRIVATE void      (*eglViewport)     (GLint x, GLint y, GLsizei width, GLsizei height);
GLPRIVATE void      (*eglPushMatrix)   (void);
GLPRIVATE void      (*eglPopMatrix)    (void);
GLPRIVATE void      (*eglLoadIdentity) (void);
GLPRIVATE void      (*eglLoadMatrixd)  (const GLdouble *m);
GLPRIVATE void      (*eglLoadMatrixf)  (const GLfloat *m);
GLPRIVATE void      (*eglMultMatrixd)  (const GLdouble *m);
GLPRIVATE void      (*eglMultMatrixf)  (const GLfloat *m);
GLPRIVATE void      (*eglRotated)      (GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
GLPRIVATE void      (*eglRotatef)      (GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
GLPRIVATE void      (*eglScaled)       (GLdouble x, GLdouble y, GLdouble z);
GLPRIVATE void      (*eglScalef)       (GLfloat x, GLfloat y, GLfloat z);
GLPRIVATE void      (*eglTranslated)   (GLdouble x, GLdouble y, GLdouble z);
GLPRIVATE void      (*eglTranslatef)   (GLfloat x, GLfloat y, GLfloat z);
GLPRIVATE GLboolean (*eglIsList)       (GLuint list);
GLPRIVATE void      (*eglDeleteLists)  (GLuint list, GLsizei range);
GLPRIVATE GLuint    (*eglGenLists)     (GLsizei range);
GLPRIVATE void      (*eglNewList)      (GLuint list, GLenum mode);
GLPRIVATE void      (*eglEndList)      (void);
GLPRIVATE void      (*eglCallList)     (GLuint list);
GLPRIVATE void      (*eglCallLists)    (GLsizei n, GLenum type, const GLvoid *lists);
GLPRIVATE void      (*eglListBase)     (GLuint base);
GLPRIVATE void      (*eglBegin)        (GLenum mode);
GLPRIVATE void      (*eglEnd)          (void);

GLPRIVATE void (*eglVertex2d)  (GLdouble x, GLdouble y);
GLPRIVATE void (*eglVertex2f)  (GLfloat x, GLfloat y);
GLPRIVATE void (*eglVertex2i)  (GLint x, GLint y);
GLPRIVATE void (*eglVertex2s)  (GLshort x, GLshort y);
GLPRIVATE void (*eglVertex3d)  (GLdouble x, GLdouble y, GLdouble z);
GLPRIVATE void (*eglVertex3f)  (GLfloat x, GLfloat y, GLfloat z);
GLPRIVATE void (*eglVertex3i)  (GLint x, GLint y, GLint z);
GLPRIVATE void (*eglVertex3s)  (GLshort x, GLshort y, GLshort z);
GLPRIVATE void (*eglVertex4d)  (GLdouble x, GLdouble y, GLdouble z, GLdouble w);
GLPRIVATE void (*eglVertex4f)  (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
GLPRIVATE void (*eglVertex4i)  (GLint x, GLint y, GLint z, GLint w);
GLPRIVATE void (*eglVertex4s)  (GLshort x, GLshort y, GLshort z, GLshort w);
GLPRIVATE void (*eglVertex2dv) (const GLdouble *v);
GLPRIVATE void (*eglVertex2fv) (const GLfloat *v);
GLPRIVATE void (*eglVertex2iv) (const GLint *v);
GLPRIVATE void (*eglVertex2sv) (const GLshort *v);
GLPRIVATE void (*eglVertex3dv) (const GLdouble *v);
GLPRIVATE void (*eglVertex3fv) (const GLfloat *v);
GLPRIVATE void (*eglVertex3iv) (const GLint *v);
GLPRIVATE void (*eglVertex3sv) (const GLshort *v);
GLPRIVATE void (*eglVertex4dv) (const GLdouble *v);
GLPRIVATE void (*eglVertex4fv) (const GLfloat *v);
GLPRIVATE void (*eglVertex4iv) (const GLint *v);
GLPRIVATE void (*eglVertex4sv) (const GLshort *v);
GLPRIVATE void (*eglNormal3b)  (GLbyte nx, GLbyte ny, GLbyte nz);
GLPRIVATE void (*eglNormal3d)  (GLdouble nx, GLdouble ny, GLdouble nz);
GLPRIVATE void (*eglNormal3f)  (GLfloat nx, GLfloat ny, GLfloat nz);
GLPRIVATE void (*eglNormal3i)  (GLint nx, GLint ny, GLint nz);
GLPRIVATE void (*eglNormal3s)  (GLshort nx, GLshort ny, GLshort nz);
GLPRIVATE void (*eglNormal3bv) (const GLbyte *v);
GLPRIVATE void (*eglNormal3dv) (const GLdouble *v);
GLPRIVATE void (*eglNormal3fv) (const GLfloat *v);
GLPRIVATE void (*eglNormal3iv) (const GLint *v);
GLPRIVATE void (*eglNormal3sv) (const GLshort *v);
GLPRIVATE void (*eglIndexd)    (GLdouble c);
GLPRIVATE void (*eglIndexf)    (GLfloat c);
GLPRIVATE void (*eglIndexi)    (GLint c);
GLPRIVATE void (*eglIndexs)    (GLshort c);
GLPRIVATE void (*eglIndexdv)   (const GLdouble *c);
GLPRIVATE void (*eglIndexfv)   (const GLfloat *c);
GLPRIVATE void (*eglIndexiv)   (const GLint *c);
GLPRIVATE void (*eglIndexsv)   (const GLshort *c);
GLPRIVATE void (*eglColor3b)   (GLbyte red, GLbyte green, GLbyte blue);
GLPRIVATE void (*eglColor3d)   (GLdouble red, GLdouble green, GLdouble blue);
GLPRIVATE void (*eglColor3f_)  (GLfloat red, GLfloat green, GLfloat blue);
GLPRIVATE void (*eglColor3i)   (GLint red, GLint green, GLint blue);
GLPRIVATE void (*eglColor3s)   (GLshort red, GLshort green, GLshort blue);
GLPRIVATE void (*eglColor3ub)  (GLubyte red, GLubyte green, GLubyte blue);
GLPRIVATE void (*eglColor3ui)  (GLuint red, GLuint green, GLuint blue);
GLPRIVATE void (*eglColor3us)  (GLushort red, GLushort green, GLushort blue);
GLPRIVATE void (*eglColor4b)   (GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha);
GLPRIVATE void (*eglColor4d)   (GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
GLPRIVATE void (*eglColor4f_)  (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
GLPRIVATE void (*eglColor4i)   (GLint red, GLint green, GLint blue, GLint alpha);
GLPRIVATE void (*eglColor4s)   (GLshort red, GLshort green, GLshort blue, GLshort alpha);
GLPRIVATE void (*eglColor4ub)  (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
GLPRIVATE void (*eglColor4ui)  (GLuint red, GLuint green, GLuint blue, GLuint alpha);
GLPRIVATE void (*eglColor4us)  (GLushort red, GLushort green, GLushort blue, GLushort alpha);
GLPRIVATE void (*eglColor3bv)  (const GLbyte *v);
GLPRIVATE void (*eglColor3dv)  (const GLdouble *v);
GLPRIVATE void (*eglColor3fv)  (const GLfloat *v);
GLPRIVATE void (*eglColor3iv)  (const GLint *v);
GLPRIVATE void (*eglColor3sv)  (const GLshort *v);
GLPRIVATE void (*eglColor3ubv) (const GLubyte *v);
GLPRIVATE void (*eglColor3uiv) (const GLuint *v);
GLPRIVATE void (*eglColor3usv) (const GLushort *v);
GLPRIVATE void (*eglColor4bv)  (const GLbyte *v);
GLPRIVATE void (*eglColor4dv)  (const GLdouble *v);
GLPRIVATE void (*eglColor4fv)  (const GLfloat *v);
GLPRIVATE void (*eglColor4iv)  (const GLint *v);
GLPRIVATE void (*eglColor4sv)  (const GLshort *v);
GLPRIVATE void (*eglColor4ubv) (const GLubyte *v);
GLPRIVATE void (*eglColor4uiv) (const GLuint *v);
GLPRIVATE void (*eglColor4usv) (const GLushort *v);

GLPRIVATE void (*eglTexCoord1d)  (GLdouble s);
GLPRIVATE void (*eglTexCoord1f)  (GLfloat s);
GLPRIVATE void (*eglTexCoord1i)  (GLint s);
GLPRIVATE void (*eglTexCoord1s)  (GLshort s);
GLPRIVATE void (*eglTexCoord2d)  (GLdouble s, GLdouble t);
GLPRIVATE void (*eglTexCoord2f)  (GLfloat s, GLfloat t);
GLPRIVATE void (*eglTexCoord2i)  (GLint s, GLint t);
GLPRIVATE void (*eglTexCoord2s)  (GLshort s, GLshort t);
GLPRIVATE void (*eglTexCoord3d)  (GLdouble s, GLdouble t, GLdouble r);
GLPRIVATE void (*eglTexCoord3f)  (GLfloat s, GLfloat t, GLfloat r);
GLPRIVATE void (*eglTexCoord3i)  (GLint s, GLint t, GLint r);
GLPRIVATE void (*eglTexCoord3s)  (GLshort s, GLshort t, GLshort r);
GLPRIVATE void (*eglTexCoord4d)  (GLdouble s, GLdouble t, GLdouble r, GLdouble q);
GLPRIVATE void (*eglTexCoord4f)  (GLfloat s, GLfloat t, GLfloat r, GLfloat q);
GLPRIVATE void (*eglTexCoord4i)  (GLint s, GLint t, GLint r, GLint q);
GLPRIVATE void (*eglTexCoord4s)  (GLshort s, GLshort t, GLshort r, GLshort q);
GLPRIVATE void (*eglTexCoord1dv) (const GLdouble *v);
GLPRIVATE void (*eglTexCoord1fv) (const GLfloat *v);
GLPRIVATE void (*eglTexCoord1iv) (const GLint *v);
GLPRIVATE void (*eglTexCoord1sv) (const GLshort *v);
GLPRIVATE void (*eglTexCoord2dv) (const GLdouble *v);
GLPRIVATE void (*eglTexCoord2fv) (const GLfloat *v);
GLPRIVATE void (*eglTexCoord2iv) (const GLint *v);
GLPRIVATE void (*eglTexCoord2sv) (const GLshort *v);
GLPRIVATE void (*eglTexCoord3dv) (const GLdouble *v);
GLPRIVATE void (*eglTexCoord3fv) (const GLfloat *v);
GLPRIVATE void (*eglTexCoord3iv) (const GLint *v);
GLPRIVATE void (*eglTexCoord3sv) (const GLshort *v);
GLPRIVATE void (*eglTexCoord4dv) (const GLdouble *v);
GLPRIVATE void (*eglTexCoord4fv) (const GLfloat *v);
GLPRIVATE void (*eglTexCoord4iv) (const GLint *v);
GLPRIVATE void (*eglTexCoord4sv) (const GLshort *v);

GLPRIVATE void (*eglRasterPos2d)  (GLdouble x, GLdouble y);
GLPRIVATE void (*eglRasterPos2f)  (GLfloat x, GLfloat y);
GLPRIVATE void (*eglRasterPos2i)  (GLint x, GLint y);
GLPRIVATE void (*eglRasterPos2s)  (GLshort x, GLshort y);
GLPRIVATE void (*eglRasterPos3d)  (GLdouble x, GLdouble y, GLdouble z);
GLPRIVATE void (*eglRasterPos3f)  (GLfloat x, GLfloat y, GLfloat z);
GLPRIVATE void (*eglRasterPos3i)  (GLint x, GLint y, GLint z);
GLPRIVATE void (*eglRasterPos3s)  (GLshort x, GLshort y, GLshort z);
GLPRIVATE void (*eglRasterPos4d)  (GLdouble x, GLdouble y, GLdouble z, GLdouble w);
GLPRIVATE void (*eglRasterPos4f)  (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
GLPRIVATE void (*eglRasterPos4i)  (GLint x, GLint y, GLint z, GLint w);
GLPRIVATE void (*eglRasterPos4s)  (GLshort x, GLshort y, GLshort z, GLshort w);
GLPRIVATE void (*eglRasterPos2dv) (const GLdouble *v);
GLPRIVATE void (*eglRasterPos2fv) (const GLfloat *v);
GLPRIVATE void (*eglRasterPos2iv) (const GLint *v);
GLPRIVATE void (*eglRasterPos2sv) (const GLshort *v);
GLPRIVATE void (*eglRasterPos3dv) (const GLdouble *v);
GLPRIVATE void (*eglRasterPos3fv) (const GLfloat *v);
GLPRIVATE void (*eglRasterPos3iv) (const GLint *v);
GLPRIVATE void (*eglRasterPos3sv) (const GLshort *v);
GLPRIVATE void (*eglRasterPos4dv) (const GLdouble *v);
GLPRIVATE void (*eglRasterPos4fv) (const GLfloat *v);
GLPRIVATE void (*eglRasterPos4iv) (const GLint *v);
GLPRIVATE void (*eglRasterPos4sv) (const GLshort *v);

GLPRIVATE void (*eglRectd)  (GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
GLPRIVATE void (*eglRectf)  (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
GLPRIVATE void (*eglRecti)  (GLint x1, GLint y1, GLint x2, GLint y2);
GLPRIVATE void (*eglRects)  (GLshort x1, GLshort y1, GLshort x2, GLshort y2);
GLPRIVATE void (*eglRectdv) (const GLdouble *v1, const GLdouble *v2);
GLPRIVATE void (*eglRectfv) (const GLfloat *v1, const GLfloat *v2);
GLPRIVATE void (*eglRectiv) (const GLint *v1, const GLint *v2);
GLPRIVATE void (*eglRectsv) (const GLshort *v1, const GLshort *v2);

GLPRIVATE void (*eglShadeModel)     (GLenum mode);
GLPRIVATE void (*eglLightf)         (GLenum light, GLenum pname, GLfloat param);
GLPRIVATE void (*eglLighti)         (GLenum light, GLenum pname, GLint param);
GLPRIVATE void (*eglLightfv)        (GLenum light, GLenum pname, const GLfloat *params);
GLPRIVATE void (*eglLightiv)        (GLenum light, GLenum pname, const GLint *params);
GLPRIVATE void (*eglGetLightfv)     (GLenum light, GLenum pname, GLfloat *params);
GLPRIVATE void (*eglGetLightiv)     (GLenum light, GLenum pname, GLint *params);
GLPRIVATE void (*eglLightModelf)    (GLenum pname, GLfloat param);
GLPRIVATE void (*eglLightModeli)    (GLenum pname, GLint param);
GLPRIVATE void (*eglLightModelfv)   (GLenum pname, const GLfloat *params);
GLPRIVATE void (*eglLightModeliv)   (GLenum pname, const GLint *params);
GLPRIVATE void (*eglMaterialf)      (GLenum face, GLenum pname, GLfloat param);
GLPRIVATE void (*eglMateriali)      (GLenum face, GLenum pname, GLint param);
GLPRIVATE void (*eglMaterialfv)     (GLenum face, GLenum pname, const GLfloat *params);
GLPRIVATE void (*eglMaterialiv)     (GLenum face, GLenum pname, const GLint *params);
GLPRIVATE void (*eglGetMaterialfv)  (GLenum face, GLenum pname, GLfloat *params);
GLPRIVATE void (*eglGetMaterialiv)  (GLenum face, GLenum pname, GLint *params);
GLPRIVATE void (*eglColorMaterial)  (GLenum face, GLenum mode);

GLPRIVATE void (*eglPixelZoom)      (GLfloat xfactor, GLfloat yfactor);
GLPRIVATE void (*eglPixelStoref)    (GLenum pname, GLfloat param);
GLPRIVATE void (*eglPixelStorei)    (GLenum pname, GLint param);
GLPRIVATE void (*eglPixelTransferf) (GLenum pname, GLfloat param);
GLPRIVATE void (*eglPixelTransferi) (GLenum pname, GLint param);
GLPRIVATE void (*eglPixelMapfv)     (GLenum map, GLsizei mapsize, const GLfloat *values);
GLPRIVATE void (*eglPixelMapuiv)    (GLenum map, GLsizei mapsize, const GLuint *values);
GLPRIVATE void (*eglPixelMapusv)    (GLenum map, GLsizei mapsize, const GLushort *values);
GLPRIVATE void (*eglGetPixelMapfv)  (GLenum map, GLfloat *values);
GLPRIVATE void (*eglGetPixelMapuiv) (GLenum map, GLuint *values);
GLPRIVATE void (*eglGetPixelMapusv) (GLenum map, GLushort *values);
GLPRIVATE void (*eglBitmap)         (GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap);
GLPRIVATE void (*eglReadPixels)     (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
GLPRIVATE void (*eglDrawPixels)     (GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
GLPRIVATE void (*eglCopyPixels)     (GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
GLPRIVATE void (*eglStencilFunc)    (GLenum func, GLint ref, GLuint mask);
GLPRIVATE void (*eglStencilMask)    (GLuint mask);
GLPRIVATE void (*eglStencilOp)      (GLenum fail, GLenum zfail, GLenum zpass);
GLPRIVATE void (*eglClearStencil)   (GLint s);

GLPRIVATE void (*eglTexGend)                (GLenum coord, GLenum pname, GLdouble param);
GLPRIVATE void (*eglTexGenf)                (GLenum coord, GLenum pname, GLfloat param);
GLPRIVATE void (*eglTexGeni)                (GLenum coord, GLenum pname, GLint param);
GLPRIVATE void (*eglTexGendv)               (GLenum coord, GLenum pname, const GLdouble *params);
GLPRIVATE void (*eglTexGenfv)               (GLenum coord, GLenum pname, const GLfloat *params);
GLPRIVATE void (*eglTexGeniv)               (GLenum coord, GLenum pname, const GLint *params);
GLPRIVATE void (*eglGetTexGendv)            (GLenum coord, GLenum pname, GLdouble *params);
GLPRIVATE void (*eglGetTexGenfv)            (GLenum coord, GLenum pname, GLfloat *params);
GLPRIVATE void (*eglGetTexGeniv)            (GLenum coord, GLenum pname, GLint *params);
GLPRIVATE void (*eglTexEnvf)                (GLenum target, GLenum pname, GLfloat param);
GLPRIVATE void (*eglTexEnvi)                (GLenum target, GLenum pname, GLint param);
GLPRIVATE void (*eglTexEnvfv)               (GLenum target, GLenum pname, const GLfloat *params);
GLPRIVATE void (*eglTexEnviv)               (GLenum target, GLenum pname, const GLint *params);
GLPRIVATE void (*eglGetTexEnvfv)            (GLenum target, GLenum pname, GLfloat *params);
GLPRIVATE void (*eglGetTexEnviv)            (GLenum target, GLenum pname, GLint *params);
GLPRIVATE void (*eglTexParameterf)          (GLenum target, GLenum pname, GLfloat param);
GLPRIVATE void (*eglTexParameteri)          (GLenum target, GLenum pname, GLint param);
GLPRIVATE void (*eglTexParameterfv)         (GLenum target, GLenum pname, const GLfloat *params);
GLPRIVATE void (*eglTexParameteriv)         (GLenum target, GLenum pname, const GLint *params);
GLPRIVATE void (*eglGetTexParameterfv)      (GLenum target, GLenum pname, GLfloat *params);
GLPRIVATE void (*eglGetTexParameteriv)      (GLenum target, GLenum pname, GLint *params);
GLPRIVATE void (*eglGetTexLevelParameterfv) (GLenum target, GLint level, GLenum pname, GLfloat *params);
GLPRIVATE void (*eglGetTexLevelParameteriv) (GLenum target, GLint level, GLenum pname, GLint *params);
GLPRIVATE void (*eglTexImage1D)             (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
GLPRIVATE void (*eglTexImage2D)             (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
GLPRIVATE void (*eglGetTexImage)            (GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels);
GLPRIVATE void (*eglEnableClientState)      (GLenum cap);
GLPRIVATE void (*eglDisableClientState)     (GLenum cap);
GLPRIVATE void (*eglPushClientAttrib)       (GLbitfield mask);
GLPRIVATE void (*eglPopClientAttrib)        (void);
GLPRIVATE void (*eglIndexub)                (GLubyte c);
GLPRIVATE void (*eglIndexubv)               (const GLubyte *c);

GLPRIVATE void      (*eglGenTextures)         (GLsizei n, GLuint *textures);
GLPRIVATE void      (*eglDeleteTextures)      (GLsizei n, const GLuint *textures);
GLPRIVATE void      (*eglBindTexture_)        (GLenum target, GLuint texture);
GLPRIVATE void      (*eglPrioritizeTextures)  (GLsizei n, const GLuint *textures, const GLclampf *priorities);
GLPRIVATE GLboolean (*eglAreTexturesResident) (GLsizei n, const GLuint *textures, GLboolean *residences);
GLPRIVATE GLboolean (*eglIsTexture)           (GLuint texture);
GLPRIVATE void      (*eglTexSubImage1D)       (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels);
GLPRIVATE void      (*eglTexSubImage2D)       (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
GLPRIVATE void      (*eglCopyTexImage1D)      (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
GLPRIVATE void      (*eglCopyTexImage2D)      (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
GLPRIVATE void      (*eglCopyTexSubImage1D)   (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
GLPRIVATE void      (*eglCopyTexSubImage2D)   (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
GLPRIVATE void      (*eglVertexPointer)       (GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
GLPRIVATE void      (*eglNormalPointer)       (GLenum type, GLsizei stride, const GLvoid *ptr);
GLPRIVATE void      (*eglColorPointer)        (GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
GLPRIVATE void      (*eglIndexPointer)        (GLenum type, GLsizei stride, const GLvoid *ptr);
GLPRIVATE void      (*eglTexCoordPointer)     (GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
GLPRIVATE void      (*eglEdgeFlagPointer)     (GLsizei stride, const GLvoid *ptr);
GLPRIVATE void      (*eglGetPointerv)         (GLenum pname, GLvoid **params);
GLPRIVATE void      (*eglArrayElement)        (GLint i);
GLPRIVATE void      (*eglDrawArrays)          (GLenum mode, GLint first, GLsizei count);
GLPRIVATE void      (*eglDrawElements)        (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
GLPRIVATE void      (*eglInterleavedArrays)   (GLenum format, GLsizei stride, const GLvoid *pointer);
GLPRIVATE void      (*eglMap1d)               (GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points);
GLPRIVATE void      (*eglMap1f)               (GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points);
GLPRIVATE void      (*eglMap2d)               (GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points);
GLPRIVATE void      (*eglMap2f)               (GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points);
GLPRIVATE void      (*eglGetMapdv)            (GLenum target, GLenum query, GLdouble *v);
GLPRIVATE void      (*eglGetMapfv)            (GLenum target, GLenum query, GLfloat *v);
GLPRIVATE void      (*eglGetMapiv)            (GLenum target, GLenum query, GLint *v);

GLPRIVATE void (*eglEvalCoord1d)    (GLdouble u);
GLPRIVATE void (*eglEvalCoord1f)    (GLfloat u);
GLPRIVATE void (*eglEvalCoord1dv)   (const GLdouble *u);
GLPRIVATE void (*eglEvalCoord1fv)   (const GLfloat *u);
GLPRIVATE void (*eglEvalCoord2d)    (GLdouble u, GLdouble v);
GLPRIVATE void (*eglEvalCoord2f)    (GLfloat u, GLfloat v);
GLPRIVATE void (*eglEvalCoord2dv)   (const GLdouble *u);
GLPRIVATE void (*eglEvalCoord2fv)   (const GLfloat *u);
GLPRIVATE void (*eglMapGrid1d)      (GLint un, GLdouble u1, GLdouble u2);
GLPRIVATE void (*eglMapGrid1f)      (GLint un, GLfloat u1, GLfloat u2);
GLPRIVATE void (*eglMapGrid2d)      (GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2);
GLPRIVATE void (*eglMapGrid2f)      (GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2);
GLPRIVATE void (*eglEvalPoint1)     (GLint i);
GLPRIVATE void (*eglEvalPoint2)     (GLint i, GLint j);
GLPRIVATE void (*eglEvalMesh1)      (GLenum mode, GLint i1, GLint i2);
GLPRIVATE void (*eglEvalMesh2)      (GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2);
GLPRIVATE void (*eglFogf)           (GLenum pname, GLfloat param);
GLPRIVATE void (*eglFogi)           (GLenum pname, GLint param);
GLPRIVATE void (*eglFogfv)          (GLenum pname, const GLfloat *params);
GLPRIVATE void (*eglFogiv)          (GLenum pname, const GLint *params);
GLPRIVATE void (*eglFeedbackBuffer) (GLsizei size, GLenum type, GLfloat *buffer);
GLPRIVATE void (*eglPassThrough)    (GLfloat token);
GLPRIVATE void (*eglSelectBuffer)   (GLsizei size, GLuint *buffer);
GLPRIVATE void (*eglInitNames)      (void);
GLPRIVATE void (*eglLoadName)       (GLuint name);
GLPRIVATE void (*eglPushName)       (GLuint name);
GLPRIVATE void (*eglPopName)        (void);

GLPRIVATE void (*eglDrawRangeElements)         (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices);
GLPRIVATE void (*eglTexImage3D)                (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
GLPRIVATE void (*eglTexSubImage3D)             (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels);
GLPRIVATE void (*eglCopyTexSubImage3D)         (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
GLPRIVATE void (*eglColorTable)                (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table);
GLPRIVATE void (*eglColorSubTable)             (GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data);
GLPRIVATE void (*eglColorTableParameteriv)     (GLenum target, GLenum pname, const GLint *params);
GLPRIVATE void (*eglColorTableParameterfv)     (GLenum target, GLenum pname, const GLfloat *params);
GLPRIVATE void (*eglCopyColorSubTable)         (GLenum target, GLsizei start, GLint x, GLint y, GLsizei width);
GLPRIVATE void (*eglCopyColorTable)            (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width);
GLPRIVATE void (*eglGetColorTable)             (GLenum target, GLenum format, GLenum type, GLvoid *table);
GLPRIVATE void (*eglGetColorTableParameterfv)  (GLenum target, GLenum pname, GLfloat *params);
GLPRIVATE void (*eglGetColorTableParameteriv)  (GLenum target, GLenum pname, GLint *params);
GLPRIVATE void (*eglBlendEquation)             (GLenum mode);
GLPRIVATE void (*eglBlendColor)                (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
GLPRIVATE void (*eglHistogram)                 (GLenum target, GLsizei width, GLenum internalformat, GLboolean sink);
GLPRIVATE void (*eglResetHistogram)            (GLenum target);
GLPRIVATE void (*eglGetHistogram)              (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values);
GLPRIVATE void (*eglGetHistogramParameterfv)   (GLenum target, GLenum pname, GLfloat *params);
GLPRIVATE void (*eglGetHistogramParameteriv)   (GLenum target, GLenum pname, GLint *params);
GLPRIVATE void (*eglMinmax)                    (GLenum target, GLenum internalformat, GLboolean sink);
GLPRIVATE void (*eglResetMinmax)               (GLenum target);
GLPRIVATE void (*eglGetMinmax)                 (GLenum target, GLboolean reset, GLenum format, GLenum types, GLvoid *values);
GLPRIVATE void (*eglGetMinmaxParameterfv)      (GLenum target, GLenum pname, GLfloat *params);
GLPRIVATE void (*eglGetMinmaxParameteriv)      (GLenum target, GLenum pname, GLint *params);
GLPRIVATE void (*eglConvolutionFilter1D)       (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image);
GLPRIVATE void (*eglConvolutionFilter2D)       (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image);
GLPRIVATE void (*eglConvolutionParameterf)     (GLenum target, GLenum pname, GLfloat params);
GLPRIVATE void (*eglConvolutionParameterfv)    (GLenum target, GLenum pname, const GLfloat *params);
GLPRIVATE void (*eglConvolutionParameteri)     (GLenum target, GLenum pname, GLint params);
GLPRIVATE void (*eglConvolutionParameteriv)    (GLenum target, GLenum pname, const GLint *params);
GLPRIVATE void (*eglCopyConvolutionFilter1D)   (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width);
GLPRIVATE void (*eglCopyConvolutionFilter2D)   (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height);
GLPRIVATE void (*eglGetConvolutionFilter)      (GLenum target, GLenum format, GLenum type, GLvoid *image);
GLPRIVATE void (*eglGetConvolutionParameterfv) (GLenum target, GLenum pname, GLfloat *params);
GLPRIVATE void (*eglGetConvolutionParameteriv) (GLenum target, GLenum pname, GLint *params);
GLPRIVATE void (*eglSeparableFilter2D)         (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column);
GLPRIVATE void (*eglGetSeparableFilter)        (GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span);
GLPRIVATE void (*eglActiveTexture)             (GLenum texture);
GLPRIVATE void (*eglClientActiveTexture)       (GLenum texture);
GLPRIVATE void (*eglCompressedTexImage1D)      (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data);
GLPRIVATE void (*eglCompressedTexImage2D)      (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
GLPRIVATE void (*eglCompressedTexImage3D)      (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data);
GLPRIVATE void (*eglCompressedTexSubImage1D)   (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data);
GLPRIVATE void (*eglCompressedTexSubImage2D)   (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
GLPRIVATE void (*eglCompressedTexSubImage3D)   (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data);
GLPRIVATE void (*eglGetCompressedTexImage)     (GLenum target, GLint lod, GLvoid *img);

GLPRIVATE void (*eglMultiTexCoord1d)  (GLenum target, GLdouble s);
GLPRIVATE void (*eglMultiTexCoord1dv) (GLenum target, const GLdouble *v);
GLPRIVATE void (*eglMultiTexCoord1f)  (GLenum target, GLfloat s);
GLPRIVATE void (*eglMultiTexCoord1fv) (GLenum target, const GLfloat *v);
GLPRIVATE void (*eglMultiTexCoord1i)  (GLenum target, GLint s);
GLPRIVATE void (*eglMultiTexCoord1iv) (GLenum target, const GLint *v);
GLPRIVATE void (*eglMultiTexCoord1s)  (GLenum target, GLshort s);
GLPRIVATE void (*eglMultiTexCoord1sv) (GLenum target, const GLshort *v);
GLPRIVATE void (*eglMultiTexCoord2d)  (GLenum target, GLdouble s, GLdouble t);
GLPRIVATE void (*eglMultiTexCoord2dv) (GLenum target, const GLdouble *v);
GLPRIVATE void (*eglMultiTexCoord2f)  (GLenum target, GLfloat s, GLfloat t);
GLPRIVATE void (*eglMultiTexCoord2fv) (GLenum target, const GLfloat *v);
GLPRIVATE void (*eglMultiTexCoord2i)  (GLenum target, GLint s, GLint t);
GLPRIVATE void (*eglMultiTexCoord2iv) (GLenum target, const GLint *v);
GLPRIVATE void (*eglMultiTexCoord2s)  (GLenum target, GLshort s, GLshort t);
GLPRIVATE void (*eglMultiTexCoord2sv) (GLenum target, const GLshort *v);
GLPRIVATE void (*eglMultiTexCoord3d)  (GLenum target, GLdouble s, GLdouble t, GLdouble r);
GLPRIVATE void (*eglMultiTexCoord3dv) (GLenum target, const GLdouble *v);
GLPRIVATE void (*eglMultiTexCoord3f)  (GLenum target, GLfloat s, GLfloat t, GLfloat r);
GLPRIVATE void (*eglMultiTexCoord3fv) (GLenum target, const GLfloat *v);
GLPRIVATE void (*eglMultiTexCoord3i)  (GLenum target, GLint s, GLint t, GLint r);
GLPRIVATE void (*eglMultiTexCoord3iv) (GLenum target, const GLint *v);
GLPRIVATE void (*eglMultiTexCoord3s)  (GLenum target, GLshort s, GLshort t, GLshort r);
GLPRIVATE void (*eglMultiTexCoord3sv) (GLenum target, const GLshort *v);
GLPRIVATE void (*eglMultiTexCoord4d)  (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
GLPRIVATE void (*eglMultiTexCoord4dv) (GLenum target, const GLdouble *v);
GLPRIVATE void (*eglMultiTexCoord4f)  (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
GLPRIVATE void (*eglMultiTexCoord4fv) (GLenum target, const GLfloat *v);
GLPRIVATE void (*eglMultiTexCoord4i)  (GLenum target, GLint s, GLint t, GLint r, GLint q);
GLPRIVATE void (*eglMultiTexCoord4iv) (GLenum target, const GLint *v);
GLPRIVATE void (*eglMultiTexCoord4s)  (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
GLPRIVATE void (*eglMultiTexCoord4sv) (GLenum target, const GLshort *v);

GLPRIVATE void (*eglLoadTransposeMatrixd)   (const GLdouble m[16]);
GLPRIVATE void (*eglLoadTransposeMatrixf)   (const GLfloat m[16]);
GLPRIVATE void (*eglMultTransposeMatrixd)   (const GLdouble m[16]);
GLPRIVATE void (*eglMultTransposeMatrixf)   (const GLfloat m[16]);
GLPRIVATE void (*eglSampleCoverage)         (GLclampf value, GLboolean invert);
GLPRIVATE void (*eglActiveTextureARB)       (GLenum texture);
GLPRIVATE void (*eglClientActiveTextureARB) (GLenum texture);
GLPRIVATE void (*eglMultiTexCoord1dARB)     (GLenum target, GLdouble s);
GLPRIVATE void (*eglMultiTexCoord1dvARB)    (GLenum target, const GLdouble *v);
GLPRIVATE void (*eglMultiTexCoord1fARB)     (GLenum target, GLfloat s);
GLPRIVATE void (*eglMultiTexCoord1fvARB)    (GLenum target, const GLfloat *v);
GLPRIVATE void (*eglMultiTexCoord1iARB)     (GLenum target, GLint s);
GLPRIVATE void (*eglMultiTexCoord1ivARB)    (GLenum target, const GLint *v);
GLPRIVATE void (*eglMultiTexCoord1sARB)     (GLenum target, GLshort s);
GLPRIVATE void (*eglMultiTexCoord1svARB)    (GLenum target, const GLshort *v);
GLPRIVATE void (*eglMultiTexCoord2dARB)     (GLenum target, GLdouble s, GLdouble t);
GLPRIVATE void (*eglMultiTexCoord2dvARB)    (GLenum target, const GLdouble *v);
GLPRIVATE void (*eglMultiTexCoord2fARB)     (GLenum target, GLfloat s, GLfloat t);
GLPRIVATE void (*eglMultiTexCoord2fvARB)    (GLenum target, const GLfloat *v);
GLPRIVATE void (*eglMultiTexCoord2iARB)     (GLenum target, GLint s, GLint t);
GLPRIVATE void (*eglMultiTexCoord2ivARB)    (GLenum target, const GLint *v);
GLPRIVATE void (*eglMultiTexCoord2sARB)     (GLenum target, GLshort s, GLshort t);
GLPRIVATE void (*eglMultiTexCoord2svARB)    (GLenum target, const GLshort *v);
GLPRIVATE void (*eglMultiTexCoord3dARB)     (GLenum target, GLdouble s, GLdouble t, GLdouble r);
GLPRIVATE void (*eglMultiTexCoord3dvARB)    (GLenum target, const GLdouble *v);
GLPRIVATE void (*eglMultiTexCoord3fARB)     (GLenum target, GLfloat s, GLfloat t, GLfloat r);
GLPRIVATE void (*eglMultiTexCoord3fvARB)    (GLenum target, const GLfloat *v);
GLPRIVATE void (*eglMultiTexCoord3iARB)     (GLenum target, GLint s, GLint t, GLint r);
GLPRIVATE void (*eglMultiTexCoord3ivARB)    (GLenum target, const GLint *v);
GLPRIVATE void (*eglMultiTexCoord3sARB)     (GLenum target, GLshort s, GLshort t, GLshort r);
GLPRIVATE void (*eglMultiTexCoord3svARB)    (GLenum target, const GLshort *v);
GLPRIVATE void (*eglMultiTexCoord4dARB)     (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
GLPRIVATE void (*eglMultiTexCoord4dvARB)    (GLenum target, const GLdouble *v);
GLPRIVATE void (*eglMultiTexCoord4fARB)     (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
GLPRIVATE void (*eglMultiTexCoord4fvARB)    (GLenum target, const GLfloat *v);
GLPRIVATE void (*eglMultiTexCoord4iARB)     (GLenum target, GLint s, GLint t, GLint r, GLint q);
GLPRIVATE void (*eglMultiTexCoord4ivARB)    (GLenum target, const GLint *v);
GLPRIVATE void (*eglMultiTexCoord4sARB)     (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
GLPRIVATE void (*eglMultiTexCoord4svARB)    (GLenum target, const GLshort *v);

#ifndef GLNOFUNCS

lib_func_t gl_funcs[] =
{
    { "glClearIndex",                (void **)&eglClearIndex                },
    { "glClearColor",                (void **)&eglClearColor                },
    { "glClear",                     (void **)&eglClear                     },
    { "glIndexMask",                 (void **)&eglIndexMask                 },
    { "glColorMask",                 (void **)&eglColorMask                 },
    { "glAlphaFunc",                 (void **)&eglAlphaFunc                 },
    { "glBlendFunc",                 (void **)&eglBlendFunc                 },
    { "glLogicOp",                   (void **)&eglLogicOp                   },
    { "glCullFace",                  (void **)&eglCullFace                  },
    { "glFrontFace",                 (void **)&eglFrontFace                 },
    { "glPointSize",                 (void **)&eglPointSize                 },
    { "glLineWidth",                 (void **)&eglLineWidth                 },
    { "glLineStipple",               (void **)&eglLineStipple               },
    { "glPolygonMode",               (void **)&eglPolygonMode               },
    { "glPolygonOffset",             (void **)&eglPolygonOffset             },
    { "glPolygonStipple",            (void **)&eglPolygonStipple            },
    { "glGetPolygonStipple",         (void **)&eglGetPolygonStipple         },
    { "glEdgeFlag",                  (void **)&eglEdgeFlag                  },
    { "glEdgeFlagv",                 (void **)&eglEdgeFlagv                 },
    { "glScissor",                   (void **)&eglScissor                   },
    { "glClipPlane",                 (void **)&eglClipPlane                 },
    { "glGetClipPlane",              (void **)&eglGetClipPlane              },
    { "glDrawBuffer",                (void **)&eglDrawBuffer                },
    { "glReadBuffer",                (void **)&eglReadBuffer                },
    { "glEnable",                    (void **)&eglEnable_                   },
    { "glDisable",                   (void **)&eglDisable_                  },
    { "glIsEnabled",                 (void **)&eglIsEnabled                 },
    { "glGetBooleanv",               (void **)&eglGetBooleanv               },
    { "glGetDoublev",                (void **)&eglGetDoublev                },
    { "glGetFloatv",                 (void **)&eglGetFloatv                 },
    { "glGetIntegerv",               (void **)&eglGetIntegerv               },
    { "glPushAttrib",                (void **)&eglPushAttrib                },
    { "glPopAttrib",                 (void **)&eglPopAttrib                 },
    { "glRenderMode",                (void **)&eglRenderMode                },
    { "glGetError",                  (void **)&eglGetError                  },
    { "glGetString",                 (void **)&eglGetString                 },
    { "glFinish",                    (void **)&eglFinish                    },
    { "glFlush",                     (void **)&eglFlush                     },
    { "glHint",                      (void **)&eglHint                      },
    { "glClearDepth",                (void **)&eglClearDepth                },
    { "glDepthFunc",                 (void **)&eglDepthFunc                 },
    { "glDepthMask",                 (void **)&eglDepthMask_                },
    { "glDepthRange",                (void **)&eglDepthRange                },
    { "glClearAccum",                (void **)&eglClearAccum                },
    { "glAccum",                     (void **)&eglAccum                     },
    { "glMatrixMode",                (void **)&eglMatrixMode                },
    { "glOrtho",                     (void **)&eglOrtho                     },
    { "glFrustum",                   (void **)&eglFrustum                   },
    { "glViewport",                  (void **)&eglViewport                  },
    { "glPushMatrix",                (void **)&eglPushMatrix                },
    { "glPopMatrix",                 (void **)&eglPopMatrix                 },
    { "glLoadIdentity",              (void **)&eglLoadIdentity              },
    { "glLoadMatrixd",               (void **)&eglLoadMatrixd               },
    { "glLoadMatrixf",               (void **)&eglLoadMatrixf               },
    { "glMultMatrixd",               (void **)&eglMultMatrixd               },
    { "glMultMatrixf",               (void **)&eglMultMatrixf               },
    { "glRotated",                   (void **)&eglRotated                   },
    { "glRotatef",                   (void **)&eglRotatef                   },
    { "glScaled",                    (void **)&eglScaled                    },
    { "glScalef",                    (void **)&eglScalef                    },
    { "glTranslated",                (void **)&eglTranslated                },
    { "glTranslatef",                (void **)&eglTranslatef                },
    { "glIsList",                    (void **)&eglIsList                    },
    { "glDeleteLists",               (void **)&eglDeleteLists               },
    { "glGenLists",                  (void **)&eglGenLists                  },
    { "glNewList",                   (void **)&eglNewList                   },
    { "glEndList",                   (void **)&eglEndList                   },
    { "glCallList",                  (void **)&eglCallList                  },
    { "glCallLists",                 (void **)&eglCallLists                 },
    { "glListBase",                  (void **)&eglListBase                  },
    { "glBegin",                     (void **)&eglBegin                     },
    { "glEnd",                       (void **)&eglEnd                       },
    { "glVertex2d",                  (void **)&eglVertex2d                  },
    { "glVertex2f",                  (void **)&eglVertex2f                  },
    { "glVertex2i",                  (void **)&eglVertex2i                  },
    { "glVertex2s",                  (void **)&eglVertex2s                  },
    { "glVertex3d",                  (void **)&eglVertex3d                  },
    { "glVertex3f",                  (void **)&eglVertex3f                  },
    { "glVertex3i",                  (void **)&eglVertex3i                  },
    { "glVertex3s",                  (void **)&eglVertex3s                  },
    { "glVertex4d",                  (void **)&eglVertex4d                  },
    { "glVertex4f",                  (void **)&eglVertex4f                  },
    { "glVertex4i",                  (void **)&eglVertex4i                  },
    { "glVertex4s",                  (void **)&eglVertex4s                  },
    { "glVertex2dv",                 (void **)&eglVertex2dv                 },
    { "glVertex2fv",                 (void **)&eglVertex2fv                 },
    { "glVertex2iv",                 (void **)&eglVertex2iv                 },
    { "glVertex2sv",                 (void **)&eglVertex2sv                 },
    { "glVertex3dv",                 (void **)&eglVertex3dv                 },
    { "glVertex3fv",                 (void **)&eglVertex3fv                 },
    { "glVertex3iv",                 (void **)&eglVertex3iv                 },
    { "glVertex3sv",                 (void **)&eglVertex3sv                 },
    { "glVertex4dv",                 (void **)&eglVertex4dv                 },
    { "glVertex4fv",                 (void **)&eglVertex4fv                 },
    { "glVertex4iv",                 (void **)&eglVertex4iv                 },
    { "glVertex4sv",                 (void **)&eglVertex4sv                 },
    { "glNormal3b",                  (void **)&eglNormal3b                  },
    { "glNormal3d",                  (void **)&eglNormal3d                  },
    { "glNormal3f",                  (void **)&eglNormal3f                  },
    { "glNormal3i",                  (void **)&eglNormal3i                  },
    { "glNormal3s",                  (void **)&eglNormal3s                  },
    { "glNormal3bv",                 (void **)&eglNormal3bv                 },
    { "glNormal3dv",                 (void **)&eglNormal3dv                 },
    { "glNormal3fv",                 (void **)&eglNormal3fv                 },
    { "glNormal3iv",                 (void **)&eglNormal3iv                 },
    { "glNormal3sv",                 (void **)&eglNormal3sv                 },
    { "glIndexd",                    (void **)&eglIndexd                    },
    { "glIndexf",                    (void **)&eglIndexf                    },
    { "glIndexi",                    (void **)&eglIndexi                    },
    { "glIndexs",                    (void **)&eglIndexs                    },
    { "glIndexdv",                   (void **)&eglIndexdv                   },
    { "glIndexfv",                   (void **)&eglIndexfv                   },
    { "glIndexiv",                   (void **)&eglIndexiv                   },
    { "glIndexsv",                   (void **)&eglIndexsv                   },
    { "glColor3b",                   (void **)&eglColor3b                   },
    { "glColor3d",                   (void **)&eglColor3d                   },
    { "glColor3f",                   (void **)&eglColor3f_                  },
    { "glColor3i",                   (void **)&eglColor3i                   },
    { "glColor3s",                   (void **)&eglColor3s                   },
    { "glColor3ub",                  (void **)&eglColor3ub                  },
    { "glColor3ui",                  (void **)&eglColor3ui                  },
    { "glColor3us",                  (void **)&eglColor3us                  },
    { "glColor4b",                   (void **)&eglColor4b                   },
    { "glColor4d",                   (void **)&eglColor4d                   },
    { "glColor4f",                   (void **)&eglColor4f_                  },
    { "glColor4i",                   (void **)&eglColor4i                   },
    { "glColor4s",                   (void **)&eglColor4s                   },
    { "glColor4ub",                  (void **)&eglColor4ub                  },
    { "glColor4ui",                  (void **)&eglColor4ui                  },
    { "glColor4us",                  (void **)&eglColor4us                  },
    { "glColor3bv",                  (void **)&eglColor3bv                  },
    { "glColor3dv",                  (void **)&eglColor3dv                  },
    { "glColor3fv",                  (void **)&eglColor3fv                  },
    { "glColor3iv",                  (void **)&eglColor3iv                  },
    { "glColor3sv",                  (void **)&eglColor3sv                  },
    { "glColor3ubv",                 (void **)&eglColor3ubv                 },
    { "glColor3uiv",                 (void **)&eglColor3uiv                 },
    { "glColor3usv",                 (void **)&eglColor3usv                 },
    { "glColor4bv",                  (void **)&eglColor4bv                  },
    { "glColor4dv",                  (void **)&eglColor4dv                  },
    { "glColor4fv",                  (void **)&eglColor4fv                  },
    { "glColor4iv",                  (void **)&eglColor4iv                  },
    { "glColor4sv",                  (void **)&eglColor4sv                  },
    { "glColor4ubv",                 (void **)&eglColor4ubv                 },
    { "glColor4uiv",                 (void **)&eglColor4uiv                 },
    { "glColor4usv",                 (void **)&eglColor4usv                 },
    { "glTexCoord1d",                (void **)&eglTexCoord1d                },
    { "glTexCoord1f",                (void **)&eglTexCoord1f                },
    { "glTexCoord1i",                (void **)&eglTexCoord1i                },
    { "glTexCoord1s",                (void **)&eglTexCoord1s                },
    { "glTexCoord2d",                (void **)&eglTexCoord2d                },
    { "glTexCoord2f",                (void **)&eglTexCoord2f                },
    { "glTexCoord2i",                (void **)&eglTexCoord2i                },
    { "glTexCoord2s",                (void **)&eglTexCoord2s                },
    { "glTexCoord3d",                (void **)&eglTexCoord3d                },
    { "glTexCoord3f",                (void **)&eglTexCoord3f                },
    { "glTexCoord3i",                (void **)&eglTexCoord3i                },
    { "glTexCoord3s",                (void **)&eglTexCoord3s                },
    { "glTexCoord4d",                (void **)&eglTexCoord4d                },
    { "glTexCoord4f",                (void **)&eglTexCoord4f                },
    { "glTexCoord4i",                (void **)&eglTexCoord4i                },
    { "glTexCoord4s",                (void **)&eglTexCoord4s                },
    { "glTexCoord1dv",               (void **)&eglTexCoord1dv               },
    { "glTexCoord1fv",               (void **)&eglTexCoord1fv               },
    { "glTexCoord1iv",               (void **)&eglTexCoord1iv               },
    { "glTexCoord1sv",               (void **)&eglTexCoord1sv               },
    { "glTexCoord2dv",               (void **)&eglTexCoord2dv               },
    { "glTexCoord2fv",               (void **)&eglTexCoord2fv               },
    { "glTexCoord2iv",               (void **)&eglTexCoord2iv               },
    { "glTexCoord2sv",               (void **)&eglTexCoord2sv               },
    { "glTexCoord3dv",               (void **)&eglTexCoord3dv               },
    { "glTexCoord3fv",               (void **)&eglTexCoord3fv               },
    { "glTexCoord3iv",               (void **)&eglTexCoord3iv               },
    { "glTexCoord3sv",               (void **)&eglTexCoord3sv               },
    { "glTexCoord4dv",               (void **)&eglTexCoord4dv               },
    { "glTexCoord4fv",               (void **)&eglTexCoord4fv               },
    { "glTexCoord4iv",               (void **)&eglTexCoord4iv               },
    { "glTexCoord4sv",               (void **)&eglTexCoord4sv               },
    { "glRasterPos2d",               (void **)&eglRasterPos2d               },
    { "glRasterPos2f",               (void **)&eglRasterPos2f               },
    { "glRasterPos2i",               (void **)&eglRasterPos2i               },
    { "glRasterPos2s",               (void **)&eglRasterPos2s               },
    { "glRasterPos3d",               (void **)&eglRasterPos3d               },
    { "glRasterPos3f",               (void **)&eglRasterPos3f               },
    { "glRasterPos3i",               (void **)&eglRasterPos3i               },
    { "glRasterPos3s",               (void **)&eglRasterPos3s               },
    { "glRasterPos4d",               (void **)&eglRasterPos4d               },
    { "glRasterPos4f",               (void **)&eglRasterPos4f               },
    { "glRasterPos4i",               (void **)&eglRasterPos4i               },
    { "glRasterPos4s",               (void **)&eglRasterPos4s               },
    { "glRasterPos2dv",              (void **)&eglRasterPos2dv              },
    { "glRasterPos2fv",              (void **)&eglRasterPos2fv              },
    { "glRasterPos2iv",              (void **)&eglRasterPos2iv              },
    { "glRasterPos2sv",              (void **)&eglRasterPos2sv              },
    { "glRasterPos3dv",              (void **)&eglRasterPos3dv              },
    { "glRasterPos3fv",              (void **)&eglRasterPos3fv              },
    { "glRasterPos3iv",              (void **)&eglRasterPos3iv              },
    { "glRasterPos3sv",              (void **)&eglRasterPos3sv              },
    { "glRasterPos4dv",              (void **)&eglRasterPos4dv              },
    { "glRasterPos4fv",              (void **)&eglRasterPos4fv              },
    { "glRasterPos4iv",              (void **)&eglRasterPos4iv              },
    { "glRasterPos4sv",              (void **)&eglRasterPos4sv              },
    { "glRectd",                     (void **)&eglRectd                     },
    { "glRectf",                     (void **)&eglRectf                     },
    { "glRecti",                     (void **)&eglRecti                     },
    { "glRects",                     (void **)&eglRects                     },
    { "glRectdv",                    (void **)&eglRectdv                    },
    { "glRectfv",                    (void **)&eglRectfv                    },
    { "glRectiv",                    (void **)&eglRectiv                    },
    { "glRectsv",                    (void **)&eglRectsv                    },
    { "glShadeModel",                (void **)&eglShadeModel                },
    { "glLightf",                    (void **)&eglLightf                    },
    { "glLighti",                    (void **)&eglLighti                    },
    { "glLightfv",                   (void **)&eglLightfv                   },
    { "glLightiv",                   (void **)&eglLightiv                   },
    { "glGetLightfv",                (void **)&eglGetLightfv                },
    { "glGetLightiv",                (void **)&eglGetLightiv                },
    { "glLightModelf",               (void **)&eglLightModelf               },
    { "glLightModeli",               (void **)&eglLightModeli               },
    { "glLightModelfv",              (void **)&eglLightModelfv              },
    { "glLightModeliv",              (void **)&eglLightModeliv              },
    { "glMaterialf",                 (void **)&eglMaterialf                 },
    { "glMateriali",                 (void **)&eglMateriali                 },
    { "glMaterialfv",                (void **)&eglMaterialfv                },
    { "glMaterialiv",                (void **)&eglMaterialiv                },
    { "glGetMaterialfv",             (void **)&eglGetMaterialfv             },
    { "glGetMaterialiv",             (void **)&eglGetMaterialiv             },
    { "glColorMaterial",             (void **)&eglColorMaterial             },
    { "glPixelZoom",                 (void **)&eglPixelZoom                 },
    { "glPixelStoref",               (void **)&eglPixelStoref               },
    { "glPixelStorei",               (void **)&eglPixelStorei               },
    { "glPixelTransferf",            (void **)&eglPixelTransferf            },
    { "glPixelTransferi",            (void **)&eglPixelTransferi            },
    { "glPixelMapfv",                (void **)&eglPixelMapfv                },
    { "glPixelMapuiv",               (void **)&eglPixelMapuiv               },
    { "glPixelMapusv",               (void **)&eglPixelMapusv               },
    { "glGetPixelMapfv",             (void **)&eglGetPixelMapfv             },
    { "glGetPixelMapuiv",            (void **)&eglGetPixelMapuiv            },
    { "glGetPixelMapusv",            (void **)&eglGetPixelMapusv            },
    { "glBitmap",                    (void **)&eglBitmap                    },
    { "glReadPixels",                (void **)&eglReadPixels                },
    { "glDrawPixels",                (void **)&eglDrawPixels                },
    { "glCopyPixels",                (void **)&eglCopyPixels                },
    { "glStencilFunc",               (void **)&eglStencilFunc               },
    { "glStencilMask",               (void **)&eglStencilMask               },
    { "glStencilOp",                 (void **)&eglStencilOp                 },
    { "glClearStencil",              (void **)&eglClearStencil              },
    { "glTexGend",                   (void **)&eglTexGend                   },
    { "glTexGenf",                   (void **)&eglTexGenf                   },
    { "glTexGeni",                   (void **)&eglTexGeni                   },
    { "glTexGendv",                  (void **)&eglTexGendv                  },
    { "glTexGenfv",                  (void **)&eglTexGenfv                  },
    { "glTexGeniv",                  (void **)&eglTexGeniv                  },
    { "glGetTexGendv",               (void **)&eglGetTexGendv               },
    { "glGetTexGenfv",               (void **)&eglGetTexGenfv               },
    { "glGetTexGeniv",               (void **)&eglGetTexGeniv               },
    { "glTexEnvf",                   (void **)&eglTexEnvf                   },
    { "glTexEnvi",                   (void **)&eglTexEnvi                   },
    { "glTexEnvfv",                  (void **)&eglTexEnvfv                  },
    { "glTexEnviv",                  (void **)&eglTexEnviv                  },
    { "glGetTexEnvfv",               (void **)&eglGetTexEnvfv               },
    { "glGetTexEnviv",               (void **)&eglGetTexEnviv               },
    { "glTexParameterf",             (void **)&eglTexParameterf             },
    { "glTexParameteri",             (void **)&eglTexParameteri             },
    { "glTexParameterfv",            (void **)&eglTexParameterfv            },
    { "glTexParameteriv",            (void **)&eglTexParameteriv            },
    { "glGetTexParameterfv",         (void **)&eglGetTexParameterfv         },
    { "glGetTexParameteriv",         (void **)&eglGetTexParameteriv         },
    { "glGetTexLevelParameterfv",    (void **)&eglGetTexLevelParameterfv    },
    { "glGetTexLevelParameteriv",    (void **)&eglGetTexLevelParameteriv    },
    { "glTexImage1D",                (void **)&eglTexImage1D                },
    { "glTexImage2D",                (void **)&eglTexImage2D                },
    { "glGetTexImage",               (void **)&eglGetTexImage               },
    { "glEnableClientState",         (void **)&eglEnableClientState         },
    { "glDisableClientState",        (void **)&eglDisableClientState        },
    { "glPushClientAttrib",          (void **)&eglPushClientAttrib          },
    { "glPopClientAttrib",           (void **)&eglPopClientAttrib           },
    { "glIndexub",                   (void **)&eglIndexub                   },
    { "glIndexubv",                  (void **)&eglIndexubv                  },
    { "glGenTextures",               (void **)&eglGenTextures               },
    { "glDeleteTextures",            (void **)&eglDeleteTextures            },
    { "glBindTexture",               (void **)&eglBindTexture_              },
    { "glPrioritizeTextures",        (void **)&eglPrioritizeTextures        },
    { "glAreTexturesResident",       (void **)&eglAreTexturesResident       },
    { "glIsTexture",                 (void **)&eglIsTexture                 },
    { "glTexSubImage1D",             (void **)&eglTexSubImage1D             },
    { "glTexSubImage2D",             (void **)&eglTexSubImage2D             },
    { "glCopyTexImage1D",            (void **)&eglCopyTexImage1D            },
    { "glCopyTexImage2D",            (void **)&eglCopyTexImage2D            },
    { "glCopyTexSubImage1D",         (void **)&eglCopyTexSubImage1D         },
    { "glCopyTexSubImage2D",         (void **)&eglCopyTexSubImage2D         },
    { "glVertexPointer",             (void **)&eglVertexPointer             },
    { "glNormalPointer",             (void **)&eglNormalPointer             },
    { "glColorPointer",              (void **)&eglColorPointer              },
    { "glIndexPointer",              (void **)&eglIndexPointer              },
    { "glTexCoordPointer",           (void **)&eglTexCoordPointer           },
    { "glEdgeFlagPointer",           (void **)&eglEdgeFlagPointer           },
    { "glGetPointerv",               (void **)&eglGetPointerv               },
    { "glArrayElement",              (void **)&eglArrayElement              },
    { "glDrawArrays",                (void **)&eglDrawArrays                },
    { "glDrawElements",              (void **)&eglDrawElements              },
    { "glInterleavedArrays",         (void **)&eglInterleavedArrays         },
    { "glMap1d",                     (void **)&eglMap1d                     },
    { "glMap1f",                     (void **)&eglMap1f                     },
    { "glMap2d",                     (void **)&eglMap2d                     },
    { "glMap2f",                     (void **)&eglMap2f                     },
    { "glGetMapdv",                  (void **)&eglGetMapdv                  },
    { "glGetMapfv",                  (void **)&eglGetMapfv                  },
    { "glGetMapiv",                  (void **)&eglGetMapiv                  },
    { "glEvalCoord1d",               (void **)&eglEvalCoord1d               },
    { "glEvalCoord1f",               (void **)&eglEvalCoord1f               },
    { "glEvalCoord1dv",              (void **)&eglEvalCoord1dv              },
    { "glEvalCoord1fv",              (void **)&eglEvalCoord1fv              },
    { "glEvalCoord2d",               (void **)&eglEvalCoord2d               },
    { "glEvalCoord2f",               (void **)&eglEvalCoord2f               },
    { "glEvalCoord2dv",              (void **)&eglEvalCoord2dv              },
    { "glEvalCoord2fv",              (void **)&eglEvalCoord2fv              },
    { "glMapGrid1d",                 (void **)&eglMapGrid1d                 },
    { "glMapGrid1f",                 (void **)&eglMapGrid1f                 },
    { "glMapGrid2d",                 (void **)&eglMapGrid2d                 },
    { "glMapGrid2f",                 (void **)&eglMapGrid2f                 },
    { "glEvalPoint1",                (void **)&eglEvalPoint1                },
    { "glEvalPoint2",                (void **)&eglEvalPoint2                },
    { "glEvalMesh1",                 (void **)&eglEvalMesh1                 },
    { "glEvalMesh2",                 (void **)&eglEvalMesh2                 },
    { "glFogf",                      (void **)&eglFogf                      },
    { "glFogi",                      (void **)&eglFogi                      },
    { "glFogfv",                     (void **)&eglFogfv                     },
    { "glFogiv",                     (void **)&eglFogiv                     },
    { "glFeedbackBuffer",            (void **)&eglFeedbackBuffer            },
    { "glPassThrough",               (void **)&eglPassThrough               },
    { "glSelectBuffer",              (void **)&eglSelectBuffer              },
    { "glInitNames",                 (void **)&eglInitNames                 },
    { "glLoadName",                  (void **)&eglLoadName                  },
    { "glPushName",                  (void **)&eglPushName                  },
    { "glPopName",                   (void **)&eglPopName                   },
    { "glDrawRangeElements",         (void **)&eglDrawRangeElements         },
    { "glTexImage3D",                (void **)&eglTexImage3D                },
    { "glTexSubImage3D",             (void **)&eglTexSubImage3D             },
    { "glCopyTexSubImage3D",         (void **)&eglCopyTexSubImage3D         },
    { "glColorTable",                (void **)&eglColorTable                },
    { "glColorSubTable",             (void **)&eglColorSubTable             },
    { "glColorTableParameteriv",     (void **)&eglColorTableParameteriv     },
    { "glColorTableParameterfv",     (void **)&eglColorTableParameterfv     },
    { "glCopyColorSubTable",         (void **)&eglCopyColorSubTable         },
    { "glCopyColorTable",            (void **)&eglCopyColorTable            },
    { "glGetColorTable",             (void **)&eglGetColorTable             },
    { "glGetColorTableParameterfv",  (void **)&eglGetColorTableParameterfv  },
    { "glGetColorTableParameteriv",  (void **)&eglGetColorTableParameteriv  },
    { "glBlendEquation",             (void **)&eglBlendEquation             },
    { "glBlendColor",                (void **)&eglBlendColor                },
    { "glHistogram",                 (void **)&eglHistogram                 },
    { "glResetHistogram",            (void **)&eglResetHistogram            },
    { "glGetHistogram",              (void **)&eglGetHistogram              },
    { "glGetHistogramParameterfv",   (void **)&eglGetHistogramParameterfv   },
    { "glGetHistogramParameteriv",   (void **)&eglGetHistogramParameteriv   },
    { "glMinmax",                    (void **)&eglMinmax                    },
    { "glResetMinmax",               (void **)&eglResetMinmax               },
    { "glGetMinmax",                 (void **)&eglGetMinmax                 },
    { "glGetMinmaxParameterfv",      (void **)&eglGetMinmaxParameterfv      },
    { "glGetMinmaxParameteriv",      (void **)&eglGetMinmaxParameteriv      },
    { "glConvolutionFilter1D",       (void **)&eglConvolutionFilter1D       },
    { "glConvolutionFilter2D",       (void **)&eglConvolutionFilter2D       },
    { "glConvolutionParameterf",     (void **)&eglConvolutionParameterf     },
    { "glConvolutionParameterfv",    (void **)&eglConvolutionParameterfv    },
    { "glConvolutionParameteri",     (void **)&eglConvolutionParameteri     },
    { "glConvolutionParameteriv",    (void **)&eglConvolutionParameteriv    },
    { "glCopyConvolutionFilter1D",   (void **)&eglCopyConvolutionFilter1D   },
    { "glCopyConvolutionFilter2D",   (void **)&eglCopyConvolutionFilter2D   },
    { "glGetConvolutionFilter",      (void **)&eglGetConvolutionFilter      },
    { "glGetConvolutionParameterfv", (void **)&eglGetConvolutionParameterfv },
    { "glGetConvolutionParameteriv", (void **)&eglGetConvolutionParameteriv },
    { "glSeparableFilter2D",         (void **)&eglSeparableFilter2D         },
    { "glGetSeparableFilter",        (void **)&eglGetSeparableFilter        },
    { "glActiveTexture",             (void **)&eglActiveTexture             },
    { "glClientActiveTexture",       (void **)&eglClientActiveTexture       },
    { "glCompressedTexImage1D",      (void **)&eglCompressedTexImage1D      },
    { "glCompressedTexImage2D",      (void **)&eglCompressedTexImage2D      },
    { "glCompressedTexImage3D",      (void **)&eglCompressedTexImage3D      },
    { "glCompressedTexSubImage1D",   (void **)&eglCompressedTexSubImage1D   },
    { "glCompressedTexSubImage2D",   (void **)&eglCompressedTexSubImage2D   },
    { "glCompressedTexSubImage3D",   (void **)&eglCompressedTexSubImage3D   },
    { "glGetCompressedTexImage",     (void **)&eglGetCompressedTexImage     },
    { "glMultiTexCoord1d",           (void **)&eglMultiTexCoord1d           },
    { "glMultiTexCoord1dv",          (void **)&eglMultiTexCoord1dv          },
    { "glMultiTexCoord1f",           (void **)&eglMultiTexCoord1f           },
    { "glMultiTexCoord1fv",          (void **)&eglMultiTexCoord1fv          },
    { "glMultiTexCoord1i",           (void **)&eglMultiTexCoord1i           },
    { "glMultiTexCoord1iv",          (void **)&eglMultiTexCoord1iv          },
    { "glMultiTexCoord1s",           (void **)&eglMultiTexCoord1s           },
    { "glMultiTexCoord1sv",          (void **)&eglMultiTexCoord1sv          },
    { "glMultiTexCoord2d",           (void **)&eglMultiTexCoord2d           },
    { "glMultiTexCoord2dv",          (void **)&eglMultiTexCoord2dv          },
    { "glMultiTexCoord2f",           (void **)&eglMultiTexCoord2f           },
    { "glMultiTexCoord2fv",          (void **)&eglMultiTexCoord2fv          },
    { "glMultiTexCoord2i",           (void **)&eglMultiTexCoord2i           },
    { "glMultiTexCoord2iv",          (void **)&eglMultiTexCoord2iv          },
    { "glMultiTexCoord2s",           (void **)&eglMultiTexCoord2s           },
    { "glMultiTexCoord2sv",          (void **)&eglMultiTexCoord2sv          },
    { "glMultiTexCoord3d",           (void **)&eglMultiTexCoord3d           },
    { "glMultiTexCoord3dv",          (void **)&eglMultiTexCoord3dv          },
    { "glMultiTexCoord3f",           (void **)&eglMultiTexCoord3f           },
    { "glMultiTexCoord3fv",          (void **)&eglMultiTexCoord3fv          },
    { "glMultiTexCoord3i",           (void **)&eglMultiTexCoord3i           },
    { "glMultiTexCoord3iv",          (void **)&eglMultiTexCoord3iv          },
    { "glMultiTexCoord3s",           (void **)&eglMultiTexCoord3s           },
    { "glMultiTexCoord3sv",          (void **)&eglMultiTexCoord3sv          },
    { "glMultiTexCoord4d",           (void **)&eglMultiTexCoord4d           },
    { "glMultiTexCoord4dv",          (void **)&eglMultiTexCoord4dv          },
    { "glMultiTexCoord4f",           (void **)&eglMultiTexCoord4f           },
    { "glMultiTexCoord4fv",          (void **)&eglMultiTexCoord4fv          },
    { "glMultiTexCoord4i",           (void **)&eglMultiTexCoord4i           },
    { "glMultiTexCoord4iv",          (void **)&eglMultiTexCoord4iv          },
    { "glMultiTexCoord4s",           (void **)&eglMultiTexCoord4s           },
    { "glMultiTexCoord4sv",          (void **)&eglMultiTexCoord4sv          },
    { "glLoadTransposeMatrixd",      (void **)&eglLoadTransposeMatrixd      },
    { "glLoadTransposeMatrixf",      (void **)&eglLoadTransposeMatrixf      },
    { "glMultTransposeMatrixd",      (void **)&eglMultTransposeMatrixd      },
    { "glMultTransposeMatrixf",      (void **)&eglMultTransposeMatrixf      },
    { "glSampleCoverage",            (void **)&eglSampleCoverage            },
    { "glActiveTextureARB",          (void **)&eglActiveTextureARB          },
    { "glClientActiveTextureARB",    (void **)&eglClientActiveTextureARB    },
    { "glMultiTexCoord1dARB",        (void **)&eglMultiTexCoord1dARB        },
    { "glMultiTexCoord1dvARB",       (void **)&eglMultiTexCoord1dvARB       },
    { "glMultiTexCoord1fARB",        (void **)&eglMultiTexCoord1fARB        },
    { "glMultiTexCoord1fvARB",       (void **)&eglMultiTexCoord1fvARB       },
    { "glMultiTexCoord1iARB",        (void **)&eglMultiTexCoord1iARB        },
    { "glMultiTexCoord1ivARB",       (void **)&eglMultiTexCoord1ivARB       },
    { "glMultiTexCoord1sARB",        (void **)&eglMultiTexCoord1sARB        },
    { "glMultiTexCoord1svARB",       (void **)&eglMultiTexCoord1svARB       },
    { "glMultiTexCoord2dARB",        (void **)&eglMultiTexCoord2dARB        },
    { "glMultiTexCoord2dvARB",       (void **)&eglMultiTexCoord2dvARB       },
    { "glMultiTexCoord2fARB",        (void **)&eglMultiTexCoord2fARB        },
    { "glMultiTexCoord2fvARB",       (void **)&eglMultiTexCoord2fvARB       },
    { "glMultiTexCoord2iARB",        (void **)&eglMultiTexCoord2iARB        },
    { "glMultiTexCoord2ivARB",       (void **)&eglMultiTexCoord2ivARB       },
    { "glMultiTexCoord2sARB",        (void **)&eglMultiTexCoord2sARB        },
    { "glMultiTexCoord2svARB",       (void **)&eglMultiTexCoord2svARB       },
    { "glMultiTexCoord3dARB",        (void **)&eglMultiTexCoord3dARB        },
    { "glMultiTexCoord3dvARB",       (void **)&eglMultiTexCoord3dvARB       },
    { "glMultiTexCoord3fARB",        (void **)&eglMultiTexCoord3fARB        },
    { "glMultiTexCoord3fvARB",       (void **)&eglMultiTexCoord3fvARB       },
    { "glMultiTexCoord3iARB",        (void **)&eglMultiTexCoord3iARB        },
    { "glMultiTexCoord3ivARB",       (void **)&eglMultiTexCoord3ivARB       },
    { "glMultiTexCoord3sARB",        (void **)&eglMultiTexCoord3sARB        },
    { "glMultiTexCoord3svARB",       (void **)&eglMultiTexCoord3svARB       },
    { "glMultiTexCoord4dARB",        (void **)&eglMultiTexCoord4dARB        },
    { "glMultiTexCoord4dvARB",       (void **)&eglMultiTexCoord4dvARB       },
    { "glMultiTexCoord4fARB",        (void **)&eglMultiTexCoord4fARB        },
    { "glMultiTexCoord4fvARB",       (void **)&eglMultiTexCoord4fvARB       },
    { "glMultiTexCoord4iARB",        (void **)&eglMultiTexCoord4iARB        },
    { "glMultiTexCoord4ivARB",       (void **)&eglMultiTexCoord4ivARB       },
    { "glMultiTexCoord4sARB",        (void **)&eglMultiTexCoord4sARB        },
    { "glMultiTexCoord4svARB",       (void **)&eglMultiTexCoord4svARB       },
    { NULL, NULL }
};

#endif /* !GLNOFUNCS */

#endif /* _GL_FUNCS_H */
