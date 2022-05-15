call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\vsdevcmd.bat"
cmake -B build64 -A x64
cmake -B build32 -A Win32
cmake --build build64 --config Release
cmake --build build32 --config Release
pause