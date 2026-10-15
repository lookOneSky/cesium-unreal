@echo off
REM Build cesium-native for UE 5.3 with Visual Studio 2026 + v143 (MSVC 14.34) toolset.
REM ASCII only on purpose: the file is UTF-8 and cmd.exe would garble non-ASCII text.
setlocal enabledelayedexpansion

REM ==== overridable by environment variables ====
if not defined UE_ROOT set "UE_ROOT=S:\Soft\UE532LocalBuilds_20260226\Engine\Windows"
if not defined EZVCPKG_BASEDIR set "EZVCPKG_BASEDIR=F:/.ezvcpkg"

REM VS2026 generator + v143 (14.34.31933): same MSVC toolset UE 5.3 uses.
set "GENERATOR=Visual Studio 18 2026"
set "TOOLSET=v143,version=14.34.31933,host=x64"
set "BUILD_DIR=%~dp0build-vs2026"
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if /i "%~1"=="help"      goto :help
if /i "%~1"=="clean"     goto :clean
if /i "%~1"=="configure" goto :configure_only
if /i "%~1"=="debug"     goto :debug
if /i "%~1"=="release"   goto :release
if not "%~1"==""         goto :invalid_action

call :configure      || exit /b 1
call :build Debug    || exit /b 1
call :build Release  || exit /b 1
exit /b 0

:configure_only
call :configure
exit /b %errorlevel%

:debug
call :configure || exit /b 1
call :build Debug
exit /b %errorlevel%

:release
call :configure || exit /b 1
call :build Release
exit /b %errorlevel%

:configure
REM --- locate VS2026 (18.x), fall back to the default install path ---
REM vswhere path contains "(x86)", so it must not be used inside an if(...) block.
if not exist "!VSWHERE!" goto :vswhere_done
for /f "usebackq tokens=*" %%i in (`""!VSWHERE!" -products * -latest -property installationPath"`) do set "VS_ROOT=%%i"
:vswhere_done
REM v180 only exists in VS2026; anything else (e.g. only VS2022 found) falls back.
if defined VS_ROOT if exist "!VS_ROOT!\MSBuild\Microsoft\VC\v180" goto :vs_root_ok
set "VS_ROOT=S:\Soft\VS2026"
:vs_root_ok

if exist "!VS_ROOT!\VC\Tools\MSVC\14.34.31933" goto :toolset_ok
echo [error] MSVC 14.34.31933 not found. Install "MSVC v143 - VS 2022 C++ x64/x86 build tools (v14.34)".
exit /b 1
:toolset_ok
if exist "!VS_ROOT!\MSBuild\Microsoft\VC\v170\Platforms\x64\PlatformToolsets\v143" goto :platform_ok
echo [error] v143 platform toolset missing in "!VS_ROOT!".
exit /b 1
:platform_ok

REM --- validate the Unreal root (local builds must point at ...\Engine\Windows) ---
if exist "!UE_ROOT!\Engine\Build\Build.version" goto :ue_ok
echo [error] invalid UNREAL_ENGINE_ROOT: "!UE_ROOT!"
exit /b 1
:ue_ok
if exist "!UE_ROOT!\Engine\Source\ThirdParty\OpenSSL" goto :openssl_ok
echo [error] Unreal OpenSSL not found: "!UE_ROOT!\Engine\Source\ThirdParty\OpenSSL"
exit /b 1
:openssl_ok

where cmake >nul 2>&1
if errorlevel 1 (
    echo [error] CMake was not found in PATH.
    exit /b 1
)

REM --- drop a cache produced by another generator (build-vs2026 used to be Ninja) ---
if not exist "!BUILD_DIR!\CMakeCache.txt" goto :cache_ok
findstr /c:"CMAKE_GENERATOR:INTERNAL=!GENERATOR!" "!BUILD_DIR!\CMakeCache.txt" >nul
if errorlevel 1 (
    echo [info] generator changed, removing "!BUILD_DIR!"
    rmdir /s /q "!BUILD_DIR!"
)
:cache_ok

REM --- vcpkg overlays (openssl portfile / triplet) read the engine path from the environment ---
set "UNREAL_ENGINE_ROOT=!UE_ROOT!"
echo [info] Visual Studio: !VS_ROOT!
echo [info] Unreal Engine: !UE_ROOT!

cmake -S "%~dp0." -B "!BUILD_DIR!" -G "!GENERATOR!" -A x64 -T "!TOOLSET!" ^
    "-DUNREAL_ENGINE_ROOT=!UE_ROOT:\=/!" ^
    "-DEZVCPKG_BASEDIR=!EZVCPKG_BASEDIR!" ^
    "-DCMAKE_POLICY_VERSION_MINIMUM=3.5" ^
    "-DCMAKE_CXX_FLAGS=/MP /WX-"
exit /b %errorlevel%

:build
cmake --build "!BUILD_DIR!" --config %~1 --target install -- /m /v:m
exit /b %errorlevel%

:clean
if exist "!BUILD_DIR!" rmdir /s /q "!BUILD_DIR!"
exit /b 0

:invalid_action
echo Unknown action: %~1

:help
echo Usage: %~nx0 [configure^|debug^|release^|clean^|help]
echo Without an action, configures and installs both Debug and Release.
echo Env overrides: UE_ROOT, EZVCPKG_BASEDIR
exit /b 0
