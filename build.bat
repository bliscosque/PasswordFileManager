@echo off
setlocal

:: Set MSYS2 paths
set MSYS=C:/msys64/mingw64
set PATH=%MSYS%/bin;%PATH%

:: Clean build directory
echo Cleaning build directory...
if exist build rmdir /s /q build
mkdir build
cd build

:: Configure with CMake
echo Configuring with CMake...
cmake -G "Ninja" -DCMAKE_MAKE_PROGRAM=%MSYS%/bin/ninja.exe -DCMAKE_C_COMPILER=%MSYS%/bin/gcc.exe -DCMAKE_CXX_COMPILER=%MSYS%/bin/g++.exe -DOPENSSL_ROOT_DIR=%MSYS% -DOPENSSL_INCLUDE_DIR=%MSYS%/include -DOPENSSL_CRYPTO_LIBRARY=%MSYS%/lib/libcrypto.a -DOPENSSL_SSL_LIBRARY=%MSYS%/lib/libssl.a ..

:: Build
echo Building...
cmake --build .

:: Check if build was successful
if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    exit /b 1
)

echo Build completed successfully!
cd ..
endlocal