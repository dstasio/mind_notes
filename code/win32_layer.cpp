#include "datatypes.h"

#include <windows.h>

#if DS_INTERNAL
    #define output_string(s, ...)        {char Buffer[100];sprintf_s(Buffer, s, __VA_ARGS__);OutputDebugStringA(Buffer);}
    #define throw_error_and_exit(e, ...) {output_string(" ------------------------------[ERROR] "   ## e, __VA_ARGS__); getchar(); global_error = true;}
    #define throw_error(e, ...)           output_string(" ------------------------------[ERROR] "   ## e, __VA_ARGS__)
    #define inform(i, ...)                output_string(" ------------------------------[INFO] "    ## i, __VA_ARGS__)
    #define warn(w, ...)                  output_string(" ------------------------------[WARNING] " ## w, __VA_ARGS__)
    #define ds_assert(expr) if(!(expr)) {*(int *)0 = 0;}
#else
    #define output_string(s, ...)
    #define throw_error_and_exit(e, ...)
    #define throw_error(e, ...)
    #define inform(i, ...)
    #define warn(w, ...)
    #define ds_assert(expr)
#endif

#if DS_DEBUG
    #define debug_break __debugbreak()
#else
    #define debug_break
#endif


#include "font_rasterizer.cpp"
#include "win32_renderer_software.cpp"

#include "mind.cpp"

ds_global b32 global_running;
ds_global b32 global_error;
ds_global u32 global_width  = 800;
ds_global u32 global_height = 600;

LRESULT CALLBACK
window_proc(HWND window, UINT message, WPARAM w, LPARAM l)
{
    LRESULT result = 0;
    switch(message)
    {
        case WM_CLOSE:
        {
            DestroyWindow(window);
        } break;
            
        case WM_DESTROY:
        {
            global_running = false;
            PostQuitMessage(0);
        } break;

        case WM_SIZE:
        {
            global_width  = LOWORD(l);
            global_height = HIWORD(l);
        } break;

        // @todo: implement this
        case WM_SETCURSOR:
        {
        } break;

        default:
        {
          result = DefWindowProcA(window, message, w, l);
        } break;
    }

    return result;
}


int WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int show_flag)
{
    s64 performance_counter_frequency = 0;
    auto qpf_success = QueryPerformanceFrequency((LARGE_INTEGER *)&performance_counter_frequency);
    ds_assert(qpf_success);
    r32 target_ms_per_frame = 1.f/60.f;

    WNDCLASSA wnd_class       = {};
    wnd_class.style           = CS_OWNDC|CS_VREDRAW|CS_HREDRAW;
    wnd_class.lpfnWndProc     = window_proc;
    //wnd_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wnd_class.hInstance       = instance;
    wnd_class.lpszClassName   = "wnd_class";
    auto Result               = RegisterClassA(&wnd_class);

    RECT wnd_dims = {0, 0, (s32)global_width, (s32)global_height};
    AdjustWindowRect(&wnd_dims, WS_OVERLAPPEDWINDOW, FALSE);
    wnd_dims.right  -= wnd_dims.left;
    wnd_dims.bottom -= wnd_dims.top;

    HWND window = CreateWindowA("wnd_class", "Mind Down",
                                WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                CW_USEDEFAULT, CW_USEDEFAULT,
                                wnd_dims.right,
                                wnd_dims.bottom,
                                0, 0, instance, 0);

    if(window) {
        RECT window_rect = {};
        GetWindowRect(window, &window_rect);
        u32 window_width  = window_rect.right  - window_rect.left;
        u32 window_height = window_rect.bottom - window_rect.top;

        global_running = true;
        global_error = false;

        MSG message = {};

        s64 last_performance_counter    = 0;
        s64 current_performance_counter = 0;
        qpf_success = QueryPerformanceCounter((LARGE_INTEGER *)&last_performance_counter);
        ds_assert(qpf_success);

        Renderer_Backbuffer backbuffer = {};
        sw_resize_backbuffer(&backbuffer, global_width, global_height);

        u32 dpi = GetDpiForWindow(window);
        md_init_freetype(dpi);

        while(global_running && !global_error)
        {
            qpf_success = QueryPerformanceCounter((LARGE_INTEGER *)&current_performance_counter);
            ds_assert(qpf_success);
            r32 dtime = (r32)(current_performance_counter - last_performance_counter) / (r32)performance_counter_frequency;
            while(dtime <= target_ms_per_frame)
            {
                while(PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
                {
                    switch(message.message)
                    {
                        case WM_KEYDOWN:
                            {if (message.wParam == VK_ESCAPE) global_running = false;}
                        default:
                        {
                            TranslateMessage(&message);
                            DispatchMessage (&message);
                        } break;
                    }
                }
                qpf_success = QueryPerformanceCounter((LARGE_INTEGER *)&current_performance_counter);
                ds_assert(qpf_success);
                dtime = (r32)(current_performance_counter - last_performance_counter) / (r32)performance_counter_frequency;
            }

            mind_app(window, &backbuffer);

            last_performance_counter = current_performance_counter;
        }
    }
    else
    {
        // @todo: Logging
        return 1;
    }

    if (global_error)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
