#include <iostream>
#include <windows.h>
#include <assert.h>
#include <GL/gl.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

// Disable type convertion warning
#pragma warning(disable : 4244)

#include "defines.h"
#include "opengl.hpp"
#include "utils.hpp"
#include "math.hpp"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

static bool Is_Running = true;
HDC Device_Context;

static char Text[150] = {};
static s32  CursorPos = 0;

static v2*
CreateFontLookupTable(const CreateFontTextureResult& FontData) {
    v2* TextureCoords = (v2*)AllocateMemory(sizeof(v2) * 94 * 4);

    f32 CW = (f32)FontData.CharacterWidth / (f32)FontData.TextureWidth;
    f32 CH = (f32)FontData.CharacterHeight / (f32)FontData.TextureHeight;

    s32 ArrayIndex = 0;
    for(s32 i = 33; i < 127; ++i) {
        s32 Index = i-33;
        f32 IndexX = Index % FontData.CharacterPerLine;
        f32 IndexY = Index / FontData.CharacterPerLine;

        // Top left
        TextureCoords[ArrayIndex + 0].x = CW*IndexX;
        TextureCoords[ArrayIndex + 0].y = 1.0 - CH * IndexY;
        // Top Rigth
        TextureCoords[ArrayIndex + 1].x = CW*IndexX + CW;
        TextureCoords[ArrayIndex + 1].y = 1.0 - CH * IndexY;
        // Bottom Left
        TextureCoords[ArrayIndex + 2].x = CW*IndexX;
        TextureCoords[ArrayIndex + 2].y = 1.0 - CH - CH * IndexY;
        // Bottom Right
        TextureCoords[ArrayIndex + 3].x = CW*IndexX + CW;
        TextureCoords[ArrayIndex + 3].y = 1.0 - CH - CH * IndexY;
        ArrayIndex += 4;
    }

    DebugLog("Ended\n");
    return TextureCoords;
}

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
    case WM_CHAR:
        DebugLog("WM_SYSKEYDOWN: %c\n", (char)wParam);
        Text[CursorPos++] = (char)wParam;
        break;
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

    CreateFontTextureResult TextureData = CreateFontTexture();
    v2* TextureLookupTable = CreateFontLookupTable(TextureData);

#if 0
    f32 vertices[] = {   //texture coords
        1.0, 1.0, 0.0,   // Top Right 0
        -1.0, -1.0, 0.0, // Bot left  1
        1.0, -1.0, 0.0,  // Bot Right 2
        1.0, 1.0, 0.0,   // Top Right 0
        -1.0, 1.0, 0.0,  // Top Left  3
        -1.0, -1.0, 0.0, // Bot Left  1
    };
    f32 Vertices[] = {   //texture coords
        1.0, 1.0, 0.0,   // Top Right 0
        -1.0, -1.0, 0.0, // Bot left  1
        1.0, -1.0, 0.0,  // Bot Right 2
        -1.0, 1.0, 0.0,  // Top Left  3
    };
    s32 VertexIndices[] = {0, 1, 2, 0, 3, 1};
#endif
    
    f32 Vertices[] = {   //texture coords
        -1.0, 1.0, 0.0,  // Top Left  0
        1.0, 1.0, 0.0,   // Top Right 1
        -1.0, -1.0, 0.0, // Bot left  2
        1.0, -1.0, 0.0,  // Bot Right 3
    };
    
    s32 VertexIndices[] = {1, 2, 3, 1, 0, 2};

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexIndices), VertexIndices, GL_STATIC_DRAW);

    GLuint shader_program = LoadShaderFromFiles("../shaders/vert.shader", "../shaders/frag.shader");

    glUseProgram(shader_program);
    glVertexAttribIPointer(0, 1, GL_INT, 0, 0);
    glEnableVertexAttribArray(0);
    
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

    v3 Position = v3(0, 0, 0);
    m4 TranslationMatrix;
    TranslationMatrix.SetRow(0, 1, 0, 0, 0);
    TranslationMatrix.SetRow(1, 0, 1, 0, 0);
    TranslationMatrix.SetRow(2, 0, 0, 1, 0);
    TranslationMatrix.SetRow(3, Position.x, Position.y, Position.z, 1);

    f32 CharAspectRatio = 16.0f/30.0f;
    v3 Scale = v3(16, 30, 1);
    Scale.y /= CharAspectRatio;
    m4 ScaleMatrix;
    ScaleMatrix.SetRow(0, Scale.x, 0, 0, 0);
    ScaleMatrix.SetRow(1, 0, Scale.y, 0, 0);
    ScaleMatrix.SetRow(2, 0, 0, Scale.z, 0);
    ScaleMatrix.SetRow(3, 0, 0, 0,       1);

    m4 ModelMatrix = ScaleMatrix * TranslationMatrix;
    
    s32 OrthoMatrixLocation = glGetUniformLocation(shader_program, "OrthoMatrix");
    s32 ModelMatrixLocation = glGetUniformLocation(shader_program, "ModelMatrix");
    s32 TextureLookupTableLocation = glGetUniformLocation(shader_program, "TextureLookupTable");
    s32 VertexLookupTableLocation = glGetUniformLocation(shader_program, "VertexLookupTable");
    s32 CharacterIndexLocation = glGetUniformLocation(shader_program, "CharacterIndex");
    glUniformMatrix4fv(OrthoMatrixLocation, 1, GL_TRUE, (f32*)&OrthoMatrix);
    glUniformMatrix4fv(ModelMatrixLocation, 1, GL_TRUE, (f32*)&ModelMatrix);
    glUniform2fv(TextureLookupTableLocation, 94 * 4, (f32*)TextureLookupTable);
    glUniform3fv(VertexLookupTableLocation, 6, (f32*)Vertices);
    assert(TextureLookupTableLocation > 0);
    assert(VertexLookupTableLocation > 0);
    assert(CharacterIndexLocation > 0);

    FreeMemory(TextureLookupTable);
    
    s32 CharactersPerLine = WINDOW_WIDTH / (Scale.x*2);
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0) > 0 && Is_Running) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        
        glClearColor(1,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT);

        for(u32 i = 0; i < 150; ++i) {
            if(Text[i] == 0) break;

            Position = v3((i%CharactersPerLine) * Scale.x*2 + Scale.x, WINDOW_HEIGHT - Scale.y - (i/CharactersPerLine) * Scale.y*2, 0);
            TranslationMatrix.SetRow(0, 1, 0, 0, 0);
            TranslationMatrix.SetRow(1, 0, 1, 0, 0);
            TranslationMatrix.SetRow(2, 0, 0, 1, 0);
            TranslationMatrix.SetRow(3, Position.x, Position.y, Position.z, 1);
            
            ModelMatrix = ScaleMatrix * TranslationMatrix;
            glUniformMatrix4fv(ModelMatrixLocation, 1, GL_TRUE, (f32*)&ModelMatrix);

            glUniform1i(CharacterIndexLocation, Text[i]-33);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        
        if (GetKeyState(VK_ESCAPE) & 0x8000)
        {
            Is_Running = false;
        }

        SwapBuffers(Device_Context);
    }
    
    return 0;
}
