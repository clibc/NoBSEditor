#pragma once

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

void (ENTRY *glGenBuffers)(GLsizei,GLuint*);
void (ENTRY *glBindBuffer)(GLenum, GLuint);
void (ENTRY *glBufferData)(GLenum, GLsizeiptr, const GLvoid*, GLenum);
void (ENTRY *glShaderSource)(GLuint, GLsizei, const GLchar**, const GLint*);
void (ENTRY *glCompileShader)(GLuint);
void (ENTRY *glGetShaderiv)(GLuint, GLenum, GLint*);
void (ENTRY *glGetShaderInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*);
void (ENTRY *glAttachShader)(GLuint, GLuint);
void (ENTRY *glGetProgramiv)(GLuint, GLenum, GLint*);
void (ENTRY *glDetachShader)(GLuint, GLuint);
void (ENTRY *glDeleteShader)(GLuint);
void (ENTRY *glLinkProgram)(GLuint);
void (ENTRY *glGetProgramInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*);
GLuint (ENTRY *glCreateShader)(GLenum);
GLuint (ENTRY *glCreateProgram)(void);
void (ENTRY *glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*);
void (ENTRY *glVertexAttribIPointer)(GLuint, GLint, GLenum, GLsizei, const GLvoid*);
void (ENTRY *glEnableVertexAttribArray)(GLuint);
void (ENTRY *glUseProgram)(GLuint);
void (ENTRY *glGenVertexArrays)(GLsizei n,GLuint *arrays);
void (ENTRY *glBindVertexArray)(GLuint);
HGLRC (ENTRY *wglCreateContextAttribsARB)(HDC, HGLRC, const int*);
void (ENTRY *glUniformMatrix4fv)(GLint, GLsizei, GLboolean, const GLfloat*);
GLint (ENTRY *glGetAttribLocation)(GLuint, const GLchar*);
GLint (ENTRY *glGetUniformLocation)(GLuint, const GLchar*);
void  (ENTRY *glUniform3fv)(GLuint, GLsizei, const GLfloat*);
void  (ENTRY *glUniform2fv)(GLuint, GLsizei, const GLfloat*);
void  (ENTRY *glUniform1i)(GLuint, GLint);
bool  (ENTRY *wglChoosePixelFormatARB)(HDC, const int*, const FLOAT *, UINT, int*, UINT*);

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
LoadOpenGLFuncs() {
    glGenBuffers = (void (ENTRY*)(GLsizei,GLuint*))GetFuncAddress("glGenBuffers");
    glBindBuffer = (void (ENTRY*)(GLenum, GLuint))GetFuncAddress("glBindBuffer");
    glBufferData = (void (ENTRY*)(GLenum, GLsizeiptr, const GLvoid*, GLenum))GetFuncAddress("glBufferData");
    glShaderSource = (void (ENTRY*)(GLuint, GLsizei, const GLchar**, const GLint*))GetFuncAddress("glShaderSource");
    glCompileShader = (void (ENTRY*)(GLuint))GetFuncAddress("glCompileShader");
    glGetShaderiv = (void (ENTRY*)(GLuint, GLenum, GLint*))GetFuncAddress("glGetShaderiv");
    glGetShaderInfoLog = (void (ENTRY*)(GLuint, GLsizei, GLsizei*, GLchar*))GetFuncAddress("glGetShaderInfoLog");
    glAttachShader = (void (ENTRY*)(GLuint, GLuint))GetFuncAddress("glAttachShader");
    glGetProgramiv = (void (ENTRY*)(GLuint, GLenum, GLint*))GetFuncAddress("glGetProgramiv");
    glDetachShader = (void (ENTRY*)(GLuint, GLuint))GetFuncAddress("glDetachShader");
    glDeleteShader = (void (ENTRY*)(GLuint))GetFuncAddress("glDeleteShader");
    glCreateShader = (GLuint (ENTRY*)(GLenum))GetFuncAddress("glCreateShader");
    glCreateProgram = (GLuint(ENTRY*)(void))GetFuncAddress("glCreateProgram");
    glLinkProgram = (void (ENTRY*)(GLuint))GetFuncAddress("glLinkProgram");
    glGetProgramInfoLog = (void (ENTRY*)(GLuint, GLsizei, GLsizei*, GLchar*))GetFuncAddress("glGetProgramInfoLog");
    glVertexAttribPointer =  (void (ENTRY*)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*))GetFuncAddress("glVertexAttribPointer");
    glVertexAttribIPointer =  (void (ENTRY*)(GLuint, GLint, GLenum, GLsizei, const GLvoid*))GetFuncAddress("glVertexAttribIPointer");
    glEnableVertexAttribArray = (void (ENTRY*)(GLuint))GetFuncAddress("glEnableVertexAttribArray");
    glUseProgram = (void (ENTRY*)(GLuint))GetFuncAddress("glUseProgram");
    wglCreateContextAttribsARB = (HGLRC (ENTRY *)(HDC, HGLRC, const int*))GetFuncAddress("wglCreateContextAttribsARB");
    glGenVertexArrays = (void (ENTRY*)(GLsizei n,GLuint *arrays))GetFuncAddress("glGenVertexArrays");
    glBindVertexArray = (void (ENTRY*)(GLuint))GetFuncAddress("glBindVertexArray");
    glUniformMatrix4fv = (void (ENTRY*)(GLint, GLsizei,	GLboolean, const GLfloat*))GetFuncAddress("glUniformMatrix4fv");
    glGetAttribLocation = (GLint (ENTRY*)(GLuint,const GLchar*))GetFuncAddress("glGetAttribLocation");
    glGetUniformLocation = (GLint (ENTRY*)(GLuint,const GLchar*))GetFuncAddress("glGetUniformLocation");
    glUniform3fv = (void (ENTRY*)(GLuint, GLsizei, const GLfloat*))GetFuncAddress("glUniform3fv");
    glUniform2fv = (void (ENTRY*)(GLuint, GLsizei, const GLfloat*))GetFuncAddress("glUniform2fv");
    glUniform1i = (void (ENTRY*)(GLuint, GLint))GetFuncAddress("glUniform1i");
    wglChoosePixelFormatARB = (bool (ENTRY*)(HDC, const int*, const float *, unsigned int, int*, unsigned int*))GetFuncAddress("wglChoosePixelFormatARB");
    
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
InitializeOpenGL(HWND hwnd) {
    DebugLog("Creating OPENGL Context\n");

    PIXELFORMATDESCRIPTOR pfd =
        {
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
