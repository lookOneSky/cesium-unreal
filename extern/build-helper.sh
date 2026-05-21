cmake -B build -S . -G "Visual Studio 16 2019" && cmake --build build --config Debug --target install && cmake --build build --config Release --target install

cmake -B build -S .  -G "Visual Studio 17 2022"   -T v142  -DUNREAL_ENGINE_ROOT="D:/soft/UnrealEngine/UE_5.3" -DCMAKE_CXX_FLAGS="/MP /WX-" -DEZVCPKG_BASEDIR="D:/Deps/cesium-ezvcpkg" 

cmake -B build -S . --fresh  -G "Visual Studio 17 2022"   -T v142  -DUNREAL_ENGINE_ROOT="D:/soft/UnrealEngine/UE_5.3" -DCMAKE_CXX_FLAGS="/MP /WX-" -DEZVCPKG_BASEDIR="D:/Deps/cesium-ezvcpkg" 