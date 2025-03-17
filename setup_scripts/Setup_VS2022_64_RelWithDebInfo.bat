@echo off

set buildDir=%~dp0\..\build_vs2022_64_relwithdebinfo

if not exist %buildDir%	(md %buildDir%)

cd /d %buildDir%

echo setup vs2022 x64 relwithdebinfo solution in %cd%

cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -G "Visual Studio 17 2022" ..

echo build and install project

cmake --build . --parallel --config RelWithDebInfo --target install 

pause