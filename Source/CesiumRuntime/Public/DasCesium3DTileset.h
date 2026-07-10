// Copyright 2020-2024 CesiumGS, Inc. and Contributors

#pragma once

#include "Cesium3DTileset.h"
#include "CoreMinimal.h"

#include "DasCesium3DTileset.generated.h"

#pragma region jiangs

/**
 * ACesium3DTileset 的子类,提供项目定制的默认参数:
 * IgnoreTilesetGltfUpAxis 默认 true, MaximumScreenSpaceError 默认 64。 add Jiangs
 */
UCLASS()
class CESIUMRUNTIME_API ADasCesium3DTileset : public ACesium3DTileset {
  GENERATED_BODY()

public:
  ADasCesium3DTileset();
};

#pragma endregion
