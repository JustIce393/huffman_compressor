@echo off
echo.
echo ===================================================
echo               RUNNING HUFFMAN COMPRESSOR DEMO
echo ===================================================
echo.

:: Create a sample file with repetitive text
echo Huffman Coding is an entropy encoding algorithm used for lossless data compression. > demo_input.txt
echo The program was designed by David A. Huffman while he was a Ph.D. student at MIT. >> demo_input.txt
for /L %%i in (1,1,20) do (
    echo Huffman Coding is an entropy encoding algorithm used for lossless data compression. >> demo_input.txt
    echo The program was designed by David A. Huffman while he was a Ph.D. student at MIT. >> demo_input.txt
)

echo [1] Compressing demo_input.txt ...
huffman_compressor.exe compress demo_input.txt demo_compressed.huff

echo.
echo [2] Decompressing demo_compressed.huff ...
huffman_compressor.exe decompress demo_compressed.huff demo_restored.txt

echo.
echo [3] Verifying byte-by-byte equality ...
fc /B demo_input.txt demo_restored.txt > nul
if errorlevel 1 (
    echo FAILED: Restored file differs from original!
) else (
    echo SUCCESS: Restored file matches original exactly!
)

echo.
echo Cleaning up demo files...
del demo_input.txt
del demo_compressed.huff
del demo_restored.txt
echo Done!
