#include <iostream>
#include <windows.h>
#include <assert.h>
#include <GL/gl.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "defines.h"
#include "opengl.hpp"
#include "utils.hpp"

static bool Is_Running = true;
HDC Device_Context;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { 
    switch (uMsg) 
    { 
    case WM_CREATE: {
        InitializeOpenGL(hwnd);
        return 0;
    } break;
    case WM_SIZE: {
        const u32 width  = LOWORD(lParam);
        const u32 height = HIWORD(lParam);
        glViewport(0, 0, (GLint)width, (GLint)height);
    } break;
    case WM_DESTROY: {
        Is_Running = false;
    } break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    } 
    return 0; 
}

static GLuint
CreateTexture() {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

#if 0
    u32 width  = 80;
    u32 height = 60;
    u32* buffer = (u32*)VirtualAlloc(NULL, sizeof(u32)*width*height, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    assert(buffer);
    u32* pixel = buffer;
    for(u32 v = 0; v < height; ++v) {
        for(u32 u = 0; u < width; ++u) {
            // AAGGBBRR
            f32 x = (f32)u/(f32)width; 
            f32 y = (f32)v/(f32)height;
            u32 p = 0xFF000000;
            u32 x1 = (u32)(x*255);

            p = p | (x1 << 16);

            *pixel = p;
            ++pixel;
        }
    }
#else
    HDC Device_Context = CreateCompatibleDC(0);

    wchar_t character = (wchar_t)'N';
    TEXTMETRIC text_metric;
    GetTextMetrics(Device_Context, &text_metric);
    
    SIZE font_size;
    GetTextExtentPoint32(Device_Context, (LPCSTR)&character, 1, &font_size);
    u32 width  = font_size.cx;
    u32 height = font_size.cy;
    HBITMAP bitmap = CreateCompatibleBitmap(Device_Context, width, height);
    SelectObject(Device_Context, bitmap);
    SetBkColor(Device_Context, RGB(0,0,0));
    SetTextColor(Device_Context, RGB(255,255,255));
    TextOutW(Device_Context, 0, 0, &character, 1);

    u32* buffer = (u32*)VirtualAlloc(NULL, sizeof(u32)*width*height, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    u32* pixel = buffer;
    for(u32 v = 0; v < height; ++v) {
        for(u32 u = 0; u < width; ++u) {
            // AAGGBBRR
            u32 color = GetPixel(Device_Context, u, height - 1 - v);

            color &= 0xFF00FF00;
            *pixel = color;
            ++pixel;
        }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
#endif

    Sleep(1000);
    return texture;
}

s32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                   LPSTR lpCmdLine, int nCmdShow) {
    assert(AllocConsole());

    HWND window_handle = CreateOpenGLWindow(hInstance, nCmdShow);
    Device_Context = GetDC(window_handle);

    f32 vertices[] = {   //texture coords
        1.0, 1.0, 0.0,   1.0, 1.0,
        -1.0, -1.0, 0.0, 0.0, 0.0,
        1.0, -1.0, 0.0,  1.0, 0.0,
        1.0, 1.0, 0.0,   1.0, 1.0,
        -1.0, 1.0, 0.0,  0.0, 1.0,
        -1.0, -1.0, 0.0, 0.0, 0.0
    };
    
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint shader_program = LoadShaderFromFiles("../shaders/vert.shader", "../shaders/frag.shader");

    glUseProgram(shader_program);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(f32)*5, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(f32)*5, (void*)(sizeof(f32)*3));
    glEnableVertexAttribArray(1);

    GLuint texture = CreateTexture();
    
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0) > 0 && Is_Running) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        
        glClearColor(1,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        SwapBuffers(Device_Context);
        if (GetKeyState(VK_ESCAPE) & 0x8000)
        {
            Is_Running = false;
        }
    }
    
    return 0;
}
