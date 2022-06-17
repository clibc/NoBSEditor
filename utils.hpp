#pragma once

#define Assert(Expression)                      \
if(!(Expression)) { *(int*)0 = 0; }

#define Megabytes(x) (x*1024*1024)

static inline s32
TruncateF32ToS32(f32 Value) {
    return (s32)Value;
}

static inline s32
RoundF32ToS32(f32 Value) {
    return (s32)(Value + 0.5f);
}

struct ReadEntireFileResult {
    char* content;
    s32   size;
};

static ReadEntireFileResult
ReadEntireFile() {
    ReadEntireFileResult result;
    return result;
}

static void*
AllocateMemory(u64 Size) {
    return VirtualAlloc(NULL, Size,
                        MEM_COMMIT|MEM_RESERVE,
                        PAGE_READWRITE);    
}

static void
FreeMemory(void* Memory) {
    if(Memory) {
        s32 Result = VirtualFree(Memory, 0, MEM_RELEASE);
        if(!Result) {
            DebugLog("FreeMemory failed with error code: %i", GetLastError());
        }
        Assert(Result);
    }
}

struct FrameArena {
    void* BasePointer;
    u64 MaxSize;
    u64 PushOffset;
};

struct FrameArenaMemory {
    void* Memory;
    u64 Size;
};

static FrameArena
FrameArenaCreate(u64 Size) {
    FrameArena Arena;
    Arena.MaxSize = Size;
    Arena.PushOffset = 0;
    Arena.BasePointer = AllocateMemory(Arena.MaxSize);
    return Arena;
}

static FrameArenaMemory
FrameArenaAllocateMemory(FrameArena& Arena, u64 Size) {
    Assert(Arena.PushOffset < Arena.MaxSize);

    FrameArenaMemory Memory;
    Memory.Size = Size;
    Memory.Memory = (char*)Arena.BasePointer + Arena.PushOffset;
    Arena.PushOffset += Size;
    return Memory;
}

static void
FrameArenaReset(FrameArena& Arena) {
    Arena.PushOffset = 0;
}

static void
FrameArenaDelete(FrameArena& Arena) {
    FreeMemory(Arena.BasePointer);
    Arena.BasePointer = NULL;
    Arena.MaxSize = 0;
    Arena.PushOffset = 0;
}

struct TextBox {
    f32 Width;
    f32 Height;
    f32 CharacterWidth;
    f32 CharacterHeight;
};

#define CharTopLeft  v2(0,0)
#define CharTopRight v2(1,0)
#define CharBotLeft  v2(0,1)
#define CharBotRight v2(1,1)

struct Line {
    u32 StartIndex;
    u32 EndIndex;
};

struct CalculateLinesResult {
    Line* Lines;
    u32   LineCount;
};

// Calculates line start/end in a frame
static CalculateLinesResult
CalculateLines(TextBox Box, FrameArena* Arena, char* Text, u32 TextSize) {
    // TODO : This size should be calculated by counting how many there are in the text.
    // Now I calculate all the text so maybe I need to only consider text that is on the screen.
    Line* Lines = (Line*)FrameArenaAllocateMemory(*Arena, 1000 * sizeof(Line)).Memory;

    u32 LinesIndex = 0;
    u32 LineStart = 0;
    for(u32 i = 0; i < TextSize; ++i) {
        if(Text[i] == '\n') {
            Lines[LinesIndex].StartIndex = LineStart;
            Lines[LinesIndex].EndIndex   = i;
            LinesIndex += 1;
            LineStart = i + 1;
        }
        else if(i == TextSize - 1) {
            Lines[LinesIndex].StartIndex = LineStart;
            Lines[LinesIndex].EndIndex   = i + 1;
            LinesIndex += 1;
        }
    }

    CalculateLinesResult Result = {};
    Result.Lines = Lines;
    Result.LineCount = LinesIndex;

    return Result;
}

struct CreateFontTextureResult {
    u32 TextureID;
    u32 TextureWidth;
    u32 TextureHeight;
    u32 CharactersPerLine;
    u32 CharacterWidth;
    u32 CharacterHeight;
};

static CreateFontTextureResult
CreateFontTexture() {
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
    for(s32 i = 33; i < 127; ++i) {
        wchar_t Character = (wchar_t)i;
        TextOutW(DeviceContext, PitchX, PitchY, &Character, 1);

        if((i-32) % CharacterPerLine == 0) {
            PitchX = 0;
            PitchY += CharSize.cy;
        } else {
            PitchX += CharSize.cx;
        }
    }

    u8* Buffer = (u8*)AllocateMemory(sizeof(u8)*TextureWidth*TextureHeight);
    u8* PixelPtr = Buffer;
    
    for(u32 v = 0; v < TextureHeight; ++v) {
        for(u32 u = 0; u < TextureWidth; ++u) {
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
    
    CreateFontTextureResult Result;
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
CreateFontLookupTable(const CreateFontTextureResult& FontData) {
    v2* TextureCoords = (v2*)AllocateMemory(sizeof(v2) * 94 * 4);

    f32 CW = (f32)FontData.CharacterWidth / (f32)FontData.TextureWidth;
    f32 CH = (f32)FontData.CharacterHeight / (f32)FontData.TextureHeight;

    s32 ArrayIndex = 0;
    for(s32 i = 33; i < 127; ++i) {
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

struct Vertex {
    v3 Position;
    v3 Color;
    s32 CharacterIndex;
};

struct CursorVertex {
    v3 Position;
    v3 Color;
};

static inline v3
TextBoxVertexPosition(TextBox& Box, v2 CursorPosition, v2 Corner) {
    return v3(Box.CharacterWidth * CursorPosition.x + Box.CharacterWidth * Corner.x,
              Box.Height - (Box.CharacterHeight * CursorPosition.y + Box.CharacterHeight * Corner.y),
              0);
}

struct TextBoxRenderState {
    TextBox Box;
    FrameArenaMemory ArenaMemory;
    u32 BatchCount;
    u32 VAO;
    u32 VBO;
    u32 Shader;
    u32 CursorPosition;
    CalculateLinesResult* Lines;
};

static inline TextBoxRenderState
TextBoxBeginDraw(TextBox Box, FrameArena* Arena, CalculateLinesResult* Lines, u32 VAO, u32 VBO, u32 Shader) {
    Assert(Arena != NULL);
    Assert(VAO >= 0);
    Assert(VBO >= 0);
    Assert(Shader >= 0);

    TextBoxRenderState S;
    S.Box = Box;
    S.VAO = VAO;
    S.VBO = VBO;
    S.Shader = Shader;
    S.CursorPosition = 0;
    S.BatchCount = 0;
    S.ArenaMemory = FrameArenaAllocateMemory(*Arena, 1200 * sizeof(Vertex));
    S.Lines = Lines;
    
    return S;
}

static inline void
TextBoxPushText(TextBoxRenderState& State, char* Text, u32 TextSize, v3 TextColor = v3(1,1,1)) {
    Vertex* BatchMemory = (Vertex*)State.ArenaMemory.Memory;
    TextBox Box = State.Box;
    Line* Lines = State.Lines->Lines;

    for(u32 j = 0; j < TextSize; ++j) {
        u32 i;
        for(i = 0; i < State.Lines->LineCount; ++i) {
            if(j >= Lines[i].StartIndex && j < Lines[i].EndIndex)
            {
                break;
            }
        }

        v2 CursorPosition;
        CursorPosition.x = (f32)(j - Lines[i].StartIndex);
        CursorPosition.y = (f32)i;

        Vertex V;
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
TextBoxEndDraw(TextBoxRenderState& State) {
    glBindVertexArray(State.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, State.VBO);
    glUseProgram(State.Shader);
    glBufferData(GL_ARRAY_BUFFER, State.BatchCount * sizeof(Vertex), State.ArenaMemory.Memory, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, State.BatchCount);
}

static inline void
CursorDraw(TextBox Box, FrameArena* Arena, v2 CursorPosition, u32 VAO, u32 VBO, u32 Shader) {
    Assert(Arena != NULL && "TextBoxDraw Error : Arena is not assigned!");
    FrameArenaMemory ArenaMemory = FrameArenaAllocateMemory(*Arena, 6 * sizeof(CursorVertex));

    CursorVertex* BatchMemory = (CursorVertex*)ArenaMemory.Memory;
    u32 BatchCurrentIndex = 0;

    CursorVertex V;
    V.Color = v3(1, 1, 0);
        
    V.Position = TextBoxVertexPosition(Box, CursorPosition, CharTopRight);
    BatchMemory[BatchCurrentIndex++] = V;

    V.Position = TextBoxVertexPosition(Box, CursorPosition, CharBotLeft);
    BatchMemory[BatchCurrentIndex++] = V;

    V.Position = TextBoxVertexPosition(Box, CursorPosition, CharBotRight);
    BatchMemory[BatchCurrentIndex++] = V;

    V.Position = TextBoxVertexPosition(Box, CursorPosition, CharTopRight);
    BatchMemory[BatchCurrentIndex++] = V;

    V.Position = TextBoxVertexPosition(Box, CursorPosition, CharTopLeft);
    BatchMemory[BatchCurrentIndex++] = V;

    V.Position = TextBoxVertexPosition(Box, CursorPosition, CharBotLeft);
    BatchMemory[BatchCurrentIndex++] = V;

    glUseProgram(Shader);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, BatchCurrentIndex * sizeof(CursorVertex), BatchMemory, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, BatchCurrentIndex);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static HWND
CreateOpenGLWindow(HINSTANCE hInstance, int nCmdShow, s32 width, s32 height) {
    WNDCLASS wc = {};
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "nobsed_window_class";
    assert(RegisterClass(&wc));
    HWND window_handle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
                                        wc.lpszClassName,
                                        "NoBSEditor",
                                        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                        CW_USEDEFAULT,CW_USEDEFAULT,
                                        width, height,
                                        NULL,NULL,hInstance,NULL);
    assert(window_handle);
    return window_handle;
}

static GLuint
LoadShaderFromFiles(const char * vertex_file_path,const char * fragment_file_path){
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}else {
		DebugLog("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		exit(-1);
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	DebugLog("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		DebugLog("%s\n", &VertexShaderErrorMessage[0]);
        exit(-1);
	}

	// Compile Fragment Shader
	DebugLog("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		DebugLog("%s\n", &FragmentShaderErrorMessage[0]);
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
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		DebugLog("%s\n", &ProgramErrorMessage[0]);
        exit(-1);
	}
	
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
