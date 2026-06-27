@echo off
echo ===================================================
echo   COMPILING HUFFMAN COMPRESSOR
echo ===================================================
g++ -std=c++17 -O3 -Wall -Wextra src/main.cpp src/HuffmanCoder.cpp -o huffman_compressor.exe
if errorlevel 1 (
    echo Compilation failed!
    exit /b 1
)
echo Compilation successful.
echo.

echo ===================================================
echo   RUNNING VERIFICATION SUITE
echo ===================================================
powershell -ExecutionPolicy Bypass -File .\verify.ps1
if errorlevel 1 (
    echo Verification failed!
    exit /b 1
)
echo.

echo ===================================================
echo   RUNNING DEMO
echo ===================================================
call .\demo.bat
