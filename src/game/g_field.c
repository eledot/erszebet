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

#include "game/g_private.h"

/*
=================
g_field_type_to_string
=================
*/
const char *g_field_type_to_string (int type)
{
    const char const *field_types[] =
    {
        [G_FIELD_TYPE_DOUBLE] = "double",
        [G_FIELD_TYPE_INTEGER] = "integer",
        [G_FIELD_TYPE_VECTOR] = "vector",
        [G_FIELD_TYPE_STRING] = "string_callback",
        [G_FIELD_TYPE_STRING_COPY] = "string_copy",
        [G_FIELD_TYPE_BOOL] = "bool",
        [G_FIELD_TYPE_CUSTOM_CALLBACK] = "custom_callback"
    };

    if (type < 0 || type >= G_FIELD_TYPES_NUM)
        return "INVALID";

    return field_types[type];
}

/*
=================
g_field_value_to_string
=================
*/
int g_field_value_to_string (const void *data, const g_field_t *field, char *buffer, int size)
{
    const void *value = data + field->offset;
    int len = 0;

    switch (field->type)
    {
    case G_FIELD_TYPE_DOUBLE:
        len = snprintf(buffer, size, "%-2.2lf", *(const double *)value);
        break;

    case G_FIELD_TYPE_INTEGER:
        len = snprintf(buffer, size, "%i", *(const int *)value);
        break;

    case G_FIELD_TYPE_VECTOR:
        len = snprintf(buffer, size, "{ %-2.2lf, %-2.2lf, %-2.2lf }",
                       ((const double *)value)[0],
                       ((const double *)value)[1],
                       ((const double *)value)[2]);
        break;

    case G_FIELD_TYPE_STRING:
    case G_FIELD_TYPE_STRING_COPY:
        if (NULL == *(const char **)value)
            len = snprintf(buffer, size, "null");
        else
            len = snprintf(buffer, size, "\"%s\"", *(const char **)value);
        break;

    case G_FIELD_TYPE_BOOL:
        len = snprintf(buffer, size, (*(const bool *)value) ? "true" : "false");
        break;

    default:
        break;
    }

    return len;
}

/*
=================
g_fields_set_default_values
=================
*/
void g_fields_set_default_values (void *data, const g_field_t *fields)
{
    const g_field_t *f;

    for (f = fields; NULL != f->name ;f++)
    {
        void *field;

        if (G_FIELD_TYPE_CUSTOM_CALLBACK == f->type)
            continue;

        field = data + f->offset;

        switch (f->type)
        {
        case G_FIELD_TYPE_DOUBLE:
            *(double *)field = f->default_value.DOUBLE;
            break;

        case G_FIELD_TYPE_INTEGER:
            *(int *)field = f->default_value.INTEGER;
            break;

        case G_FIELD_TYPE_VECTOR:
            if (NULL != f->default_value.VECTOR)
            {
                ((double *)field)[0] = f->default_value.VECTOR[0];
                ((double *)field)[1] = f->default_value.VECTOR[1];
                ((double *)field)[2] = f->default_value.VECTOR[2];
            }
            break;

        case G_FIELD_TYPE_STRING:
        case G_FIELD_TYPE_STRING_COPY:
            /* NULL by default */
            break;

        case G_FIELD_TYPE_BOOL:
            *(bool *)field = f->default_value.BOOL;
            break;

        default:
            sys_printf("unknown field type %i (%p->%i)\n", f->type, data, f->offset);
            break;
        }
    }
}

/*
=================
g_fields_free_values
=================
*/
void g_fields_free_values (void *data, const g_field_t *fields)
{
    const g_field_t *f;

    for (f = fields; NULL != f->name ;f++)
    {
        if (G_FIELD_TYPE_STRING_COPY == f->type)
        {
            void **ptr = ((void **)(data + f->offset));

            if (NULL != *ptr)
            {
                mem_free(*ptr);
                *ptr = NULL;
            }
        }
    }
}

/*
=================
g_fields_pop
=================
*/
void g_fields_pop (void *data, const g_field_t *fields, bool check)
{
    const g_field_t *f;

    for (f = fields; NULL != f->name ;f++)
    {
        if (G_FIELD_TYPE_CUSTOM_CALLBACK != f->type)
        {
            lua_pushstring(lua_state, f->name);
            lua_gettable(lua_state, -2);
            g_pop_field(data, f->offset, f->type, -1, check);
            lua_pop(lua_state, 1);
        }
    }
}

/*
=================
g_fields_push
=================
*/
void g_fields_push (const void *data, const g_field_t *fields)
{
    const g_field_t *f;

    for (f = fields; NULL != f->name ;f++)
    {
        if (G_FIELD_TYPE_CUSTOM_CALLBACK != f->type)
        {
            g_push_field(data, f->offset, f->type);
            lua_setfield(lua_state, -2, f->name);
        }
    }
}
