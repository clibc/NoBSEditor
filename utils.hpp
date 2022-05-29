#pragma once

#define DebugLog(...)                        \
    {                                                                   \
        char temp[200] = {};                                            \
        sprintf(temp, __VA_ARGS__);                                     \
        WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), temp, strlen(temp), NULL, NULL); \
    }                                               
