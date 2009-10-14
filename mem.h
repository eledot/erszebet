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

#ifndef _MEM_H
#define _MEM_H

typedef void * mem_pool_t;

#ifdef ENGINE_MEM_DEBUG
mem_pool_t mem_alloc_pool_real (const char *name,
                                uint32_t    max_size,
                                const char *file,
                                const char *func,
                                int         line)
    GNUC_MALLOC;
void mem_free_pool_real (mem_pool_t *pool,
                         const char *file,
                         const char *func,
                         int         line)
    GNUC_NONNULL;
void *mem_alloc_real (mem_pool_t  pool,
                      uint32_t    size,
                      const char *file,
                      const char *func,
                      int         line)
    GNUC_MALLOC;
void mem_debug (void);
char *mem_strdup_static_real (const char *src,
                              mem_pool_t  mempool,
                              const char *file,
                              const char *func,
                              int         line)
    GNUC_NONNULL;
void mem_free_real (void       *m,
                    const char *file,
                    const char *func,
                    int         line)
    GNUC_NONNULL;

#define mem_alloc_pool(name, max) mem_alloc_pool_real((name), (max), __FILE__, __FUNCTION__, __LINE__)
#define mem_alloc(pool, size) mem_alloc_real((pool), (size), __FILE__, __FUNCTION__, __LINE__)
#define mem_strdup_static(s) mem_strdup_static_real((s), mempool, __FILE__, __FUNCTION__, __LINE__)
#define mem_free_pool(pool) mem_free_pool_real((pool), __FILE__, __FUNCTION__, __LINE__)
#define mem_free(m) mem_free_real((m), __FILE__, __FUNCTION__, __LINE__)
#else /* !ENGINE_MEM_DEBUG */
mem_pool_t mem_alloc_pool_real (const char *name, uint32_t max_size)
    GNUC_MALLOC;
void mem_free_pool_real (mem_pool_t *pool)
    GNUC_NONNULL;
void *mem_alloc_real (mem_pool_t pool, uint32_t size)
    GNUC_MALLOC;
char *mem_strdup_static_real (const char *src, mem_pool_t mempool)
    GNUC_MALLOC;
void mem_free_real (void *m)
    GNUC_NONNULL;

#define mem_alloc_pool(name, max) mem_alloc_pool_real((name), (max))
#define mem_alloc mem_alloc_real
#define mem_strdup_static(s) mem_strdup_static_real((s), mempool)
#define mem_free_pool(pool) mem_free_pool_real((pool))
#define mem_free mem_free_real
#define mem_debug()
#endif /* ENGINE_MEM_DEBUG */


#define mem_alloc_static_pool(name, max) mempool = mem_alloc_pool((name), (max))
#define mem_alloc_static(size) mem_alloc(mempool, size)

#define mem_free_static_pool() mem_free_pool(&mempool)

#endif /* !_MEM_H */
