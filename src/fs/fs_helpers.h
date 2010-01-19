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
 * @file fs_helpers.h
 *
 * @brief FS module helpers header
 *
 * @author ftrvxmtrx
 */

#ifndef _FS_HELPERS_H
#define _FS_HELPERS_H

/**
 * @brief Opens, reads and closes a file
 *
 * @param name File name
 * @param buffer Pointer to buffer to allocate and fill with data
 * @param max_size Maximal size of buffer to allocate and data to read or <= 0 to load file of any size.
 * @param pool Memory pool to use for buffer allocation
 * @param shout true if it should shout about failures
 *
 * @return Size of data on success, <0 on failure
 */
int fs_open_read_close (const char *name, void **buffer, int max_size, mem_pool_t pool, erbool shout)
    GNUC_NONNULL;

/**
 * @brief Opens/creates/truncates, writes and closes a file
 *
 * @param name File name
 * @param buffer Buffer to write to the file
 * @param size Buffer size
 * @param shout true if it should shout about failures
 *
 * @return Size of written data on success, <0 on failure
 */
int fs_open_write_close (const char *name, const void *buffer, int size, erbool shout)
    GNUC_NONNULL;

int fs_get_size (fs_file_t f) GNUC_NONNULL;

int fs_eof (fs_file_t) GNUC_NONNULL;

int fs_file_exists (const char *name) GNUC_NONNULL;

int fs_printf (fs_file_t f, const char *format, ...)
    GNUC_PRINTF(2, 3);

#endif /* !_FS_HELPERS_H */
