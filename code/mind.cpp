// mind.cpp

void mind_app(HWND window, Renderer_Backbuffer *backbuffer)
{
    sw_clear_backbuffer(backbuffer);

    sw_draw_quad(backbuffer, {0, 0}, {300, 90}, 0xc6c6c6);

    v2i pos = {0, 50};
    pos.x += md_print_char(backbuffer, pos, 'A');
    pos.x += md_print_char(backbuffer, pos, 'b');
    pos.x += md_print_char(backbuffer, pos, 'C');
    pos.x += md_print_char(backbuffer, pos, 'd');
    pos.x += md_print_char(backbuffer, pos, 'E');
    pos.x += md_print_char(backbuffer, pos, 'f');
    pos.x += md_print_char(backbuffer, pos, 'G');

    sw_show_backbuffer(window, backbuffer);
}
