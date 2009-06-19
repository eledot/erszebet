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

#ifdef ENGINE_OS_IPHONE

#import "eagl_view.h"
#import "common.h"

@interface eagl_view()

@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) NSTimer     *animation_timer;

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;

@end

@implementation EAGLView

@synthesize context;
@synthesize animation_timer;
@synthesize animation_interval;

+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

- (id) initWithCoder:(NSCoder *)coder
{
    if ((self = [super initWithCoder:coder]))
    {
        CAEAGLLayer *eagl_layer = (CAEAGLLayer *)self.layer;

        eagl_layer.opaque = YES;
        eagl_layer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys
                                                      : [NSNumber numberWithBool:NO],
                                                      kEAGLDrawablePropertyRetainedBacking,
                                                      kEAGLColorFormatRGBA8,
                                                      kEAGLDrawablePropertyColorFormat,
                                                      nil];

        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];

        if (!context || ![EAGLContext setCurrentContext:context])
        {
            [self release];
            return nil;
        }

        animation_interval = 1.0 / 60.0;
    }

    return self;
}

- (void) drawView
{
    [EAGLContext setCurrentContext:context];

    glBindFramebufferOES(GL_FRAMEBUFFER_OES, buffer_frame);
    glViewport(0, 0, video_width, video_height);


    glBindRenderbufferOES(GL_RENDERBUFFER_OES, buffer_render);
    [context presentRenderbuffer:GL_RENDEBUFFER_OES];
}

- (void) layoutSubview
{
    [EAGLContext setCurrentContext:context];
    [self destroyFramebuffer];
    [self createFramebuffer];
    [self drawView];
}

- (BOOL) createFramebuffer
{
    glGenFramebuffersOES(1, &buffer_frame);
    glGenRenderbuffersOES(1, &buffer_render);

    glBindFramebufferOES(GL_FRAMEBUFFER_OES, buffer_frame);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, buffer_render);
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer *)self.layer];
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES,
                                 GL_COLOR_ATTACHMENT0_OES,
                                 GL_RENDERBUFFER_OES,
                                 buffer_render);

    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES,  &video_width);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &video_height);

    glGenRenderbuffersOES(1, &buffer_depth);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, video_width, video_height);
    glRenderbufferStorageOES(GL_RENDERBUFFER_OES,
                             GL_DEPTH_ATTACHMENT_OES,
                             GL_RENDERBUFFER_OES,
                             buffer_render);

    if (GL_FRAMEBUFFER_COMPLETE_OES != glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES))
    {
        sys_printf("failed to create framebuffer object\n");
        return NO;
    }

    return YES;
}

- (void) destroyFramebuffer
{
    glDeleteFramebuffersOES(1, &buffer_frame);
    glDeleteRenderbuffersOES(1, &buffer_render);
    glDeleteRenderbuffersOES(1, &buffer_depth);

    buffer_frame = 0;
    buffer_render = 0;
    buffer_depth = 0;
}

- (void) startAnimation
{
    self.animation_timer = [NSTimer scheduledTimerWithTimeInterval:
                                        animation_interval target:
                                        self selector:
                                        @selector(drawView) userInfo:
                                        nil repeats:
                                        YES];
}

- (void) stopAnimation
{
    self.animation_timer = nil;
}

- (void) setAnimationTimer:(NSTimer *)new_timer
{
    [animation_timer invalidate];
    animation_timer = new_timer;
}

- (void) setAnimationInterval:(NSTimeInterval)interval
{
    animation_interval = interval;

    if (animation_timer)
    {
        [self stopAnimation];
        [self startAnimation];
    }
}

- (void) dealloc
{
    [self stopAnimation];

    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];

    [context release];
    [super dealloc];
}

@end

#endif /* ENGINE_OS_IPHONE */
