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

#ifndef _CVAR_H
#define _CVAR_H

#define CVAR_MAX_STR 64

/**
 * @brief cvar flags
 */
typedef enum
{
    CVAR_FL_SAVE   = (1 << 0),  /**< Cvar is to be saved to config */
    CVAR_FL_HIDDEN = (1 << 1),  /**< Cvar is hidden from user      */
    CVAR_FL_RDONLY = (1 << 2),  /**< Cvar can't be changed by user */
    CVAR_FL_MIN    = (1 << 3),  /**< Use minimal value             */
    CVAR_FL_MAX    = (1 << 4),  /**< Use maximal value             */
    CVAR_FL_SERVER = (1 << 5)   /**< This is server-side cvar      */
}cvar_flags_e;

#define CVAR_FL_MINMAX (CVAR_FL_MIN | CVAR_FL_MAX)

struct cvar_s;

typedef void (*cvar_callback_t) (const struct cvar_s *cvar);

/**
 * @brief cvar structure
 */
typedef struct cvar_s
{
    char  name[MISC_MAX_VARNAME];  /**< Cvar name             */

    int   i;                       /**< Integer value         */
    float f;                       /**< Float value           */
    char  s[CVAR_MAX_STR];         /**< String value          */

    float min;                     /**< Minimal value         */
    float max;                     /**< Maximal value         */
    int   flags;                   /**< Cvar flags            */

    cvar_callback_t callback;      /**< Called on each change */

    const char *help;              /**< Help message          */

    struct cvar_s *next;
}cvar_t;

/**
 * @brief searches for a cvar by it's name
 */
cvar_t *cvar_find (const char *name) GNUC_NONNULL;

/**
 * @brief sets cvar with new value
 */
void cvar_set (cvar_t *c, const char *str) GNUC_NONNULL;

void cvar_set_min (cvar_t *c, float min) GNUC_NONNULL;
void cvar_set_max (cvar_t *c, float max) GNUC_NONNULL;

#define cvar_set_minmax(c, min, max) { cvar_set_min((c), (min)); cvar_set_max((c), (max)); }

/**
 * @brief creates a cvar
 */
cvar_t *cvar_get (const char *name, const char *str, int flags) GNUC_NONNULL;

/**
 * @brief initializes cvar module
 */
int cvar_init (void);

/**
 * @brief deinitializes cvar module
 */
void cvar_shutdown (void);

#endif /* !_CVAR_H */
