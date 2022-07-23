#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720
#define TARGET_FPS 60
#define SCROLL_SIZE 6

#include <windows.h>

#include "defines.h"
#include "math.hpp"
#include "opengl.hpp"

#include "utils.hpp"
#include "input.hpp"

static bool Is_Running = true;
HDC Device_Context;

#define MAX_CLIPBOARD_BUFFER 1000
static char Clipboard[MAX_CLIPBOARD_BUFFER];
static s32 TextSize = 0;
static s32 ClipboardSize = 0;
static u32 FirstLineIndexOnScreen = 0;

static inline s64
GetPerformanceCounter()
{
    LARGE_INTEGER Ticks;
    Assert(QueryPerformanceCounter(&Ticks))
    return Ticks.QuadPart;
}

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

s32
main()
{
    // Set timer interval to 1 ms
    timeBeginPeriod(1);

    HWND window_handle = CreateOpenGLWindow(GetModuleHandle(0),
                                            WINDOW_WIDTH, WINDOW_HEIGHT);
    Device_Context = GetDC(window_handle);
    
    CreateFontTextureResult TextureData = CreateFontTexture();
    v2* TextureLookupTable = CreateFontLookupTable(TextureData);

    // NOTE: Define orthographic projection
    f32 Left  = 0;
    f32 Right = WINDOW_WIDTH;
    f32 Top   = WINDOW_HEIGHT;
    f32 Bottom = 0;
    f32 Near = 0;
    f32 Far = 5;
    m4 OrthoMatrix = MakeOrthoMatrix(Left, Right, Top, Bottom, Near, Far);
    f32 CharAspectRatio = 16.0f/30.0f;
    v3 Scale = v3(12, 30, 1);
    Scale.y = Scale.x / CharAspectRatio;
    v3 TextColor = v3(51, 82, 59) / 200.0f;

    GLuint TextShader   = LoadShaderFromFiles("../shaders/vert.shader", "../shaders/frag.shader");
    GLuint CursorShader = LoadShaderFromFiles("../shaders/cursor_vertex.shader", "../shaders/cursor_frag.shader");
    // NOTE: This is for drawing color filled boxes
    // NOTE: Ortho matrix does not change when scrolling
    // NOTE: Used for debugging for now
    GLuint ScreenSpaceBoxShader = LoadShaderFromFiles("../shaders/cursor_vertex.shader", "../shaders/cursor_frag.shader");
    GLuint ScreenSpaceTextShader = LoadShaderFromFiles("../shaders/vert.shader", "../shaders/frag.shader");

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

    glUseProgram(ScreenSpaceTextShader);
    u32 OrthoMatrixLocationScreen = glGetUniformLocation(ScreenSpaceTextShader, "OrthoMatrix");
    u32 TextureLookupTableLocationScreen = glGetUniformLocation(ScreenSpaceTextShader, "TextureLookupTable");
    glUniformMatrix4fv(OrthoMatrixLocationScreen, 1, GL_TRUE, (f32*)&OrthoMatrix);
    glUniform2fv(TextureLookupTableLocationScreen, 94 * 4, (f32*)TextureLookupTable);

    glUseProgram(CursorShader);
    glUniformMatrix4fv(OrthoMatrixLocationCursor, 1, GL_TRUE, (f32*)&OrthoMatrix);

    glUseProgram(ScreenSpaceBoxShader);
    glUniformMatrix4fv(OrthoMatrixLocationCursor, 1, GL_TRUE, (f32*)&OrthoMatrix);

    WarnIfNot(OrthoMatrixLocation >= 0);
    WarnIfNot(OrthoMatrixLocationCursor >= 0);
    WarnIfNot(TextureLookupTableLocation >= 0);
    WarnIfNot(OrthoMatrixLocationScreen >= 0);
    WarnIfNot(TextureLookupTableLocationScreen >= 0);
    FreeMemory(TextureLookupTable);

    FrameArena Arena = FrameArenaCreate(Megabytes(2));
    
    TextBox Box = {WINDOW_WIDTH, WINDOW_HEIGHT, Scale.x*2, Scale.y*2, {200,0,0}};
    TextBox DebugBox = {300, 150, Scale.x, Scale.y, {WINDOW_WIDTH - 300,0,0}};
    
    char* FillText = "Test text thomg\n\n\nldsaoflasdfoasd f\nint main() {\nreturn 0;\n} TestText1\nTestText2\nTestText3\nTestText4\nTestText5\nTestText6\nTestText7\nTestText8\nTestText9\nTestText10";
    SplitBuffer SB = SplitBufferCreate(1024, FillText, (u32)strlen(FillText));
    CalculateLinesResult Lines = CalculateLinesSB(Box, &Arena, SB);

    u32 PrimaryCursorPos = 0;
    u32 SecondaryCursorPos = 0;
    bool IsCursorMoved = false;

    // Scroll interpolation
    f32 CurrentTop = Top;
    f32 CurrentBottom = Bottom;
    f32 TargetTop = Top;
    f32 TargetBottom = Bottom;
    f32 ScrollSpeeed = 15;
    //

    // Timing vars
    LARGE_INTEGER FrequencyReturn;
    QueryPerformanceFrequency(&FrequencyReturn);
    s64 Frequency = FrequencyReturn.QuadPart;
    s64 FrameStartCounter = GetPerformanceCounter();
    f32 DeltaTime = 0.0f;
    f64 TimeSinceStart = 0.0;
    f64 ElapsedTimeInMs = 0;
    //
    
    InputHandle Input;
    MSG M = {};
    ProcessInputWin32(&Input,M);
    while(Is_Running)
    {
        M = {};
        KeyState* Keys = Input.Keys;
        for(s32 I = 0; I < KeyCode_Count; ++I)
        {
            if(I != KeyCode_Ctrl && I != KeyCode_Alt)
            { // CTRL & Alt keys are kinda nasty
                if(Keys[I] == DOWN)
                {
                    Keys[I] = NONE;
                }
                else if(Keys[I] == UP)
                {
                    Keys[I] = NONE;
                }
            }
        }

        while(PeekMessage(&M, window_handle, 0,0, PM_REMOVE))
        {
            if(M.message == WM_KEYDOWN || M.message == WM_SYSKEYDOWN
               || M.message == WM_KEYUP || M.message == WM_SYSKEYUP)
            {
                ProcessInputWin32(&Input,M);
            }
            else
            {
                DispatchMessage(&M);
            }
        }
        
        if(GetKeyDown(Input, KeyCode_Escape))
        {
            Is_Running = false;
        }

        if(M.message == WM_CHAR && M.wParam != VK_BACK) 
        {
            u8 Char = (u8)M.wParam;
            u32 CursorAdvanceAmount = 1;

            if(IsCursorMoved)
            {
                SplitBufferSetCursor(SB, PrimaryCursorPos);
                IsCursorMoved = false;
            }

            if (Char == '\n' || Char == '\r')
            {
                Char = '\n';
            }
            
            if(Char == '\t') // so only spaces supported lol
            {
                SplitBufferAddChar(SB, ' ');
                SplitBufferAddChar(SB, ' ');
                SplitBufferAddChar(SB, ' ');
                SplitBufferAddChar(SB, ' ');
                CursorAdvanceAmount = 4;
            }
            else
            {
                SplitBufferAddChar(SB, Char);
            }
            
            if(SecondaryCursorPos > PrimaryCursorPos)
            {
                SecondaryCursorPos += CursorAdvanceAmount;
            }
            PrimaryCursorPos += CursorAdvanceAmount;
        }
        
        if(GetKey(Input, KeyCode_Up) && !GetKey(Input, KeyCode_Alt))
        {
            v2 FakeScreenPos = CursorTextToScreen(&Lines, PrimaryCursorPos);
            u32 NewY = Clamp(TruncateF32ToS32(FakeScreenPos.y - 1), 0, Max(Lines.Count - 1, 0));
            u32 NewX = Clamp(TruncateF32ToS32(FakeScreenPos.x), 0, Lines.Lines[NewY].EndIndex - Lines.Lines[NewY].StartIndex);
            PrimaryCursorPos = Lines.Lines[NewY].StartIndex + NewX;
            IsCursorMoved = true;
        }
        else if(GetKey(Input, KeyCode_Down) && !GetKey(Input, KeyCode_Alt))
        {
            v2 FakeScreenPos = CursorTextToScreen(&Lines, PrimaryCursorPos);
            u32 NewY = Clamp(TruncateF32ToS32(FakeScreenPos.y + 1), 0, Max(Lines.Count - 1, 0));
            u32 NewX = Clamp(TruncateF32ToS32(FakeScreenPos.x), 0, Lines.Lines[NewY].EndIndex - Lines.Lines[NewY].StartIndex);
            PrimaryCursorPos = Lines.Lines[NewY].StartIndex + NewX;
            IsCursorMoved = true;
        }
        else if(GetKey(Input, KeyCode_Left))
        {
            PrimaryCursorPos = Clamp(PrimaryCursorPos -= 1, 0, SB.TextSize);
            IsCursorMoved = true;
        }
        else if(GetKey(Input, KeyCode_Right))
        {
            PrimaryCursorPos = Clamp(PrimaryCursorPos += 1, 0, SB.TextSize);            
            IsCursorMoved = true;
        }
        else if(GetKey(Input, KeyCode_Delete))
        {
            if(IsCursorMoved)
            {
                SplitBufferSetCursor(SB, PrimaryCursorPos);
            }

            SplitBufferRemoveCharDeleteKey(SB);

            if(SecondaryCursorPos > PrimaryCursorPos) // If secondary cursor position is ahead of primary move it's position one left 
            {
                SecondaryCursorPos -= 1;
            }
        }
        else if(GetKey(Input, KeyCode_Backspace))
        {
            if(IsCursorMoved)
            {
                SplitBufferSetCursor(SB, PrimaryCursorPos);
            }
            
            SplitBufferRemoveCharBackKey(SB);
            PrimaryCursorPos -= 1; // Cursor Move Left
            PrimaryCursorPos = Max(PrimaryCursorPos, 0);
            if(SecondaryCursorPos > PrimaryCursorPos) // If secondary cursor position is ahead of primary move it's position one left 
            {
                SecondaryCursorPos -= 1;
            }
        }

        if(GetKey(Input, KeyCode_Ctrl))
        {
            if(GetKeyDown(Input, KeyCode_C))
            {
                DebugLog("Copy\n");
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
            }
            else if(GetKeyDown(Input, KeyCode_Y))
            {
                DebugLog("Paste\n");

                SecondaryCursorPos = PrimaryCursorPos;
                SplitBufferSetCursor(SB, SecondaryCursorPos);
                Memcpy(Clipboard, SB.Start + SB.Middle, ClipboardSize);
                SB.Middle += ClipboardSize; // TODO: Handle buffer collisions
                SB.TextSize += ClipboardSize; // TODO: Handle buffer collisions
                
                Lines = CalculateLinesSB(Box, &Arena, SB);
                PrimaryCursorPos = SB.Middle;
            }
            else if(GetKeyDown(Input, KeyCode_W))
            {
                // Cutting is same as coping but you just delete after
                DebugLog("Cut\n");
                
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

                SplitBufferSetCursor(SB, PrimaryCursorPos);
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
                PrimaryCursorPos = SB.Middle;
            }
            else if(GetKeyDown(Input, KeyCode_Space))
            {
                SecondaryCursorPos = PrimaryCursorPos;
            }
        }

        if(GetKeyDown(Input, KeyCode_Home))
        {
            DebugLog("Home Key\n");
            PrimaryCursorPos = Lines.Lines[CursorGetCurrentLine(&Lines, PrimaryCursorPos)].StartIndex;
            IsCursorMoved = true;
        }
        if(GetKeyDown(Input, KeyCode_End))
        {
            DebugLog("End Key\n");
            PrimaryCursorPos = Lines.Lines[CursorGetCurrentLine(&Lines, PrimaryCursorPos)].EndIndex;
            IsCursorMoved = true;
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

        // Scroll Stuff
        u32 CursorCurrentLineIndex = CursorGetCurrentLine(&Lines, PrimaryCursorPos);                
        u32 LastLineIndexOnScreen = FirstLineIndexOnScreen + Lines.MaxLinesOnScreen - 1;
        bool Scroll = false;

        if(GetKey(Input, KeyCode_Alt) && GetKey(Input, KeyCode_Up))
        {
            FirstLineIndexOnScreen = Clamp(FirstLineIndexOnScreen - SCROLL_SIZE, 0, Lines.Count-1);
            LastLineIndexOnScreen = FirstLineIndexOnScreen + Lines.MaxLinesOnScreen - 1;
            if(CursorCurrentLineIndex > LastLineIndexOnScreen)
            {
                // TODO : Add CursorSetLine() function for this
                s32 CursorX = PrimaryCursorPos - Lines.Lines[CursorCurrentLineIndex].StartIndex;
                PrimaryCursorPos = Clamp((s32)Lines.Lines[LastLineIndexOnScreen].StartIndex + CursorX,
                                         (s32)Lines.Lines[LastLineIndexOnScreen].StartIndex,
                                         (s32)Lines.Lines[LastLineIndexOnScreen].EndIndex);
                CursorCurrentLineIndex = LastLineIndexOnScreen;
            }

            Scroll = true;
            DebugLog("Scroll up\n");
        }

        if(GetKey(Input, KeyCode_Alt) && GetKey(Input, KeyCode_Down))
        {
            FirstLineIndexOnScreen = Clamp(FirstLineIndexOnScreen + SCROLL_SIZE, 0, Lines.Count-1);
            LastLineIndexOnScreen = FirstLineIndexOnScreen + Lines.MaxLinesOnScreen - 1;
            if(CursorCurrentLineIndex < FirstLineIndexOnScreen)
            {
                // TODO : Add CursorSetLine() function for this
                s32 CursorX = PrimaryCursorPos - Lines.Lines[CursorCurrentLineIndex].StartIndex;
                PrimaryCursorPos = Clamp((s32)Lines.Lines[FirstLineIndexOnScreen].StartIndex + CursorX,
                                         (s32)Lines.Lines[FirstLineIndexOnScreen].StartIndex,
                                         (s32)Lines.Lines[FirstLineIndexOnScreen].EndIndex);
                CursorCurrentLineIndex = FirstLineIndexOnScreen;
            }
            
            Scroll = true;
            DebugLog("Scroll down\n");
        }
        
        if(CursorCurrentLineIndex < FirstLineIndexOnScreen) // Scroll up
        {
            u32 Diff = FirstLineIndexOnScreen - CursorCurrentLineIndex;
            if(Diff < 2) // Is not jump
            {
                FirstLineIndexOnScreen -= Lines.MaxLinesOnScreen / 2 + Lines.MaxLinesOnScreen%2; // center cursor position
            }
            else // Is jump
            {
                FirstLineIndexOnScreen = CursorCurrentLineIndex - (Lines.MaxLinesOnScreen / 2 + Lines.MaxLinesOnScreen%2);
            }
            
            FirstLineIndexOnScreen = Clamp(FirstLineIndexOnScreen, 0, Lines.Count-1);
            DebugLog("FirstLineIndexOnScreen : %i\n", FirstLineIndexOnScreen);
            Scroll = true;
        }
        else if(CursorCurrentLineIndex > LastLineIndexOnScreen) // Scroll Down
        {
            u32 Diff = CursorCurrentLineIndex - LastLineIndexOnScreen;
            if(Diff < 2) // Is not jump
            {
                FirstLineIndexOnScreen += Lines.MaxLinesOnScreen / 2 + Lines.MaxLinesOnScreen%2; // center cursor position
            }
            else // Is jump
            {
                FirstLineIndexOnScreen = CursorCurrentLineIndex - (Lines.MaxLinesOnScreen / 2 + Lines.MaxLinesOnScreen%2);
            }

            FirstLineIndexOnScreen = Clamp(FirstLineIndexOnScreen, 0, Lines.Count-1);
            DebugLog("FirstLineIndexOnScreen : %i\n", FirstLineIndexOnScreen);
            Scroll = true;
        }

        if(Scroll)
        {
            f32 AdvanceSize = FirstLineIndexOnScreen * Box.CharacterHeight;
            TargetTop = Top - AdvanceSize;
            TargetBottom = Bottom - AdvanceSize;
        }

        { // TODO : stop when interpolation is done
            f32 T = ScrollSpeeed * DeltaTime;
            CurrentTop = Lerp(CurrentTop, TargetTop, T);
            CurrentBottom = Lerp(CurrentBottom, TargetBottom, T);
            OrthoMatrix = MakeOrthoMatrix(Left, Right, CurrentTop, CurrentBottom,
                                          Near, Far);
            glUseProgram(TextShader);
            glUniformMatrix4fv(OrthoMatrixLocation, 1, GL_TRUE, (f32*)&OrthoMatrix);
            glUseProgram(CursorShader);
            glUniformMatrix4fv(OrthoMatrixLocationCursor, 1, GL_TRUE, (f32*)&OrthoMatrix);
        }
        // Scroll End
                
        v2 PrimaryCursorScreenPosition = CursorTextToScreen(&Lines, PrimaryCursorPos);
        v2 SecondaryCursorScreenPosition = CursorTextToScreen(&Lines, SecondaryCursorPos);
        CursorDraw(Box, &Arena, PrimaryCursorScreenPosition, 0.0f, CursorVAO, CursorVBO, CursorShader);
        CursorDraw(Box, &Arena, SecondaryCursorScreenPosition, 1.0f, CursorVAO, CursorVBO, CursorShader);

        TextBoxRenderState RenderState = TextBoxBeginDraw(Box, &Arena, &Lines, TextVAO, TextVBO, TextShader);
        TextBoxPushText(RenderState, SB.Start, SB.Middle, v3(1,0,1));
        TextBoxPushText(RenderState, SB.Start + SB.Second, SB.TextSize - SB.Middle, TextColor);
        TextBoxEndDraw(RenderState);

        { // Render debug text
            v2 CursorScreenPosition = CursorTextToScreen(&Lines, PrimaryCursorPos);
            char* DebugText = (char*)FrameArenaAllocateMemory(Arena, 1000);
            s32 WrittenChar = 0;
            WrittenChar += sprintf_s(DebugText, 1000, "DeltaTime %f\nTimePassed: %f\nElapsedTimeInMs : %f\nFPS : %f\nCursorX: %f\nCursorY: %f",
                                     DeltaTime, TimeSinceStart,
                                     ElapsedTimeInMs, 1000.0/ElapsedTimeInMs,
                                     CursorScreenPosition.x, CursorScreenPosition.y);
            CalculateLinesResult DebugLines = CalculateLines(DebugBox, &Arena, DebugText, WrittenChar);
            TextBoxFillColor(DebugBox, &Arena, CursorVAO, CursorVBO, ScreenSpaceBoxShader, v3(95.0f/255.0f, 110.0f/255.0f, 133.0f/255.0f));
            RenderState = TextBoxBeginDraw(DebugBox, &Arena, &DebugLines, TextVAO, TextVBO, ScreenSpaceTextShader);
            TextBoxPushText(RenderState, DebugText, WrittenChar, v3(0,1,0));
            TextBoxEndDraw(RenderState);
        }
        
        glFlush();
        SwapBuffers(Device_Context);
        FrameArenaReset(Arena);
        
        { // FPS lock
            TimeSinceStart += DeltaTime;
            s64 FrameEndCounter = GetPerformanceCounter();
            s64 ElapsedCounter = FrameEndCounter - FrameStartCounter;
            const s64 DesiredElapsedCounter = Frequency / TARGET_FPS;
            while(DesiredElapsedCounter > ElapsedCounter)
            {
                u32 SleepTime = (u32)((DesiredElapsedCounter - ElapsedCounter) * 1000 / Frequency);
                Sleep(SleepTime);
                ElapsedCounter = GetPerformanceCounter() - FrameStartCounter;
            }
            ElapsedTimeInMs = (f64)(ElapsedCounter * 1000)/(f64)Frequency;
            DeltaTime = (f32)(ElapsedCounter)/(f32)Frequency;
            DeltaTime = Clamp(DeltaTime, 0.0f, FLT_MAX);
            FrameStartCounter = GetPerformanceCounter();
        }
    }

    FreeMemory(SB.Start);
    FrameArenaDelete(Arena);
    return 0;
}
