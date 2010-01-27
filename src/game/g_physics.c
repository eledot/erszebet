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

#include "game/g_physics_private.h"

#define PHYSICS_STEPS 3
#define PHYSICS_MIN_DELTA 0.018

typedef struct g_physics_data_s
{
    /* internal fields */
    cpBody   *body;
    cpShape **shapes;
    int       shapes_num;

    erbool is_static;
    erbool is_solid;

    /* fields accessible by lua code */
    double velocity[3];
    double rotation;
    double gravity;
    double elasticity;
    double friction;
    double mass;
    double inertia;

    int group;
    cpLayers layers;
}g_physics_data_t;

static cpSpace *physics_space;
static double   physics_last_update;
static double   physics_speed;
static int      point_query_shapes_num;

static void physics_common_callback (g_entity_t *ent) GNUC_NONNULL;
static void physics_shape_parm_change_callback (g_entity_t *ent) GNUC_NONNULL;
static void physics_set_body_callback (g_entity_t *ent) GNUC_NONNULL;

static g_field_t ent_fields_physics[] =
{
#define STRUCTURE_FOR_OFFSETS g_physics_data_t
    G_FIELD("solid",          is_solid,    ERBOOL,   true,         NULL),
    G_FIELD("velocity",       velocity,    VECTOR,   NULL,         &physics_common_callback),
    G_FIELD("velocity_x",     velocity[0], DOUBLE,   0.0,          &physics_common_callback),
    G_FIELD("velocity_y",     velocity[1], DOUBLE,   0.0,          &physics_common_callback),
    G_FIELD("velocity_z",     velocity[2], DOUBLE,   0.0,          &physics_common_callback),
    G_FIELD("rotation",       rotation,    DOUBLE,   0.0,          &physics_common_callback),
    G_FIELD("gravity",        gravity,     DOUBLE,   0.0,          &physics_common_callback),
    G_FIELD("elasticity",     elasticity,  DOUBLE,   0.0,          &physics_shape_parm_change_callback),
    G_FIELD("friction",       friction,    DOUBLE,   0.6,          &physics_shape_parm_change_callback),
    G_FIELD("mass",           mass,        DOUBLE,   1.0,          &physics_common_callback),
    G_FIELD("inertia",        inertia,     DOUBLE,   100.0,        &physics_common_callback),
    G_FIELD("physics_group",  group,       INTEGER,  0,            &physics_shape_parm_change_callback),
    G_FIELD("physics_layers", layers,      INTEGER,  (cpLayers)-1, &physics_shape_parm_change_callback),
    G_FIELD_NULL
};

static g_field_t ent_field_physics_body =
    G_FIELD("body", body, CUSTOM_CALLBACK, NULL, &physics_set_body_callback);

extern const g_physics_shape_type_t g_physics_shape_type_segment;
extern const g_physics_shape_type_t g_physics_shape_type_circle;
extern const g_physics_shape_type_t g_physics_shape_type_polygon;

static const g_physics_shape_type_t * const g_physics_shape_types[] =
{
    &g_physics_shape_type_segment,
    &g_physics_shape_type_circle,
    &g_physics_shape_type_polygon
};

enum { g_physics_shape_types_num = STSIZE(g_physics_shape_types) };

/*
=================
g_physics_add_shape
=================
*/
GNUC_NONNULL static void g_physics_add_shape (g_entity_t *ent, cpShape *shape)
{
    g_physics_data_t *data = ent->physics_data;

    if (NULL == shape)
        return;

    data->shapes[data->shapes_num++] = shape;
    shape->data = ent;
    shape->group = data->group;
    shape->layers = data->layers;

    if (data->is_static)
        cpSpaceAddStaticShape(physics_space, shape);
    else
        cpSpaceAddShape(physics_space, shape);

    cpResetShapeIdCounter();
}

/*
=================
g_physics_delete_shapes
=================
*/
GNUC_NONNULL static void g_physics_delete_shapes (g_entity_t *ent)
{
    g_physics_data_t *data = ent->physics_data;
    int i;

    if (NULL == data->shapes)
        return;

    if (data->is_static)
    {
        for (i = 0; i < data->shapes_num ;i++)
        {
            if (NULL != data->shapes[i])
            {
                cpSpaceRemoveStaticShape(physics_space, data->shapes[i]);
                cpShapeFree(data->shapes[i]);
            }
        }
    }
    else
    {
        for (i = 0; i < data->shapes_num ;i++)
        {
            if (NULL != data->shapes[i])
            {
                cpSpaceRemoveShape(physics_space, data->shapes[i]);
                cpShapeFree(data->shapes[i]);
            }
        }
    }

    data->shapes_num = 0;
}

/*
=================
g_physics_mem_free
=================
*/
GNUC_NONNULL void g_physics_mem_free (g_entity_t *ent)
{
    g_physics_data_t *data = ent->physics_data;

    if (NULL == data)
        return;

    g_physics_delete_shapes(ent);

    /* remove body */
    cpSpaceRemoveBody(physics_space, data->body);
    cpBodyFree(data->body);

    g_fields_free_values(data, ent_fields_physics);

    mem_free(data);
    ent->physics_data = NULL;
}

/*
=================
g_physics_new_obj
=================
*/
GNUC_NONNULL static void g_physics_new_obj (g_entity_t *ent, int shapes_num, erbool is_static)
{
    g_physics_data_t *data;

    g_physics_mem_free(ent);
    data = ent->physics_data = mem_alloc(g_mempool, sizeof(g_physics_data_t) + sizeof(void *) * shapes_num);

    if (NULL == data)
        return;

    data->shapes = (void *)ent->physics_data + sizeof(g_physics_data_t);
    data->is_static = is_static;

    if (data->is_static)
    {
        data->body = cpBodyNew(INFINITY, INFINITY);
        cpSpaceAddBody(physics_space, data->body);
    }
    else
    {
        data->body = cpBodyNew(data->mass, data->inertia);
        cpSpaceAddBody(physics_space, data->body);
    }

    data->body->data = ent;
}

/*
=================
g_physics_point_query
=================
*/
GNUC_NONNULL static void g_physics_point_query (cpShape *shape, void (*callback) (g_entity_t *shape))
{
    callback(shape->data);
}

/*
=================
g_physics_update_ent
=================
*/
GNUC_NONNULL static void g_physics_update_ent (cpBody *body, GNUC_UNUSED void *unused)
{
    g_entity_t *ent = body->data;
    g_physics_data_t *data = ent->physics_data;

    if (data->shapes_num < 1)
        return;

    ent->origin[0] = body->p.x;
    ent->origin[1] = body->p.y;
    ent->angle     = body->a;

    data->velocity[0] = body->v.x;
    data->velocity[1] = body->v.y;
    data->rotation    = body->w;
    data->gravity     = -body->gravity;
    data->mass        = body->m;
    data->inertia     = body->i;

    data->elasticity = data->shapes[0]->e;
    data->friction   = data->shapes[0]->u;
}

/*
=================
g_physics_frame
=================
*/
void g_physics_frame (void)
{
    int    i;
    double dt = (g_time - physics_last_update) / (double)PHYSICS_STEPS;

    if (dt < PHYSICS_MIN_DELTA)
        return;

    physics_last_update = g_time;

    for (i = 0; i < PHYSICS_STEPS ;i++)
    {
        cpSpaceStep(physics_space, dt * physics_speed);
    }

    cpSpaceEachBody(physics_space, &g_physics_update_ent, NULL);
}

/*
=================
g_physics_touch
=================
*/
GNUC_NONNULL_ARGS(1, 3, 4) static erbool g_physics_touch (g_entity_t *self,
                                                        g_entity_t *other,
                                                        const double *origin,
                                                        const double *normal)
{
    erbool ret = false;

    lua_getref(lua_state, self->lua_dataref);
    lua_getfield(lua_state, -1, "touch");

    if (lua_isfunction(lua_state, -1))
    {
        lua_getref(lua_state, self->lua_ref);

        if (NULL == other)
            lua_pushnil(lua_state);
        else
            lua_getref(lua_state, other->lua_ref);

        g_push_vector(origin, 3);
        g_push_vector(normal, 3);

        g_lua_call(4, 1);
        ret = lua_toboolean(lua_state, 0);
    }

    lua_pop(lua_state, 2);

    return ret;
}

/*
=================
g_physics_collision
=================
*/
GNUC_NONNULL static int g_physics_collision (cpShape   *a,
                                             cpShape   *b,
                                             cpContact *contacts,
                                             int        num_contacts,
                                             cpFloat    normal_coef,
                                             GNUC_UNUSED void *data)
{
    int i;
    erbool valid;
    const g_physics_data_t *apd;
    const g_physics_data_t *bpd;

    if (!g_entity_is_valid(a->data) || !g_entity_is_valid(b->data))
        return 0;

    apd = ((const g_entity_t *)a->data)->physics_data;
    bpd = ((const g_entity_t *)b->data)->physics_data;

    for (i = 0, valid = true; i < num_contacts && valid ;i++)
    {
        erbool atouch_blocked = true;
        erbool btouch_blocked = true;
        double origin[3];
        double normal[3];

        origin[0] = contacts[i].p.x;
        origin[1] = contacts[i].p.y;
        origin[2] = 0.0;
        normal[0] = normal_coef * contacts[i].n.x;
        normal[1] = normal_coef * contacts[i].n.y;
        normal[2] = 0.0;

        valid = g_entity_is_valid(a->data) && g_entity_is_valid(b->data);

        if (valid && bpd->is_solid)
            atouch_blocked = g_physics_touch(a->data, b->data, origin, normal);

        valid = g_entity_is_valid(a->data) && g_entity_is_valid(b->data);

        if (valid && apd->is_solid)
            btouch_blocked = g_physics_touch(b->data, a->data, origin, normal);

        if (!(!(atouch_blocked & btouch_blocked) || (!apd->is_solid || !bpd->is_solid)))
            return 1;
    }

    return 0;
}

/*
=================
g_physics_update_ent_origin_angle
=================
*/
void g_physics_update_ent_origin_angle (g_entity_t *ent)
{
    g_physics_data_t *data = ent->physics_data;

    if (NULL == data)
        return;

    cpBodySetPos(data->body, cpv(ent->origin[0], ent->origin[1]));
    cpBodySetAngle(data->body, ent->angle);

    /* rehash static objects */
    if (data->is_static)
        cpSpaceRehashStatic(physics_space);
}

/*
=================
physics_common_callback
=================
*/
static void physics_common_callback (g_entity_t *ent)
{
    g_physics_data_t *data = ent->physics_data;

    data->body->v = cpv(data->velocity[0], data->velocity[1]);
    data->body->w = data->rotation; /* FIXME */
    data->body->t = 0.0; /* FIXME */
    data->body->gravity = -data->gravity;

    if (!data->is_static)
    {
        cpBodySetMass(data->body, data->mass);
        cpBodySetMoment(data->body, data->inertia);
    }
}

/*
=================
physics_shape_parm_change_callback
=================
*/
static void physics_shape_parm_change_callback (g_entity_t *ent)
{
    g_physics_data_t *data = ent->physics_data;
    cpShape *shape;
    int i;

    for (i = 0; i < data->shapes_num ;i++)
    {
        shape = data->shapes[i];
        shape->e = data->elasticity;
        shape->u = data->friction;
        shape->group = data->group;
        shape->layers = data->layers;
    }
}

/*
=================
physics_set_body_callback
=================
*/
static void physics_set_body_callback (g_entity_t *ent)
{
    g_physics_data_t *data;
    int i, type, shapes_num;
    erbool is_static = false;

    g_physics_mem_free(ent);

    if (lua_isnil(lua_state, lua_entity_value_index))
        return;

    if (!lua_istable(lua_state, lua_entity_value_index))
    {
        sys_printf("physical body not a table\n");
        return;
    }

    shapes_num = lua_objlen(lua_state, lua_entity_value_index);

    if (shapes_num < 1)
        return;

    lua_getfield(lua_state, lua_entity_value_index, "static");
    is_static = lua_toboolean(lua_state, -1);
    lua_pop(lua_state, 1);

    g_physics_new_obj(ent, shapes_num, is_static);
    data = ent->physics_data;

    g_fields_set_default_values(ent->physics_data, ent_fields_physics);
    lua_getref(lua_state, ent->lua_dataref);
    g_fields_pop(ent->physics_data, ent_fields_physics, true);
    lua_pop(lua_state, 1);

    for (i = 0; i < shapes_num ;i++)
    {
        /* push subtable (a shape) */
        lua_rawgeti(lua_state, lua_entity_value_index, i + 1);

        if (lua_istable(lua_state, -1))
        {
            /* parse shape */
            lua_getfield(lua_state, -1, "type");
            type = lua_tonumber(lua_state, -1);
            lua_pop(lua_state, 1);

            if (type >= 1 && type <= g_physics_shape_types_num)
            {
                g_physics_add_shape(ent, g_physics_shape_types[type - 1]->create(data->body));
            }
            else
            {
                sys_printf("unknown shape type %i\n", type);
            }
        }

        /* pop subtable */
        lua_pop(lua_state, 1);
    }

    if (data->shapes_num < 1)
        g_physics_mem_free(ent);
}

/*
=================
phys_lua_point_query_callback
=================
*/
GNUC_NONNULL static void phys_lua_point_query_callback (g_entity_t *ent)
{
    point_query_shapes_num++;
    lua_pushinteger(lua_state, point_query_shapes_num);

    if (NULL == ent)
        lua_pushnil(lua_state);
    else
        lua_getref(lua_state, ent->lua_ref);

    lua_settable(lua_state, -3);
}

/*
=================
phys_lua_point_query
=================
*/
GNUC_NONNULL static int phys_lua_point_query (lua_State *lst)
{
    double point[3];

    point_query_shapes_num = 0;
    g_pop_vector(1, point, 3);

    lua_newtable(lst);

    cpSpacePointQuery(physics_space,
                      cpv(point[0], point[1]),
                      (cpLayers)-1,
                      0,
                      (cpSpacePointQueryFunc)&g_physics_point_query,
                      &phys_lua_point_query_callback);

    return 1;
}

/*
=================
phys_lua_apply_impulse
=================
*/
GNUC_NONNULL static int phys_lua_apply_impulse (lua_State *lst)
{
    g_physics_data_t *data;
    double point[2], impulse[2];
    g_entity_t *ent;

    lua_getfield(lst, 1, "__ref");
    ent = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == ent)
    {
        sys_printf("called \"physics_apply_impulse\" without entity\n");
        return 0;
    }

    data = ent->physics_data;

    if (NULL == data)
    {
        sys_printf("tried to apply impulse on entity without physics\n");
        return 0;
    }

    g_pop_vector(2, point, 2);
    g_pop_vector(3, impulse, 2);

    cpBodyApplyImpulse(data->body, cpv(impulse[0], impulse[1]), cpv(point[0], point[1]));

    return 0;
}

/*
=================
phys_lua_set_gravity
=================
*/
GNUC_NONNULL static int phys_lua_set_gravity (lua_State *lst)
{
    physics_space->gravity = cpv(0.0, -luaL_checknumber(lst, 1));

    return 0;
}

/*
=================
phys_lua_get_gravity
=================
*/
GNUC_NONNULL static int phys_lua_get_gravity (lua_State *lst)
{
    lua_pushnumber(lst, -physics_space->gravity.y);

    return 1;
}

/*
=================
phys_lua_set_speed
=================
*/
GNUC_NONNULL static int phys_lua_set_speed (lua_State *lst)
{
    double speed = luaL_checknumber(lst, 1);

    if (speed >= 0.01)
        physics_speed = speed;

    return 0;
}

/*
=================
phys_lua_get_speed
=================
*/
GNUC_NONNULL static int phys_lua_get_speed (lua_State *lst)
{
    lua_pushnumber(lst, physics_speed);

    return 1;
}

/*
=================
g_physics_init
=================
*/
void g_physics_init (void)
{
    int i;

    cpInitChipmunk();

    for (i = 0; i < g_physics_shape_types_num ;i++)
    {
        char tmp[32];
        snprintf(tmp, sizeof(tmp), "g_physics_%s", g_physics_shape_types[i]->name);
        g_set_integer(tmp, i + 1);
    }

    physics_last_update = g_time;
    physics_speed = 1.0;

    physics_space = cpSpaceNew();
    physics_space->gravity = cpv(0.0f, -900.0f);

    cpSpaceSetDefaultCollisionPairFunc(physics_space, g_physics_collision, NULL);

    lua_register(lua_state, "physics_point_query", &phys_lua_point_query);
    lua_register(lua_state, "physics_apply_impulse", &phys_lua_apply_impulse);
    lua_register(lua_state, "physics_set_gravity", &phys_lua_set_gravity);
    lua_register(lua_state, "physics_get_gravity", &phys_lua_get_gravity);
    lua_register(lua_state, "physics_set_speed", &phys_lua_set_speed);
    lua_register(lua_state, "physics_get_speed", &phys_lua_get_speed);

    g_entity_add_field_list(ent_fields_physics, G_ENT_FIELD_INDEX_PHYSICS);
    ent_field_physics_body.index = G_ENT_FIELD_INDEX_BASE;
    g_entity_add_field(&ent_field_physics_body);

    sys_printf("+g_physics\n");
}

/*
=================
g_physics_shutdown
=================
*/
void g_physics_shutdown (void)
{
    cpSpaceFree(physics_space);

    g_entity_delete_field_list(ent_fields_physics);
    g_entity_delete_field(&ent_field_physics_body);

    sys_printf("-g_physics\n");
}
