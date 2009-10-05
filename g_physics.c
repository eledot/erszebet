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
#include "g_entity.h"
#include "g_physics.h"
#include "3rd/chipmunk/chipmunk.h"

#define PHYS_STEPS 3

static mem_pool_t mempool;

static cpSpace *phys_space;
static cpBody  *phys_world;
static cpShape *phys_world_shape;
static double   phys_last_update;
static double   phys_speed;

/*
=================
g_add_shape
=================
*/
static void g_add_shape (g_entity_t *ent, cpShape *shape)
{
    ent->shapes[ent->shapes_num++] = shape;
    shape->data = ent;

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
static void g_delete_constraint (void *ptr, void *data)
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
static void g_delete_constraint_full (void *ptr, void *data)
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
static void g_delete_shapes (g_entity_t *ent)
{
    int i;

    if (ent->cflags & ENT_CFL_PHYS_STATIC)
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
g_delete_body
=================
*/
static void g_delete_body (g_entity_t *ent)
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
void g_physics_free_obj (g_entity_t *ent)
{
    if (NULL != ent->shapes)
        g_delete_shapes(ent);

    if (NULL != ent->body)
        g_delete_body(ent);

    ent->cflags |= ENT_CFL_PHYS_STATIC;
    ent->flags |= ENT_FL_STATIC;
}

/*
=================
g_physics_new_obj
=================
*/
static void g_physics_new_obj (g_entity_t *ent, int shapes_num)
{
    int     flags;
    cpBody *body;

    flags = ent->flags;
    g_physics_free_obj(ent);
    ent->flags = flags;

    if (ent->flags & ENT_FL_STATIC)
    {
        body = cpBodyNew(INFINITY, INFINITY);
        ent->cflags |= ENT_CFL_PHYS_STATIC;
    }
    else
    {
        body = cpBodyNew(ent->mass, ent->inertia);
        cpSpaceAddBody(phys_space, body);
        ent->cflags -= (ent->cflags & ENT_CFL_PHYS_STATIC);
    }

    ent->shapes = mem_alloc_static(sizeof(void *) * shapes_num);

    ent->body = body;
    g_physics_update_body(ent);
}

/*
=================
g_physics_set_segment
=================
*/
void g_physics_set_segment (g_entity_t *ent, double radius, const double *coords)
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
void g_physics_set_circle (g_entity_t *ent, double radius)
{
    g_physics_new_obj(ent, 1);
    g_add_shape(ent, cpCircleShapeNew(ent->body, radius, cpvzero));
}

/*
=================
g_physics_set_poly
=================
*/
void g_physics_set_poly (g_entity_t   *ent,
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
    v = mem_alloc_static(sizeof(*v) * (max_num + 1));

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
g_physics_attach_pin
=================
*/
void g_physics_attach_pin (g_entity_t *a, g_entity_t *b)
{
    if (NULL == a->body || NULL == b->body)
    {
        sys_printf("tried to attach entities without bodies\n");
        return;
    }

    cpSpaceAddConstraint(phys_space, cpPinJointNew(a->body, b->body, cpvzero, cpvzero));
}

/*
=================
g_physics_detach
=================
*/
void g_physics_detach (g_entity_t *a, g_entity_t *b)
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
g_point_query
=================
*/
static void g_point_query (cpShape *shape, void (*callback) (g_entity_t *shape))
{
    callback(shape->data);
}

/*
=================
g_physics_point_query
=================
*/
void g_physics_point_query (const double *point, void (*callback) (g_entity_t *shape))
{
    cpSpacePointQuery(phys_space,
                      cpv(point[0], point[1]),
                      (cpLayers)-1,
                      0,
                      (cpSpacePointQueryFunc)&g_point_query,
                      (void *)callback);
}

/*
=================
g_physics_apply_impulse
=================
*/
void g_physics_apply_impulse (g_entity_t *ent, const double *point, const double *impulse)
{
    if (NULL == ent->body)
    {
        sys_printf("tried to apply impulse on entity without body\n");
        return;
    }

    cpBodyApplyImpulse(ent->body, cpv(impulse[0], impulse[1]), cpv(point[0], point[1]));
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
    }

    cpBodySetMass(body, ent->mass);
    cpBodySetMoment(body, ent->inertia);
}

/*
=================
g_physics_draw_collisions
=================
*/
void g_physics_draw_collisions (void)
{
    const int segs = 9;
    const double coeff = 2.0 * M_PI / (double)segs;
    const g_entity_t *ent;
    cpVect a, b;
    float coords[segs * 2];
    int i, shn;

    for (ent = entities; NULL != ent ;ent = ent->next)
    {
        const cpBody *body = ent->body;

        if (NULL == body)
            continue;

        for (shn = 0; shn < ent->shapes_num ;shn++)
        {
            const cpShape *shape = ent->shapes[shn];
            const cpSegmentShape *segment = ent->shapes[shn];
            const cpPolyShape *poly = ent->shapes[shn];
            const cpCircleShape *circle = ent->shapes[shn];

///*
  #define CLENGTH 16
  gl_color(0, 1, 0, 1);
  gl_draw_line2d(body->p.x, body->p.y - CLENGTH,
  body->p.x, body->p.y + CLENGTH);
  gl_draw_line2d(body->p.x - CLENGTH, body->p.y,
  body->p.x + CLENGTH, body->p.y);
  gl_color(1, 1, 1, 1);
//*/

            switch (shape->klass->type)
            {
            case CP_SEGMENT_SHAPE:
                a = cpvadd(body->p, cpvrotate(segment->a, body->rot));
                b = cpvadd(body->p, cpvrotate(segment->b, body->rot));
                gl_draw_line2d(a.x, a.y, b.x, b.y);
                break;

            case CP_POLY_SHAPE:
                for (i = 0; i < poly->numVerts ;i++)
                {
                    a = cpvadd(body->p, cpvrotate(poly->verts[i], body->rot));
                    coords[(i << 1) + 0] = a.x;
                    coords[(i << 1) + 1] = a.y;
                }

                gl_draw_line2d_loop(coords, poly->numVerts);
                break;

            case CP_CIRCLE_SHAPE:
                a = cpvadd(body->p, cpvrotate(circle->c, body->rot));

                for (i = 0; i < segs ;i++)
                {
                    double rads = i * coeff;
                    coords[(i << 1) + 0] = circle->r * cos(rads + body->a) + a.x;
                    coords[(i << 1) + 1] = circle->r * sin(rads + body->a) + a.y;
                }

                gl_draw_line2d_loop(coords, segs);
                break;

            default:
                break;
            }
        }
    }
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
g_physics_set_gravity
=================
*/
void g_physics_set_gravity (double gravity)
{
    phys_space->gravity = cpv(0.0f, -gravity);
}

/*
=================
g_physics_get_gravity
=================
*/
double g_physics_get_gravity (void)
{
    return -phys_space->gravity.y;
}

/*
=================
g_physics_set_speed
=================
*/
void g_physics_set_speed (double speed)
{
    if (speed >= 0.01)
        phys_speed = speed;
}

/*
=================
g_physics_get_speed
=================
*/
double g_physics_get_speed (void)
{
    return phys_speed;
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
    int afl = 0, bfl = 0;
    int acfl = 0, bcfl = 0;
    g_entity_t *ae = a->data, *be = b->data;

    if (NULL != ae)
    {
        afl = ae->flags;
        acfl = ae->cflags;
    }

    if (NULL != be)
    {
        bfl = be->flags;
        bcfl = be->cflags;
    }

    for (i = 0; i < num_contacts ;i++)
    {
        int atouch_blocked = 1;
        int btouch_blocked = 1;
        double origin[2];
        double normal[2];

        origin[0] = contacts[i].p.x;
        origin[1] = contacts[i].p.y;
        normal[0] = normal_coef * contacts[i].n.x;
        normal[1] = normal_coef * contacts[i].n.y;

        if (acfl & ENT_CFL_TOUCH && !(bfl & ENT_FL_NON_SOLID))
            atouch_blocked = g_entity_touch(ae, be, origin, normal);

        if (bcfl & ENT_CFL_TOUCH && !(afl & ENT_FL_NON_SOLID))
            btouch_blocked = g_entity_touch(be, ae, origin, normal);

        if (!(!(atouch_blocked & btouch_blocked) || ((afl | bfl) & ENT_FL_NON_SOLID)))
            return 1;
    }

    return 0;
}

/*
=================
g_physics_init
=================
*/
void g_physics_init (mem_pool_t pool)
{
    mempool = pool;

    cpInitChipmunk();

    phys_last_update = g_time;
    phys_speed = 1.0;

    phys_space = cpSpaceNew();
    phys_space->gravity = cpv(0.0f, -900.0f);

    phys_world = cpBodyNew(INFINITY, INFINITY);
    phys_world_shape = cpSegmentShapeNew(phys_world,
                                         cpv(-4096.0, -29.0),
                                         cpv(4096.0, -29.0),
                                         30.0f);
    phys_world_shape->e = 0.9;
    phys_world_shape->u = 0.7;

    cpSpaceAddStaticShape(phys_space, phys_world_shape);
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
    cpBodyFree(phys_world);
    cpSpaceFree(phys_space);

    sys_printf("-g_physics\n");
}
