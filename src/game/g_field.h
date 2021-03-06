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

#ifndef _G_FIELD_H
#define _G_FIELD_H

typedef enum
{
    G_FIELD_TYPE_DOUBLE = 0,
    G_FIELD_TYPE_INTEGER,
    G_FIELD_TYPE_VECTOR,
    G_FIELD_TYPE_STRING,
    G_FIELD_TYPE_STRING_COPY,
    G_FIELD_TYPE_ERBOOL,
    G_FIELD_TYPE_CUSTOM_CALLBACK,
    G_FIELD_TYPES_NUM
}ent_fields_types_e;

typedef struct g_field_s
{
    const char * const name;

    const union
    {
        const double  DOUBLE;
        const int     INTEGER;
        const double *VECTOR;
        const char   *STRING;
        const char   *STRING_COPY;
        const erbool    ERBOOL;
        const void   *CUSTOM_CALLBACK;
    }default_value;

    int       index;
    const int offset;
    const int type;

    void (* const callback) (void *data);

    struct g_field_s *next;
}g_field_t;

#define G_FIELD(_f_name, _f_real, _f_type, _default_value, _callback)   \
    {                                                            \
        .name = _f_name,                                         \
        .default_value._f_type = _default_value,                 \
        .offset = offsetof(STRUCTURE_FOR_OFFSETS, _f_real),      \
        .type = G_FIELD_TYPE_##_f_type,                          \
        .callback = (const void *)_callback                      \
    }

#define G_FIELD_NULL                                                    \
    {                                                                   \
        .name = NULL,                                                   \
        .offset = 0,                                                    \
        .type =  -1,                                                    \
        .callback = NULL                                                \
    }

const char *g_field_type_to_string (int type) GNUC_CONST;
int g_field_value_to_string (const void *data, const g_field_t *field, char *buffer, int size);

void g_fields_set_default_values (void *data, const g_field_t *fields) GNUC_NONNULL;
void g_fields_free_values (void *data, const g_field_t *fields) GNUC_NONNULL;
void g_fields_pop (void *data, const g_field_t *fields, erbool check) GNUC_NONNULL;
void g_fields_push (const void *data, const g_field_t *fields) GNUC_NONNULL;

#endif /* !_G_FIELD_H */
