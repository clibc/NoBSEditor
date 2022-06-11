#include <iostream>
#include <windows.h>
#include <assert.h>
#include <GL/gl.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "defines.h"
#include "math.hpp"
#include "opengl.hpp"
#include "utils.hpp"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

static bool Is_Running = true;
HDC Device_Context;

#define MAX_CHARACTER_BUFFER 1000
static char Text[MAX_CHARACTER_BUFFER];
static s32  CursorPos = 0;

static v2*
CreateFontLookupTable(const CreateFontTextureResult& FontData) {
    v2* TextureCoords = (v2*)AllocateMemory(sizeof(v2) * 94 * 4);

    f32 CW = (f32)FontData.CharacterWidth / (f32)FontData.TextureWidth;
    f32 CH = (f32)FontData.CharacterHeight / (f32)FontData.TextureHeight;

    s32 ArrayIndex = 0;
    for(s32 i = 33; i < 127; ++i) {
        s32 Index = i-33;
        f32 IndexX = (f32)(Index % FontData.CharacterPerLine);
        f32 IndexY = (f32)(Index / FontData.CharacterPerLine);

        // Top left
        TextureCoords[ArrayIndex + 0].x = CW*IndexX;
        TextureCoords[ArrayIndex + 0].y = 1.0f - CH * IndexY;
        // Top Rigth
        TextureCoords[ArrayIndex + 1].x = CW*IndexX + CW;
        TextureCoords[ArrayIndex + 1].y = 1.0f - CH * IndexY;
        // Bottom Left
        TextureCoords[ArrayIndex + 2].x = CW*IndexX;
        TextureCoords[ArrayIndex + 2].y = 1.0f - CH - CH * IndexY;
        // Bottom Right
        TextureCoords[ArrayIndex + 3].x = CW*IndexX + CW;
        TextureCoords[ArrayIndex + 3].y = 1.0f - CH - CH * IndexY;
        ArrayIndex += 4;
    }
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
        DebugLog("WM_SYSKEYDOWN: %c ", (char)wParam);
        DebugLog("%i\n", (char)wParam);
        Text[CursorPos++] = (char)wParam;
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    } 
    return 0; 
}

s32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                   LPSTR lpCmdLine, int nCmdShow) {
    Assert(AllocConsole());

    HWND window_handle = CreateOpenGLWindow(hInstance, nCmdShow,
                                            WINDOW_WIDTH, WINDOW_HEIGHT);
    Device_Context = GetDC(window_handle);

    CreateFontTextureResult TextureData = CreateFontTexture();
    v2* TextureLookupTable = CreateFontLookupTable(TextureData);
    
    // Define orthographic projection
    f32 Left  = 0;
    f32 Right = WINDOW_WIDTH;
    f32 Top   = WINDOW_HEIGHT;
    f32 Bottom = 0;
    f32 Near = 0;
    f32 Far = 5;
    
    m4 OrthoMatrix;
    OrthoMatrix.SetRow(0, 2.0f/(Right - Left), 0, 0, 0);
    OrthoMatrix.SetRow(1, 0, 2.0f/(Top - Bottom), 0, 0);
    OrthoMatrix.SetRow(2, 0, 0, -2.0f/(Far - Near), 0);
    OrthoMatrix.SetRow(3, -((Right + Left)/(Right - Left)), -((Top + Bottom)/(Top - Bottom)), -((Far+Near)/(Far-Near)), 1);

    f32 CharAspectRatio = 16.0f/30.0f;
    v3 Scale = v3(30, 30, 1);
    Scale.y = Scale.x / CharAspectRatio;

    GLuint TextShader   = LoadShaderFromFiles("../shaders/vert.shader", "../shaders/frag.shader");
    GLuint CursorShader = LoadShaderFromFiles("../shaders/cursor_vertex.shader", "../shaders/cursor_frag.shader");

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(v3));
    glVertexAttribIPointer(2, 1, GL_INT, sizeof(Vertex), (void*)(sizeof(v3)*2));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    u32 CursorVAO, CursorVBO;
    glGenVertexArrays(1, &CursorVAO);
    glBindVertexArray(CursorVAO);
    glGenBuffers(1, &CursorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, CursorVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CursorVertex), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CursorVertex), (void*)sizeof(v3));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    glUseProgram(TextShader);
    s32 OrthoMatrixLocation = glGetUniformLocation(TextShader, "OrthoMatrix");
    s32 OrthoMatrixLocationCursor = glGetUniformLocation(CursorShader, "OrthoMatrix");
    s32 TextureLookupTableLocation = glGetUniformLocation(TextShader, "TextureLookupTable");
    glUniformMatrix4fv(OrthoMatrixLocation, 1, GL_TRUE, (f32*)&OrthoMatrix);
    glUniform2fv(TextureLookupTableLocation, 94 * 4, (f32*)TextureLookupTable);

    glUseProgram(CursorShader);
    glUniformMatrix4fv(OrthoMatrixLocationCursor, 1, GL_TRUE, (f32*)&OrthoMatrix);

    WarnIfNot(OrthoMatrixLocation >= 0);
    WarnIfNot(OrthoMatrixLocationCursor >= 0);
    WarnIfNot(TextureLookupTableLocation >= 0);
    FreeMemory(TextureLookupTable);

    FrameArena Arena = FrameArenaCreate(Megabytes(2));
    
    TextBox Box = {WINDOW_WIDTH, WINDOW_HEIGHT, Scale.x*2, Scale.y*2};
    
    s32 CharactersPerLine = (s32)(WINDOW_WIDTH / (Scale.x*2));

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0) > 0 && Is_Running) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        glClearColor(30.0f/255.0f,30.0f/255.0f,30.0f/255.0f,30.0f/255.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        CursorDraw(Box, &Arena, CursorPos, CursorVAO, CursorVBO, CursorShader);

        TextBoxRenderState RenderState = TextBoxBeginDraw(Box, &Arena, VAO, VBO, TextShader);
        TextBoxPushText(RenderState, Text, CursorPos, v3(1,0,0));

        TextBoxEndDraw(RenderState);
        
        if (GetKeyState(VK_ESCAPE) & 0x8000) {
            Is_Running = false;
        }
        
        if(GetKeyState(VK_CONTROL) & 0x8000) {
            f32 ScaleAmount = 1.5f;
            if (GetKeyState(VK_ADD) & 0x8000) {
                Scale.x = Scale.x + ScaleAmount;
                Scale.y = Scale.x / CharAspectRatio;
                CharactersPerLine = (s32)Clamp(WINDOW_WIDTH / (Scale.x*2), 1, FLT_MAX);
            }
            else if (GetKeyState(VK_SUBTRACT) & 0x8000) {
                Scale.x = Scale.x - ScaleAmount;
                Scale.y = Scale.x / CharAspectRatio;
                CharactersPerLine = (s32)Clamp(WINDOW_WIDTH / (Scale.x*2), 1, FLT_MAX);
            }
            Box.CharacterWidth = Scale.x*2;
            Box.CharacterHeight = Scale.y*2;
        }
        
        SwapBuffers(Device_Context);
        FrameArenaReset(Arena);
    }

    FrameArenaDelete(Arena);
    return 0;
}
