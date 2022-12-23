@echo off 

clang -o .\bin\PNG_Decoder3_Practice.exe -I src\SDL2-2.26.0\include -I include\ -I include\zlib -L src/SDL2-2.26.0/lib/x64 -L include\zlib  .\src\PNG_Decoder3_Practice.c -lSDL2 -lzlibwapi

.\bin\PNG_Decoder3_Practice.exe .\materials\test2.png