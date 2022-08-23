// mind.cpp

void mind_app(HWND window, Renderer_Backbuffer *backbuffer)
{
    sw_clear_backbuffer(backbuffer);

    sw_draw_quad(backbuffer, {50, 0}, {300, 90}, 0x3F77F3);

    md_print_char(backbuffer, 'O');

    sw_show_backbuffer(window, backbuffer);
}
