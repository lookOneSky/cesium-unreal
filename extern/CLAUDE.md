# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

This is the `extern/` directory for the `cesium-unreal` plugin. It builds native C++ libraries that are installed into `../Source/ThirdParty/` for use by Unreal Engine. **Always build from this directory, not from `cesium-native/` directly.**

For cesium-native internals, see `cesium-native/CLAUDE.md`.

## Build Commands

### Windows (Visual Studio 2022)

```bash
# Configure (one-time)
cmake -B build -S . -G "Visual Studio 17 2022" -T v142 \
  -DUNREAL_ENGINE_ROOT="C:\Soft\UnrealEngine\UE_5.3" \
  -DCMAKE_CXX_FLAGS="/MP /WX-"

# Build and install
cmake --build build --config Release --target install
cmake --build build --config Debug --target install
```

### Android (Cross-compile from Windows)

Prerequisites: Android NDK (`choco install ninja` for Ninja).

```bash
SET ANDROID_NDK_ROOT=C:/Android/ndk/r25c

cmake -B build-android -S . -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="unreal-android-toolchain.cmake" \
  -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build-android --config Release --target install
```

Other platforms use the corresponding `unreal-<platform>-toolchain.cmake` file.

### Key CMake Variables

- `UNREAL_ENGINE_ROOT` — Required. Path to UE installation (defaults to `C:/Program Files/Epic Games/UE_5.3` on Windows if omitted and directory exists).
- `CMAKE_CXX_FLAGS="/MP /WX-"` — Recommended: parallel builds, warnings not treated as errors.

### Output Location

Built libraries install to: `../Source/ThirdParty/lib/<Platform>-<Arch>-<Debug|Release>/`

## Running Tests

Tests are excluded from the default build (`EXCLUDE_FROM_DEFAULT_BUILD`). To run them:

```bash
cmake --build build --target cesium-native-tests
./build/cesium-native/CesiumNativeTests/cesium-native-tests
```

## Architecture

### What Gets Built

| Component | Source | Purpose |
|---|---|---|
| cesium-native | `cesium-native/` | Core geospatial library (28 modules) |
| DasCesium | `cesium-native/DasCesium/` | Custom terrain loader with height offset |
| DasCesiumExtra | `cesium-native/DasCesiumExtra/` | Additional Das utilities |
| MikkTSpace | `MikkTSpace/` | Tangent space library (Android/iOS only) |
| tidy-html5 | `tidy-html5/` | HTML parser |
| swl-variant | `swl-variant/` | Optimized `std::variant` replacement |

### DasCesium Custom Extension

`DasCesium/DasTerrainLoader` extends `EllipsoidTilesetLoader` to apply a global height offset (`DasTerrainOptionsExtra::dAddHeightMeter`) to terrain meshes. All modifications to upstream Cesium code are marked with `#pragma region Das`.

`DasCesiumExtra` adds `DasLayerJsonTerrainLoader`, `TerrainLayerInfoLoader`, and SQLite caching (`EVSqliteCache`).

### VCPKG Dependencies

Custom package overlays are in `vcpkg-overlays/`. Triplets are auto-detected; Unreal-specific triplets (e.g., `arm64-android-unreal.cmake`) are in `vcpkg-overlays/triplets/`.

OpenSSL is excluded from installation (Unreal Engine provides its own). SQLite3 symbols are renamed to `cesium_sqlite3_*` to avoid conflicts with UE's sqlite3.

### Important Compiler Flags

- Windows: `/Zp8` struct alignment to match Unreal Engine's `64-bit` build setting — critical for ABI compatibility across DLL boundaries.
- All builds: `NDEBUG` always defined (Debug builds also add `CESIUM_FORCE_ASSERTIONS`).
- Non-Windows: `-fno-rtti` to match Unreal Engine.

## 输出规范

### 结论规范
所有代码推导出的结论，都需要关联上对应的代码说明。

## 修改规范

### 源码修改规范
这个是开源代码，对于开源代码的修改需要用#pragma region jiangs;#pragma endregion jiangs  标记出来。#pragma region jiangs要再一行的开头写