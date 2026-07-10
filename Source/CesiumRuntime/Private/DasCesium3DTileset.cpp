// Copyright 2020-2024 CesiumGS, Inc. and Contributors

#include "DasCesium3DTileset.h"

#pragma region jiangs

// 构造函数,设置项目定制的默认参数 add Jiangs
ADasCesium3DTileset::ADasCesium3DTileset() : Super() {
  MaximumScreenSpaceError = 64.0;
  SetIgnoreTilesetGltfUpAxis(true);
  SetTilesetSource(ETilesetSource::FromUrl);
}

#pragma endregion
