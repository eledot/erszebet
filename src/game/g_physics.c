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

#include "g_private.h"
#include "chipmunk.h"

#define PHYS_STEPS 3

typedef enum
{
    PHYS_BODY_EMPTY = -1,
    PHYS_BODY_CIRCLE = 0,
    PHYS_BODY_SEGMENT,
    PHYS_BODY_POLYGON,
    PHYS_BODY_POLYGON_CENTERED
}phys_body_types_e;

static cpSpace *phys_space;
static double   phys_last_update;
static double   phys_speed;
static int      point_query_shapes_num;

/*
=================
g_add_shape
=================
*/
GNUC_NONNULL static void g_add_shape (g_entity_t *ent, cpShape *shape)
{
    ent->shapes[ent->shapes_num++] = shape;
    shape->data = ent;
    shape->group = ent->phys_group;
    shape->layers = ent->phys_layers;

    if (ent->flags & ENT_FL_STATIC)
        cpSpaceAddStaticShape(phys_space, shape);
    else
        cpSpaceAddShape(phys_space, shape);

    cpResetShapeIdCounter();
}

/*
=================
g_delete_constraint
=================
*/
GNUC_NONNULL static void g_delete_constraint (void *ptr, void *data)
{
    cpBody      **bodies = (cpBody **)data;
    cpConstraint *con = ptr;

    if ((con->a == bodies[0] && con->b == bodies[1]) ||
        (con->b == bodies[0] && con->a == bodies[1]))
    {
        cpSpaceRemoveConstraint(phys_space, con);
    }
}

/*
=================
g_delete_constraint_full
=================
*/
GNUC_NONNULL static void g_delete_constraint_full (void *ptr, void *data)
{
    cpBody       *body = data;
    cpConstraint *con = ptr;

    if (con->a == body || con->b == body)
    {
        cpSpaceRemoveConstraint(phys_space, con);
    }
}

/*
=================
g_delete_shapes
=================
*/
GNUC_NONNULL static void g_delete_shapes (g_entity_t *ent)
{
    int i;

    if (ent->internal_flags & ENT_INTFL_PHYS_STATIC)
    {
        for (i = 0; i < ent->shapes_num ;i++)
        {
            if (NULL != ent->shapes[i])
            {
                cpSpaceRemoveStaticShape(phys_space, ent->shapes[i]);
                cpShapeFree(ent->shapes[i]);
            }
        }
    }
    else
    {
        for (i = 0; i < ent->shapes_num ;i++)
        {
            if (NULL != ent->shapes[i])
            {
                cpSpaceRemoveShape(phys_space, ent->shapes[i]);
                cpShapeFree(ent->shapes[i]);
            }
        }
    }

    mem_free(ent->shapes);
    ent->shapes = NULL;
    ent->shapes_num = 0;
}

/*
=================
g_physics_detach
=================
*/
GNUC_NONNULL_ARGS(1) static void g_physics_detach (g_entity_t *a, g_entity_t *b)
{
    cpBody *bodies[2] = { a->body, NULL };

    if (NULL == a->body || (NULL != b && NULL == b->body))
    {
        sys_printf("tried to detach entities without bodies\n");
        return;
    }

    if (NULL == b)
    {
        cpArrayEach(phys_space->constraints, &g_delete_constraint_full, a->body);
    }
    else
    {
        bodies[1] = b->body;
        cpArrayEach(phys_space->constraints, &g_delete_constraint, bodies);
    }
}

/*
=================
g_delete_body
=================
*/
GNUC_NONNULL static void g_delete_body (g_entity_t *ent)
{
    g_physics_detach(ent, NULL);
    cpSpaceRemoveBody(phys_space, ent->body);
    cpBodyFree(ent->body);
    ent->body = NULL;
}

/*
=================
g_physics_free_obj
=================
*/
GNUC_NONNULL void g_physics_free_obj (g_entity_t *ent)
{
    if (NULL != ent->shapes)
        g_delete_shapes(ent);

    if (NULL != ent->body)
        g_delete_body(ent);

    ent->internal_flags |= ENT_INTFL_PHYS_STATIC;
    ent->flags |= ENT_FL_STATIC;
}

/*
=================
g_physics_new_obj
=================
*/
GNUC_NONNULL static void g_physics_new_obj (g_entity_t *ent, int shapes_num)
{
    int     flags;
    cpBody *body;

    flags = ent->flags;
    g_physics_free_obj(ent);
    ent->flags = flags;

    if (ent->flags & ENT_FL_STATIC)
    {
        body = cpBodyNew(INFINITY, INFINITY);
        ent->internal_flags |= ENT_INTFL_PHYS_STATIC;
    }
    else
    {
        body = cpBodyNew(ent->mass, ent->inertia);
        cpSpaceAddBody(phys_space, body);
        ent->internal_flags -= (ent->internal_flags & ENT_INTFL_PHYS_STATIC);
    }

    ent->shapes = mem_alloc(g_mempool, sizeof(void *) * shapes_num);

    ent->body = body;
    g_physics_update_body(ent);
}

/*
=================
g_physics_set_segment
=================
*/
GNUC_NONNULL static void g_physics_set_segment (g_entity_t *ent, double radius, const double *coords)
{
    g_physics_new_obj(ent, 1);
    g_add_shape(ent, cpSegmentShapeNew(ent->body,
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
    g_physics_new_obj(ent, 1);
    g_add_shape(ent, cpCircleShapeNew(ent->body, radius, cpvzero));
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
    cpVect *v;
    int     offset, shape, max_num;

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

        g_add_shape(ent, cpPolyShapeNew(ent->body, num, v, cv));
    }

    mem_free(v);
}

/*
=================
g_point_query
=================
*/
GNUC_NONNULL static void g_point_query (cpShape *shape, void (*callback) (g_entity_t *shape))
{
    callback(shape->data);
}

/*
=================
g_physics_update_ent
=================
*/
void g_physics_update_ent (g_entity_t *ent)
{
    cpBody *body = ent->body;
    cpShape *shape;

    if (NULL == body || ent->shapes_num < 1)
        return;

    shape = ent->shapes[0];

    ent->origin[0]   = body->p.x;
    ent->origin[1]   = body->p.y;
    ent->velocity[0] = body->v.x;
    ent->velocity[1] = body->v.y;
    ent->angle       = body->a;
    ent->rotation    = body->w;
    ent->gravity     = -body->gravity;
    ent->elasticity  = shape->e;
    ent->friction    = shape->u;
    ent->mass        = body->m;
    ent->inertia     = body->i;
}

/*
=================
g_physics_update_body
=================
*/
void g_physics_update_body (g_entity_t *ent)
{
    cpBody  *body = ent->body;
    cpShape *shape;
    int      i;

    if (NULL == body || ent->shapes_num < 1)
        return;

    body->p = cpv(ent->origin[0], ent->origin[1]);
    body->v = cpv(ent->velocity[0], ent->velocity[1]);
    cpBodySetAngle(body, ent->angle);
    body->w = ent->rotation; /* FIXME */
    body->t = 0.0; /* FIXME */
    body->gravity = -ent->gravity;

    for (i = 0; i < ent->shapes_num ;i++)
    {
        shape = ent->shapes[i];
        shape->e = ent->elasticity;
        shape->u = ent->friction;
        shape->group = ent->phys_group;
        shape->layers = ent->phys_layers;
    }

    cpBodySetMass(body, ent->mass);
    cpBodySetMoment(body, ent->inertia);
}

/*
=================
g_physics_frame
=================
*/
void g_physics_frame (void)
{
    int     i;
    cpFloat dt = (g_time - phys_last_update) / (cpFloat)PHYS_STEPS;

    if (dt <= 0.0f)
        return;

    phys_last_update = g_time;

    for (i = 0; i < PHYS_STEPS ;i++)
    {
        cpSpaceStep(phys_space, dt * phys_speed);
    }
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

    lua_getref(lua_state, self->dataref);
    lua_getfield(lua_state, -1, "touch");
    lua_getref(lua_state, self->ref);

    if (NULL == other)
        lua_pushnil(lua_state);
    else
        lua_getref(lua_state, other->ref);

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
g_default_coll_func
=================
*/
static int g_default_coll_func (cpShape   *a,
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

        if (*aintfl & ENT_INTFL_TOUCH && !(*bfl & ENT_FL_NON_SOLID))
            atouch_blocked = g_physics_touch(ae, be, origin, normal);

        if (*bintfl & ENT_INTFL_TOUCH && !(*afl & ENT_FL_NON_SOLID))
            btouch_blocked = g_physics_touch(be, ae, origin, normal);

        if (!(!(atouch_blocked & btouch_blocked) || ((*afl | *bfl) & ENT_FL_NON_SOLID)))
            return 1;
    }

    return 0;
}


/*
=================
ent_lua_attach_pin
=================
*/
GNUC_NONNULL static int ent_lua_attach_pin (lua_State *lst)
{
    g_entity_t *a, *b;

    lua_getfield(lst, 1, "__ref");
    a = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == a)
    {
        sys_printf("called \"phys_attach_pin\" without entity\n");
        return 0;
    }

    lua_getfield(lst, 2, "__ref");
    b = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == b)
    {
        sys_printf("called \"phys_attach_pin\" without entity\n");
        return 0;
    }

    if (NULL == a->body || NULL == b->body)
    {
        sys_printf("tried to attach entities without bodies\n");
        return 0;
    }

    cpSpaceAddConstraint(phys_space, cpPinJointNew(a->body, b->body, cpvzero, cpvzero));

    return 0;
}

/*
=================
ent_lua_detach
=================
*/
GNUC_NONNULL static int ent_lua_detach (lua_State *lst)
{
    int         top = lua_gettop(lst);
    g_entity_t *a, *b = NULL;

    lua_getfield(lst, 1, "__ref");
    a = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == a)
    {
        sys_printf("called \"phys_detach\" without entity\n");
        return 0;
    }

    if (top > 1)
    {
        lua_getfield(lst, 2, "__ref");
        b = (g_entity_t *)lua_touserdata(lst, -1);
    }

    g_physics_detach(a, b);

    return 0;
}

/*
=================
ent_lua_point_query_callback
=================
*/
GNUC_NONNULL static void ent_lua_point_query_callback (g_entity_t *ent)
{
    point_query_shapes_num++;
    lua_pushinteger(lua_state, point_query_shapes_num);

    if (NULL == ent)
        lua_pushnil(lua_state);
    else
        lua_getref(lua_state, ent->ref);

    lua_settable(lua_state, -3);
}

/*
=================
ent_lua_point_query
=================
*/
GNUC_NONNULL static int ent_lua_point_query (lua_State *lst)
{
    double point[3];

    point_query_shapes_num = 0;
    g_pop_vector(1, point, 3);

    lua_newtable(lst);

    cpSpacePointQuery(phys_space,
                      cpv(point[0], point[1]),
                      (cpLayers)-1,
                      0,
                      (cpSpacePointQueryFunc)&g_point_query,
                      &ent_lua_point_query_callback);

    return 1;
}

/*
=================
ent_lua_apply_impulse
=================
*/
GNUC_NONNULL static int ent_lua_apply_impulse (lua_State *lst)
{
    double point[2], impulse[2];
    g_entity_t *ent;

    lua_getfield(lst, 1, "__ref");
    ent = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == ent)
    {
        sys_printf("called \"phys_apply_impulse\" without entity\n");
        return 0;
    }

    if (NULL == ent->body)
    {
        sys_printf("tried to apply impulse on entity without body\n");
        return 0;
    }

    g_pop_vector(2, point, 2);
    g_pop_vector(3, impulse, 2);

    cpBodyApplyImpulse(ent->body, cpv(impulse[0], impulse[1]), cpv(point[0], point[1]));

    return 0;
}

/*
=================
ent_lua_set_body
=================
*/
GNUC_NONNULL static int ent_lua_set_body (lua_State *lst)
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
    case PHYS_BODY_EMPTY:
        g_physics_free_obj(ent);
        break;

    case PHYS_BODY_CIRCLE:
        radius = luaL_checknumber(lst, arg++);
        g_physics_set_circle(ent, radius);
        break;

    case PHYS_BODY_SEGMENT:
        radius = luaL_checknumber(lst, arg++);
        g_pop_vector(arg, points, 4);
        g_physics_set_segment(ent, radius, points);
        break;

    case PHYS_BODY_POLYGON:
    case PHYS_BODY_POLYGON_CENTERED:
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

        g_physics_set_poly(ent, shapes_num, coords, vertices_num, type == PHYS_BODY_POLYGON_CENTERED);
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
game_lua_set_gravity
=================
*/
static int game_lua_set_gravity (lua_State *lst)
{
    phys_space->gravity = cpv(0.0, -luaL_checknumber(lst, 1));

    return 0;
}

/*
=================
game_lua_get_gravity
=================
*/
static int game_lua_get_gravity (lua_State *lst)
{
    lua_pushnumber(lst, -phys_space->gravity.y);

    return 1;
}

/*
=================
game_lua_set_speed
=================
*/
static int game_lua_set_speed (lua_State *lst)
{
    double speed = luaL_checknumber(lst, 1);

    if (speed >= 0.01)
        phys_speed = speed;

    return 0;
}

/*
=================
game_lua_get_speed
=================
*/
static int game_lua_get_speed (lua_State *lst)
{
    lua_pushnumber(lst, phys_speed);

    return 1;
}

/*
=================
g_physics_init
=================
*/
void g_physics_init (void)
{
    g_set_integer("BODY_EMPTY", PHYS_BODY_EMPTY);
    g_set_integer("BODY_CIRCLE", PHYS_BODY_CIRCLE);
    g_set_integer("BODY_SEGMENT", PHYS_BODY_SEGMENT);
    g_set_integer("BODY_POLYGON", PHYS_BODY_POLYGON);
    g_set_integer("BODY_POLYGON_CENTERED", PHYS_BODY_POLYGON_CENTERED);

    lua_register(lua_state, "phys_set_body", &ent_lua_set_body);
    lua_register(lua_state, "phys_attach_pin", &ent_lua_attach_pin);
    lua_register(lua_state, "phys_detach", &ent_lua_detach);
    lua_register(lua_state, "phys_point_query", &ent_lua_point_query);
    lua_register(lua_state, "phys_apply_impulse", &ent_lua_apply_impulse);

    lua_register(lua_state, "phys_set_gravity", &game_lua_set_gravity);
    lua_register(lua_state, "phys_get_gravity", &game_lua_get_gravity);
    lua_register(lua_state, "phys_set_speed", &game_lua_set_speed);
    lua_register(lua_state, "phys_get_speed", &game_lua_get_speed);

    cpInitChipmunk();

    phys_last_update = g_time;
    phys_speed = 1.0;

    phys_space = cpSpaceNew();
    phys_space->gravity = cpv(0.0f, -900.0f);

    cpSpaceSetDefaultCollisionPairFunc(phys_space, g_default_coll_func, NULL);

    sys_printf("+g_physics\n");
}

/*
=================
g_physics_shutdown
=================
*/
void g_physics_shutdown (void)
{
    cpSpaceFree(phys_space);

    sys_printf("-g_physics\n");
}
