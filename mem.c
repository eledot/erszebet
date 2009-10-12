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

#include "common.h"

#define SEN1_POOL  0xdead1337
#define SEN1_ALLOC 0x1337beef
#define SEN2_ALLOC 0xbeefdead

struct alloc_s;

typedef struct pool_s
{
    uint32_t sen1;
    char     name[9];
    uint32_t num_allocs;
    uint32_t size;
    uint32_t size_real;
    uint32_t size_max;
    struct alloc_s *allocs;

#ifdef ENGINE_MEM_DEBUG
    const char *file;
    const char *func;
    int         line;
#endif /* ENGINE_MEM_DEBUG */

    struct pool_s *next;
    struct pool_s *prev;
}pool_t;

typedef struct alloc_s
{
    uint32_t sen1;
    uint32_t size;
    pool_t  *pool;

#ifdef ENGINE_MEM_DEBUG
    const char *file;
    const char *func;
    int         line;
    double      time;
#endif /* ENGINE_MEM_DEBUG */

    struct alloc_s *next;
    struct alloc_s *prev;

    uint8_t   data[0];
}alloc_t;

static pool_t *pools = NULL;

/*
=================
mem_alloc_pool
=================
*/
mem_pool_t mem_alloc_pool_real (const char *name,
                                uint32_t    max_size
#ifdef ENGINE_MEM_DEBUG
                                ,
                                const char *file,
                                const char *func,
                                int         line
#endif /* ENGINE_MEM_DEBUG */
    )
{
    pool_t *p;

    if (NULL == (p = malloc(sizeof(*p))))
    {
        sys_printf("no memory for pool\n");
        return NULL;
    }

    memset(p, 0, sizeof(*p));

    strlcpy(p->name, name, sizeof(p->name));

    p->sen1      = SEN1_POOL;
    p->size_real = sizeof(*p);
    p->size_max  = max_size;

#ifdef ENGINE_MEM_DEBUG
    p->file = file;
    p->func = func;
    p->line = line;
#endif /* ENGINE_MEM_DEBUG */

    if (NULL != pools)
    {
        pools->prev = p;
        p->next = pools;
    }

    pools = p;

    return p;
}

/*
=================
mem_free_pool
=================
*/
void mem_free_pool_real (mem_pool_t *pool
#ifdef ENGINE_MEM_DEBUG
                         ,
                         const char *file,
                         const char *func,
                         int         line
#endif /* ENGINE_MEM_DEBUG */
    )
{
    pool_t  *p, *i;
    alloc_t *a;

    if (NULL == pool)
    {
        sys_printf("NULL pool address\n");
        return;
    }

    p = *pool;

    if (NULL == p)
    {
#ifndef ENGINE_MEM_DEBUG
        sys_printf("NULL pool (double free?)\n");
#else /* ENGINE_MEM_DEBUG */
        sys_printf("NULL pool (double free?) (%s - %s:%i)\n", func, file, line);
#endif /* !ENGINE_MEM_DEBUG */
        return;
    }

    if (SEN1_POOL != p->sen1)
    {
        p->name[sizeof(p->name) - 1] = 0;
        sys_printf("broken pool \"%s\" (%p)\n", p->name, p);
#ifdef ENGINE_MEM_DEBUG
        sys_printf("pool was allocated at %s - %s:%i\n", p->func, p->file, p->line);
#endif /* ENGINE_MEM_DEBUG */
        return;
    }

    while (NULL != p->allocs)
    {
#ifdef ENGINE_MEM_FREE_DEBUG_ALLOCS
        sys_printf("alloc at %s - %s:%i (%i bytes) freed\n", p->func, p->file, p->line, p->size);
#endif /* ENGINE_MEM_FREE_DEBUG_ALLOCS */

        a = p->allocs->next;
        mem_free(p->allocs->data);
        p->allocs = a;
    }

    for (i = pools; NULL != i ;i = i->next)
    {
        if (p == i)
        {
            if (NULL == p->prev)
                pools = p->next;

            if (NULL != p->next)
                p->next->prev = p->prev;

            if (NULL != p->prev)
                p->prev->next = p->next;

            free(p);
            return;
        }
    }

    sys_printf("pool (%p) wasn\'t found in pools list \"%s\"\n", p, p->name);

#ifdef ENGINE_MEM_DEBUG
    sys_printf("pool was created at %s - %s:%i\n", p->func, p->file, p->line);
#endif /* ENGINE_MEM_DEBUG */
}

/*
=================
mem_strcpy_static
=================
*/
char *mem_strdup_static_real (const char *src,
                              mem_pool_t  mempool
#ifdef ENGINE_MEM_DEBUG
                              ,
                              const char *file,
                              const char *func,
                              int         line
#endif /* ENGINE_MEM_DEBUG */
    )
{
    char *res;
    int   len = strlen(src) + 1;

#ifdef ENGINE_MEM_DEBUG
    res = mem_alloc_real(mempool, len, file, func, line);
#else
    res = mem_alloc_real(mempool, len);
#endif
    memcpy(res, src, len);

    return res;
}

/*
=================
mem_alloc_real
=================
*/
void *mem_alloc_real (mem_pool_t  pool,
                      uint32_t    size
#ifdef ENGINE_MEM_DEBUG
                      ,
                      const char *file,
                      const char *func,
                      int         line
#endif /* ENGINE_MEM_DEBUG */
    )
{
    uint32_t real;
    pool_t  *p = pool;
    alloc_t *a;

    if (NULL == p)
    {
#ifndef ENGINE_MEM_DEBUG
        sys_printf("NULL pool\n");
#else /* ENGINE_MEM_DEBUG */
        sys_printf("NULL pool (%s - %s:%i)\n", func, file, line);
#endif /* !ENGINE_MEM_DEBUG */
        return NULL;
    }

    if (SEN1_POOL != p->sen1)
    {
        p->name[sizeof(p->name) - 1] = 0;
        sys_printf("broken pool \"%s\" (%p)\n", p->name, p);
#ifdef ENGINE_MEM_DEBUG
        sys_printf("pool was allocated at %s - %s:%i\n", p->func, p->file, p->line);
#endif /* ENGINE_MEM_DEBUG */
        return NULL;
    }

    if (p->size_max && p->size_max < p->size + size)
    {
        sys_printf("pool \"%s\" overflow (%i bytes alloc failed)\n", p->name);
#ifdef ENGINE_MEM_DEBUG
        sys_printf("alloc was at %s - %s:%i\n", func, file, line);
#endif /* ENGINE_MEM_DEBUG */
        return NULL;
    }

    real = sizeof(*a) + size + sizeof(uint32_t);

    if (NULL == (a = malloc(real)))
    {
        sys_printf("failed to allocate %i bytes\n", real);
#ifdef ENGINE_MEM_DEBUG
        sys_printf("alloc was at %s - %s:%i\n", func, file, line);
#endif /* ENGINE_MEM_DEBUG */
    }

    memset(a, 0, real);

    p->size      += size;
    p->size_real += real;

    a->sen1 = SEN1_ALLOC;
    a->size = size;
    a->pool = p;

#ifdef ENGINE_MEM_DEBUG
    a->file = file;
    a->func = func;
    a->line = line;
    a->time = sys_get_time();
#endif /* ENGINE_MEM_DEBUG */

    *((uint32_t *)(a->data + size)) = SEN2_ALLOC;

    if (NULL != p->allocs)
    {
        p->allocs->prev = a;
        a->next = p->allocs;
    }

    p->allocs = a;

    return a->data;
}

/*
=================
mem_free
=================
*/
void mem_free_real (void       *m
#ifdef ENGINE_MEM_DEBUG
                    ,
                    const char *file,
                    const char *func,
                    int         line
#endif /* ENGINE_MEM_DEBUG */
    )
{
    alloc_t *i;
    alloc_t *a = m - sizeof(alloc_t);

    if (SEN1_ALLOC != a->sen1)
    {
        sys_printf("broken sen1 in alloc (%p)\n", a);
#ifdef ENGINE_MEM_DEBUG
        sys_printf("alloc was done at %s - %s:%i, t=%.5lf\n", a->func, a->file, a->line, a->time);
        sys_printf("tried to free memory at %s - %s:%i\n", func, file, line);
#endif /* ENGINE_MEM_DEBUG */
        return;
    }

    if (SEN2_ALLOC != *((uint32_t *)(a->data + a->size)))
    {
        sys_printf("broken sen2 in alloc (%p)\n", a);
#ifdef ENGINE_MEM_DEBUG
        sys_printf("alloc was done at %s - %s:%i, t=%.5lf\n", a->func, a->file, a->line, a->time);
        sys_printf("tried to free memory at %s - %s:%i\n", func, file, line);
#endif /* ENGINE_MEM_DEBUG */
        return;
    }

    for (i = a->pool->allocs; NULL != i ;i = i->next)
    {
        if (a == i)
        {
            if (NULL == a->prev)
                a->pool->allocs = a->next;

            if (NULL != a->next)
                a->next->prev = a->prev;

            if (NULL != a->prev)
                a->prev->next = a->next;

            a->pool->size      -= a->size;
            a->pool->size_real -= sizeof(*a) + a->size + sizeof(uint32_t);

            free(a);
            return;
        }
    }

    sys_printf("alloc (%p) wasn\'t found in pool \"%s\"\n", a, a->pool->name);

#ifdef ENGINE_MEM_DEBUG
    sys_printf("alloc was done at %s - %s:%i, t=%.5lf\n", a->func, a->file, a->line, a->time);
    sys_printf("tried to free memory at %s - %s:%i\n", func, file, line);
#endif /* ENGINE_MEM_DEBUG */
}

#ifdef ENGINE_MEM_DEBUG

/*
=================
mem_debug
=================
*/
void mem_debug (void)
{
    uint32_t pool_allocs;
    pool_t  *p;
    alloc_t *a;

    for (p = pools; NULL != p ;p = p->next)
    {
        sys_printf(">>> pool \"%s\" (%s - %s:%i): size=%u, size_real=%u, max=%u\n",
                   p->name,
                   p->func,
                   p->file,
                   p->line,
                   p->size,
                   p->size_real,
                   p->size_max);

        for (pool_allocs = 0, a = p->allocs; NULL != a ;a = a->next, pool_allocs++)
        {
            sys_printf("        alloc %p: size=%u\n", a, a->size);
        }

        sys_printf("<<< %u allocs in \"%s\"\n", pool_allocs, p->name);
    }
}

#endif /* ENGINE_MEM_DEBUG */
