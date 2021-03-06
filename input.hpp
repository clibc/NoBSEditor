#pragma once

enum key_state
{
    NONE    = 0 << 0,
    DOWN    = 1 << 1,
    PRESSED = 1 << 2,
    UP      = 1 << 3
};

enum key_code
{
    KeyCode_A,
    KeyCode_B,
    KeyCode_C,
    KeyCode_D,
    KeyCode_E,
    KeyCode_F,
    KeyCode_G,
    KeyCode_H,
    KeyCode_I,
    KeyCode_J,
    KeyCode_K,
    KeyCode_L,
    KeyCode_M,
    KeyCode_N,
    KeyCode_O,
    KeyCode_P,
    KeyCode_Q,
    KeyCode_R,
    KeyCode_S,
    KeyCode_T,
    KeyCode_U,
    KeyCode_V,
    KeyCode_W,
    KeyCode_X,
    KeyCode_Y,
    KeyCode_Z,

    KeyCode_Shift,
    KeyCode_Escape,
    KeyCode_Delete,
    KeyCode_Backspace,
    KeyCode_Ctrl,
    KeyCode_Space,
    KeyCode_Alt,
    KeyCode_Home,
    KeyCode_End,

    // TODO : These 2 are not working
    KeyCode_Plus, 
    KeyCode_Minus,
    
    KeyCode_Up,
    KeyCode_Down,
    KeyCode_Left,
    KeyCode_Right,
    
    KeyCode_Count
};

struct input_handle
{
    bool IsInitialized = false;
    bool NewInput = false;
    key_state* Keys;
};

// TODO : More robust input system?

static void
ProcessInputWin32(input_handle* Input, MSG& M)
{
    if(!Input->IsInitialized)
    {
        Input->IsInitialized = true;
        Input->Keys = (key_state*)AllocateMemory(KeyCode_Count * sizeof(key_state));
    }

    key_state* Keys = Input->Keys;
    key_state State = NONE;
    
    if(M.message == WM_KEYDOWN || M.message == WM_SYSKEYDOWN
       || M.message == WM_KEYUP || M.message == WM_SYSKEYUP)
    {
        bool WasDown = (HIWORD(M.lParam) & KF_REPEAT) == KF_REPEAT;
        bool IsUp = (HIWORD(M.lParam) & KF_UP) == KF_UP;
        if(WasDown && !IsUp)
        {
            State = PRESSED;
        }
        else if(!WasDown && !IsUp)
        {
            State = DOWN;
        }
        else if(IsUp)
        {
            State = UP;
        }
        Input->NewInput = true;
        
        if(!(Keys[KeyCode_Ctrl] & (DOWN|PRESSED)))
        {
            TranslateMessage(&M);
        }
    }
    else
    {
        DispatchMessage(&M);
        Input->NewInput = false;
        //return;
    }
    
    if(M.wParam == 'A')
    {
        Keys[KeyCode_A] = State;
    }
    else if(M.wParam == 'B')
    {
        Keys[KeyCode_B] = State;
    }
    else if(M.wParam == 'C')
    {
        Keys[KeyCode_C] = State;
    }
    else if(M.wParam == 'D')
    {
        Keys[KeyCode_D] = State;
    }
    else if(M.wParam == 'E')
    {
        Keys[KeyCode_E] = State;
    }
    else if(M.wParam == 'F')
    {
        Keys[KeyCode_F] = State;
    }
    else if(M.wParam == 'G')
    {
        Keys[KeyCode_G] = State;
    }
    else if(M.wParam == 'H')
    {
        Keys[KeyCode_H] = State;
    }
    else if(M.wParam == 'I')
    {
        Keys[KeyCode_I] = State;
    }
    else if(M.wParam == 'J')
    {
        Keys[KeyCode_J] = State;
    }
    else if(M.wParam == 'K')
    {
        Keys[KeyCode_K] = State;
    }
    else if(M.wParam == 'L')
    {
        Keys[KeyCode_L] = State;
    }
    else if(M.wParam == 'M')
    {
        Keys[KeyCode_M] = State;
    }
    else if(M.wParam == 'N')
    {
        Keys[KeyCode_N] = State;
    }
    else if(M.wParam == 'O')
    {
        Keys[KeyCode_O] = State;
    }
    else if(M.wParam == 'P')
    {
        Keys[KeyCode_P] = State;
    }
    else if(M.wParam == 'Q')
    {
        Keys[KeyCode_Q] = State;
    }
    else if(M.wParam == 'R')
    {
        Keys[KeyCode_R] = State;
    }
    else if(M.wParam == 'S')
    {
        Keys[KeyCode_S] = State;
    }
    else if(M.wParam == 'T')
    {
        Keys[KeyCode_T] = State;
    }
    else if(M.wParam == 'U')
    {
        Keys[KeyCode_U] = State;
    }
    else if(M.wParam == 'V')
    {
        Keys[KeyCode_V] = State;
    }
    else if(M.wParam == 'W')
    {
        Keys[KeyCode_W] = State;
    }
    else if(M.wParam == 'X')
    {
        Keys[KeyCode_X] = State;
    }
    else if(M.wParam == 'Y')
    {
        Keys[KeyCode_Y] = State;
    }
    else if(M.wParam == 'Z')
    {
        Keys[KeyCode_Z] = State;
    }
    else if(M.wParam == VK_BACK)
    {
        Keys[KeyCode_Backspace] = State;
    }
    else if(M.wParam == VK_UP)
    {
        Keys[KeyCode_Up] = State;
    }
    else if(M.wParam == VK_DOWN)
    {
        Keys[KeyCode_Down] = State;
    }
    else if(M.wParam == VK_LEFT)
    {
        Keys[KeyCode_Left] = State;
    }
    else if(M.wParam == VK_RIGHT)
    {
        Keys[KeyCode_Right] = State;
    }
    else if(M.wParam == VK_SHIFT)
    {
        Keys[KeyCode_Shift] = State;
    }
    else if(M.wParam == VK_ESCAPE)
    {
        Keys[KeyCode_Escape] = State;
    }
    else if(M.wParam == VK_DELETE)
    {
        Keys[KeyCode_Delete] = State;
    }
    else if(M.wParam == VK_CONTROL)
    {
        Keys[KeyCode_Ctrl] = State;
    }
    else if(M.wParam == VK_SPACE)
    {
        Keys[KeyCode_Space] = State;
    }
    else if(M.wParam == VK_MENU)
    {
        Keys[KeyCode_Alt] = State;
    }
    else if(M.wParam == VK_OEM_PLUS)
    {
        Keys[KeyCode_Plus] = State;
    }
    else if(M.wParam == VK_OEM_MINUS)
    {
        Keys[KeyCode_Plus] = State;
    }
    else if(M.wParam == VK_HOME)
    {
        Keys[KeyCode_Home] = State;
    }
    else if(M.wParam == VK_END)
    {
        Keys[KeyCode_End] = State;
    }
}

static inline bool
GetKeyDown(const input_handle& Input, key_code Key)
{
    if(!Input.NewInput) return false;
    return Input.Keys[Key] == DOWN;
}

static inline bool
GetKeyUp(const input_handle& Input, key_code Key)
{
    if(!Input.NewInput) return false;
    return Input.Keys[Key] == UP;
}

static inline bool
GetKeyPressed(const input_handle& Input, key_code Key)
{
    if(!Input.NewInput) return false;
    return Input.Keys[Key] == PRESSED;
}

static inline bool
GetKey(const input_handle& Input, key_code Key)
{
    if(!Input.NewInput) return false;
    return (Input.Keys[Key] & (DOWN|PRESSED)) > 1;
}
