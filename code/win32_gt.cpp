#include <windows.h>
#include <dsound.h>

#include "gt.cpp"

#include "win32_gt_opengl.cpp"

global_variable WINDOWPLACEMENT global_window_position = {sizeof(global_window_position)};
global_variable LONGLONG global_perf_count_frequency;
global_variable bool global_running = false;
global_variable HWND global_window;
global_variable HCURSOR default_cursor;

global_variable LPDIRECTSOUNDBUFFER win32_sound_buffer;

global_variable wgl_create_context_attribs_arb *wglCreateContextAttribsARB;
global_variable wgl_choose_pixel_format_arb *wglChoosePixelFormatARB;

global_variable App_State state = {};
global_variable Game_Input input = {};
global_variable Game_Sound_Buffer sound_buffer = {};

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
        // NOTE(diego): Without this the cursor flashes when we move it too fast.
        ShowCursor(TRUE);
    } else {
        SetCursor(0);
        // NOTE(diego): Without this the cursor flashes when we move it too fast.
        ShowCursor(FALSE);
    }
}

internal void
platform_set_cursor_position(Vector2i position) {
    POINT point;
    point.x = position.x;
    point.y = position.y;
    
    ClientToScreen(global_window, &point);
    SetCursorPos(point.x, point.y);
}

internal void
platform_get_cursor_position(Vector2i *position) {
    assert(position);
    
    POINT point;
    GetCursorPos(&point);
    
    ScreenToClient(global_window, &point);
    
    position->x = point.x;
    position->y = point.y;
}

internal b32
platform_write_entire_file(char *filepath, void *data, u32 size) {
    HANDLE file_handle = CreateFileA(filepath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (file_handle == INVALID_HANDLE_VALUE) {
        CloseHandle(file_handle);
        return false;
    }
    
    DWORD bytes_written;
    WriteFile(file_handle, data, size, &bytes_written, 0);
    CloseHandle(file_handle);
    
    return bytes_written == size;
}

internal File_Contents
platform_read_entire_file(char *filepath) {
    File_Contents result = {};
    
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
    } else { invalid_code_path; }
    
    CloseHandle(file_handle);
    return result;
}

internal inline real32
win32_get_seconds_elapsed(LARGE_INTEGER start, LARGE_INTEGER end) {
    real32 result = ((real32) (end.QuadPart - start.QuadPart) /
                     (real32) global_perf_count_frequency);
    return result;
}

internal inline LARGE_INTEGER
win32_get_wallclock() {
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
    
    opengl_get_function(glGetDebugMessageLog);
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
    opengl_get_function(glDeleteProgram);
    opengl_get_function(glGenFramebuffers);
    opengl_get_function(glDeleteFramebuffers);
    opengl_get_function(glGenBuffers);
    opengl_get_function(glDeleteBuffers);
    opengl_get_function(glBindVertexArray);
    opengl_get_function(glBindBuffer);
    opengl_get_function(glBindFramebuffer);
    opengl_get_function(glBlitFramebuffer);
    opengl_get_function(glBufferData);
    opengl_get_function(glBufferSubData);
    opengl_get_function(glVertexAttribPointer);
    opengl_get_function(glGenVertexArrays);
    opengl_get_function(glDeleteVertexArrays);
    opengl_get_function(glDrawArrays);
    opengl_get_function(glDrawElements);
    opengl_get_function(glGetUniformLocation);
    opengl_get_function(glUniform1f);
    opengl_get_function(glUniform3f);
    opengl_get_function(glUniform4f);
    opengl_get_function(glUniformMatrix4fv);
    opengl_get_function(glEnableVertexAttribArray);
    opengl_get_function(glDisableVertexAttribArray);
    opengl_get_function(glActiveTexture);
    opengl_get_function(glUniform1i);
    opengl_get_function(glGenerateMipmap);
    opengl_get_function(glCheckFramebufferStatus);
    opengl_get_function(glFramebufferTexture2D);
    opengl_get_function(glFramebufferRenderbuffer);
    opengl_get_function(glTexImage2DMultisample);
    opengl_get_function(wglSwapIntervalEXT);
}

internal void
win32_opengl_refresh_vsync() {
    if (!open_gl) return;
    
    int value = global_vsync ? 1 : 0;
    open_gl->wglSwapIntervalEXT(value);
}

internal void
win32_init_audio(HWND window) {
    LPDIRECTSOUND8 direct_sound;
    if (SUCCEEDED(DirectSoundCreate8(0, &direct_sound, 0))) {
        if (SUCCEEDED(direct_sound->SetCooperativeLevel(window, DSSCL_PRIORITY))) {
            // Set buffer values
            sound_buffer.num_channels = 2;
            sound_buffer.samples_per_second = 44100;
            sound_buffer.bytes_per_sample = sound_buffer.num_channels * sizeof(s16);
            sound_buffer.size = sound_buffer.samples_per_second * sound_buffer.bytes_per_sample;
            
            // Create secondary buffer
            // NOTE(diego): DirectSound8 creates the primary buffer for us.
            WAVEFORMATEX wave_format = {};
            wave_format.wFormatTag = WAVE_FORMAT_PCM; 
            wave_format.nChannels = (WORD) sound_buffer.num_channels; 
            wave_format.nSamplesPerSec = sound_buffer.samples_per_second; 
            wave_format.wBitsPerSample = 16;
            wave_format.nBlockAlign = (wave_format.nChannels * wave_format.wBitsPerSample) / 8;
            wave_format.nAvgBytesPerSec = wave_format.nSamplesPerSec * wave_format.nBlockAlign; 
            
            DSBUFFERDESC buf_desc = {};
            buf_desc.dwSize = sizeof(buf_desc);
            buf_desc.dwFlags = DSBCAPS_GLOBALFOCUS;
            buf_desc.dwBufferBytes = sound_buffer.size;
            buf_desc.lpwfxFormat = &wave_format;
            
            if (SUCCEEDED(direct_sound->CreateSoundBuffer(&buf_desc, &win32_sound_buffer, 0))) {
                OutputDebugStringA("Secondary buffer created successfully.\n");
            } else {
                invalid_code_path;
            }
            
        } else {
            invalid_code_path;
        }
    } else {
        invalid_code_path;
    }
}

internal void
win32_clear_sound_buffer() {
    void *region_1;
    DWORD region_1_size;
    void *region_2;
    DWORD region_2_size;
    
    HRESULT result = win32_sound_buffer->Lock(0, sound_buffer.size, &region_1, &region_1_size, &region_2, &region_2_size, 0);
    if (result == DSERR_BUFFERLOST) {
        win32_sound_buffer->Restore();
        result = win32_sound_buffer->Lock(0, sound_buffer.size, &region_1, &region_1_size, &region_2, &region_2_size, 0);
    }
    if (SUCCEEDED(result)) {
        s16 *at = (s16 *) region_1;
        DWORD region_1_sample_count = region_1_size / sound_buffer.bytes_per_sample;
        for (DWORD i = 0; i < region_1_sample_count; i++) {
            *at++ = 0; 
            *at++ = 0;
        }
        
        DWORD region_2_sample_count = region_2_size / sound_buffer.bytes_per_sample;
        at = (s16 *) region_2;
        for (DWORD i = 0; i < region_2_sample_count; i++) {
            *at++ = 0; 
            *at++ = 0;
        }
        
        if (!SUCCEEDED(win32_sound_buffer->Unlock(region_1, region_1_size, region_2, region_2_size))) {
            invalid_code_path;
        }
    } else {
        invalid_code_path;
    }
    
}


internal void
win32_fill_sound_buffer(DWORD byte_to_lock, DWORD bytes_to_write) {
    void *region_1;
    DWORD region_1_size;
    void *region_2;
    DWORD region_2_size;
    
    HRESULT result = win32_sound_buffer->Lock(byte_to_lock, bytes_to_write, &region_1, &region_1_size, &region_2, &region_2_size, 0);
    if (result == DSERR_BUFFERLOST) {
        win32_sound_buffer->Restore();
        result = win32_sound_buffer->Lock(byte_to_lock, bytes_to_write, &region_1, &region_1_size, &region_2, &region_2_size, 0);
    }
    if (SUCCEEDED(result)) {
        s16 *source = sound_buffer.samples;
        
        s16 *dest = (s16 *) region_1;
        DWORD region_1_sample_count = region_1_size / sound_buffer.bytes_per_sample;
        for (DWORD i = 0; i < region_1_sample_count; i++) {
            *dest++ = *source++;
            *dest++ = *source++;
            ++sound_buffer.running_sample_index;
        }
        
        DWORD region_2_sample_count = region_2_size / sound_buffer.bytes_per_sample;
        dest = (s16 *) region_2;
        for (DWORD i = 0; i < region_2_sample_count; i++) {
            *dest++ = *source++;
            *dest++ = *source++;
            ++sound_buffer.running_sample_index;
        }
        
        if (!SUCCEEDED(win32_sound_buffer->Unlock(region_1, region_1_size, region_2, region_2_size))) {
            invalid_code_path;
        }
    } else {
        invalid_code_path;
    }
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
            
            if (input.mouse.lock) {
                //platform_set_cursor_position(state.window_center);
            } 
            input.mouse.position = state.window_center;
            
        } break;
        case WM_ACTIVATEAPP: {
            if (wparam) {
                input.mouse.lock = true;
                platform_show_cursor(false);
            } else {
                input.mouse.lock = false;
                platform_show_cursor(true);
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
            WGL_ACCELERATION_ARB, GL_TRUE, 
            WGL_FULL_ACCELERATION_ARB, // 1
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
    
    open_gl = (Opengl *) VirtualAlloc(0, sizeof(Opengl), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    
    HDC window_dc = GetDC(window);
    win32_load_wgl_extensions();
    win32_set_pixel_format(window_dc);
    
    HGLRC gl_rc = 0;
    // TODO(diego): Watch handmade hero 245
    if (wglCreateContextAttribsARB) {
        int attribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 5,
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
        win32_opengl_refresh_vsync();
        open_gl->info = opengl_get_info();
        opengl_init();
        
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
                process_button(VK_CONTROL, Button_Control);
                
                process_button(VK_UP, Button_Up);
                process_button(VK_DOWN, Button_Down);
                process_button(VK_LEFT, Button_Left);
                process_button(VK_RIGHT, Button_Right);
                
                process_button(0x41, Button_A);
                process_button(0x44, Button_D);
                process_button(0x57, Button_W);
                process_button(0x53, Button_S);
                
                process_button(0x59, Button_Y);
                process_button(0x5A, Button_Z);
                
                // Function keys
                process_button(VK_F1, Button_F1);
                process_button(VK_F2, Button_F2);
                
                input.alt_is_down = alt_key_was_down;
                
                if (vk_code == 0x55) {
                    if (was_down && !is_down) {
                        global_lock_fps = !global_lock_fps;
                    }
                }
                if (vk_code == 0x56) { // V
                    if (was_down && !is_down) {
                        global_vsync = !global_vsync;
                        win32_opengl_refresh_vsync();
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

struct Platform_Work_Queue_Entry
{
    Platform_Work_Queue_Callback *callback;
    void *data;
};

struct Platform_Work_Queue
{
    u32 volatile completion_goal;
    u32 volatile completion_count;
    
    u32 volatile next_entry_to_write;
    u32 volatile next_entry_to_read;
    HANDLE semaphore_handle;
    
    Platform_Work_Queue_Entry entries[256];
};

struct Win32_Thread_Info {
    int logical_thread_index;
    Platform_Work_Queue *queue;
};


internal void
win32_add_entry(Platform_Work_Queue *queue, Platform_Work_Queue_Callback *callback, void *data) {
    uint32 new_next_entry_to_write = (queue->next_entry_to_write + 1) % array_count(queue->entries);
    assert(new_next_entry_to_write != queue->next_entry_to_read);
    Platform_Work_Queue_Entry *entry = queue->entries + queue->next_entry_to_write;
    entry->callback = callback;
    entry->data = data;
    ++queue->completion_goal;
    _WriteBarrier();
    _mm_sfence();
    queue->next_entry_to_write = new_next_entry_to_write;
    ReleaseSemaphore(queue->semaphore_handle, 1, 0);
}

internal b32
win32_do_next_work_queue_entry(Platform_Work_Queue *queue) {
    b32 should_sleep = false;
    
    u32 next_entry_to_read = queue->next_entry_to_read;
    u32 new_next_entry_to_read = (next_entry_to_read + 1) % array_count(queue->entries);
    if (next_entry_to_read != queue->next_entry_to_write) {
        u32 index = InterlockedCompareExchange((LONG volatile *) &queue->next_entry_to_read,
                                               new_next_entry_to_read,
                                               next_entry_to_read);
        if (index == next_entry_to_read) {
            Platform_Work_Queue_Entry entry = queue->entries[index];
            entry.callback(queue, entry.data);
            InterlockedIncrement((LONG volatile *) &queue->completion_count);
        }
    } else {
        should_sleep = true;
    }
    
    return should_sleep;
}

internal void
win32_complete_all_work(Platform_Work_Queue *queue) {
    while (queue->completion_goal != queue->completion_count) {
        win32_do_next_work_queue_entry(queue);
    }
    queue->completion_goal = 0;
    queue->completion_count = 0;
}

DWORD WINAPI
thread_proc(LPVOID lpParameter) {
    Win32_Thread_Info *thread_info = (Win32_Thread_Info *) lpParameter;
    
    for (;;) {
        if (win32_do_next_work_queue_entry(thread_info->queue)) {
            WaitForSingleObjectEx(thread_info->queue->semaphore_handle, INFINITE, FALSE);
        }
    }
    
    // return 0;
}

internal PLATFORM_WORK_QUEUE_CALLBACK(do_worker_work) {
    
}

int CALLBACK
WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR command_line, int show_code) {
    
    Win32_Thread_Info thread_info[4]; // NOTE(diego): My CPU has only 8 cores. Pick half to reduce usage.
    
    Platform_Work_Queue queue = {};
    
    u32 initial_count = 0;
    u32 thread_count = array_count(thread_info);
    queue.semaphore_handle = CreateSemaphoreEx(0,
                                               initial_count,
                                               thread_count,
                                               0, 0, SEMAPHORE_ALL_ACCESS);
    for (u32 thread_index = 0;
         thread_index < thread_count;
         ++thread_index) {
        Win32_Thread_Info *info = thread_info + thread_index;
        info->queue = &queue;
        info->logical_thread_index = thread_index;
        
        DWORD thread_id;
        HANDLE thread_handle = CreateThread(0, 0, thread_proc, info, 0, &thread_id);
        CloseHandle(thread_handle);
    }
    
    win32_complete_all_work(&queue);
    
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
        
        int monitor_hz = 60;
        real32 app_update_hz = monitor_hz / 2.f;
        
        real32 last_dt = 1.f / app_update_hz;
        real32 target_dt = last_dt;
        
        global_window = CreateWindowExA(0,
                                        window_class.lpszClassName,
                                        "Game Training",
                                        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                        CW_USEDEFAULT,
                                        CW_USEDEFAULT,
                                        (int) (1920 * 0.9f),
                                        (int) (1080 * 0.9f),
                                        0,
                                        0,
                                        instance,
                                        0);
        HWND window = global_window;
        if (window) {
            HDC window_dc = GetDC(window);
            
            win32_init_opengl(window);
            {
                win32_init_audio(window);
                win32_clear_sound_buffer();
                if (!SUCCEEDED(win32_sound_buffer->Play(0, 0, DSBPLAY_LOOPING))) {
                    invalid_code_path;
                }
                sound_buffer.samples = (s16 *) VirtualAlloc(0, sound_buffer.size, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
            }
            
            input.mouse.position = state.window_center;
            
            LARGE_INTEGER frequency_counter_large;
            QueryPerformanceFrequency(&frequency_counter_large);
            real32 frequency_counter = (real32) frequency_counter_large.QuadPart;
            
            LARGE_INTEGER last_counter;
            QueryPerformanceCounter(&last_counter);
            
            core.time_info.start_counter = platform_get_perf_counter();
            
            global_running = true;
            
            Game_Memory memory = {};
            memory.high_priority_queue = &queue;
            memory.platform_add_entry = win32_add_entry;
            memory.platform_complete_all_work = win32_complete_all_work;
            
            state.memory = &memory;
            
            init_texture_catalog();
            init_shaders();
            init_draw();
            
            hotloader_register_catalog(&texture_catalog.base);
            hotloader_register_catalog(&shader_catalog.base);
            hotloader_init();
            
            while (global_running) {
                
                core.time_info.current_time += last_dt;
                core.time_info.dt = last_dt;
                
                draw_call_count = 0;
                
                begin_profiler();
                
                {
                    PROFILING_TIMED_BLOCK(ProfilerItem_Input);
                    for (int button_index = 0; button_index < Button_Count; ++button_index) {
                        input.buttons[button_index].changed = false;
                    }
                    win32_process_pending_messages(window);
                }
                memory.window_center = state.window_center;
                memory.window_dimensions = state.window_dimensions;
                
                // Game Update and render
                {
                    PROFILING_TIMED_BLOCK(ProfilerItem_GameUpdateAndRender);
                    game_update_and_render(&state, &memory, &input);
                }
                
                // Audio
                {
                    PROFILING_TIMED_BLOCK(ProfilerItem_Audio);
                    DWORD play_cursor, write_cursor;
                    if (win32_sound_buffer->GetCurrentPosition(&play_cursor, &write_cursor) == DS_OK) {
                        
                        DWORD byte_to_lock = (sound_buffer.running_sample_index * sound_buffer.bytes_per_sample) % sound_buffer.size;
                        
                        DWORD safety_bytes = (int) ((real32) (sound_buffer.samples_per_second * sound_buffer.bytes_per_sample) * target_dt);
                        
                        DWORD expected_bytes_per_tick = (DWORD) ((real32) (sound_buffer.samples_per_second * sound_buffer.bytes_per_sample) * target_dt);
                        
                        DWORD expected_boundary_byte = play_cursor + expected_bytes_per_tick;
                        
                        DWORD safe_write_buffer = write_cursor;
                        if (safe_write_buffer < play_cursor) {
                            safe_write_buffer += sound_buffer.size;
                        }
                        safe_write_buffer += safety_bytes;
                        
                        DWORD target_cursor;
                        if (safe_write_buffer < expected_boundary_byte) {
                            target_cursor = expected_boundary_byte + expected_bytes_per_tick;
                        } else {
                            target_cursor = write_cursor + expected_bytes_per_tick + safety_bytes;
                        }
                        target_cursor %= sound_buffer.size;
                        
                        DWORD bytes_to_write;
                        if (byte_to_lock > target_cursor) {
                            bytes_to_write = sound_buffer.size - byte_to_lock + target_cursor;
                        } else {
                            bytes_to_write = target_cursor - byte_to_lock;
                        }
                        
                        if (bytes_to_write) {
                            sound_buffer.samples_to_write = bytes_to_write / sound_buffer.bytes_per_sample;
                            assert(bytes_to_write % 4 == 0);
                            game_output_sound(&sound_buffer, &input);
                            win32_fill_sound_buffer(byte_to_lock, bytes_to_write);
                        }
                    } else {
                        invalid_code_path;
                    }
                }
                
                render_profiler(state.window_dimensions, last_dt);
                
                SwapBuffers(window_dc);
                
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
                            seconds_elapsed_for_frame = win32_get_seconds_elapsed(last_counter,
                                                                                  win32_get_wallclock());
                        }
                    }
                }
                
                while (hotloader_process_change()) { }
                
                perform_reloads(&texture_catalog);
                perform_reloads(&shader_catalog);
                
                // Get the frame time
                LARGE_INTEGER end_counter = win32_get_wallclock();
                last_dt = win32_get_seconds_elapsed(last_counter, end_counter);
                last_counter = end_counter;
            }
            
            ReleaseDC(window, window_dc);
            
            deinit_draw();
            hotloader_shutdown();
            
            win32_free_opengl();
        }
    }
    
    return 0;
}