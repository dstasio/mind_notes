#if !defined(WIN32_RENDERER_SOFTWARE_H)

struct Renderer_Backbuffer
{
    void *memory;
    u32   width;
    u32   height;

    BITMAPINFO info;
};

#define WIN32_RENDERER_SOFTWARE_H
#endif
