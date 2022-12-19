@echo off 

clang -o .\bin\PNG_Compressor3.exe -I src\SDL2-2.26.0\include -I include\ -L src/SDL2-2.26.0/lib/x64   .\src\PNG_Compressor3.c -lSDL2

.\bin\PNG_Compressor3.exe .\materials\test2.png