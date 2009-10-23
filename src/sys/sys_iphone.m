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

#import "sys/sys_iphone.h"
#import "eagl_view.h"

#import "engine.h"

@implementation erszebet_iphoneAppDelegate

@synthesize window;
@synthesize glView;

- (void) applicationDidFinishLaunching:(UIApplication *)application
{
    errno = 0;

    glView.animation_interval = 1.0 / 60.0;
    [glView createFramebuffer];
    [glView startAnimation];

    engine_start();
}

- (void) applicationWillResignActive:(UIApplication *)application
{
    glView.animation_interval = 1.0 / 5.0;
}

- (void) applicationDidBecomeActive:(UIApplication *)application
{
    glView.animation_interval = 1.0 / 60.0;
}

- (void) applicationWillTerminate:(UIApplication *)application
{
    engine_stop();
    [glView destroyFramebuffer];
}

- (void) dealloc
{
    [glView release];
    [window release];
    [super dealloc];
}

@end

/*
=================
main
=================
*/
int main (int argc, char **argv)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    sys_arg_set(argc, argv);

    int res = UIApplicationMain(argc, argv, nil, nil);
    [pool release];

    return res;
}

#endif /* ENGINE_OS_IPHONE */
