@echo off
set TARGET=%1
echo Executing script with TARGET=%TARGET%
if "%TARGET%" == "" goto :EXIT
if "%TARGET%" == "clean" goto :CLEAN
if "%TARGET%" == "all" goto :ALL
if "%TARGET%" == "configure" goto :CONFIGURE

echo Building target %TARGET%
call cmake --build build --target %TARGET%
goto :EXIT

:CLEAN
echo Cleaning build directory
call cmake --build build --target clean
goto :EXIT

:CONFIGURE
echo Configuring CMake cache
call cmake -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B build
goto :EXIT

:ALL
echo Building all targets
call cmake --build build
goto :EXIT

:EXIT
echo Done
