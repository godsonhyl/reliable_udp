@echo off

set buildDir=%~dp0\..\build_vs2022_64_release

if not exist %buildDir%	(md %buildDir%)

cd /d %buildDir%

echo setup vs2022 x64 release solution in %cd%

cmake -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 17 2022" ..

echo build and install project

cmake --build . --parallel --config Release --target install 

pause