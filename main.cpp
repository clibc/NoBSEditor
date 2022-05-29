#include <iostream>
#include <windows.h>
#include <assert.h>
#include <gl/gl.h>

#include "defines.h"
#include "utils.hpp"
#include "opengl.hpp"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static bool Is_Running = true;

s32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                   LPSTR lpCmdLine, int nCmdShow) {
    assert(AllocConsole());

    WNDCLASS wc = {};
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "window";
    RegisterClass(&wc);

    HWND window_handle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
                                        TEXT("window"),
                                        TEXT("fasted"),
                                        WS_OVERLAPPEDWINDOW,
                                        CW_USEDEFAULT,CW_USEDEFAULT,
                                        CW_USEDEFAULT,CW_USEDEFAULT,
                                        NULL,NULL,hInstance,NULL);

    HDC device_context = GetDC(window_handle);

    ShowWindow(window_handle, nCmdShow);

    MSG msg;

    while(GetMessage(&msg, NULL, 0, 0) > 0 && Is_Running) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT);
        
        SwapBuffers(device_context);
        if (GetKeyState(VK_ESCAPE) & 0x8000)
        {
            Is_Running = false;
        }
    }
    
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { 
    switch (uMsg) 
    { 
    case WM_CREATE: {
        InitializeOpenGL(hwnd);
        return 0;
    }break;
    case WM_SIZE: {
    }break;
    case WM_DESTROY: {
        Is_Running = false;
    }break;
    default: 
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    } 
    return 0; 
}
