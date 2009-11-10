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

/**
 * @file keyboard.h
 *
 * @brief Keyboard input interface header
 *
 * @author ftrvxmtrx
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

/**
 * @brief Some special keys
 */
typedef enum
{
    KEY_UNKNOWN = 0,
    KEY_ALPHANUM = 1, /* [0-9a-z] */
    KEY_BACKSPACE,
    KEY_TAB,
    KEY_DELETE,
    KEY_ENTER,
    KEY_PAUSE,
    KEY_ESCAPE,
    KEY_SPACE,
    KEY_QUOTE,
    KEY_PLUS,
    KEY_COMMA,
    KEY_MINUS,
    KEY_PERIOD,
    KEY_SLASH,
    KEY_SEMICOLON,
    KEY_EQUALS,
    KEY_LEFTBRACKET,
    KEY_BACKSLASH,
    KEY_RIGHTBRACKET,
    KEY_BACKQUOTE,
    KEY_KP_0,
    KEY_KP_1,
    KEY_KP_2,
    KEY_KP_3,
    KEY_KP_4,
    KEY_KP_5,
    KEY_KP_6,
    KEY_KP_7,
    KEY_KP_8,
    KEY_KP_9,
    KEY_KP_PERIOD,
    KEY_KP_SLASH,
    KEY_KP_MULTIPLY,
    KEY_KP_MINUS,
    KEY_KP_PLUS,
    KEY_KP_ENTER,
    KEY_KP_EQUALS,
    KEY_UP,
    KEY_DOWN,
    KEY_RIGHT,
    KEY_LEFT,
    KEY_INSERT,
    KEY_HOME,
    KEY_END,
    KEY_PAGEUP,
    KEY_PAGEDOWN,
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,
    KEY_F13,
    KEY_F14,
    KEY_F15,
    KEY_RSHIFT,
    KEY_LSHIFT,
    KEY_RALT,
    KEY_LALT,
    KEY_RCTRL,
    KEY_LCTRL,
    KEY_RMETA,
    KEY_LMETA,
    KEY_RSUPER,
    KEY_LSUPER,
    KEY_PRINT,
    KEY_BREAK,
    KEY_MENU,
    KEY_POWER,
    KEY_EURO,
    KEY_NUMLOCK,
    KEY_CAPSLOCK,
    KEY_SCROLLLOCK,

    KEY_INVALID
}keys_e;

/**
 * @brief Mod flags
 */
typedef enum
{
    KEYMOD_SHIFT = (1 << 0),
    KEYMOD_CTRL  = (1 << 1),
    KEYMOD_ALT   = (1 << 2),
    KEYMOD_META  = (1 << 3)
}key_mods_e;

/**
 * @brief Toggles key repeat
 *
 * @param repeat true to turn on key repeats, false to switch it off
 */
void key_set_repeat (bool repeat);

/**
 * @brief Receives keyvoard-related events
 *
 * @param printable Printable version of the key pressed ('a' or '5', for ex.)
 * @param printable_shift Just as one before, but uppercase ('A' or '5', for ex.)
 * @param normal One of the keys_e
 * @param mod Mod flags (key_mods_e)
 * @param down true - 'down' event, false - 'up' event
 */
void key_event (int printable, int printable_shift, int normal, int mod, bool is_down);

bool keyboard_init (void);
void keyboard_shutdown (void);

#endif /* !_KEYBOARD_H */
