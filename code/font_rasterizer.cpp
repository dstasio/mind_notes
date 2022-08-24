// font_rasterizer.cpp

#include "ft2build.h"
#include FT_FREETYPE_H

#include "font_rasterizer.h"

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

MD_Rasterized_Glyph md_rasterize_glyph(u32 codepoint)
{
    MD_Rasterized_Glyph result = {};

    FT_Error error;
    auto glyph_index = FT_Get_Char_Index(global_face, codepoint);

    error = FT_Load_Glyph(global_face, glyph_index, 0);
    if (error) { debug_break; }

    if (global_face->glyph->format != FT_GLYPH_FORMAT_BITMAP) {
        error = FT_Render_Glyph(global_face->glyph,
                                FT_RENDER_MODE_NORMAL);
        if (error) { debug_break; }
    }

    result.buffer    = global_face->glyph->bitmap.buffer;
    result.rows      = global_face->glyph->bitmap.rows;
    result.pitch     = global_face->glyph->bitmap.pitch;
    result.width     = global_face->glyph->bitmap.width;
    result.left      = global_face->glyph->bitmap_left;;
    result.top       = global_face->glyph->bitmap_top;
    result.advance.x = global_face->glyph->advance.x;
    result.advance.y = global_face->glyph->advance.y;

    return result;
}
