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
#include "r_private.h"
#include "gl.h"
#include "image_jpeg.h"
#include "image_png.h"
#include "g_entity.h"
#include "g_physics.h"

static int r_i = 0;
mem_pool_t mempool;

static cvar_t *r_show_fps;
static cvar_t *r_show_collisions;
cvar_t *r_fov;

r_texture_t *tex;

/* FIXME -- threaded screenshot cmd and shout if failed to take a shot */

/*
=================
screenshot_f
=================
*/
static void screenshot_f (const struct cmd_s *cmd, int source UV, int argc, const char **argv)
{
    char        tmp[MISC_MAX_FILENAME];
    const char *ext;
    image_t     im;
    int         i, *last_number;
    int       (*save) (const char *name, image_t *im);
    static int  last_number_jpeg = 0;
    static int  last_number_png = 0;

    if (!strcmp(cmd->name, "screenshot_jpeg"))
    {
        last_number = &last_number_jpeg;
        save = &image_jpeg_save;
        ext = "jpg";
    }
    else if (!strcmp(cmd->name, "screenshot_png"))
    {
        last_number = &last_number_png;
        save = &image_png_save;
        ext = "png";
    }
    else
    {
        return;
    }

    if (0 != fs_mkdir("scr"))
        return;

    if (0 != gl_get_screen_rgb(&im))
        return;

    for (i = *last_number; i <= 99999 ;i++)
    {
        if (argc > 1)
            snprintf(tmp, sizeof(tmp), "scr/%s.%s", argv[1], ext);
        else
            snprintf(tmp, sizeof(tmp), "scr/shot%05i.%s", i, ext);

        if (!fs_file_exists(tmp) || argc > 1)
        {
            if (0 == save(tmp, &im))
            {
                sys_printf("screenshot saved as \"%s\"\n", tmp);
                *last_number = i;
            }
            else
            {
                sys_printf("failed to save screenshot as \"%s\"\n", tmp);
            }

            break;
        }
    }

    mem_free(im.data);
}

/*
=================
r_get_fps
=================
*/
int r_get_fps (void)
{
#define FPSCNT 32
    int        i, total, t, frame_time;
    static int previous_times[FPSCNT], index, fps, previous;

    t = (int)(sys_get_time() * 1000);
    frame_time = t - previous;
    previous = t;

    previous_times[index % FPSCNT] = frame_time;
    index++;

    if (index > FPSCNT/2)
    {
        for (total = i = 0; i < FPSCNT ;i++)
            total += previous_times[i];

        if (!total)
            total = 1;

        fps = 1000 * FPSCNT / total;
    }

    return MIN(9999, fps);
}

/*
=================
r_frame
=================
*/
void r_frame (void)
{
    gl_clear();
    //gl_switch_3d();
    gl_switch_2d();

    if (r_show_collisions->i)
    {
        gl_disable_textures();
        g_physics_draw_collisions();
        gl_enable_textures();
    }

    //if (r_show_fps->i)
    {
        char tmp[16];
        snprintf(tmp, sizeof(tmp), "fps: %i", r_get_fps());
        gl_draw_text(tmp, 0, video_height);

        gl_draw_stretched(tex->gltex);
    }

    if (video_grabbed)
        gl_draw_text("X", cursor_x - GL_INTERNAL_FONT_CW/2, cursor_y + GL_INTERNAL_FONT_CH/2);
}

/*
=================
r_init
=================
*/
int r_init (void)
{
    r_show_fps = cvar_get("r_show_fps", "0", 0);
    r_show_collisions = cvar_get("r_show_collisions", "1", 0);
    r_fov = cvar_get("r_fov", "90", 0);

    cmd_register("screenshot_jpeg", NULL, &screenshot_f, 0);
    cmd_register("screenshot_png",  NULL, &screenshot_f, 0);

    mem_alloc_static_pool("renderer", 0);

    if (0 != r_texture_init())
        return -1;

    if (0 == r_texture_load("tex", 0, R_TEX_DEFAULT, &tex))
    {
        printf("OK!!!!111111111111111111\n");
    }

    r_i = 1;

    sys_printf("+r\n");

    return 0;
}

/*
=================
r_shutdown
=================
*/
void r_shutdown (void)
{
    if (!r_i)
        return;

    r_texture_shutdown();

    r_i = 0;

    sys_printf("-r\n");
}
