@echo off 

clang -o .\bin\PNG_Compressor2.exe -I src\SDL2-2.26.0\include -L src/SDL2-2.26.0/lib/x64   .\src\PNG_Compressor2.c -lSDL2

.\bin\PNG_Compressor2.exe .\materials\test.png