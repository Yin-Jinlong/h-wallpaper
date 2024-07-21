@echo off
cmd /c pnpm --version >nul
if %errorlevel% neq 0 (
    echo "pnpm not found"
    set /p answer="Do you want to install it? (y/n):"
    if /i "%answer%"=="y" (
        npm install -g pnpm
    )
    else (
        exit /b 1
    )
)
@echo on

pnpm i && pnpm run update-libs
cmake --build build-release --target all clean
cmake -B build-release -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build-release -j 8 --config Release

