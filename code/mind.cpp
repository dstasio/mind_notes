// mind.cpp

void mind_app(HWND window, Renderer_Backbuffer *backbuffer)
{
    sw_clear_backbuffer(backbuffer);

    sw_draw_quad(backbuffer, {0, 0}, {300, 90}, 0xc6c6c6);

    char *text = "Sample text!";
    sw_draw_text(backbuffer, text, {20, 50}, 0x333333);

    sw_show_backbuffer(window, backbuffer);
}
