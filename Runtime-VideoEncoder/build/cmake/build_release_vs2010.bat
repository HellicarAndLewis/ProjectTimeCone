@echo off

set d=%CD%

if not exist "%d%\build.release" (
   mkdir %d%\build.release
)

if not exist "%d%\..\..\bin\data" (
   mkdir %d%\..\..\bin\data
)

cd %d%\build.release
cmake -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 10" ..
cmake --build . --target install --config Release

cd %d%