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

#import "sys_iphone.h"
#import "eagl_view.h"

#include "common.h"

@implementation application_delegate

@synthesize window;
@synthesize glView;

- (void) applicationDidFinishLaunching:(UIApllication *)application
{
    engine_init();

    glView.animationInterval = 1.0 / 60.0;
    [glView startAnimation];
}

- (void) applicationWillResignActive:(UIApplication *)application
{
    glView.animationInterval = 1.0 / 5.0;
}

- (void) applicationDidBecomeActive:(UIApplication *)application
{
    glview.animationInterval = 1.0 / 60.0;
}

- (void) applicationWillTerminate:(UIApplication *)application
{
    engine_stop();
}

- (void) dealloc
{
    [glView release];
    [window release];
    [super dealloc];
}

@end

#endif /* ENGINE_OS_IPHONE */
