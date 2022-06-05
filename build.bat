@echo off
pushd _build
      REM 4100: unreferenced formal parameter
      REM 4505: unreferenced formal function
      cl /Od /D_CRT_SECURE_NO_WARNINGS /FC /nologo /EHsc /Zi /W4 /WX /wd4100 /wd4505 ..\main.cpp /link user32.lib OpenGL32.lib Gdi32.lib && main.exe
popd
