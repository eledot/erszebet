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
#include "chipmunk.h"

#define PHYSICS_STEPS 3

typedef enum
{
    PHYSICS_BODY_EMPTY = -1,
    PHYSICS_BODY_CIRCLE,
    PHYSICS_BODY_SEGMENT,
    PHYSICS_BODY_POLYGON,
    PHYSICS_BODY_POLYGON_CENTERED
}physics_body_types_e;

typedef struct g_physics_data_s
{
    /* internal fields */
    cpBody   *body;
    cpShape **shapes;
    int       shapes_num;

    /* fields accessible by lua code */
    double velocity[3];
    double rotation;
    double gravity;
    double elasticity;
    double friction;
    double mass;
    double inertia;

    int group;
    int layers;
}g_physics_data_t;

static cpSpace *physics_space;
static double   physics_last_update;
static double   physics_speed;
static int      point_query_shapes_num;

static void physics_common_callback (g_entity_t *ent) GNUC_NONNULL;
static void physics_group_layers_callback (g_entity_t *ent) GNUC_NONNULL;
static void physics_body_callback (g_entity_t *ent) GNUC_NONNULL;

static g_field_t ent_fields_physics[] =
{
#define STRUCTURE_FOR_OFFSETS g_physics_data_t
    G_FIELD("velocity",       velocity,    VECTOR,  NULL, &physics_common_callback),
    G_FIELD("velocity_x",     velocity[0], DOUBLE,  0.0,  &physics_common_callback),
    G_FIELD("velocity_y",     velocity[1], DOUBLE,  0.0,  &physics_common_callback),
    G_FIELD("velocity_z",     velocity[2], DOUBLE,  0.0,  &physics_common_callback),
    G_FIELD("rotation",       rotation,    DOUBLE,  0.0,  &physics_common_callback),
    G_FIELD("gravity",        gravity,     DOUBLE,  0.0,  &physics_common_callback),
    G_FIELD("elasticity",     elasticity,  DOUBLE,  0.0,  &physics_common_callback),
    G_FIELD("friction",       friction,    DOUBLE,  0.0,  &physics_common_callback),
    G_FIELD("mass",           mass,        DOUBLE,  0.0,  &physics_common_callback),
    G_FIELD("inertia",        inertia,     DOUBLE,  0.0,  &physics_common_callback),
    G_FIELD("physics_group",  group,       INTEGER, 0,    &physics_group_layers_callback),
    G_FIELD("physics_layers", layers,      INTEGER, -1,   &physics_group_layers_callback),
    G_FIELD_NULL
};

static g_field_t ent_field_physics_body =
    G_FIELD("body", body, CUSTOM_CALLBACK, NULL, &physics_body_callback);

/*
=================
g_physics_add_shape
=================
*/
GNUC_NONNULL static void g_physics_add_shape (g_entity_t *ent, cpShape *shape)
{
    g_physics_data_t *data = ent->physics_data;

    data->shapes[data->shapes_num++] = shape;
    shape->data = ent;
    shape->group = data->group;
    shape->layers = data->layers;

    if (ent->flags & G_ENT_FL_STATIC)
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

    if (ent->internal_flags & G_ENT_INTFL_PHYSICS_STATIC)
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
GNUC_NONNULL static void g_physics_new_obj (g_entity_t *ent, int shapes_num)
{
    g_physics_data_t *data;

    g_physics_mem_free(ent);
    data = ent->physics_data = mem_alloc(g_mempool, sizeof(g_physics_data_t) + sizeof(void *) * shapes_num);

    if (NULL == data)
        return;

    data->shapes = ent->physics_data + sizeof(g_physics_data_t);

    if (ent->flags & G_ENT_FL_STATIC)
    {
        data->body = cpBodyNew(INFINITY, INFINITY);
        cpSpaceAddBody(physics_space, data->body);
        ent->internal_flags |= G_ENT_INTFL_PHYSICS_STATIC;
    }
    else
    {
        data->body = cpBodyNew(data->mass, data->inertia);
        cpSpaceAddBody(physics_space, data->body);
        ent->internal_flags -= (ent->internal_flags & G_ENT_INTFL_PHYSICS_STATIC);
    }
}

/*
=================
g_physics_set_segment
=================
*/
GNUC_NONNULL static void g_physics_set_segment (g_entity_t *ent, double radius, const double *coords)
{
    g_physics_data_t *data = ent->physics_data;

    g_physics_new_obj(ent, 1);
    g_physics_add_shape(ent, cpSegmentShapeNew(data->body,
                                       cpv(coords[0], coords[1]),
                                       cpv(coords[2], coords[3]),
                                       radius));
}

/*
=================
g_physics_set_circle
=================
*/
GNUC_NONNULL static void g_physics_set_circle (g_entity_t *ent, double radius)
{
    g_physics_data_t *data = ent->physics_data;

    g_physics_new_obj(ent, 1);
    g_physics_add_shape(ent, cpCircleShapeNew(data->body, radius, cpvzero));
}

/*
=================
g_physics_set_poly
=================
*/
GNUC_NONNULL static void g_physics_set_poly (g_entity_t   *ent,
                                             int           shapes_num,
                                             const double *vertices,
                                             const int    *vertices_num,
                                             int           centered)
{
    g_physics_data_t *data = ent->physics_data;
    int offset, shape, max_num;
    cpVect *v;

    for (max_num = shape = 0; shape < shapes_num ;shape++)
        if (vertices_num[shape] > max_num)
            max_num = vertices_num[shape];

    /* extra vector to calculate centroid */
    v = mem_alloc(g_mempool, sizeof(*v) * (max_num + 1));

    g_physics_new_obj(ent, shapes_num);

    for (offset = shape = 0; shape < shapes_num ;shape++)
    {
        const double *coords = vertices + offset;
        int           i, num = vertices_num[shape];
        double        da, p;
        cpVect        cv = cpvzero;

        offset += num * 2;

        for (i = 0; i < num ;i++)
        {
            v[i].x = coords[(i << 1) + 0];
            v[i].y = coords[(i << 1) + 1];
        }

        v[num].x = v[0].x;
        v[num].y = v[0].y;

        if (centered)
        {
            /* calculate centroid */
            for (cv.x = 0.0, cv.y = 0.0, da = 0.0, i = 0; i < num ;i++)
            {
                p = (v[i].x * v[i+1].y) - (v[i].y * v[i+1].x);
                cv.x += (v[i].x + v[i+1].x) * p;
                cv.y += (v[i].y + v[i+1].y) * p;
                da += p;
            }

            cv.x /= -3 * da;
            cv.y /= -3 * da;
        }

        g_physics_add_shape(ent, cpPolyShapeNew(data->body, num, v, cv));
    }

    mem_free(v);
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

    ent->origin[0]   = body->p.x;
    ent->origin[1]   = body->p.y;
    ent->angle       = body->a;

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

    if (dt < 0.03)
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
GNUC_NONNULL_ARGS(1, 3, 4) static int g_physics_touch (g_entity_t *self,
                                                       g_entity_t *other,
                                                       const double *origin,
                                                       const double *normal)
{
    int ret;

    lua_getref(lua_state, self->lua_dataref);
    lua_getfield(lua_state, -1, "touch");
    lua_getref(lua_state, self->lua_ref);

    if (NULL == other)
        lua_pushnil(lua_state);
    else
        lua_getref(lua_state, other->lua_ref);

    g_push_vector(origin, 3);
    g_push_vector(normal, 3);

    g_lua_call(4, 1);
    lua_pop(lua_state, 1);
    ret = lua_toboolean(lua_state, 0);
    lua_pop(lua_state, 1);

    return ret;
}

/*
=================
g_physics_collision
=================
*/
static int g_physics_collision (cpShape   *a,
                                cpShape   *b,
                                cpContact *contacts,
                                int        num_contacts,
                                cpFloat    normal_coef,
                                GNUC_UNUSED void *data)
{
    int i;
    int zero = 0;
    const int *afl = &zero, *bfl = &zero;
    const int *aintfl = &zero, *bintfl = &zero;
    g_entity_t *ae = a->data, *be = b->data;

    if (NULL != ae)
    {
        afl = &ae->flags;
        aintfl = &ae->internal_flags;
    }

    if (NULL != be)
    {
        bfl = &be->flags;
        bintfl = &be->internal_flags;
    }

    for (i = 0; i < num_contacts ;i++)
    {
        int atouch_blocked = 1;
        int btouch_blocked = 1;
        double origin[3];
        double normal[3];

        origin[0] = contacts[i].p.x;
        origin[1] = contacts[i].p.y;
        origin[2] = 0.0;
        normal[0] = normal_coef * contacts[i].n.x;
        normal[1] = normal_coef * contacts[i].n.y;
        normal[2] = 0.0;

        if (*aintfl & G_ENT_INTFL_TOUCH && !(*bfl & G_ENT_FL_NON_SOLID))
            atouch_blocked = g_physics_touch(ae, be, origin, normal);

        if (*bintfl & G_ENT_INTFL_TOUCH && !(*afl & G_ENT_FL_NON_SOLID))
            btouch_blocked = g_physics_touch(be, ae, origin, normal);

        if (!(!(atouch_blocked & btouch_blocked) || ((*afl | *bfl) & G_ENT_FL_NON_SOLID)))
            return 1;
    }

    return 0;
}

/*
=================
g_physics_update_ent_origin
=================
*/
void g_physics_update_ent_origin (g_entity_t *ent)
{
    g_physics_data_t *data = ent->physics_data;

    if (NULL == data)
        return;

    data->body->p = cpv(ent->origin[0], ent->origin[1]);
}

/*
=================
physics_common_callback
=================
*/
static void physics_common_callback (g_entity_t *ent)
{
    g_physics_data_t *data = ent->physics_data;

    data->body->p = cpv(ent->origin[0], ent->origin[1]);
    data->body->v = cpv(data->velocity[0], data->velocity[1]);
    cpBodySetAngle(data->body, ent->angle);
    data->body->w = data->rotation; /* FIXME */
    data->body->t = 0.0; /* FIXME */
    data->body->gravity = -data->gravity;

    cpBodySetMass(data->body, data->mass);
    cpBodySetMoment(data->body, data->inertia);
}

/*
=================
physics_group_layers_callback
=================
*/
static void physics_group_layers_callback (g_entity_t *ent)
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
physics_body_callback
=================
*/
static void physics_body_callback (g_entity_t *ent)
{
    /* FIXME */
    sys_printf("FIXME ent=%p\n", ent);
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
phys_lua_set_body
=================
*/
GNUC_NONNULL static int phys_lua_set_body (lua_State *lst)
{
    g_entity_t *ent;
    double      radius, points[4], *coords;
    int         arg, i, type, num, shapes_num, *vertices_num;

    arg = 1;
    lua_getfield(lst, arg++, "__ref");
    ent = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == ent)
    {
        sys_printf("called \"set_physics\" without entity\n");
        return 0;
    }

    type = luaL_checknumber(lst, arg++);

    switch (type)
    {
    case PHYSICS_BODY_EMPTY:
        g_physics_mem_free(ent);
        break;

    case PHYSICS_BODY_CIRCLE:
        radius = luaL_checknumber(lst, arg++);
        g_physics_set_circle(ent, radius);
        break;

    case PHYSICS_BODY_SEGMENT:
        radius = luaL_checknumber(lst, arg++);
        g_pop_vector(arg, points, 4);
        g_physics_set_segment(ent, radius, points);
        break;

    case PHYSICS_BODY_POLYGON:
    case PHYSICS_BODY_POLYGON_CENTERED:
        shapes_num = lua_gettop(lst) - arg;

        vertices_num = mem_alloc(g_mempool, sizeof(*vertices_num) * shapes_num);

        for (num = i = 0; i < shapes_num ;i++)
        {
            vertices_num[i] = lua_objlen(lst, arg + i);
            num += vertices_num[i];

            if (vertices_num[i]/2 < 3)
            {
                sys_printf("bad polygon (got %i vertices)\n", vertices_num[i]/2);
                mem_free(vertices_num);
                break;
            }
        }

        coords = mem_alloc(g_mempool, sizeof(double) * num);

        for (num = i = 0; i < shapes_num ;i++)
        {
            g_pop_vector(arg + i, &coords[num], vertices_num[i]);
            num += vertices_num[i];
            vertices_num[i] >>= 1;
        }

        g_physics_set_poly(ent, shapes_num, coords, vertices_num, type == PHYSICS_BODY_POLYGON_CENTERED);
        mem_free(vertices_num);
        mem_free(coords);
        break;

    default:
        sys_printf("unknown body shape %i\n", type);
        break;
    }

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
    g_set_integer("BODY_EMPTY", PHYSICS_BODY_EMPTY);
    g_set_integer("BODY_CIRCLE", PHYSICS_BODY_CIRCLE);
    g_set_integer("BODY_SEGMENT", PHYSICS_BODY_SEGMENT);
    g_set_integer("BODY_POLYGON", PHYSICS_BODY_POLYGON);
    g_set_integer("BODY_POLYGON_CENTERED", PHYSICS_BODY_POLYGON_CENTERED);

    lua_register(lua_state, "physics_set_body", &phys_lua_set_body);
    lua_register(lua_state, "physics_point_query", &phys_lua_point_query);
    lua_register(lua_state, "physics_apply_impulse", &phys_lua_apply_impulse);

    lua_register(lua_state, "physics_set_gravity", &phys_lua_set_gravity);
    lua_register(lua_state, "physics_get_gravity", &phys_lua_get_gravity);
    lua_register(lua_state, "physics_set_speed", &phys_lua_set_speed);
    lua_register(lua_state, "physics_get_speed", &phys_lua_get_speed);

    cpInitChipmunk();

    physics_last_update = g_time;
    physics_speed = 1.0;

    physics_space = cpSpaceNew();
    physics_space->gravity = cpv(0.0f, -900.0f);

    cpSpaceSetDefaultCollisionPairFunc(physics_space, g_physics_collision, NULL);

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
