# cesium-unreal extern Android 构建指南

## 环境依赖

| 依赖 | 版本 | 路径 |
|------|------|------|
| CMake | 3.31.7 | `C:/Program Files/CMake/bin/cmake.exe` |
| Ninja | 1.10.2 (Android SDK自带) | `C:/Users/das/AppData/Local/Android/Sdk/cmake/3.10.2.4988404/bin/ninja.exe` |
| Ninja (备选) | 系统环境变量 | `S:/SoftPack/ninja-win` |
| Android NDK | r25c (25.1.8937393) | `C:/Users/das/AppData/Local/Android/Sdk/ndk/25.1.8937393` |
| Clang | 14.0.6 (NDK自带) | `...ndk/25.1.8937393/toolchains/llvm/prebuilt/windows-x86_64/bin/clang++.exe` |
| Unreal Engine | 5.3.2 (本地构建版) | `E:/UE532LocalBuilds_20251113` |
| VCPKG缓存 | 2024.11.16 | `F:/.ezvcpkg` |

## 构建步骤

### 1. 设置环境变量

```bash
export ANDROID_NDK_HOME="C:/Users/das/AppData/Local/Android/Sdk/ndk/25.1.8937393"
export ANDROID_NDK_ROOT="C:/Users/das/AppData/Local/Android/Sdk/ndk/25.1.8937393"
```

### 2. Configure

```bash
cmake -B build-android -S . -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="unreal-android-toolchain.cmake" \
  -DUNREAL_ENGINE_ROOT="E:/UE532LocalBuilds_20251113/Engine/Windows" \
  -DEZVCPKG_BASEDIR="F:/.ezvcpkg" \
  -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_MAKE_PROGRAM="C:/Users/das/AppData/Local/Android/Sdk/cmake/3.10.2.4988404/bin/ninja.exe"
```

### 3. Build & Install

```bash
cmake --build build-android --config Release --target install
```

### 输出

库文件安装到: `../Source/ThirdParty/lib/Android-aarch64-Release/`

共计约 25 个静态库 (.a)，包括:
- libCesium3DTilesSelection.a
- libCesiumAsync.a
- libCesiumGeospatial.a
- libCesiumGltf.a / libCesiumGltfReader.a
- libDasCesium.a
- libDasCesiumExtra.a
- libMikkTSpace.a
- libtidy.a
- 以及所有 vcpkg 依赖库

## 构建过程中遇到的问题与解决方法

### 问题 1: 之前的 build-android 使用了错误的编译器

**现象**: `build.ninja` 中所有编译规则指向 `F:\Soft\msys64\ucrt64\bin\c++.exe` (MSYS2 GCC)，而不是 NDK 的 clang。

**原因**: 上次 configure 时未正确传入 `-DCMAKE_TOOLCHAIN_FILE`，CMake 回退到系统默认编译器。

**解决**: 删除旧 `build-android` 目录，重新 configure 并确保传入 `-DCMAKE_TOOLCHAIN_FILE="unreal-android-toolchain.cmake"`。

---

### 问题 2: Ninja 未找到

**现象**: 系统 PATH 中没有 Ninja。

**解决**: 使用 Android SDK 自带的 Ninja，通过 `-DCMAKE_MAKE_PROGRAM` 显式指定路径:
```
-DCMAKE_MAKE_PROGRAM="C:/Users/das/AppData/Local/Android/Sdk/cmake/3.10.2.4988404/bin/ninja.exe"
```

备选: 使用系统安装的 Ninja (`S:/SoftPack/ninja-win`)。

---

### 问题 3: vcpkg 找不到 Android NDK

**现象**:
```
CMake Error at scripts/toolchains/android.cmake:29 (message):
  Could not find android ndk. Searched at C:\Program Files (x86)/Android/android-sdk/ndk-bundle
```

**原因**: vcpkg 内部的 android toolchain 从 `ANDROID_NDK_HOME` / `ANDROID_NDK_ROOT` 环境变量获取 NDK 路径，而不是从 CMake 变量 `CMAKE_ANDROID_NDK`。

**解决**: 在 configure 前设置环境变量:
```bash
export ANDROID_NDK_HOME="C:/Users/das/AppData/Local/Android/Sdk/ndk/25.1.8937393"
export ANDROID_NDK_ROOT="C:/Users/das/AppData/Local/Android/Sdk/ndk/25.1.8937393"
```

---

### 问题 4: OpenSSL 找不到 UE 路径

**现象**:
```
Could not deduce the OpenSSL root directory because there is not exactly one directory matching
`E:/UE532LocalBuilds_20251113/Engine/Source/ThirdParty/OpenSSL/*`.
```

**原因**: 本地构建版 UE 的目录结构与 Epic Launcher 安装版不同。OpenSSL 实际位于:
```
E:/UE532LocalBuilds_20251113/Engine/Windows/Engine/Source/ThirdParty/OpenSSL/1.1.1t/
```

**解决**: 将 `UNREAL_ENGINE_ROOT` 从 `E:/UE532LocalBuilds_20251113` 改为 `E:/UE532LocalBuilds_20251113/Engine/Windows`:
```
-DUNREAL_ENGINE_ROOT="E:/UE532LocalBuilds_20251113/Engine/Windows"
```

---

### 问题 5: 未使用变量警告 (Clang -Werror)

**现象**: 多处编译错误，Clang 的 `-Werror` 将警告转为错误。

**涉及文件与修复**:

#### 5a. `RasterOverlayTileProvider.cpp` — 未使用调试变量
```
error: unused variable 'pTile1' [-Werror,-Wunused-variable]
error: unused variable 'pTile2' [-Werror,-Wunused-variable]
```
**修复**: 添加 `(void)pTile1; (void)pTile2;` 消除警告。

#### 5b. `EllipsoidTilesetLoader.cpp` — 缺少字段初始化器
```
error: missing field 'tileBoundingVolume' initializer [-Werror,-Wmissing-field-initializers]
```
**原因**: `TileLoadResult` 结构体新增了 Das 自定义字段 `tileBoundingVolume`(在 `#pragma region Das` 中），但 13 处聚合初始化未提供该字段值。

**修复**: 在所有 `TileLoadResult{...}` 初始化末尾添加 `std::nullopt`（对应 `tileBoundingVolume`），涉及以下文件:
- `EllipsoidTilesetLoader.cpp` — 添加 `ellipsoid` 和 `std::nullopt`
- `DasTerrainLoader.cpp` — 添加 `ellipsoid` 和 `std::nullopt`
- `TerrainLayerInfoLoader.cpp` — 添加 `ellipsoid` 和 `std::nullopt`
- `TilesetJsonLoader.cpp` (3处)
- `TilesetContentLoader.cpp` (2处)
- `LayerJsonTerrainLoader.cpp` (2处)
- `ImplicitQuadtreeLoader.cpp` (2处)
- `ImplicitOctreeLoader.cpp` (2处)
- `RasterOverlayUpsampler.cpp` (1处)

#### 5c. `Tileset.cpp` — 有符号/无符号比较 + 未使用变量
```
error: comparison of integers of different signs [-Werror,-Wsign-compare]
error: unused variable 'boundingVolumeCenter' [-Werror,-Wunused-variable]
```
**修复**:
- `vecTiles.size() < nNeed` → `vecTiles.size() < static_cast<size_t>(nNeed)`
- `boundingVolumeCenter` 变量改为 `(void)getBoundingVolumeCenter(...)`

---

## 注意事项

1. **Windows 构建 vs Android 构建**: Windows 使用 MSVC (`/Zp8`, `/WX-`)，Android 使用 Clang (`-Werror`)。Clang 对警告更严格，因此 Windows 编译通过的代码在 Android 上可能报错。

2. **Das 自定义修改**: 所有对上游 cesium-native 的修改都在 `#pragma region Das` 中标记。添加新字段到结构体时，务必搜索所有聚合初始化位置并补全。

3. **本地构建版 UE**: 如果使用从源码编译的 UE，目录结构为 `Engine/Windows/Engine/Source/...`，需要相应调整 `UNREAL_ENGINE_ROOT`。

4. **VCPKG 缓存**: 首次构建时 vcpkg 会下载并编译所有依赖，耗时较长。后续构建会使用缓存。
