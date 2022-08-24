// win32_renderer_software.cpp
#include "win32_renderer_software.h"
#include "ds_math.h"

#define pixel_at_index(mem, x, y, w) (((u32*)(mem)) + ((y) * (w)) + (x))

ds_internal Renderer_Backbuffer global_backbuffer = {};


#if 0
ds_internal void sw_draw_char(Renderer_Backbuffer *backbuffer,
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

ds_internal void sw_draw_quad(Renderer_Backbuffer *backbuffer,
                           v2i pos, v2i size, u32 color = 0xFFFFFFFF)
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

ds_internal void sw_draw_pixel(Renderer_Backbuffer *backbuffer,
                               v2i pos, u32 color = 0xFFFFFFFF)
{
    b32 x_in_bounds = pos.x >= 0 && pos.x < (s32)backbuffer->width;
    b32 y_in_bounds = pos.y >= 0 && pos.y < (s32)backbuffer->height;

    if (!x_in_bounds || !y_in_bounds) return;

    u32 *dest = pixel_at_index(backbuffer->memory, pos.x, pos.y, backbuffer->width);

    r32 src_a = (r32)((0xFF000000 & color) >> 24) / 255.f;
    r32 src_r = (r32)((0x00FF0000 & color) >> 16) / 255.f;
    r32 src_g = (r32)((0x0000FF00 & color) >> 8)  / 255.f;
    r32 src_b = (r32) (0x000000FF & color)        / 255.f;

    u32 dest_color = *dest;
    r32 dst_a = (r32)((0xFF000000 & dest_color) >> 24) / 255.f;
    r32 dst_r = (r32)((0x00FF0000 & dest_color) >> 16) / 255.f;
    r32 dst_g = (r32)((0x0000FF00 & dest_color) >> 8)  / 255.f;
    r32 dst_b = (r32) (0x000000FF & dest_color)        / 255.f;

    r32 a = 1.f;
    r32 r = lerp(dst_r, src_r, src_a);
    r32 g = lerp(dst_g, src_g, src_a);
    r32 b = lerp(dst_b, src_b, src_a);

    *dest = (u32)a | (((u32)(r*255.f)) << 16 |
                      ((u32)(g*255.f)) <<  8 |
                      ((u32)(b*255.f)));
}

ds_internal void sw_clear_backbuffer(Renderer_Backbuffer *backbuffer)
{
    SecureZeroMemory(backbuffer->memory, backbuffer->width*backbuffer->height*4);
}

ds_internal void sw_show_backbuffer(HWND window, Renderer_Backbuffer *backbuffer)
{
    HDC device_context = GetDC(window);
    StretchDIBits(device_context,
                  0, 0, backbuffer->width, backbuffer->height,
                  0, 0, backbuffer->width, backbuffer->height,
                  backbuffer->memory, &backbuffer->info,
                  DIB_RGB_COLORS, SRCCOPY);
    ReleaseDC(window, device_context);
}

ds_internal void sw_resize_backbuffer(Renderer_Backbuffer *backbuffer, u32 width, u32 height)
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

// pos: lower left corner of the letter (its y is the baseline of the text)
s32 sw_draw_char(Renderer_Backbuffer *backbuffer, char c, v2i pos, u32 color)
{
    // 
    // @note: the color alpha is currently ignored
    //

    MD_Rasterized_Glyph glyph = md_rasterize_glyph(c);

    pos.x += glyph.left;
    pos.y -= glyph.top;
    for (u32 y = 0; y < glyph.rows; ++y) {
        ds_assert(glyph.width == glyph.pitch);
        for (u32 x = 0; x < glyph.width; ++x) {
            u32 alpha = glyph.buffer[y*glyph.pitch + x] << 24;
            color    &= 0xFFFFFF;  // deleting the incoming alpha
            color    |= alpha;     // using the font's alpha
            sw_draw_pixel(backbuffer, {(s32)x + pos.x, (s32)y + pos.y}, color);
        }
    }

    return glyph.advance.x >> 6;
}

v2i sw_draw_text(Renderer_Backbuffer *backbuffer, char *text, v2i pen_pos, u32 color)
{
    for (char *c = text; *c; ++c) {
        pen_pos.x += sw_draw_char(backbuffer, *c, pen_pos, color);
    }

    return pen_pos;
}
