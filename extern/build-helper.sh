cmake -B build -S . -G "Visual Studio 16 2019" -DUNREAL_ENGINE_ROOT="C:/Soft/UnrealEngine/UE_5.3" -DCMAKE_CXX_FLAGS="/MP" && cmake --build build --config Debug --target install && cmake --build build --config Release --target install

cmake -B build -S . -G "Visual Studio 17 2022"   -T v142  -DUNREAL_ENGINE_ROOT="C:/Soft/UnrealEngine/UE_5.3" -DCMAKE_CXX_FLAGS="/MP /WX-" 

#构建Android版本
#注意: 1. 必须设置ANDROID_NDK_HOME环境变量 2. UE本地构建版本需要用Engine/Windows作为根路径
export ANDROID_NDK_HOME="C:/Users/das/AppData/Local/Android/Sdk/ndk/25.1.8937393"
export ANDROID_NDK_ROOT="C:/Users/das/AppData/Local/Android/Sdk/ndk/25.1.8937393"
cmake -B build-android -S . -G Ninja -DCMAKE_TOOLCHAIN_FILE="unreal-android-toolchain.cmake" -DUNREAL_ENGINE_ROOT="E:/UE532LocalBuilds_20251113/Engine/Windows" -DEZVCPKG_BASEDIR="F:/.ezvcpkg" -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM="C:/Users/das/AppData/Local/Android/Sdk/cmake/3.10.2.4988404/bin/ninja.exe"

cmake --build build-android --config Release --target install