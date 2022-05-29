@echo off
pushd _build
      cl /Zi ..\main.cpp && main.exe
popd
