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
#include "render/r_private.h"
#include "gl/gl.h"

static bool r_i = false;
mem_pool_t r_mempool;

static cvar_t *r_show_fps;
cvar_t *r_fov;

/* FIXME -- threaded screenshot cmd and shout if failed to take a shot */

/*
=================
screenshot_f
=================
*/
static void screenshot_f (const struct cmd_s *cmd, GNUC_UNUSED int source, int argc, const char **argv)
{
    char        tmp[MISC_MAX_FILENAME];
    const char *ext;
    image_t     im;
    int         i, *last_number;
    bool       (*save) (const char *name, image_t *im) = NULL;
    static int  last_number_jpeg = 0;
    static int  last_number_png = 0;

    if (source == CMD_SRC_KEY_UP)
        return;

    if (!strcmp(cmd->name, "screenshot_jpeg"))
    {
        last_number = &last_number_jpeg;
        //save = &image_jpeg_save;
        ext = "jpg";
    }
    else if (!strcmp(cmd->name, "screenshot_png"))
    {
        last_number = &last_number_png;
        //save = &image_png_save;
        ext = "png";
    }
    else
    {
        return;
    }

    if (NULL == save)
        return;

    if (!fs_mkdir("scr"))
        return;

    if (!gl_get_screen_rgb(&im))
        return;

    for (i = *last_number; i <= 99999 ;i++)
    {
        if (argc > 1)
            snprintf(tmp, sizeof(tmp), "scr/%s.%s", argv[1], ext);
        else
            snprintf(tmp, sizeof(tmp), "scr/shot%05i.%s", i, ext);

        if (!fs_file_exists(tmp) || argc > 1)
        {
            if (save(tmp, &im))
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
static int r_get_fps (void)
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
    gl_enable_textures();
    gl_clear();

    /* FIXME -- 3d here */
    //gl_switch_3d();

    gl_switch_2d();
    g_draw();

    if (r_show_fps->i)
    {
        char tmp[16];
        snprintf(tmp, sizeof(tmp), "fps: %i", r_get_fps());
        gl_color(1, 1, 1, 1);
        gl_draw_text(tmp, 0, video_height);
    }

    if (video_grabbed)
        gl_draw_text("X", cursor_x - GL_INTERNAL_FONT_CW/2, cursor_y + GL_INTERNAL_FONT_CH/2);
}

/*
=================
r_init
=================
*/
bool r_init (void)
{
    r_show_fps = cvar_get("r_show_fps", "0", 0);
    r_fov = cvar_get("r_fov", "90", 0);

    cmd_register("screenshot_jpeg", NULL, &screenshot_f, 0);
    cmd_register("screenshot_png",  NULL, &screenshot_f, 0);

    r_mempool = mem_alloc_pool("renderer", 0);

    if (!r_texture_init() ||
        !r_sprite_init() ||
        !r_font_init())
    {
        return false;
    }

    r_i = true;

    sys_printf("+r\n");

    return true;
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

    r_i = false;

    r_font_shutdown();
    r_sprite_shutdown();
    r_texture_shutdown();

    mem_free_pool(&r_mempool);

    sys_printf("-r\n");
}
