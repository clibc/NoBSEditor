#pragma once
#include <GL/gl.h>
#include <assert.h>

#define ENTRY __stdcall

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB  0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_DRAW_TO_WINDOW_ARB    0x2001
#define WGL_SUPPORT_OPENGL_ARB    0x2010
#define WGL_DOUBLE_BUFFER_ARB     0x2011
#define WGL_PIXEL_TYPE_ARB        0x2013
#define WGL_TYPE_RGBA_ARB         0x202B
#define WGL_ACCELERATION_ARB      0x2003
#define WGL_COLOR_BITS_ARB        0x2014
#define WGL_ALPHA_BITS_ARB        0x201B
#define WGL_DEPTH_BITS_ARB        0x2022
#define WGL_STENCIL_BITS_ARB      0x2023
#define WGL_SAMPLE_BUFFERS_ARB    0x2041
#define WGL_SAMPLES_ARB           0x2042
#define WGL_FULL_ACCELERATION_ARB 0x2027

#define GL_ARRAY_BUFFER    0x8892
#define GL_STATIC_DRAW     0x88E4
#define GL_VERTEX_SHADER   0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_COMPILE_STATUS  0x8B81
#define GL_INFO_LOG_LENGTH 0x8B84
#define GL_LINK_STATUS     0x8B82
#define GL_LINES           0x0001
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_CLAMP_TO_EDGE   0x812F

typedef ptrdiff_t GLsizeiptr;
typedef char GLchar;

typedef void (ENTRY *TglGenBuffers)(GLsizei,GLuint*);
typedef void (ENTRY *TglBufferData)(GLenum, GLsizeiptr, const GLvoid*, GLenum);
typedef void (ENTRY *TglBindBuffer)(GLenum, GLuint);
typedef void (ENTRY *TglBufferData)(GLenum, GLsizeiptr, const GLvoid*, GLenum);
typedef void (ENTRY *TglShaderSource)(GLuint, GLsizei, const GLchar**, const GLint*);
typedef void (ENTRY *TglCompileShader)(GLuint);
typedef void (ENTRY *TglGetShaderiv)(GLuint, GLenum, GLint*);
typedef void (ENTRY *TglGetShaderInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*);
typedef void (ENTRY *TglAttachShader)(GLuint, GLuint);
typedef void (ENTRY *TglGetProgramiv)(GLuint, GLenum, GLint*);
typedef void (ENTRY *TglDetachShader)(GLuint, GLuint);
typedef void (ENTRY *TglDeleteShader)(GLuint);
typedef void (ENTRY *TglLinkProgram)(GLuint);
typedef void (ENTRY *TglGetProgramInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*);
typedef GLuint (ENTRY *TglCreateShader)(GLenum);
typedef GLuint (ENTRY *TglCreateProgram)(void);
typedef void (ENTRY *TglVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*);
typedef void (ENTRY *TglVertexAttribIPointer)(GLuint, GLint, GLenum, GLsizei, const GLvoid*);
typedef void (ENTRY *TglEnableVertexAttribArray)(GLuint);
typedef void (ENTRY *TglUseProgram)(GLuint);
typedef void (ENTRY *TglGenVertexArrays)(GLsizei n,GLuint *arrays);
typedef void (ENTRY *TglBindVertexArray)(GLuint);
typedef HGLRC (ENTRY *TwglCreateContextAttribsARB)(HDC, HGLRC, const int*);
typedef void (ENTRY *TglUniformMatrix4fv)(GLint, GLsizei, GLboolean, const GLfloat*);
typedef GLint (ENTRY *TglGetAttribLocation)(GLuint, const GLchar*);
typedef GLint (ENTRY *TglGetUniformLocation)(GLuint, const GLchar*);
typedef void  (ENTRY *TglUniform3fv)(GLuint, GLsizei, const GLfloat*);
typedef void  (ENTRY *TglUniform2fv)(GLuint, GLsizei, const GLfloat*);
typedef void  (ENTRY *TglUniform1i)(GLuint, GLint);
typedef bool  (ENTRY *TwglChoosePixelFormatARB)(HDC, const int*, const FLOAT *, UINT, int*, UINT*);

TglGenBuffers glGenBuffers;
TglBufferData glBufferData;
TglBindBuffer glBindBuffer;
TglShaderSource glShaderSource;
TglCompileShader glCompileShader;
TglGetShaderiv glGetShaderiv;
TglGetShaderInfoLog glGetShaderInfoLog;
TglAttachShader glAttachShader;
TglGetProgramiv glGetProgramiv;
TglDetachShader glDetachShader;
TglDeleteShader glDeleteShader;
TglLinkProgram glLinkProgram;
TglGetProgramInfoLog glGetProgramInfoLog;
TglCreateShader glCreateShader;
TglCreateProgram glCreateProgram;
TglVertexAttribPointer glVertexAttribPointer;
TglVertexAttribIPointer glVertexAttribIPointer;
TglEnableVertexAttribArray glEnableVertexAttribArray;
TglUseProgram glUseProgram;
TglGenVertexArrays glGenVertexArrays;
TglBindVertexArray glBindVertexArray;
TwglCreateContextAttribsARB wglCreateContextAttribsARB;
TglUniformMatrix4fv glUniformMatrix4fv;
TglGetAttribLocation glGetAttribLocation;
TglGetUniformLocation glGetUniformLocation;
TglUniform3fv glUniform3fv;
TglUniform2fv glUniform2fv;
TglUniform1i glUniform1i;
TwglChoosePixelFormatARB wglChoosePixelFormatARB;

static void*
GetFuncAddress(const char *name)
{
    void *p = (void *)wglGetProcAddress(name);
    if(p == 0 ||
       (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
       (p == (void*)-1) )
    {
        HMODULE module = LoadLibraryA("opengl32.dll");
        p = (void*)GetProcAddress(module, name);
    }
    return p;
}

static void
LoadOpenGLFuncs()
{
    glGenBuffers = (TglGenBuffers)GetFuncAddress("glGenBuffers");
    glBindBuffer = (TglBindBuffer)GetFuncAddress("glBindBuffer");
    glBufferData = (TglBufferData)GetFuncAddress("glBufferData");
    glShaderSource = (TglShaderSource)GetFuncAddress("glShaderSource");
    glCompileShader = (TglCompileShader)GetFuncAddress("glCompileShader");
    glGetShaderiv = (TglGetShaderiv)GetFuncAddress("glGetShaderiv");
    glGetShaderInfoLog = (TglGetShaderInfoLog)GetFuncAddress("glGetShaderInfoLog");
    glAttachShader = (TglAttachShader)GetFuncAddress("glAttachShader");
    glGetProgramiv = (TglGetProgramiv)GetFuncAddress("glGetProgramiv");
    glDetachShader = (TglDetachShader)GetFuncAddress("glDetachShader");
    glDeleteShader = (TglDeleteShader)GetFuncAddress("glDeleteShader");
    glCreateShader = (TglCreateShader)GetFuncAddress("glCreateShader");
    glCreateProgram = (TglCreateProgram)GetFuncAddress("glCreateProgram");
    glLinkProgram = (TglLinkProgram)GetFuncAddress("glLinkProgram");
    glGetProgramInfoLog = (TglGetProgramInfoLog)GetFuncAddress("glGetProgramInfoLog");
    glVertexAttribPointer =  (TglVertexAttribPointer)GetFuncAddress("glVertexAttribPointer");
    glVertexAttribIPointer =  (TglVertexAttribIPointer)GetFuncAddress("glVertexAttribIPointer");
    glEnableVertexAttribArray = (TglEnableVertexAttribArray)GetFuncAddress("glEnableVertexAttribArray");
    glUseProgram = (TglUseProgram)GetFuncAddress("glUseProgram");
    wglCreateContextAttribsARB = (TwglCreateContextAttribsARB)GetFuncAddress("wglCreateContextAttribsARB");
    glGenVertexArrays = (TglGenVertexArrays)GetFuncAddress("glGenVertexArrays");
    glBindVertexArray = (TglBindVertexArray)GetFuncAddress("glBindVertexArray");
    glUniformMatrix4fv = (TglUniformMatrix4fv)GetFuncAddress("glUniformMatrix4fv");
    glGetAttribLocation = (TglGetAttribLocation)GetFuncAddress("glGetAttribLocation");
    glGetUniformLocation = (TglGetUniformLocation)GetFuncAddress("glGetUniformLocation");
    glUniform3fv = (TglUniform3fv)GetFuncAddress("glUniform3fv");
    glUniform2fv = (TglUniform2fv)GetFuncAddress("glUniform2fv");
    glUniform1i = (TglUniform1i)GetFuncAddress("glUniform1i");
    wglChoosePixelFormatARB = (TwglChoosePixelFormatARB)GetFuncAddress("wglChoosePixelFormatARB");
    
    assert(glGenBuffers);
    assert(glBindBuffer);
    assert(glBufferData);
    assert(glShaderSource);
    assert(glCompileShader);
    assert(glGetShaderiv);
    assert(glGetShaderInfoLog);
    assert(glAttachShader);
    assert(glGetProgramiv);
    assert(glDetachShader);
    assert(glDeleteShader);
    assert(glCreateShader);
    assert(glCreateProgram);
    assert(glLinkProgram);
    assert(glGetProgramInfoLog);
    assert(glVertexAttribPointer);
    assert(glVertexAttribIPointer);
    assert(glEnableVertexAttribArray);
    assert(glUseProgram);
    assert(wglCreateContextAttribsARB);
    assert(glGenVertexArrays);
    assert(glBindVertexArray);
    assert(glUniformMatrix4fv);
    assert(glGetAttribLocation);
    assert(glGetUniformLocation);
    assert(glUniform3fv);
    assert(glUniform2fv);
    assert(glUniform1i);
    assert(wglChoosePixelFormatARB);
}

static void
InitializeOpenGL(HWND hwnd)
{
    DebugLog("Creating OPENGL Context\n");

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, //Flags
        PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
        32,                   // Colordepth of the framebuffer.
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,                   // Number of bits for the depthbuffer
        8,                    // Number of bits for the stencilbuffer
        0,                    // Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };
    
    s32 gl_context_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    HDC device_context = GetDC(hwnd);
    s32 chosen_pix_format = ChoosePixelFormat(device_context, &pfd);
    SetPixelFormat(device_context, chosen_pix_format, &pfd);
    HGLRC gl_rendering_context = wglCreateContext(device_context);
    assert(wglMakeCurrent(device_context, gl_rendering_context));
    LoadOpenGLFuncs();
    gl_rendering_context = wglCreateContextAttribsARB(device_context, 0, gl_context_attribs);
    assert(wglMakeCurrent(device_context, gl_rendering_context));
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static inline void 
OpenGLGetError()
{
    GLenum err;

    while((err = glGetError()) != GL_NO_ERROR)
    {
        DebugLog("OpenGL error code : %x\n", err);
    }
}
