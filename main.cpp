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

#define MAX_CLIPBOARD_BUFFER 1000
static char Clipboard[MAX_CLIPBOARD_BUFFER];
static s32 TextSize = 0;
static s32 ClipboardSize = 0;

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
    v3 TextColor = v3(51, 82, 59) / 200.0f;

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
    SplitBuffer SB = SplitBufferCreate(1024, FillText, (u32)strlen(FillText));
    CalculateLinesResult Lines = CalculateLinesSB(Box, &Arena, SB);

    s32 CursorX = 0;
    s32 CursorY = 0;

    u32 SecondaryCursorPos = 0;
    bool IsCursorMoved = false;

    InputHandle Input;
    MSG M;
    while(Is_Running && GetMessage(&M, window_handle, 0,0))
    {
        ProcessInputWin32(&Input,M);
        
        if(GetKeyDown(Input, KeyCode_Escape))
        {
            Is_Running = false;
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

            u32 SecondaryAnvanceAmount = 1;
            
            if(Char == '\t') // so only spaces supported lol
            {
                SplitBufferAddChar(SB, ' ');
                SplitBufferAddChar(SB, ' ');
                SplitBufferAddChar(SB, ' ');
                SplitBufferAddChar(SB, ' ');
                CursorX += 4;
                SecondaryAnvanceAmount = 4;
            }
            else
            {
                SplitBufferAddChar(SB, Char);
            }
            
            IsCursorMoved = false;
            if(SecondaryCursorPos > CursorScreenToText(&Lines, CursorX, CursorY))
            {
                SecondaryCursorPos += SecondaryAnvanceAmount;
            }
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

        if(GetKey(Input, KeyCode_Ctrl))
        {
            if(GetKeyDown(Input, KeyCode_C))
            {
                DebugLog("Copy\n");

                u32 PrimaryCursorPos = Lines.Lines[CursorY].StartIndex + CursorX;
                
                u32 CopyTextSize = 0;

                if(PrimaryCursorPos <= SB.Middle && SecondaryCursorPos <= SB.Middle) // Both in first segment
                {
                    if(PrimaryCursorPos > SecondaryCursorPos) // Primary is ahead of Secondary
                    {
                        CopyTextSize = PrimaryCursorPos - SecondaryCursorPos;
                        Memcpy(SB.Start + SecondaryCursorPos, Clipboard, CopyTextSize);
                    }
                    else // Secondary is ahead of Primary
                    {
                        CopyTextSize = SecondaryCursorPos - PrimaryCursorPos;
                        Memcpy(SB.Start + PrimaryCursorPos, Clipboard, CopyTextSize);
                    }
                }
                else if (PrimaryCursorPos >= SB.Middle && SecondaryCursorPos <= SB.Middle) // Primary is in second, Secondary is in first
                {
                    CopyTextSize = SB.Middle - SecondaryCursorPos;
                    Memcpy(SB.Start + SecondaryCursorPos, Clipboard, CopyTextSize);
                    Memcpy(SB.Start + SB.Second, Clipboard + CopyTextSize, (PrimaryCursorPos + (SB.Second - SB.Middle)) - SB.Second);
                    CopyTextSize += (PrimaryCursorPos + (SB.Second - SB.Middle)) - SB.Second;
                }
                else if (PrimaryCursorPos <= SB.Middle && SecondaryCursorPos >= SB.Middle) // Primary is in first, Secondary is in second
                {
                    CopyTextSize = SB.Middle - PrimaryCursorPos;
                    Memcpy(SB.Start + PrimaryCursorPos, Clipboard, CopyTextSize);
                    Memcpy(SB.Start + SB.Second, Clipboard + CopyTextSize, (SecondaryCursorPos + (SB.Second - SB.Middle)) - SB.Second);
                    CopyTextSize += (SecondaryCursorPos + (SB.Second - SB.Middle)) - SB.Second;
                }
                else if (PrimaryCursorPos >= SB.Middle && SecondaryCursorPos >= SB.Middle) // Both in second segment
                {
                    u32 Primary   = (PrimaryCursorPos + (SB.Second - SB.Middle));
                    u32 Secondary = (SecondaryCursorPos + (SB.Second - SB.Middle));
                    
                    if(Primary > Secondary) // Primary is ahead of Secondary
                    {
                        CopyTextSize = Primary - Secondary;
                        Memcpy(SB.Start + Secondary, Clipboard, CopyTextSize);
                    }
                    else // Secondary is ahead of Primary
                    {
                        CopyTextSize = Secondary - Primary;
                        Memcpy(SB.Start + Primary, Clipboard, CopyTextSize);
                    }
                }
                ClipboardSize = CopyTextSize;
                Clipboard[ClipboardSize] = 0;
                //DebugLog("%s\n", Clipboard);
            }
            else if(GetKeyDown(Input, KeyCode_Y))
            {
                DebugLog("Paste\n");

                SecondaryCursorPos = CursorScreenToText(&Lines, CursorX, CursorY);
                SplitBufferSetCursor(SB, SecondaryCursorPos);
                Memcpy(Clipboard, SB.Start + SB.Middle, ClipboardSize);
                SB.Middle += ClipboardSize; // TODO: Handle buffer collisions
                SB.TextSize += ClipboardSize; // TODO: Handle buffer collisions
                
                Lines = CalculateLinesSB(Box, &Arena, SB);

                v2 NewCursorPos = CursorTextToScreen(&Lines, SB.Middle);

                CursorX = TruncateF32ToS32(NewCursorPos.x);
                CursorY = TruncateF32ToS32(NewCursorPos.y);
            }
            else if(GetKeyDown(Input, KeyCode_W))
            {
                // Cutting is same as coping but you just delete after
                DebugLog("Cut\n");
                
                u32 PrimaryCursorPos = Lines.Lines[CursorY].StartIndex + CursorX;
                
                u32 CopyTextSize = 0;

                if(PrimaryCursorPos <= SB.Middle && SecondaryCursorPos <= SB.Middle) // Both in first segment
                {
                    if(PrimaryCursorPos > SecondaryCursorPos) // Primary is ahead of Secondary
                    {
                        CopyTextSize = PrimaryCursorPos - SecondaryCursorPos;
                        Memcpy(SB.Start + SecondaryCursorPos, Clipboard, CopyTextSize);
                    }
                    else // Secondary is ahead of Primary
                    {
                        CopyTextSize = SecondaryCursorPos - PrimaryCursorPos;
                        Memcpy(SB.Start + PrimaryCursorPos, Clipboard, CopyTextSize);
                    }
                }
                else if (PrimaryCursorPos >= SB.Middle && SecondaryCursorPos <= SB.Middle) // Primary is in second, Secondary is in first
                {
                    CopyTextSize = SB.Middle - SecondaryCursorPos;
                    Memcpy(SB.Start + SecondaryCursorPos, Clipboard, CopyTextSize);
                    Memcpy(SB.Start + SB.Second, Clipboard + CopyTextSize, (PrimaryCursorPos + (SB.Second - SB.Middle)) - SB.Second);
                    CopyTextSize += (PrimaryCursorPos + (SB.Second - SB.Middle)) - SB.Second;
                }
                else if (PrimaryCursorPos <= SB.Middle && SecondaryCursorPos >= SB.Middle) // Primary is in first, Secondary is in second
                {
                    CopyTextSize = SB.Middle - PrimaryCursorPos;
                    Memcpy(SB.Start + PrimaryCursorPos, Clipboard, CopyTextSize);
                    Memcpy(SB.Start + SB.Second, Clipboard + CopyTextSize, (SecondaryCursorPos + (SB.Second - SB.Middle)) - SB.Second);
                    CopyTextSize += (SecondaryCursorPos + (SB.Second - SB.Middle)) - SB.Second;
                }
                else if (PrimaryCursorPos >= SB.Middle && SecondaryCursorPos >= SB.Middle) // Both in second segment
                {
                    u32 Primary   = (PrimaryCursorPos + (SB.Second - SB.Middle));
                    u32 Secondary = (SecondaryCursorPos + (SB.Second - SB.Middle));
                    
                    if(Primary > Secondary) // Primary is ahead of Secondary
                    {
                        CopyTextSize = Primary - Secondary;
                        Memcpy(SB.Start + Secondary, Clipboard, CopyTextSize);
                    }
                    else // Secondary is ahead of Primary
                    {
                        CopyTextSize = Secondary - Primary;
                        Memcpy(SB.Start + Primary, Clipboard, CopyTextSize);
                    }
                }
                ClipboardSize = CopyTextSize;
                Clipboard[ClipboardSize] = 0;

                SplitBufferSetCursor(SB, CursorScreenToText(&Lines, CursorX, CursorY));
                if(PrimaryCursorPos < SecondaryCursorPos) // Delete forwards
                {
                    for(s32 I = 0; I < ClipboardSize; ++I)
                    {
                        SplitBufferRemoveCharDeleteKey(SB);
                    }
                }
                else // Delete backwards
                {
                    for(s32 I = 0; I < ClipboardSize; ++I)
                    {
                        SplitBufferRemoveCharBackKey(SB);
                    }
                }

                // update screen poition && secondary cursor position
                SecondaryCursorPos = SB.Middle;
                v2 NewCursorPos = CursorTextToScreen(&Lines, SB.Middle);
                CursorX = TruncateF32ToS32(NewCursorPos.x);
                CursorY = TruncateF32ToS32(NewCursorPos.y);
            }
            else if(GetKeyDown(Input, KeyCode_Space))
            {
                SecondaryCursorPos = CursorScreenToText(&Lines, CursorX, CursorY);
            }
        }

        if(GetKey(Input, KeyCode_Alt) && GetKeyDown(Input, KeyCode_W))
        {
            DebugLog("Alt Copy\n");
        }

        if(GetKey(Input, KeyCode_Ctrl) && GetKeyDown(Input, KeyCode_P))
        {
            DebugLog("CTRL PLUS\n");
            Scale.x += 1.5f;
            Scale.y = Scale.x / CharAspectRatio;
            Box = {WINDOW_WIDTH, WINDOW_HEIGHT, Scale.x*2, Scale.y*2};
        }
        if(GetKey(Input, KeyCode_Ctrl) && GetKeyDown(Input, KeyCode_M))
        {
            DebugLog("CTRL MINUS\n");
            Scale.x -= 1.5f;
            Scale.y = Scale.x / CharAspectRatio;
            Box = {WINDOW_WIDTH, WINDOW_HEIGHT, Scale.x*2, Scale.y*2};
        }
        
        glClearColor(30.0f/255.0f,30.0f/255.0f,30.0f/255.0f,30.0f/255.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Lines = CalculateLinesSB(Box, &Arena, SB);
        v2 SecondaryCursorScreenPosition = CursorTextToScreen(&Lines, SecondaryCursorPos);
        CursorDraw(Box, &Arena, v2(f32(CursorX), f32(CursorY)), 0.0f, CursorVAO, CursorVBO, CursorShader);
        CursorDraw(Box, &Arena, SecondaryCursorScreenPosition, 1.0f, CursorVAO, CursorVBO, CursorShader);

        TextBoxRenderState RenderState = TextBoxBeginDraw(Box, &Arena, &Lines, TextVAO, TextVBO, TextShader);

        TextBoxPushText(RenderState, SB.Start, SB.Middle, v3(1,0,1));
        TextBoxPushText(RenderState, SB.Start + SB.Second, SB.TextSize - SB.Middle, TextColor);
        TextBoxEndDraw(RenderState);
        
        glFlush();
        SwapBuffers(Device_Context);
        FrameArenaReset(Arena);
    }

    FrameArenaDelete(Arena);
    return 0;
}
