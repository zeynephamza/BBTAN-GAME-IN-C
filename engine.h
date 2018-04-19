#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int background_color;
int foreground_color;
BITMAP *buffer;

int engine_init()
{
    if (allegro_init() != 0)
        return 1;

    install_keyboard();

    install_timer();

    if (install_mouse() < 0) {
      textout_centre_ex(screen, font, "No mouse detected, but you need one!",
			SCREEN_W/2, SCREEN_H/2, makecol(0, 0, 0),
			makecol(255, 255, 255));
      readkey();
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
            allegro_message("No mouse detected, but you need one!\n%s\n", allegro_error);
      return 0;
   }

    if (set_gfx_mode(GFX_AUTODETECT, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0) != 0)
    {

        if (set_gfx_mode(GFX_SAFE, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0) != 0)
        {
            set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
            allegro_message("Unable to set any graphic mode\n%s\n", allegro_error);
            return 1;
        }
    }

    set_palette(desktop_palette);
    return 0;
}
void engine_start()
{
    buffer = create_bitmap(SCREEN_W, SCREEN_H);
    background_color= makecol(0xBB, 0xBB, 0x77);
    foreground_color= makecol(0xFF,0xFF,0xFF);
    poll_mouse();
    acquire_screen();
}
void engine_stop()
{
    release_screen();
    destroy_bitmap(buffer);
}

void paint_background()
{
    clear_to_color(buffer,background_color);
}

void paint_text(const char *text,int x,int y,int color)
{
    textout_centre_ex(buffer, font,text,x, y, color, -1);
}
void paint_circle_filled(int x,int y,int radius,int color)
{
    circlefill(buffer,x,y,radius,color);
}
void paint_circle(int x,int y,int radius,int color)
{
    circle(buffer,x,y,radius,color);
}
void paint_rectangle(int x,int y,int width,int height,int color)
{
     rect(buffer,x-width/2,y-height/2,x+width/2,y+height/2,color);
}
void paint_triangle(int x1,int y1,int x2,int y2,int x3,int y3,int color)
{
    line(buffer,x1,y1,x2,y2,color);
    line(buffer,x2,y2,x3,y3,color);
    line(buffer,x3,y3,x1,y1,color);
}
#endif // ENGINE_H_INCLUDED
