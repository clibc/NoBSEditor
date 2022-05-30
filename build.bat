@echo off
pushd _build
      cl /FC /nologo /EHsc /Zi ..\main.cpp /link user32.lib OpenGL32.lib Gdi32.lib && main.exe
popd
