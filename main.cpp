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
static s32  TextSize = 0;

static inline v2
CursorScreenPositionToText(CalculateLinesResult* Lines, v2 CursorPosition) {
    u32 LineIndex = TruncateF32ToS32(CursorPosition.y);
    u32 CharacterIndex = TruncateF32ToS32(CursorPosition.x);

    LineIndex = Clamp(LineIndex, 0, Lines->LineCount - 1);
    CharacterIndex = Clamp(CharacterIndex, 0, Lines->Lines[LineIndex].EndIndex - Lines->Lines[LineIndex].StartIndex);
    
    return v2((f32)CharacterIndex, (f32)LineIndex);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { 
    switch (uMsg) 
    { 
    case WM_CREATE: {
        InitializeOpenGL(hwnd);
        return 0;
    } break;
    case WM_SIZE: {
        const u32 Width  = LOWORD(lParam);
        const u32 Height = HIWORD(lParam);
        glViewport(0, 0, (GLint)Width, (GLint)Height);
    } break;
    case WM_DESTROY: {
        Is_Running = false;
    } break;
    case WM_CHAR:
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
    v3 Scale = v3(12, 30, 1);
    Scale.y = Scale.x / CharAspectRatio;

    GLuint TextShader   = LoadShaderFromFiles("../shaders/vert.shader", "../shaders/frag.shader");
    GLuint CursorShader = LoadShaderFromFiles("../shaders/cursor_vertex.shader", "../shaders/cursor_frag.shader");

    GLuint TextVAO, TextVBO;
    glGenVertexArrays(1, &TextVAO);
    glBindVertexArray(TextVAO);
    glGenBuffers(1, &TextVBO);
    glBindBuffer(GL_ARRAY_BUFFER, TextVBO);
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
    
    char* FillText = "Test text thomg\n\n\nldsaoflasdfoasd f \nint main() {\n    return 0 \n}";
    strcpy(Text, FillText);
    TextSize += (u32)strlen(FillText);

    CalculateLinesResult Lines = CalculateLines(Box, &Arena, Text, TextSize);
    
    MSG Msg;

    s32 CursorX = 0;
    s32 CursorY = 0;

    while(GetMessage(&Msg, NULL, 0, 0) > 0 && Is_Running) {
        TranslateMessage(&Msg);
        if(Msg.message == WM_KEYDOWN) {
            if((Msg.lParam & (1 << 30)) == 0) {
                // Key down
            }
            else {
                // Key hold down
            }

            if(Msg.wParam == VK_ESCAPE)
            {
                Is_Running = false;
            }

            if(Msg.wParam == VK_UP)
            {
                CursorY = Clamp(CursorY - 1, 0, Lines.LineCount - 1);
                CursorX = Clamp(CursorX, 0, Lines.Lines[CursorY].EndIndex - Lines.Lines[CursorY].StartIndex);
            }
            else if (Msg.wParam == VK_DOWN)
            {
                CursorY = Clamp(CursorY + 1, 0, Lines.LineCount - 1);
                CursorX = Clamp(CursorX, 0, Lines.Lines[CursorY].EndIndex - Lines.Lines[CursorY].StartIndex);
            }
            else if (Msg.wParam == VK_LEFT)
            {
                CursorX -= 1;
                if(CursorX < 0) { // go one up
                    if(CursorY > 0) {
                        CursorY -= 1;
                        CursorX = Lines.Lines[CursorY].EndIndex - Lines.Lines[CursorY].StartIndex;
                    }
                    else { // first row
                        CursorX = 0;
                    }
                }
            }
            else if (Msg.wParam == VK_RIGHT)
            {
                CursorX += 1;
                if(CursorX > (s32)(Lines.Lines[CursorY].EndIndex - Lines.Lines[CursorY].StartIndex)) { 
                    if(CursorY < (s32)Lines.LineCount - 1) { // go one down
                        CursorX = 0;
                        CursorY += 1;
                    }
                    else { // Last row last colomn
                        CursorX -= 1;
                    }
                }
            }
        }
        else if(Msg.message == WM_CHAR) {
            u8 Char = (u8)Msg.wParam;
            if(Char >= 32 && Char <= 126) {
                Text[TextSize++] = Char;
            }
            else if (Char == '\n' || Char == '\r') {
                Text[TextSize++] = Char;
            }
        }
        else {
            DispatchMessage(&Msg);
        }

        glClearColor(30.0f/255.0f,30.0f/255.0f,30.0f/255.0f,30.0f/255.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Lines = CalculateLines(Box, &Arena, Text, TextSize);
        TextBoxRenderState RenderState = TextBoxBeginDraw(Box, &Arena, &Lines, TextVAO, TextVBO, TextShader);
        TextBoxPushText(RenderState, Text, TextSize, v3(1,0,0));
        TextBoxEndDraw(RenderState);

        v2 CursorPositionVector = v2((f32)CursorX, (f32)CursorY);
        CursorDraw(Box, &Arena, CursorPositionVector, CursorVAO, CursorVBO, CursorShader);
        
        glFlush();
        SwapBuffers(Device_Context);
        FrameArenaReset(Arena);
    }

    FrameArenaDelete(Arena);
    return 0;
}
