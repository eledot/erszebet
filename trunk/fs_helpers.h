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
 * @param max_size Maximal size of buffer to allocate and data to read
 * @param pool Memory pool to use for buffer allocation
 * @param shout Non-zero to shout about failures
 *
 * @return 0 on success
 */
int fs_open_read_close (const char *name, void **buffer, int max_size, mem_pool_t pool, int shout);

/**
 * @brief Opens/creates/truncates, writes and closes a file
 *
 * @param name File name
 * @param buffer Buffer to write to the file
 * @param size Buffer size
 * @param shout Non-zero to shout about failures
 *
 * @return 0 on success
 */
int fs_open_write_close (const char *name, const void *buffer, int size, int shout);

int fs_get_size (fs_file_t f);

int fs_eof (fs_file_t);

int fs_file_exists (const char *name);

int fs_printf (fs_file_t f, const char *format, ...);

#endif /* !_FS_HELPERS_H */
