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
CreateFontTexture() {
    GLuint Texture;
    glGenTextures(1, &Texture);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
       
    HDC DeviceContext = CreateCompatibleDC(0);
    Assert(DeviceContext);

    AddFontResourceA("C:/Windows/Fonts/LiberationMono-Regular.ttf");
    HFONT Font = CreateFontA(30, 0, 0, 0,
                             FW_REGULAR, // weight
                             FALSE, // italic
                             FALSE, // underline
                             FALSE, // strike out
                             ANSI_CHARSET,
                             OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS,
                             ANTIALIASED_QUALITY,
                             DEFAULT_PITCH|FF_DONTCARE,
                             "Liberation Mono");
    Assert(Font);

    SelectObject(DeviceContext, Font);
    SetBkColor(DeviceContext, RGB(0,0,0));
    SetTextColor(DeviceContext, RGB(255,255,255));

    
    wchar_t TempCharacter = (wchar_t)'A';
    SIZE CharSize;
    GetTextExtentPoint32A(DeviceContext, (LPCSTR)&TempCharacter, 1, &CharSize);
    u32 CharacterPerLine = 20;
    u32 TextureWidth  = CharSize.cx * 20;
    u32 TextureHeight = CharSize.cy * 5;

    HBITMAP Bitmap = CreateCompatibleBitmap(DeviceContext, TextureWidth, TextureHeight);
    Assert(Bitmap);
    SelectObject(DeviceContext, Bitmap);

    
    u32 PitchX = 0;
    u32 PitchY = 0;
    for(s32 i = 33; i < 127; ++i) {
        wchar_t Character = (wchar_t)i;
        TextOutW(DeviceContext, PitchX, PitchY, &Character, 1);

        if((i-32) % 20 == 0) {
            PitchX = 0;
            PitchY += CharSize.cy;
        } else {
            PitchX += CharSize.cx;
        }
    }

    u32* Buffer = (u32*)VirtualAlloc(NULL, sizeof(u32)*TextureWidth*TextureHeight,
                                     MEM_COMMIT|MEM_RESERVE,
                                     PAGE_READWRITE);

    u32* PixelPtr = Buffer;
    
    for(s32 v = 0; v < TextureHeight; ++v) {
        for(s32 u = 0; u < TextureWidth; ++u) {
            u32 Color =  GetPixel(DeviceContext, u, TextureHeight - v - 1);
            *PixelPtr = Color;
            ++PixelPtr;
        }
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TextureWidth, TextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, Buffer);

    return Texture;
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

    GLuint texture = CreateFontTexture();
    
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
