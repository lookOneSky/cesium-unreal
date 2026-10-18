# 已失效：本机未装 VS2019；VS2022/VS2026 均无 v142 工具集；C:/Soft/UnrealEngine/UE_5.3 不存在
# cmake -B build -S . -G "Visual Studio 16 2019" -DUNREAL_ENGINE_ROOT="C:/Soft/UnrealEngine/UE_5.3" -DCMAKE_CXX_FLAGS="/MP" && cmake --build build --config Debug --target install && cmake --build build --config Release --target install
# cmake -B build -S . -G "Visual Studio 17 2022"   -T v142  -DUNREAL_ENGINE_ROOT="C:/Soft/UnrealEngine/UE_5.3" -DCMAKE_CXX_FLAGS="/MP /WX-"

# 构建 Windows 版本（VS2026 + v143/14.34 工具集，与 UE5.3 一致）
# 推荐直接跑 build-vs2026.bat；下面三行可直接粘贴到 Git Bash / cmd / PowerShell 运行
# 兼容约束：不续行、不用 && 串联（PS5.1 不认 &&，cmd 不认 ;）；-D 参数整体加引号（PS5.1 会把 =3.5 拆成 =3 .5）
# MSVC 开关用 - 前缀：Git Bash 会把 /MP 转换成 C:/Program Files/Git/MP
cmake -B build-vs2026 -S . -G "Visual Studio 18 2026" -A x64 -T "v143,version=14.34.31933,host=x64" "-DUNREAL_ENGINE_ROOT=S:/Soft/UE532LocalBuilds_20260226/Engine/Windows" "-DEZVCPKG_BASEDIR=F:/.ezvcpkg" "-DCMAKE_POLICY_VERSION_MINIMUM=3.5" "-DCMAKE_CXX_FLAGS=-MP -WX-"
cmake --build build-vs2026 --config Debug --target install
cmake --build build-vs2026 --config Release --target install

# 构建 Android 版本
# 注意：1. 必须设置 ANDROID_NDK_HOME 环境变量；2. UE 本地构建版本需要用 Engine/Windows 作为根路径。
export ANDROID_NDK_HOME="C:/Users/das/AppData/Local/Android/Sdk/ndk/25.1.8937393"
export ANDROID_NDK_ROOT="C:/Users/das/AppData/Local/Android/Sdk/ndk/25.1.8937393"
cmake -B build-android -S . -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="unreal-android-toolchain.cmake" \
  -DUNREAL_ENGINE_ROOT="S:/Soft/UE532LocalBuilds_20260226/Engine/Windows" \
  -DEZVCPKG_BASEDIR="F:/.ezvcpkg" \
  -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_MAKE_PROGRAM="C:/Users/das/AppData/Local/Android/Sdk/cmake/3.10.2.4988404/bin/ninja.exe" && \
cmake --build build-android --config Release --target install
