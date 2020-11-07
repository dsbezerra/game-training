#include <windows.h>

#include "gt.cpp"

global_variable WINDOWPLACEMENT global_window_position = {sizeof(global_window_position)};
global_variable LONGLONG global_perf_count_frequency;
global_variable bool global_running = false;
global_variable HWND global_window;
global_variable HCURSOR default_cursor;
global_variable b32 global_lock_fps = true;

global_variable wgl_create_context_attribs_arb *wglCreateContextAttribsARB;
global_variable wgl_choose_pixel_format_arb *wglChoosePixelFormatARB;

global_variable app_state state = {};
global_variable game_input input = {};

#define process_button(vk, b) \
if (vk_code == vk) {\
input.buttons[b].changed = is_down != input.buttons[b].is_down;\
input.buttons[b].is_down = is_down;\
}

internal void
platform_free(void *memory) {
    if (!memory) {
        return;
    }
    VirtualFree(memory, 0, MEM_RELEASE);
}

internal void *
platform_alloc(u64 size) {
    if (size == 0) {
        return 0;
    }
    return VirtualAlloc(0, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
}

internal void
platform_show_cursor(b32 show) {
    if (show) {
        SetCursor(default_cursor);
    } else {
        SetCursor(0);
    }
}

internal void
platform_set_cursor_position(v2i position) {
    SetCursorPos(position.x, position.y);
}

internal void
platform_get_cursor_position(v2i *position) {
    assert(position);
    
    POINT point;
    GetCursorPos(&point);
    
    // @Cleanup commenting this makes Dodger works, but breaks Simon mouse position.
    // ScreenToClient(global_window, &point);
    
    position->x = point.x;
    position->y = point.y;
}
internal file_contents
plataform_read_entire_file(char *filepath) {
    file_contents result = {};
    
    HANDLE file_handle = CreateFileA(filepath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (file_handle == INVALID_HANDLE_VALUE) {
        CloseHandle(file_handle);
        return result;
    }
    
    DWORD file_size = GetFileSize(file_handle, 0);
    result.file_size = file_size;
    result.contents = (u8 *) VirtualAlloc(0, file_size,
                                          MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    
    DWORD bytes_read;
    if (ReadFile(file_handle, result.contents, file_size, &bytes_read, 0) && file_size == bytes_read) {
        // Success;
    } else {
        assert(0);
    }
    
    CloseHandle(file_handle);
    return result;
}

internal inline real32
win32_get_seconds_elapsed(LARGE_INTEGER start, LARGE_INTEGER end)
{
    real32 result = ((real32) (end.QuadPart - start.QuadPart) /
                     (real32) global_perf_count_frequency);
    return result;
}

internal inline LARGE_INTEGER
win32_get_wallclock(void)
{
    LARGE_INTEGER result;
    QueryPerformanceCounter(&result);
    return result;
}

inline real32
platform_seconds_elapsed(u64 last_counter) {
    LARGE_INTEGER current_counter;
    QueryPerformanceCounter(&current_counter);
    
    LARGE_INTEGER start;
    start.QuadPart = last_counter;
    return win32_get_seconds_elapsed(start, current_counter);
}

internal u64
platform_get_perf_counter() {
    LARGE_INTEGER result = win32_get_wallclock();
    return result.QuadPart;
}

internal void
win32_opengl_get_functions() {
    assert(open_gl);
    
    opengl_get_function(glGetShaderInfoLog);
    opengl_get_function(glGetShaderiv);
    opengl_get_function(glGetProgramiv);
    opengl_get_function(glGetProgramInfoLog);
    opengl_get_function(glShaderSource);
    opengl_get_function(glCreateShader);
    opengl_get_function(glCompileShader);
    opengl_get_function(glDetachShader);
    opengl_get_function(glDeleteShader);
    opengl_get_function(glCreateProgram);
    opengl_get_function(glAttachShader);
    opengl_get_function(glLinkProgram);
    opengl_get_function(glUseProgram);
    opengl_get_function(glGenBuffers);
    opengl_get_function(glBindVertexArray);
    opengl_get_function(glBindBuffer);
    opengl_get_function(glBufferData);
    opengl_get_function(glBufferSubData);
    opengl_get_function(glVertexAttribPointer);
    opengl_get_function(glGenVertexArrays);
    opengl_get_function(glDrawArrays);
    opengl_get_function(glGetUniformLocation);
    opengl_get_function(glUniform4f);
    opengl_get_function(glUniformMatrix4fv);
    opengl_get_function(glEnableVertexAttribArray);
    opengl_get_function(glDisableVertexAttribArray);
    opengl_get_function(glActiveTexture);
    opengl_get_function(glUniform1i);
    opengl_get_function(wglSwapIntervalEXT);
    opengl_get_function(wglSwapIntervalEXT);
}

internal void
win32_toggle_fullscreen(HWND window) {
    // This follow Raymund Chen's "How do I switch a window between normal and fullscreen
    // https://blogs.msdn.microsoft.com/oldnewthing/20100412-00/?p=14353/
    DWORD style = GetWindowLong(window, GWL_STYLE);
    if (style & WS_OVERLAPPEDWINDOW) {
        MONITORINFO mi = { sizeof(mi) };
        if (GetWindowPlacement(window, &global_window_position) &&
            GetMonitorInfo(MonitorFromWindow(window,
                                             MONITOR_DEFAULTTOPRIMARY), &mi)) {
            SetWindowLong(window, GWL_STYLE,
                          style & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(window, HWND_TOP,
                         mi.rcMonitor.left, mi.rcMonitor.top,
                         mi.rcMonitor.right - mi.rcMonitor.left,
                         mi.rcMonitor.bottom - mi.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    } else {
        SetWindowLong(window, GWL_STYLE, WS_VISIBLE|WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(window, &global_window_position);
        SetWindowPos(window, NULL, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

internal LRESULT CALLBACK
default_proc(HWND window,
             UINT message,
             WPARAM wparam,
             LPARAM lparam) {
    LRESULT result = 0;
    switch (message) {
        case WM_DESTROY: {
            global_running = false;
        } break;
        case WM_CLOSE: {
            global_running = false;
        };
        case WM_SIZE: {
            RECT rect;
            GetClientRect(window, &rect);
            
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            state.window_center.x = width / 2;
            state.window_center.y = height / 2;
            
            state.window_dimensions.x = width;
            state.window_dimensions.y = height;
            
            // @Cleanup fix this for Dodger.
#if 0
            if (input.mouse.lock) {
                platform_set_cursor_position(state.window_center);
            }
#endif 
            input.mouse.position = state.window_center;
            
        } break;
        case WM_ACTIVATEAPP: {
            if (wparam) {
                input.mouse.lock = true;
                SetCursor(0);
            } else {
                input.mouse.lock = false;
                SetCursor(default_cursor);
            }
        } break;
        default: {
            result = DefWindowProcA(window, message, wparam, lparam);
        } break;
    }
    return result;
}

// TODO(diego): Watch handmade hero 475
internal void
win32_set_pixel_format(HDC window_dc) {
    int suggested_pixel_format_index = 0;
    GLuint extended_pick = 0;
    if (wglChoosePixelFormatARB) {
        int int_attr_list[] = {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE, // 0
            WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB, // 1
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE, // 2
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE, // 3
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB, // 4
            WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, GL_TRUE, // 5
            0,
        };
        
        wglChoosePixelFormatARB(window_dc, int_attr_list, 0, 1,
                                &suggested_pixel_format_index, &extended_pick);
    }
    
    if (!extended_pick) {
        PIXELFORMATDESCRIPTOR desired_pixel_format = {};
        desired_pixel_format.nSize = sizeof(desired_pixel_format);
        desired_pixel_format.nVersion = 1;
        desired_pixel_format.iPixelType = PFD_TYPE_RGBA;
        desired_pixel_format.dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;
        desired_pixel_format.cColorBits = 32;
        desired_pixel_format.cAlphaBits = 8;
        desired_pixel_format.cDepthBits = 24;
        desired_pixel_format.iLayerType = PFD_MAIN_PLANE;
        
        suggested_pixel_format_index = ChoosePixelFormat(window_dc, &desired_pixel_format);
    }
    
    PIXELFORMATDESCRIPTOR suggested_pixel_format;
    DescribePixelFormat(window_dc, suggested_pixel_format_index,
                        sizeof(suggested_pixel_format), &suggested_pixel_format);
    SetPixelFormat(window_dc, suggested_pixel_format_index, &suggested_pixel_format);
}

// TODO(diego): Watch handmade hero 475
internal void
win32_load_wgl_extensions() {
    
    WNDCLASSA window_class = {};
    
    window_class.lpfnWndProc = default_proc;
    window_class.hInstance = GetModuleHandle(0);
    window_class.lpszClassName = "GTWGLLoader";
    
    if (RegisterClassA(&window_class)) {
        HWND window = CreateWindowExA(
                                      0,
                                      window_class.lpszClassName,
                                      "Game Training",
                                      0,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      0,
                                      0,
                                      window_class.hInstance,
                                      0);
        
        HDC window_dc = GetDC(window);
        win32_set_pixel_format(window_dc);
        HGLRC opengl_rc = wglCreateContext(window_dc);
        if (wglMakeCurrent(window_dc, opengl_rc)) {
            wglChoosePixelFormatARB =
                (wgl_choose_pixel_format_arb *) wglGetProcAddress("wglChoosePixelFormatARB");
            wglCreateContextAttribsARB =
                (wgl_create_context_attribs_arb *) wglGetProcAddress("wglCreateContextAttribsARB");
            wglMakeCurrent(0, 0);
        }
        wglDeleteContext(opengl_rc);
        ReleaseDC(window, window_dc);
        DestroyWindow(window);
    }
}

internal void
win32_init_opengl(HWND window) {
    
    open_gl = (opengl *) VirtualAlloc(0, sizeof(open_gl), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    
    HDC window_dc = GetDC(window);
    win32_load_wgl_extensions();
    win32_set_pixel_format(window_dc);
    
    HGLRC gl_rc = 0;
    // TODO(diego): Watch handmade hero 245
    if (wglCreateContextAttribsARB) {
        int attribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0,
        };
        gl_rc = wglCreateContextAttribsARB(window_dc, 0, attribs);
    } else {
        gl_rc = wglCreateContext(window_dc);
    }
    if (wglMakeCurrent(window_dc, gl_rc)) {
        win32_opengl_get_functions();
        open_gl->wglSwapIntervalEXT(1);
        open_gl->info = opengl_get_info();
        
    } else {
        invalid_code_path;
    }
    
    ReleaseDC(window, window_dc);
}

internal void
win32_free_opengl() {
    if (!open_gl) {
        return;
    }
    
    VirtualFree(open_gl, 0, MEM_RELEASE);
}

internal void
win32_process_pending_messages(HWND window) {
    MSG message;
    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
        switch (message.message) {
            case WM_QUIT: {
                global_running = false;
            } break;
            
            case WM_LBUTTONDOWN: {
                input.buttons[Button_Mouse1].changed = true;
                input.buttons[Button_Mouse1].is_down = true;
            } break;
            
            case WM_LBUTTONUP: {
                input.buttons[Button_Mouse1].changed = true;
                input.buttons[Button_Mouse1].is_down = false;
            } break;
            
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP: {
                
                u32 vk_code = (u32) message.wParam;
                b32 was_down = ((message.lParam & (1 << 30)) != 0);
                b32 is_down = ((message.lParam & (1UL << 31)) == 0);
                b32 alt_key_was_down = (message.lParam & (1 << 29));
                
                process_button(VK_RETURN, Button_Enter);
                process_button(VK_ESCAPE, Button_Escape);
                process_button(VK_SPACE, Button_Space);
                
                process_button(VK_UP, Button_Up);
                process_button(VK_DOWN, Button_Down);
                process_button(VK_LEFT, Button_Left);
                process_button(VK_RIGHT, Button_Right);
                
                if (vk_code == 0x55) {
                    if (was_down && !is_down) {
                        global_lock_fps = !global_lock_fps;
                    }
                }
                
                if (was_down != is_down) {
                    if (state.current_mode != Mode_PlayingGame) {
                        if (vk_code == VK_ESCAPE && is_down) {
                            global_running = false;
                        }
                    }
                    if (is_down) {
                        if (vk_code == VK_RETURN && alt_key_was_down) {
                            win32_toggle_fullscreen(window);
                        }
                    }
                }
                
            } break;
            
            default: {
                TranslateMessage(&message);
                DispatchMessageA(&message);
            } break;
        }
    }
}


int CALLBACK
WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR command_line, int show_code) {
    
    LARGE_INTEGER perf_count_freq_res;
    QueryPerformanceFrequency(&perf_count_freq_res);
    global_perf_count_frequency = perf_count_freq_res.QuadPart;
    
    default_cursor = LoadCursorA(0, IDC_ARROW);
    SetCursor(default_cursor);
    
    bool sleep_is_granular = timeBeginPeriod(1) == TIMERR_NOERROR;
    
    WNDCLASSA window_class = {};
    window_class.style = CS_HREDRAW|CS_VREDRAW;
    window_class.lpfnWndProc = default_proc;
    window_class.hInstance = instance;
    window_class.lpszClassName = "GTWindowsClass";
    
    if (RegisterClassA(&window_class)) {
        
        int monitor_hz = 144;
        real32 app_update_hz = monitor_hz / 2.f;
        
        real32 last_dt = 1.f / app_update_hz;
        real32 target_dt = last_dt;
        
        
        global_window = CreateWindowExA(0,
                                        window_class.lpszClassName,
                                        "Game Training",
                                        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                        CW_USEDEFAULT,
                                        CW_USEDEFAULT,
                                        960,
                                        540,
                                        0,
                                        0,
                                        instance,
                                        0);
        HWND window = global_window;
        if (window) {
            HDC window_dc = GetDC(window);
            
            win32_init_opengl(window);
            
            LARGE_INTEGER frequency_counter_large;
            QueryPerformanceFrequency(&frequency_counter_large);
            real32 frequency_counter = (real32) frequency_counter_large.QuadPart;
            
            input.mouse.position = state.window_center;
            
            LARGE_INTEGER last_counter;
            QueryPerformanceCounter(&last_counter);
            
            global_running = true;
            
            game_memory memory = {};
            
            state.memory = &memory;
            
            init_draw();
            
            
            while (global_running) {
                
                begin_profiler();
                
                begin_profiling(ProfilerItem_Input);
                for (int button_index = 0; button_index < Button_Count; ++button_index) {
                    input.buttons[button_index].changed = false;
                }
                
                win32_process_pending_messages(window);
                
                end_profiling(ProfilerItem_Input);
                
                // Game Update and render
                memory.window_center = state.window_center;
                memory.window_dimensions = state.window_dimensions;
                
                time_info.dt = last_dt;
                time_info.current_time += last_dt;
                
                game_update_and_render(&state, &memory, &input);
                
                render_profiler(state.window_dimensions, last_dt);
                
                // Ensure a forced frame time
                if (global_lock_fps) {
                    LARGE_INTEGER work_counter = win32_get_wallclock();
                    real32 work_seconds_elapsed = win32_get_seconds_elapsed(last_counter, work_counter);
                    
                    real32 seconds_elapsed_for_frame = work_seconds_elapsed;
                    if (seconds_elapsed_for_frame < target_dt) {
                        if (sleep_is_granular) {
                            DWORD sleep_ms = (DWORD)(1000.0f * (target_dt -
                                                                seconds_elapsed_for_frame));
                            if (sleep_ms > 0) {
                                Sleep(sleep_ms);
                            }
                        }
                        
                        while (seconds_elapsed_for_frame < target_dt) {
                            seconds_elapsed_for_frame= win32_get_seconds_elapsed(last_counter,
                                                                                 win32_get_wallclock());
                        }
                    }
                }
                SwapBuffers(window_dc);
                
                // Get the frame time
                LARGE_INTEGER end_counter = win32_get_wallclock();
                real32 ms_per_frame = 1000.0f * win32_get_seconds_elapsed(last_counter, end_counter);
                int fps = (int) (global_perf_count_frequency / (end_counter.QuadPart - last_counter.QuadPart));
                last_dt = win32_get_seconds_elapsed(last_counter, end_counter);
                last_counter = end_counter;
            }
            
            ReleaseDC(window, window_dc);
            
            deinit_draw();
            
            win32_free_opengl();
        }
    }
    
    return 0;
}