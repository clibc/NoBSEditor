#pragma once

#define Assert(x) assert(x)
#define Megabytes(x) (x*1024*1024)

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
    s32 Result = VirtualFree(Memory, 0, MEM_RELEASE);
    if(!Result) {
        DebugLog("FreeMemory failed with error code: %i", GetLastError());
    }
    assert(Result);
}

struct FrameArena {
    void* BasePointer;
    u64 PushOffset;
    u64 MaxSize;
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
    Arena.PushOffset += Size;

    FrameArenaMemory Memory;
    Memory.Size = Size;
    Memory.Memory = (char*)Arena.BasePointer + Arena.PushOffset;
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

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static HWND
CreateOpenGLWindow(HINSTANCE hInstance, int nCmdShow, s32 width, s32 height) {
    WNDCLASS wc = {};
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "fasted_window_class";
    assert(RegisterClass(&wc));
    HWND window_handle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
                                        wc.lpszClassName,
                                        "fasted",
                                        WS_OVERLAPPEDWINDOW,
                                        CW_USEDEFAULT,CW_USEDEFAULT,
                                        width, height,
                                        NULL,NULL,hInstance,NULL);
    assert(window_handle);
    ShowWindow(window_handle, nCmdShow);
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

struct CreateFontTextureResult {
    u32 TextureID;
    u32 TextureWidth;
    u32 TextureHeight;
    u32 CharacterPerLine;
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
    Result.CharacterPerLine = CharacterPerLine;
    Result.CharacterWidth  = CharSize.cx;
    Result.CharacterHeight = CharSize.cy;

    FreeMemory(Buffer);
    DeleteObject(Bitmap);
    DeleteObject(Font);
    return Result;
}

struct Vertex {
    v3 Position;
    v3 Color;
    s32 CharacterIndex;
};

struct TextBox {
    f32 BoxWidth;
    f32 BoxHeight;
    f32 CharacterWidth;
    f32 CharacterHeight;
    u32 VBO;
    FrameArena* Arena;
};

static void
TextBoxDraw(const TextBox& Box, char* Text, u32 TextSize) {
    Assert(Box.Arena != NULL && "TextBoxDraw Error : Arena is not assigned!");
    
    FrameArenaMemory ArenaMemory = FrameArenaAllocateMemory(*Box.Arena, TextSize * sizeof(Vertex));

    Vertex* BatchMemory = (Vertex*)ArenaMemory.Memory;
    u32 BatchCurrentIndex = 0;

    u32 CharactersPerLine = (u32)(Box.BoxWidth / Box.CharacterWidth);
    for(u32 i = 0; i < TextSize; ++i) {
        if(Text[i] == 0) break;

        // Space
        if(Text[i] == 32) continue;

        f32 CharWidth  = Box.CharacterWidth;
        f32 CharHeight = Box.CharacterHeight;
            
        v3 TopLeft  = v3((i%CharactersPerLine) * CharWidth, Box.BoxHeight - CharHeight * (i/CharactersPerLine), 0);
        v3 TopRight = v3((i%CharactersPerLine) * CharWidth + CharWidth, Box.BoxHeight - CharHeight * (i/CharactersPerLine), 0);
        v3 BotLeft  = v3((i%CharactersPerLine) * CharWidth, Box.BoxHeight - CharHeight - CharHeight * (i/CharactersPerLine), 0);
        v3 BotRight = v3((i%CharactersPerLine) * CharWidth + CharWidth, Box.BoxHeight - CharHeight - CharHeight * (i/CharactersPerLine), 0);

        Vertex V;
        V.Color = v3(1, 0, 0);
        
        V.Position = TopRight;
        V.CharacterIndex = (Text[i] - 33) * 4 + 1;
        BatchMemory[BatchCurrentIndex++] = V;

        V.Position = BotLeft;
        V.CharacterIndex = (Text[i] - 33) * 4 + 2;
        BatchMemory[BatchCurrentIndex++] = V;

        V.Position = BotRight;
        V.CharacterIndex = (Text[i] - 33) * 4 + 3;
        BatchMemory[BatchCurrentIndex++] = V;

        V.Position = TopRight;
        V.CharacterIndex = (Text[i] - 33) * 4 + 1;
        BatchMemory[BatchCurrentIndex++] = V;

        V.Position = TopLeft;
        V.CharacterIndex = (Text[i] - 33) * 4 + 0;
        BatchMemory[BatchCurrentIndex++] = V;

        V.Position = BotLeft;
        V.CharacterIndex = (Text[i] - 33) * 4 + 2;
        BatchMemory[BatchCurrentIndex++] = V;
    }

    glBindBuffer(GL_ARRAY_BUFFER, Box.VBO);
    glBufferData(GL_ARRAY_BUFFER, BatchCurrentIndex * sizeof(Vertex), BatchMemory, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, BatchCurrentIndex);
}
