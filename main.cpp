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
#include "input.hpp"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

static bool Is_Running = true;
HDC Device_Context;

#define MAX_CHARACTER_BUFFER 1000
static char Text[MAX_CHARACTER_BUFFER];
static s32  TextSize = 0;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
    switch (uMsg) 
    {
    case WM_CREATE: 
    {
        InitializeOpenGL(hwnd);
        return 0;
    } break;
    case WM_SIZE: 
    {
        const u32 Width  = LOWORD(lParam);
        const u32 Height = HIWORD(lParam);
        glViewport(0, 0, (GLint)Width, (GLint)Height);
    } break;
    case WM_DESTROY: 
    {
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
                   LPSTR lpCmdLine, int nCmdShow) 
{
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
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(CursorVertex), (void*)sizeof(v3));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(CursorVertex), (void*)(sizeof(v3) + sizeof(v4)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

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
    
    char* FillText = "Test text thomg\n\n\nldsaoflasdfoasd f\nint main() {\nreturn 0;\n}";
    strcpy_s(Text, FillText);
    TextSize += (u32)strlen(FillText);
    SplitBuffer SB = SplitBufferCreate(1024, Text, TextSize);
    CalculateLinesResult Lines = CalculateLinesSB(Box, &Arena, SB);

    s32 CursorX = 0;
    s32 CursorY = 0;
    // TODO : Store secondary cursor position in 'Text Space'
    u32 SecondaryCursorPos = 0;
    bool IsCursorMoved = false;

    InputHandle Input;
    MSG M;
    while(GetMessage(&M, NULL, WM_KEYFIRST, WM_KEYLAST) > 0 && Is_Running)
    {
        ProcessInputWin32(&Input, M);

        if(GetKeyDown(Input, KeyCode_Escape))
        {
            Is_Running = false;
        }

        if(GetKeyDown(Input, KeyCode_Shift))
        {
            SecondaryCursorPos = Lines.Lines[CursorY].StartIndex + CursorX;
        }

        if(GetKey(Input, KeyCode_Up))
        {
            u32 LineCount = Max(0, Lines.Count - 1);
            CursorY = Clamp(CursorY - 1, 0, LineCount);
            CursorX = Clamp(CursorX, 0, Lines.Lines[CursorY].EndIndex - Lines.Lines[CursorY].StartIndex);
            IsCursorMoved = true;
        }
        else if(GetKey(Input, KeyCode_Down))
        {
            u32 LineCount = Max(0, Lines.Count - 1);
            CursorY = Clamp(CursorY + 1, 0, LineCount);
            CursorX = Clamp(CursorX, 0, Lines.Lines[CursorY].EndIndex - Lines.Lines[CursorY].StartIndex);
            IsCursorMoved = true;
        }
        else if(GetKey(Input, KeyCode_Left))
        {
            CursorMoveLeft(&CursorX, &CursorY, Lines);
            IsCursorMoved = true;
        }
        else if(GetKey(Input, KeyCode_Right))
        {
            CursorMoveRight(&CursorX, &CursorY, Lines);
            IsCursorMoved = true;
        }
        else if(GetKey(Input, KeyCode_Delete))
        {
            if(IsCursorMoved)
            {
                SplitBufferSetCursor(SB, Lines.Lines[CursorY].StartIndex + CursorX);
            }
            SplitBufferRemoveCharDeleteKey(SB);

            if(SecondaryCursorPos > CursorScreenToText(&Lines, CursorX, CursorY))
            {
                SecondaryCursorPos -= 1;
            }
        }
        else if(GetKey(Input, KeyCode_Backspace))
        {
            if(IsCursorMoved)
            {
                SplitBufferSetCursor(SB, Lines.Lines[CursorY].StartIndex + CursorX);
            }
            SplitBufferRemoveCharBackKey(SB);
            CursorMoveLeft(&CursorX, &CursorY, Lines);
            if(SecondaryCursorPos > CursorScreenToText(&Lines, CursorX, CursorY))
            {
                SecondaryCursorPos -= 1;
            }
        }
           
        if(M.message == WM_CHAR && M.wParam != VK_BACK) 
        {
            u8 Char = (u8)M.wParam;

            if(IsCursorMoved)
            {
                SplitBufferSetCursor(SB, Lines.Lines[CursorY].StartIndex + CursorX);
            }

            if(Char >= 32 && Char <= 126) 
            {
                CursorX += 1;
            }
            else if (Char == '\n' || Char == '\r')
            {
                Char = '\n';
                CursorY += 1;
                CursorX = 0;
            }
            SplitBufferAddChar(SB, Char);
            IsCursorMoved = false;
            if(SecondaryCursorPos > CursorScreenToText(&Lines, CursorX, CursorY))
            {
                SecondaryCursorPos += 1;
            }
        }
        
        if(GetKey(Input, KeyCode_Ctrl))
        {
            if(GetKeyDown(Input, KeyCode_C))
            {
                DebugLog("Copy\n");
            }
            if(GetKeyDown(Input, KeyCode_V))
            {
                DebugLog("Paste\n");
            }
        }
        
        glClearColor(30.0f/255.0f,30.0f/255.0f,30.0f/255.0f,30.0f/255.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Lines = CalculateLinesSB(Box, &Arena, SB);
        v2 SecondaryCursorScreenPosition = CursorTextToScreen(&Lines, SecondaryCursorPos);
        CursorDraw(Box, &Arena, v2(f32(CursorX), f32(CursorY)), 0.0f, CursorVAO, CursorVBO, CursorShader);
        CursorDraw(Box, &Arena, SecondaryCursorScreenPosition, 1.0f, CursorVAO, CursorVBO, CursorShader);

        TextBoxRenderState RenderState = TextBoxBeginDraw(Box, &Arena, &Lines, TextVAO, TextVBO, TextShader);

        TextBoxPushText(RenderState, SB.Start, SB.Middle, v3(1,0,1));
        TextBoxPushText(RenderState, SB.Start + SB.Second, SB.TextSize - SB.Middle, v3(1,1,1));
        TextBoxEndDraw(RenderState);
        
        glFlush();
        SwapBuffers(Device_Context);
        FrameArenaReset(Arena);
    }

    FrameArenaDelete(Arena);
    return 0;
}
