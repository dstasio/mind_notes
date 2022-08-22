// win32_renderer_software.cpp
#include "win32_renderer_software.h"
#include "ds_math.h"

#define pixel_at_index(mem, x, y, w) (((u32*)(mem)) + ((y) * (w)) + (x))

#if 0
internal void sw_draw_char(Renderer_Backbuffer *backbuffer,
                           v2i pos, char c, u32 color = 0xFFFFFF)
{
    v2i tl = {}; // top-left
    v2i br = {}; // bottom-right
    tl.x = max(pos.x,       0);
    tl.y = max(pos.y+4,     0);
    br.x = min(pos.x   + 8, backbuffer->width);
    br.y = min(pos.y+4 + 8, backbuffer->height);

    for (s32 y = 0; y < 8; ++y) {
        u8 row = font8x8_basic[c][y];
        for (s32 x = 0; x < 8; ++x) {
            s32 pixelX = tl.x + x;
            s32 pixelY = tl.y + y;
            if ((pixelX >= 0) && (pixelX < (s32)backbuffer->width)  &&
                (pixelY >= 0) && (pixelY < (s32)backbuffer->height) &&
                ((row >> x) & 0b1))
            {
                *pixel_at_index(backbuffer->memory, pixelX, pixelY, backbuffer->width) = color;
            }
        }
    }
}
#endif

internal void sw_draw_quad(Renderer_Backbuffer *backbuffer,
                           v2i pos, v2i size, u32 color = 0xFFFFFF)
{
    v2i tl = {}; // top-left
    v2i br = {}; // bottom-right
    tl.x = max(pos.x, 0);
    tl.y = max(pos.y, 0);
    br.x = min(pos.x + size.x, backbuffer->width);
    br.y = min(pos.y + size.y, backbuffer->height);

    for (s32 y = tl.y; y < br.y; ++y) {
    for (s32 x = tl.x; x < br.x; ++x) {
        *pixel_at_index(backbuffer->memory, x, y, backbuffer->width) = color;
    }}
}

internal void sw_clear_backbuffer(Renderer_Backbuffer *backbuffer)
{
    SecureZeroMemory(backbuffer->memory, backbuffer->width*backbuffer->height*4);
}

internal void sw_show_backbuffer(HWND window, Renderer_Backbuffer *backbuffer)
{
    HDC device_context = GetDC(window);
    StretchDIBits(device_context,
                  0, 0, backbuffer->width, backbuffer->height,
                  0, 0, backbuffer->width, backbuffer->height,
                  backbuffer->memory, &backbuffer->info,
                  DIB_RGB_COLORS, SRCCOPY);
    ReleaseDC(window, device_context);
}

internal void sw_resize_backbuffer(Renderer_Backbuffer *backbuffer, u32 width, u32 height)
{
    backbuffer->width                        = width;
    backbuffer->height                       = height;
    backbuffer->info.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    backbuffer->info.bmiHeader.biWidth       = width;
    backbuffer->info.bmiHeader.biHeight      = -(s32)height; // negative height -> origin is upper-left
    backbuffer->info.bmiHeader.biPlanes      = 1;
    backbuffer->info.bmiHeader.biBitCount    = 32;
    backbuffer->info.bmiHeader.biCompression = BI_RGB;
    //backbuffer->info.bmiHeader.biXPelsPerMeter = ; // @todo: Raymond Chen says this is unused (by GDI), check if it has any meaning for dpi-awareness
    //backbuffer->info.bmiHeader.biYPelsPerMeter = ;
    CreateDIBSection(0, &backbuffer->info, DIB_RGB_COLORS, &backbuffer->memory, 0, 0);
    ds_assert(backbuffer->memory);

}
