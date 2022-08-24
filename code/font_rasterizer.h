#if !defined(FONT_RASTERIZER_H)

struct MD_Rasterized_Glyph
{
    u8* buffer;

    u32 rows;
    u32 width;
    u32 pitch;

    s32 top;
    s32 left;
    v2i advance;
};

#define FONT_RASTERIZER_H
#endif // FONT_RASTERIZER_H
