# PasswordFileManager
Converts PasswordConnectionFile to encrypted data in DB and vice-versa

# Requirements
mingw-w64-x86_64-openssl

# Build process (CMD)
```
set MSYS=C:/msys64/mingw64
set PATH=%MSYS%/bin;%PATH%
cd C:\thiago\vscode\PasswordFileManager
rmdir /s /q build
mkdir build
cd build

cmake -G "Ninja" -DCMAKE_MAKE_PROGRAM=%MSYS%/bin/ninja.exe -DCMAKE_C_COMPILER=%MSYS%/bin/gcc.exe -DCMAKE_CXX_COMPILER=%MSYS%/bin/g++.exe -DOPENSSL_ROOT_DIR=%MSYS% -DOPENSSL_INCLUDE_DIR=%MSYS%/include -DOPENSSL_CRYPTO_LIBRARY=%MSYS%/lib/libcrypto.a -DOPENSSL_SSL_LIBRARY=%MSYS%/lib/libssl.a ..
cmake --build .
```

# Run APP
PasswordFileManager.exe lock testFile1.txt pass1234567891011121314151617181920
PasswordFileManager.exe unlock testFile1.txt pass1234567891011121314151617181920

# Test Database
testDatabase.exe

# Test FileParser
testFileParser.exe

# Test FileGenerator
testFileGenerator.exe