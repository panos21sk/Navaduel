@echo off
for %%F in ("%CD%") do set "currentDir=%%~nF"

REM Check the current directory name
if /I "%currentDir%"=="Navaduel" (
    echo Proceeding with install for "Navaduel".
    goto :install
) else if /I "%currentDir%"=="Navaduel-main" (
    echo Proceeding with install for "Navaduel-main".
    goto :install
) else (
    echo Script must be run from the directory installed from GitHub.
    timeout /t 3 /nobreak >nul
    exit /b
)

:install
mkdir build
echo install.bat supports MinGW build tools by default.
echo Modify the cmake makefile gen line if you want to use a different build system.
timeout /t 1 /nobreak >nul
cmake -S . -B build -G "MinGW Makefiles"
echo Finished generating makefile.
echo Building...
cmake --build build
echo Done building.

REM Move executable to root directory
cd build
move "*.exe" ".."
cd ..

REM Create run_navaduel.bat file
echo Creating run_navaduel.bat file...
(
    echo @echo off
    echo cd /d "%CD%"
    echo start "" "Navaduel.exe"
) > run_navaduel.bat

REM Move run_navaduel.bat to desktop
set "desktop=%USERPROFILE%\Desktop"
move "run_navaduel.bat" "%desktop%\run_navaduel.bat"
echo Finished! Have fun and thanks for playing!

REM Pause at the end so the user can see the result
pause