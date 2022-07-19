@echo off
pushd _build
      REM 4100: unreferenced formal parameter
      REM 4505: unreferenced formal function
      REM Oi: use compiler intrinsics
      REM /EHa-: exeption handling stuff
      REM /GR- : disable runtime type information
      cl /Od /Oi /GR- /EHa- /FC /nologo /EHsc /Zi /W4 /WX /wd4100 /wd4505 ..\main.cpp /link user32.lib OpenGL32.lib Gdi32.lib /SUBSYSTEM:CONSOLE
popd
