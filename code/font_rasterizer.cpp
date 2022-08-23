// font_rasterizer.cpp

#include "ft2build.h"
#include FT_FREETYPE_H

ds_global FT_Library ftlib;
ds_global FT_Face global_face;

void md_init_freetype(u32 dpi)
{
    FT_Error error = FT_Init_FreeType(&ftlib);
    if (error) {
        // @todo: Logging
        debug_break;
    }

    error = FT_New_Face(ftlib, "fonts/Roboto-Regular.ttf", 0, &global_face);
    if (error == FT_Err_Unknown_File_Format) {
        // @todo: Logging
        debug_break;
    }
    else if (error) {
        // @todo: Logging
        debug_break;
    }

    error = FT_Set_Char_Size(global_face, 0, 24*64, dpi, dpi);
    if (error) { debug_break; }

}

u32 md_print_char(Renderer_Backbuffer *backbuffer, v2i pos, u32 codepoint)
{
    FT_Error error;
    auto glyph_index = FT_Get_Char_Index(global_face, codepoint);

    error = FT_Load_Glyph(global_face, glyph_index, 0);
    if (error) { debug_break; }

    if (global_face->glyph->format != FT_GLYPH_FORMAT_BITMAP) {
        error = FT_Render_Glyph(global_face->glyph,
                                FT_RENDER_MODE_NORMAL);
        if (error) { debug_break; }
    }

    pos.x += global_face->glyph->bitmap_left;
    pos.y -= global_face->glyph->bitmap_top;
    for (u32 y = 0; y < global_face->glyph->bitmap.rows; ++y) {
        auto pitch = global_face->glyph->bitmap.pitch;
        ds_assert(global_face->glyph->bitmap.width == pitch);
        for (u32 x = 0; x < global_face->glyph->bitmap.width; ++x) {
            u32 alpha  = global_face->glyph->bitmap.buffer[y*pitch + x] << 24;
            u32 c     = 0x00;
            u32 color = c | alpha;
            sw_draw_pixel(backbuffer, {(s32)x + pos.x, (s32)y + pos.y}, color);
        }
    }

    return global_face->glyph->advance.x >> 6;
}
