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

#ifndef _EVENT_MAPS
#define _EVENT_MAPS

#include "keyboard.h"

static const unsigned char map_print[SDLK_LAST] =
{
/*             0     1     2     3     4     5     6     7     8     9    10    11    12    13    14    15 */
/*    0  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*    1  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*    2  */   ' ',  '!',  '\"', '#',  '$',   0,   '&',  '\'', '(',  ')',  '*',  '+',  ',',  '-',  '.',  '/',
/*    3  */   '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  ':',  ';',  '<',  '=',  '>',  '?',
/*    4  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*    5  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   '[',  '\\', ']',   0,   '_',
/*    6  */   '`',  'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',
/*    7  */   'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',   0,    0,    0,    0,    0,
/*    8  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*    9  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*   10  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*   11  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*   12  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*   13  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*   14  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*   15  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*   16  */   '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '.',  '/',  '*',  '-',  '+',   0,
/*   17  */   '=',   0
};

static const unsigned char map_print_shift[SDLK_LAST] =
{
/*             0     1     2     3     4     5     6     7     8     9    10    11    12    13    14    15 */
/*    0  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*    1  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*    2  */   ' ',  '!',  '\"', '#',  '$',   0,   '&',  '\"', '(',  ')',  '*',  '+',  '<',  '_',  '>',  '?',
/*    3  */   ')',  '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',  '(',  ':',  ':',  '<',  '+',  '>',  '?',
/*    4  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*    5  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   '{',  '|',  '}',   0,   '_',
/*    6  */   '~',  'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O',
/*    7  */   'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',   0,    0,    0,    0,    0,
/*    8  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*    9  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*   10  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*   11  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*   12  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*   13  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*   14  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*   15  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*   16  */   '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '.',  '/',  '*',  '-',  '+',   0,
/*   17  */   '=',   0
};

static const unsigned char map_normal[SDLK_LAST] =
{
/*             0     1     2     3     4     5     6     7     8     9    10    11    12    13    14    15 */
/*    0  */    0,    0,    0,    0,    0,    0,    0,    0, KEY_BACKSPACE, KEY_TAB, 0, 0, 0, KEY_ENTER, 0, 0,
/*    1  */    0,    0,    0, KEY_PAUSE, 0,  0,    0,    0,    0,    0,    0, KEY_ESCAPE, 0, 0,    0,    0,
/*    2  */   KEY_SPACE, 0, 0,   0,    0,    0,    0, KEY_QUOTE, 0, 0, 0,  0, KEY_COMMA, KEY_MINUS, KEY_PERIOD, KEY_SLASH,
/*    3  */    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    0, KEY_SEMICOLON, 0, KEY_EQUALS, 0, 0,
/*    4  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*    5  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, KEY_LEFTBRACKET, KEY_BACKSLASH, KEY_RIGHTBRACKET, 0, 0,
/*    6  */   KEY_BACKQUOTE, 1, 1, 1,  1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
/*    7  */    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    0,    0,    0,    0, KEY_DELETE,
/*    8  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*    9  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*   10  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*   11  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*   12  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*   13  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*   14  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*   15  */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/*   16  */   KEY_KP_0, KEY_KP_1, KEY_KP_2, KEY_KP_3, KEY_KP_4, KEY_KP_5, KEY_KP_6, KEY_KP_7, KEY_KP_8, KEY_KP_9, KEY_KP_PERIOD, KEY_KP_SLASH, KEY_KP_MULTIPLY, KEY_KP_MINUS, KEY_KP_PLUS, KEY_KP_ENTER,
/*   17  */   KEY_KP_EQUALS, KEY_UP, KEY_DOWN, KEY_RIGHT, KEY_LEFT, KEY_INSERT, KEY_HOME, KEY_END, KEY_PAGEUP, KEY_PAGEDOWN, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6,
/*   18  */   KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, KEY_F13, KEY_F14, KEY_F15, 0, 0, 0, KEY_NUMLOCK, KEY_CAPSLOCK, KEY_SCROLLLOCK, KEY_RSHIFT,
/*   19  */   KEY_LSHIFT, KEY_RCTRL, KEY_LCTRL, KEY_RALT, KEY_LALT, KEY_RMETA, KEY_LMETA, KEY_LSUPER, KEY_RSUPER, 0, 0, 0, KEY_PRINT, 0, KEY_BREAK, KEY_MENU,
/*   20  */   KEY_POWER, KEY_EURO, 0
};

#endif /* !_EVENT_MAPS */
