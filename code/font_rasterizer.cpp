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

    error = FT_Set_Char_Size(global_face, 0, 300*64, dpi, dpi);
    if (error) { debug_break; }

}

void md_print_char(Renderer_Backbuffer *backbuffer, u32 codepoint)
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

    for (u32 y = 0; y < global_face->glyph->bitmap.rows; ++y) {
        auto pitch = global_face->glyph->bitmap.pitch;
        ds_assert(global_face->glyph->bitmap.width == pitch);
        for (u32 x = 0; x < global_face->glyph->bitmap.width; ++x) {
            u8 gray   = global_face->glyph->bitmap.buffer[y*pitch + x];
            u8 alpha  = 8;
            u32 color = (alpha << 24) | (gray << 16) | (gray << 8) | gray;
            sw_draw_pixel(backbuffer, {(s32)x, (s32)y}, color);
        }
    }
}
