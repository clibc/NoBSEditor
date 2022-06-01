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
#include "math.hpp"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

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

s32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                   LPSTR lpCmdLine, int nCmdShow) {
    assert(AllocConsole());

    HWND window_handle = CreateOpenGLWindow(hInstance, nCmdShow,
                                            WINDOW_WIDTH, WINDOW_HEIGHT);
    Device_Context = GetDC(window_handle);

    f32 vertices[] = {   //texture coords
        1.0, 1.0, 0.0,   1.0, 1.0,
        -1.0, -1.0, 0.0, 0.0, 0.0,
        1.0, -1.0, 0.0,  1.0, 0.0,
        1.0, 1.0, 0.0,   1.0, 1.0,
        -1.0, 1.0, 0.0,  0.0, 1.0,
        -1.0, -1.0, 0.0, 0.0, 0.0
    };

    f32 GliphWidth  = 1.0/30;
    f32 GliphHeight = 1.0/5;
    
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

    // Define orthographic projection
    f32 Left  = 0;
    f32 Right = WINDOW_WIDTH;
    f32 Top   = WINDOW_HEIGHT;
    f32 Bottom = 0;
    f32 Near = 0;
    f32 Far = 5;
    
    m4 OrthoMatrix;
    OrthoMatrix.SetRow(0, 2.0/(Right - Left), 0, 0, 0);
    OrthoMatrix.SetRow(1, 0, 2.0/(Top - Bottom), 0, 0);
    OrthoMatrix.SetRow(2, 0, 0, -2.0/(Far - Near), 0);
    OrthoMatrix.SetRow(3, -((Right + Left)/(Right - Left)), -((Top + Bottom)/(Top - Bottom)), -((Far+Near)/(Far-Near)), 1);

    v3 Position = v3(400, 300, 0);
    m4 TranslationMatrix;
    TranslationMatrix.SetRow(0, 1, 0, 0, 0);
    TranslationMatrix.SetRow(1, 0, 1, 0, 0);
    TranslationMatrix.SetRow(2, 0, 0, 1, 0);
    TranslationMatrix.SetRow(3, Position.x, Position.y, Position.z, 1);
   
    v3 Scale = v3(180, 140, 1);
    m4 ScaleMatrix;
    ScaleMatrix.SetRow(0, Scale.x, 0, 0, 0);
    ScaleMatrix.SetRow(1, 0, Scale.y, 0, 0);
    ScaleMatrix.SetRow(2, 0, 0, Scale.z, 0);
    ScaleMatrix.SetRow(3, 0, 0, 0,       1);

    m4 ModelMatrix = ScaleMatrix * TranslationMatrix;
    
    GLint OrthoMatrixLocation = glGetUniformLocation(shader_program, "OrthoMatrix");
    GLint ModelMatrixLocation = glGetUniformLocation(shader_program, "ModelMatrix");
    glUniformMatrix4fv(OrthoMatrixLocation, 1, GL_TRUE, (f32*)&OrthoMatrix);
    glUniformMatrix4fv(ModelMatrixLocation, 1, GL_TRUE, (f32*)&ModelMatrix);
    
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
