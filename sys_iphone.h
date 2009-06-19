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

#ifndef _SYS_IPHONE_H
#define _SYS_IPHONE_H

#import <UIKit/UIKit.h>

@class eagl_view;

@interface application_delegate : NSObject <UIApplicationDelegate>
{
    UIWindow  *window;
    eagl_view *glView;
}

@property (nonatomic, retain) IBOutlet UIWindow  *window;
@property (nonatomic, retain) IBOutlet eagl_view *glView;

#endif /* !_SYS_IPHONE_H */
