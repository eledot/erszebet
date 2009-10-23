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

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>
#import "eagl_view.h"
#import "common.h"
#import "gl_private.h"

@interface EAGLView()

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
        [self setMultipleTouchEnabled:YES];
    }

    return self;
}

- (void) drawView
{
    [EAGLContext setCurrentContext:context];

    glBindFramebufferOES(GL_FRAMEBUFFER_OES, buffer_frame);
    GLERROR();

    engine_frame();

    glBindRenderbufferOES(GL_RENDERBUFFER_OES, buffer_render);
    GLERROR();
    [context presentRenderbuffer:GL_RENDERBUFFER_OES];
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
    GLERROR();
    glGenRenderbuffersOES(1, &buffer_render);
    GLERROR();

    glBindFramebufferOES(GL_FRAMEBUFFER_OES, buffer_frame);
    GLERROR();
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, buffer_render);
    GLERROR();
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer *)self.layer];
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES,
                                 GL_COLOR_ATTACHMENT0_OES,
                                 GL_RENDERBUFFER_OES,
                                 buffer_render);
    GLERROR();

    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES,  &video_width);
    GLERROR();
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &video_height);
    GLERROR();

    sys_printf("== %i %i\n", video_width, video_height);

    glGenRenderbuffersOES(1, &buffer_depth);
    GLERROR();
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, buffer_depth);
    GLERROR();
    glRenderbufferStorageOES(GL_RENDERBUFFER_OES,
                             GL_DEPTH_COMPONENT16_OES,
                             video_width,
                             video_height);
    GLERROR();
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES,
                                 GL_DEPTH_ATTACHMENT_OES,
                                 GL_RENDERBUFFER_OES,
                                 buffer_depth);
    GLERROR();

    if (GL_FRAMEBUFFER_COMPLETE_OES != glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES))
    {
        GLERROR();
        sys_printf("failed to create framebuffer object\n");
        return NO;
    }

    return YES;
}

- (void) destroyFramebuffer
{
    glDeleteFramebuffersOES(1, &buffer_frame);
    GLERROR();
    glDeleteRenderbuffersOES(1, &buffer_render);
    GLERROR();
    glDeleteRenderbuffersOES(1, &buffer_depth);
    GLERROR();

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

static void apply_touches (NSSet *touches, int down)
{
    NSEnumerator *e = [touches objectEnumerator];
    UITouch      *touch;

    while (nil != (touch = [e nextObject]))
    {
        CGPoint point = [touch locationInView: [touch view]];
        g_call_func("g_touchpad_event", "iidd", 1, down, (double)point.x, (double)point.y);
        sys_printf("touch %i - %i %i\n", down, (int)point.x, (int)point.y);
    }
}

- (void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    apply_touches(touches, 1);
    event = nil;    
}

- (void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{    
    apply_touches(touches, 0);
    event = nil;
}

- (void) touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    apply_touches(touches, -1);
    event = nil;    
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
