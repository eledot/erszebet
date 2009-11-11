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
 * @file fs.h
 *
 * @brief File system module interface header
 *
 * @author ftrvxmtrx
 */

#ifndef _FS_H
#define _FS_H

/**
 * @brief Default game resources path
 */
#define FS_DEFAULT_BASE "/usr/share/games/"GAME_NAME

/**
 * @brief Default user-home directory name of the game
 */
#define FS_HOME_BASE    "."GAME_NAME

/**
 * @brief Default mod directory
 */
#define FS_DEFAULT_GAME "base"

/**
 * @brief File handle
 */
typedef void * fs_file_t;

/**
 * @brief fs_open mode
 */
typedef enum
{
    FS_RDONLY = 0,  /**< Open in read-only mode           */
    FS_WRONLY,      /**< Truncate/create, write-only mode */
    FS_APPEND,      /**< Open for appending               */
    FS_RDWR         /**< Open for read/write operations   */
}fs_modes_e;

/**
 * @brief fs_seek origins
 */
typedef enum
{
    FS_CURRENT = 0,  /**< From current position */
    FS_START,        /**< From the file start   */
    FS_END           /**< From the file end     */
}fs_origins_e;

/**
 * @brief Opens a file
 *
 * @param name File name
 * @param mode One of fs_modes_e
 * @param size Pointer to store file size (or NULL if you don't want it)
 * @param shout true if it should shout about failures
 *
 * @return File handle on success, NULL on error
 */
fs_file_t fs_open (const char *name, int mode, int *size, bool shout) GNUC_NONNULL_ARGS(1);

/**
 * @brief Closes a file
 *
 * @param f File handle
 */
void fs_close (fs_file_t f) GNUC_NONNULL;

/**
 * @brief Reads from file
 *
 * @param f File handle
 * @param buffer Buffer to read into
 * @param size Number of bytes to read
 *
 * @return Number of bytes actually read (< 0 on errors)
 */
int fs_read (fs_file_t f, void *buffer, int size) GNUC_NONNULL;

/**
 * @brief Writes to file
 *
 * @param f File handle
 * @param buffer Buffer to write
 * @param size Number of bytes to write
 *
 * @return Number of bytes actually written (-1 on error)
 */
int fs_write (fs_file_t f, const void *buffer, int size) GNUC_NONNULL;

/**
 * @brief Repositions the cursor in a file
 *
 * @param f File handle
 * @param offset Offset (any number)
 * @param origin One of fs_origins_e
 *
 * @return Number of bytes from start after the repositioning, -1 on error
 */
int fs_seek (fs_file_t f, int offset, int origin) GNUC_NONNULL;

/**
 * @brief Returns file cursor position
 *
 * @return Number of bytes from start to the cursor position, -1 on error
 */
int fs_tell (fs_file_t f) GNUC_NONNULL;

bool fs_mkdir (const char *name) GNUC_NONNULL;

bool fs_unlink (const char *name) GNUC_NONNULL;

bool fs_init (void);

void fs_shutdown (void);

#endif /* !_FS_H */
