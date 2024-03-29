#pragma once

#define Assert(Expression)                      \
if(!(Expression)) { *(int*)0 = 0; }

#define Megabytes(x) (x*1024*1024)

static void*
AllocateMemory(u64 Size)
{
    return VirtualAlloc(NULL, Size,
                        MEM_COMMIT|MEM_RESERVE,
                        PAGE_READWRITE);    
}

static void
Memcpy(void* Source, void* Dest, u32 Size)
{
    for(u32 i = 0; i < Size; ++i)
    {
        ((char*)Dest)[i] = ((char*)Source)[i];
    }
}

static void
FreeMemory(void* Memory)
{
    if(Memory)
    {
        s32 Result = VirtualFree(Memory, 0, MEM_RELEASE);
        if(!Result)
        {
            DebugLog("FreeMemory failed with error code: %i", GetLastError());
        }
        Assert(Result);
    }
}

struct read_entire_file_result
{
    char* Content;
    s64   Size;
};

static bool
ReadEntireFile(read_entire_file_result* Result, const char* FilePath)
{
    HANDLE FileHandle = CreateFileA(FilePath,
                                    GENERIC_READ,
                                    0, // Maybe not?
                                    NULL,
                                    OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL);
    bool IsRead = false;    
    
    if(FileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD HighFileSize;
        DWORD FileSize = GetFileSize(FileHandle, &HighFileSize);
        char* FileBuffer = (char*)AllocateMemory(FileSize);

        IsRead = ReadFile(FileHandle, 
                               FileBuffer,
                               FileSize,
                               NULL,
                               NULL);
        if(IsRead)
        {
            Result->Content = FileBuffer;
            Result->Size = FileSize;
        }
        else
        {
            FreeMemory(FileBuffer);
        }
    }
    
    CloseHandle(FileHandle);
    
    return IsRead;
}

struct frame_arena
{
    void* BasePointer;
    u64 MaxSize;
    u64 PushOffset;
};

static frame_arena
FrameArenaCreate(u64 Size)
{
    frame_arena Arena;
    Arena.MaxSize = Size;
    Arena.PushOffset = 0;
    Arena.BasePointer = AllocateMemory(Arena.MaxSize);
    return Arena;
}

static void*
FrameArenaAllocateMemory(frame_arena& Arena, u64 Size)
{
    Assert(Arena.PushOffset < Arena.MaxSize);

    void* Memory = (char*)Arena.BasePointer + Arena.PushOffset;
    Arena.PushOffset += Size;
    return Memory;
}

static void
FrameArenaReset(frame_arena& Arena)
{
    Arena.PushOffset = 0;
}

static void
FrameArenaDelete(frame_arena& Arena)
{
    FreeMemory(Arena.BasePointer);
    Arena.BasePointer = NULL;
    Arena.MaxSize = 0;
    Arena.PushOffset = 0;
}

struct text_box
{
    f32 Width;
    f32 Height;
    f32 CharacterWidth;
    f32 CharacterHeight;

    v3 Position;
};

#define CharTopLeft  v2(0,0)
#define CharTopRight v2(1,0)
#define CharBotLeft  v2(0,1)
#define CharBotRight v2(1,1)

struct line
{
    u32 StartIndex;
    u32 EndIndex;
};

struct calculate_lines_result
{
    line* Lines;
    u32   Count;
    u32 MaxLinesOnScreen;
};

// Calculates line start/end in a frame
static calculate_lines_result
CalculateLines(text_box Box, frame_arena* Arena, char* Text, u32 TextSize)
{
    // TODO : This size should be calculated by counting how many there are in the text.
    // Now I calculate all the text so maybe I need to only consider text that is on the screen.
    line* Lines = (line*)FrameArenaAllocateMemory(*Arena, 1000 * sizeof(line));

    u32 LinesIndex = 0;
    u32 LineStart = 0;
    for(u32 i = 0; i < TextSize; ++i)
    {
        if(Text[i] == '\n')
        {
            Lines[LinesIndex].StartIndex = LineStart;
            Lines[LinesIndex].EndIndex   = i;
            LinesIndex += 1;
            LineStart = i + 1;
        }
        if(i == TextSize - 1)
        {
            Lines[LinesIndex].StartIndex = LineStart;
            Lines[LinesIndex].EndIndex   = i + 1;
            LinesIndex += 1;
        }
    }

    calculate_lines_result Result = {};
    Result.Lines = Lines;
    Result.Count = LinesIndex;
    // TODO: Box.Height is not always WINDOW_HEIGHT!!
    Result.MaxLinesOnScreen = TruncateF32ToS32(Box.Height / Box.CharacterHeight);

    return Result;
}

struct create_font_texture_result
{
    u32 TextureID;
    u32 TextureWidth;
    u32 TextureHeight;
    u32 CharactersPerLine;
    u32 CharacterWidth;
    u32 CharacterHeight;
};

static create_font_texture_result
CreateFontTexture()
{
    HDC DeviceContext = CreateCompatibleDC(GetDC(0));
    Assert(DeviceContext);

    AddFontResourceA("C:/Windows/Fonts/LiberationMono-Regular.ttf");
    HFONT Font = CreateFontA(60, 0, 0, 0,
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
    SetBkColor(DeviceContext, RGB(255,0,0));
    SetTextColor(DeviceContext, RGB(255,255,255));
    SetBkMode(DeviceContext, TRANSPARENT);

    wchar_t TempCharacter = (wchar_t)'A';
    SIZE CharSize;
    GetTextExtentPoint32A(DeviceContext, (LPCSTR)&TempCharacter, 1, &CharSize);
    u32 CharacterPerLine = 20;
    u32 TextureWidth  = CharSize.cx * 20;
    u32 TextureHeight = CharSize.cy * 5;
    
    BITMAPINFO Info;
    Info.bmiHeader.biSize = sizeof(Info.bmiHeader);
    Info.bmiHeader.biWidth = TextureWidth;
    Info.bmiHeader.biHeight = TextureHeight;
    Info.bmiHeader.biPlanes = 1;
    Info.bmiHeader.biBitCount = 32;
    Info.bmiHeader.biCompression = BI_RGB;
    Info.bmiHeader.biSizeImage = 0;
    Info.bmiHeader.biXPelsPerMeter = 0;
    Info.bmiHeader.biYPelsPerMeter = 0;
    Info.bmiHeader.biClrUsed = 0;
    Info.bmiHeader.biClrImportant = 0;
    
    u32* BitmapPixels;
    HBITMAP Bitmap = CreateDIBSection(DeviceContext, &Info, DIB_RGB_COLORS, (void**)&BitmapPixels, NULL, NULL);
    Assert(Bitmap);

    SelectObject(DeviceContext, Bitmap);

    u32 PitchX = 0;
    u32 PitchY = 0;
    for(s32 i = 33; i < 127; ++i)
    {
        wchar_t Character = (wchar_t)i;
        TextOutW(DeviceContext, PitchX, PitchY, &Character, 1);

        if((i-32) % CharacterPerLine == 0)
        {
            PitchX = 0;
            PitchY += CharSize.cy;
        }
        else
        {
            PitchX += CharSize.cx;
        }
    }

    u8* Buffer = (u8*)AllocateMemory(sizeof(u8)*TextureWidth*TextureHeight);
    u8* PixelPtr = Buffer;
    
    for(u32 v = 0; v < TextureHeight; ++v)
    {
        for(u32 u = 0; u < TextureWidth; ++u)
        {
            u32 Color = *BitmapPixels++;
            *PixelPtr = (u8)((u32*)&Color)[0];
            ++PixelPtr;
        }
    }

    GLuint Texture;
    glGenTextures(1, &Texture);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, TextureWidth, TextureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, Buffer);
    
    create_font_texture_result Result;
    Result.TextureID = Texture;
    Result.TextureWidth = TextureWidth;
    Result.TextureHeight = TextureHeight;
    Result.CharactersPerLine = CharacterPerLine;
    Result.CharacterWidth  = CharSize.cx;
    Result.CharacterHeight = CharSize.cy;

    FreeMemory(Buffer);
    DeleteObject(Bitmap);
    DeleteObject(Font);
    return Result;
}

static v2*
CreateFontLookupTable(const create_font_texture_result& FontData)
{
    v2* TextureCoords = (v2*)AllocateMemory(sizeof(v2) * 94 * 4);

    f32 CW = (f32)FontData.CharacterWidth / (f32)FontData.TextureWidth;
    f32 CH = (f32)FontData.CharacterHeight / (f32)FontData.TextureHeight;

    s32 ArrayIndex = 0;
    for(s32 i = 33; i < 127; ++i)
    {
        s32 Index = i-33;
        f32 IndexX = (f32)(Index % FontData.CharactersPerLine);
        f32 IndexY = (f32)(Index / FontData.CharactersPerLine);

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

struct vertex
{
    v3 Position;
    v3 Color;
    s32 CharacterIndex;
};

struct cursor_vertex
{
    v3 Position;
    v4 Color; // W component is cursor ID (primary, secondary)
    v2 UV;
};

static inline v3
TextBoxVertexPosition(text_box& Box, v2 CursorPosition, v2 Corner)
{
    v3 Result = v3(Box.CharacterWidth * CursorPosition.x + Box.CharacterWidth * Corner.x,
                   (f32)WINDOW_HEIGHT - (Box.CharacterHeight * CursorPosition.y + Box.CharacterHeight * Corner.y),
                   0);

    return Result + Box.Position;
}

struct text_box_render_state
{
    void* ArenaMemory;
    text_box Box;
    u32 BatchCount;
    u32 VAO;
    u32 VBO;
    u32 Shader;
    u32 CursorPosition;
    calculate_lines_result* Lines;
};

static inline text_box_render_state
TextBoxBeginDraw(text_box Box, frame_arena* Arena, calculate_lines_result* Lines, u32 VAO, u32 VBO, u32 Shader)
{
    Assert(Arena != NULL);
    Assert(VAO >= 0);
    Assert(VBO >= 0);
    Assert(Shader >= 0);

    text_box_render_state S;
    S.Box = Box;
    S.VAO = VAO;
    S.VBO = VBO;
    S.Shader = Shader;
    S.CursorPosition = 0;
    S.BatchCount = 0;
    S.ArenaMemory = FrameArenaAllocateMemory(*Arena, 1200 * sizeof(vertex));
    S.Lines = Lines;

    return S;
}

static inline v2
CursorTextToScreen(const calculate_lines_result* Lines,
                   u32 CursorPosition)
{
    v2 OutPosition = v2(0,0);
    for(u32 i = 0; i < Lines->Count; ++i)
    {
        if(CursorPosition >= Lines->Lines[i].StartIndex && CursorPosition <= Lines->Lines[i].EndIndex)
        {
            OutPosition.x = f32(CursorPosition - Lines->Lines[i].StartIndex);
            OutPosition.y = f32(i);
        }
    }
    return OutPosition;
}

static inline u32
CursorGetCurrentLine(const calculate_lines_result* Lines,
                   u32 CursorPosition)
{
    return (u32)CursorTextToScreen(Lines, CursorPosition).y;
}

static inline u32
CursorSetLine(u32 CursorCurrentPosition, u32 CursorCurrentLineIndex, u32 LineIndex, const calculate_lines_result& Lines)
{
    s32 CursorX = CursorCurrentPosition - Lines.Lines[CursorCurrentLineIndex].StartIndex;
    return Clamp((s32)Lines.Lines[LineIndex].StartIndex + CursorX,
                             (s32)Lines.Lines[LineIndex].StartIndex,
                             (s32)Lines.Lines[LineIndex].EndIndex);

}

static inline void
TextBoxPushText(text_box_render_state& State, char* Text, u32 TextSize, v3 TextColor = v3(1,1,1))
{
    if(TextSize <= 0) return;
    
    vertex* BatchMemory = (vertex*)State.ArenaMemory;
    text_box Box = State.Box;

    s32 StartPosition = State.CursorPosition;
    
    for(u32 j = 0; j < TextSize; ++j)
    {
        if(Text[j] == '\n' || Text[j] == ' ' || Text[j] == '\r')
        {
            State.CursorPosition++;
            continue;
        }

        v2 CursorPosition = CursorTextToScreen(State.Lines, j + StartPosition);
        
        vertex V;
        V.Color = TextColor;
        
        V.Position = TextBoxVertexPosition(Box, CursorPosition, CharTopRight);
        V.CharacterIndex = (Text[j] - 33) * 4 + 1;
        BatchMemory[State.BatchCount++] = V;

        V.Position = TextBoxVertexPosition(Box, CursorPosition, CharBotLeft);
        V.CharacterIndex = (Text[j] - 33) * 4 + 2;
        BatchMemory[State.BatchCount++] = V;

        V.Position = TextBoxVertexPosition(Box, CursorPosition, CharBotRight);
        V.CharacterIndex = (Text[j] - 33) * 4 + 3;
        BatchMemory[State.BatchCount++] = V;

        V.Position = TextBoxVertexPosition(Box, CursorPosition, CharTopRight);
        V.CharacterIndex = (Text[j] - 33) * 4 + 1;
        BatchMemory[State.BatchCount++] = V;

        V.Position = TextBoxVertexPosition(Box, CursorPosition, CharTopLeft);
        V.CharacterIndex = (Text[j] - 33) * 4 + 0;
        BatchMemory[State.BatchCount++] = V;

        V.Position = TextBoxVertexPosition(Box, CursorPosition, CharBotLeft);
        V.CharacterIndex = (Text[j] - 33) * 4 + 2;
        BatchMemory[State.BatchCount++] = V;

        State.CursorPosition++;
    }
}

static inline void
TextBoxEndDraw(text_box_render_state& State)
{
    glBindVertexArray(State.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, State.VBO);
    glUseProgram(State.Shader);
    glBufferData(GL_ARRAY_BUFFER, State.BatchCount * sizeof(vertex), State.ArenaMemory, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, State.BatchCount);
}

static inline void
CursorDraw(text_box Box, frame_arena* Arena, v2 CursorPosition, float CursorID, u32 VAO, u32 VBO, u32 Shader)
{
    Assert(Arena != NULL && "TextBoxDraw Error : Arena is not assigned!");

    cursor_vertex* BatchMemory = (cursor_vertex*)FrameArenaAllocateMemory(*Arena, 6 * sizeof(cursor_vertex));
    u32 BatchCurrentIndex = 0;

    cursor_vertex V;
    V.Color = v4(1, 1, 0, CursorID);

    V.Position = TextBoxVertexPosition(Box, CursorPosition, CharTopRight);
    V.UV = CharTopRight;
    BatchMemory[BatchCurrentIndex++] = V;

    V.Position = TextBoxVertexPosition(Box, CursorPosition, CharBotLeft);
    V.UV = CharBotLeft;
    BatchMemory[BatchCurrentIndex++] = V;

    V.Position = TextBoxVertexPosition(Box, CursorPosition, CharBotRight);
    V.UV = CharBotRight;
    BatchMemory[BatchCurrentIndex++] = V;

    V.Position = TextBoxVertexPosition(Box, CursorPosition, CharTopRight);
    V.UV = CharTopRight;
    BatchMemory[BatchCurrentIndex++] = V;

    V.Position = TextBoxVertexPosition(Box, CursorPosition, CharTopLeft);
    V.UV = CharTopLeft;
    BatchMemory[BatchCurrentIndex++] = V;

    V.Position = TextBoxVertexPosition(Box, CursorPosition, CharBotLeft);
    V.UV = CharBotLeft;
    BatchMemory[BatchCurrentIndex++] = V;

    glUseProgram(Shader);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, BatchCurrentIndex * sizeof(cursor_vertex), BatchMemory, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, BatchCurrentIndex);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static HWND
CreateOpenGLWindow(HINSTANCE hInstance, s32 width, s32 height)
{
    WNDCLASS wc = {};
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "nobsed_window_class";
    Assert(RegisterClass(&wc));
    HWND window_handle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
                                        wc.lpszClassName,
                                        "NoBSEditor",
                                        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                        CW_USEDEFAULT,CW_USEDEFAULT,
                                        width, height,
                                        NULL,NULL,hInstance,NULL);
    Assert(window_handle);
    return window_handle;
}

static GLuint
LoadShaderFromFiles(const char* vertex_file_path, const char* fragment_file_path)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
    read_entire_file_result VertexShaderFile;
    if(!ReadEntireFile(&VertexShaderFile, vertex_file_path))
    {
		DebugLog("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		exit(-1);
    }

    read_entire_file_result FragmentShaderFile;
    if(!ReadEntireFile(&FragmentShaderFile, fragment_file_path))
    {
		DebugLog("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", fragment_file_path);
		getchar();
		exit(-1);
    }

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	DebugLog("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderFile.Content;
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
    {
        char* VertexShaderErrorMessage = (char*)AllocateMemory(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		DebugLog("%s\n", &VertexShaderErrorMessage[0]);
        FreeMemory(VertexShaderErrorMessage);
        exit(-1);
	}

	// Compile Fragment Shader
	DebugLog("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderFile.Content;
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
    {
        char* FragmentShaderErrorMessage = (char*)AllocateMemory(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		DebugLog("%s\n", &FragmentShaderErrorMessage[0]);
        FreeMemory(FragmentShaderErrorMessage);
        exit(-1);
	}

	// Link the program
	DebugLog("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
    {
	    char* ProgramErrorMessage = (char*)AllocateMemory(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		DebugLog("%s\n", &ProgramErrorMessage[0]);
        FreeMemory(ProgramErrorMessage);
        exit(-1);
	}
	
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

    FreeMemory(VertexShaderFile.Content);
    FreeMemory(FragmentShaderFile.Content);
	return ProgramID;
}

struct split_buffer
{
    char* Start;
    u32 Middle; // First End
    u32 Second;
    u32 Size;
    u32 TextSize;
};

static inline split_buffer
SplitBufferCreate(u32 Size, char* Text, u32 TextSize)
{
    split_buffer SB;
    SB.Size = Size;
    SB.TextSize = TextSize;
    SB.Start = (char*)AllocateMemory(Size);
    SB.Middle = 0;
    SB.Second = Size/3 * 2;

    Memcpy(Text, SB.Start + SB.Second, TextSize);
    return SB;
}

static inline void
SplitBufferSetCursor(split_buffer& SB, u32 CursorPosition)
{
    // @TODO: Need to manage gap size based on how much of the buffer is filled
    // 20 is danger zone for now
    Assert(SB.Second - SB.Middle > 20);
        
    if(CursorPosition < SB.Middle)
    {  // Cursor moved backwards 
        u32 Diff = SB.Middle - CursorPosition;
        SB.Middle -= Diff;
        SB.Second -= Diff;
        Memcpy(SB.Start + SB.Middle, SB.Start + SB.Second, Diff);
    }
    else
    { // Cursor moved forwards
        u32 Diff = CursorPosition - SB.Middle;
        Memcpy(SB.Start + SB.Second, SB.Start + SB.Middle, Diff);
        SB.Middle += Diff;
        SB.Second += Diff;
    }
}

static inline void
SplitBufferAddChar(split_buffer& SB, char Character)
{
    Assert(SB.Second - SB.Middle > 20);
    SB.Start[SB.Middle] = Character;
    SB.Middle += 1;
    SB.TextSize += 1;
}

static inline void
SplitBufferRemoveCharDeleteKey(split_buffer& SB)
{
    if(SB.Second < SB.Size && SB.Middle < SB.TextSize) {
        SB.Second += 1;
        SB.TextSize -= 1;
    }
}

static inline void
SplitBufferRemoveCharBackKey(split_buffer& SB)
{
    if(SB.Middle > 0) {
        SB.Middle -= 1;
        SB.TextSize -= 1;
    }
}

static calculate_lines_result
CalculateLinesSB(const text_box& Box, frame_arena* Arena, split_buffer& SB)
{
    // TODO : This size should be calculated by counting how many there are in the text.
    // Now I calculate all the text so maybe I need to only consider text that is on the screen.
    line* Lines = (line*)FrameArenaAllocateMemory(*Arena, 100000 * sizeof(line));

    u32 LinesIndex = 0;
    u32 LineStart = 0;

    if(SB.TextSize == 0)
    {
        Lines[LinesIndex].StartIndex = 0;
        Lines[LinesIndex].EndIndex   = 0;
        goto ReturnResult;
    }
    
    char* Text = SB.Start;

    u32 Index = 0;
    for(u32 I = 0; I < SB.Second + (SB.TextSize - SB.Middle); ++I)
    {
        // @TODO : Sometimes this loop never ends! Because SB.Second == SB.Middle
        if(I == SB.Middle)
        {
            I = SB.Second - 1;
            continue;
        }
        
        if(Text[I] == '\n')
        {
            Lines[LinesIndex].StartIndex = LineStart;
            Lines[LinesIndex].EndIndex   = Index;
            LinesIndex += 1;
            LineStart = Index + 1;
        }
        if(Index == SB.TextSize - 1)
        {
            Lines[LinesIndex].StartIndex = LineStart;
            Lines[LinesIndex].EndIndex   = Index + 1;
            LinesIndex += 1;
        }
        
        Index += 1;
    }

ReturnResult:
    calculate_lines_result Result = {};
    Result.Lines = Lines;
    Result.Count = LinesIndex;
    Result.MaxLinesOnScreen = TruncateF32ToS32(Box.Height / Box.CharacterHeight);
    
    return Result;
}

static void
TextBoxFillColor(text_box& Box, frame_arena* Arena, u32 VAO, u32 VBO, u32 Shader, v3 Color)
{
    // NOTE: This is just like drawing cursor, but with different vertex positions
    // NOTE: This shader should be not effected by scrolling, so it's projection matrix stay unchanged
    glUseProgram(Shader);
    cursor_vertex* BatchMemory = (cursor_vertex*)FrameArenaAllocateMemory(*Arena, 6 * sizeof(cursor_vertex));
    u32 BatchCurrentIndex = 0;

    cursor_vertex V;
    f32 CursorID = 0.0f; // solid
    V.Color = v4(Color.r, Color.g, Color.b, CursorID);

    v3 BoxPosition = v3(Box.Position.x, WINDOW_HEIGHT - Box.Position.y, Box.Position.z);

    v3 VTopLeft  = BoxPosition;
    v3 VBotLeft  = v3(BoxPosition.x, BoxPosition.y - Box.Height, 0);
    v3 VTopRight = v3(BoxPosition.x + Box.Width, BoxPosition.y, 0);
    v3 VBotRight = v3(BoxPosition.x + Box.Width, BoxPosition.y - Box.Height, 0);
    
    V.Position = VTopRight;
    V.UV = CharTopRight;
    BatchMemory[BatchCurrentIndex++] = V;

    V.Position = VBotLeft;
    V.UV = CharBotLeft;
    BatchMemory[BatchCurrentIndex++] = V;

    V.Position = VBotRight;
    V.UV = CharBotRight;
    BatchMemory[BatchCurrentIndex++] = V;

    V.Position = VTopRight;
    V.UV = CharTopRight;
    BatchMemory[BatchCurrentIndex++] = V;

    V.Position = VTopLeft;
    V.UV = CharTopLeft;
    BatchMemory[BatchCurrentIndex++] = V;

    V.Position = VBotLeft;
    V.UV = CharBotLeft;
    BatchMemory[BatchCurrentIndex++] = V;

    glUseProgram(Shader);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, BatchCurrentIndex * sizeof(cursor_vertex), BatchMemory, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, BatchCurrentIndex);
}

static inline s64
GetPerformanceCounter()
{
    LARGE_INTEGER Ticks;
    Assert(QueryPerformanceCounter(&Ticks))
    return Ticks.QuadPart;
}
