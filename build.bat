@echo off
pushd _build
      cl /D_CRT_SECURE_NO_WARNINGS /FC /nologo /EHsc /Zi /W3 /Od ..\main.cpp /link user32.lib OpenGL32.lib Gdi32.lib && main.exe
popd
