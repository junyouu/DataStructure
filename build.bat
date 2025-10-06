@echo off
echo ========================================
echo Building Job & Resume Matching System
echo ========================================
echo.

cd src

REM First, delete old executable
if exist main.exe (
    echo Removing old main.exe...
    del main.exe
    echo.
)

REM Try g++
echo [1/4] Trying g++...
g++ -std=c++11 main.cpp job_linkedlist.cpp resume_linkedlist.cpp matching.cpp -o main.exe >nul 2>&1

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo ✓ Build successful with g++!
    echo ========================================
    echo.
    echo Run the program with:
    echo   cd src
    echo   main.exe
    echo.
    goto success
)

REM Try mingw32-g++
echo g++ not found, trying mingw32-g++...
mingw32-g++ -std=c++11 main.cpp job_linkedlist.cpp resume_linkedlist.cpp matching.cpp -o main.exe >nul 2>&1

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo ✓ Build successful with mingw32-g++!
    echo ========================================
    echo.
    goto success
)

REM Try gcc
echo mingw32-g++ not found, trying gcc...
gcc -std=c++11 -lstdc++ main.cpp job_linkedlist.cpp resume_linkedlist.cpp matching.cpp -o main.exe >nul 2>&1

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo ✓ Build successful with gcc!
    echo ========================================
    echo.
    goto success
)

REM Try Visual Studio cl
echo gcc not found, trying Visual Studio cl...
cl /EHsc /std:c++14 /nologo main.cpp job_linkedlist.cpp resume_linkedlist.cpp matching.cpp >nul 2>&1

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo ✓ Build successful with Visual Studio!
    echo ========================================
    echo.
    goto success
)

REM All methods failed
echo.
echo ========================================
echo ✗ Build failed - No compiler found!
echo ========================================
echo.
echo SOLUTIONS:
echo.
echo 1. Install MinGW (Recommended):
echo    Download from: https://sourceforge.net/projects/mingw-w64/
echo    After installation, add C:\MinGW\bin to PATH
echo.
echo 2. Install Visual Studio Community (Free):
echo    Download from: https://visualstudio.microsoft.com/
echo    Select "Desktop development with C++"
echo.
echo 3. Use Code::Blocks or Dev-C++ (Includes compiler):
echo    These IDEs come with MinGW bundled
echo.
echo 4. Use online compiler (Quick test):
echo    https://www.onlinegdb.com/online_c++_compiler
echo.
echo See COMPILE_INSTRUCTIONS.md for detailed help
echo.
goto end

:success
echo To test the new features:
echo   1. Match Top 3 Resumes for a Job ID
echo   2. Add New Record (Job/Resume)
echo   3. Delete Record (Head/Middle/Tail)
echo.
echo All operations include performance timing!
echo.

:end
cd ..
pause

