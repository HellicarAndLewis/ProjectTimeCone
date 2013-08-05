@echo off

set d=%CD%

if not exist "%d%\build.debug" (
   mkdir %d%\build.debug
)

if not exist "%d%\..\..\bin\data" (
   mkdir %d%\..\..\bin\data
)

cd %d%\build.debug
cmake -DCMAKE_BUILD_TYPE=Debug -G "Visual Studio 10" ..\
cmake --build . --target install --config Debug

cd %d%