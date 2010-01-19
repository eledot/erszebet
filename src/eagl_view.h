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

#ifndef _EAGL_VIEW_H
#define _EAGL_VIEW_H

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

@interface EAGLView : UIView
{
@private

    EAGLContext *context;
    GLuint       buffer_render;
    GLuint       buffer_frame;
    GLuint       buffer_depth;

    NSTimer       *animation_timer;
    NSTimeInterval animation_interval;
}

@property NSTimeInterval animation_interval;

- (BOOL)createFramebuffer;
- (void)destroyFramebuffer;
- (void)startAnimation;
- (void)stopAnimation;
- (void)drawView;

@end

#endif /* !_EAGLVIEW_H */
