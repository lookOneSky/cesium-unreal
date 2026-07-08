#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DeveloperSettings.h"
#include "Das3DTilesSetting.generated.h"

/**
 * UDas3DTilesSetting
 * 用于存储 3D Tiles 加载管理器的设置。
 */
UCLASS(Config = Game, DefaultConfig)
class CESIUMRUNTIME_API UDas3DTilesSetting : public UDeveloperSettings {
  GENERATED_BODY()
public:
  // 保存设置到文件
  UFUNCTION(BlueprintCallable, Category = "Das3DTilesSetting")
  void SaveSettings();

  // 从文件加载设置
  UFUNCTION(BlueprintCallable, Category = "Das3DTilesSetting")
  bool LoadSettings();

public:
  // 获取项目路径下的配置文件路径
  UFUNCTION(BlueprintCallable, Category = "Das3DTilesSetting")
  FString GetSettingsFilePath() const;

public:
  /** Gets the settings container name for the settings, either Project or
   * Editor */
  virtual FName GetContainerName() const override { return TEXT("Project"); }

  /** Gets the category for the settings, some high-level grouping like Editor,
   * Engine, Game, etc. */
  virtual FName GetCategoryName() const override { return TEXT("DasSettings"); }

  /** The unique name for your section of settings, uses the class's FName. */
  virtual FName GetSectionName() const override { return TEXT("3DTiles"); }

public:
  // 是否启用控制
  UPROPERTY(Config, EditAnywhere, Category = "Das3DTilesSetting")
  bool EnableControl = true;

  // 保持的游戏帧率
  UPROPERTY(Config, EditAnywhere, Category = "Das3DTilesSetting")
  int Keep3DTilesGameFPS = 60;

  // 是否显示配置UI
  UPROPERTY(Config, EditAnywhere, Category = "Das3DTilesSetting")
  bool ShowConfigUI = false;

  // 是否保持 LOD 0
  UPROPERTY(Config, EditAnywhere, Category = "Das3DTilesSetting")
  bool KeeepLod0 = true;

  // 是否自动计算最大加载数量
  UPROPERTY(Config, EditAnywhere, Category = "Das3DTilesSetting | MaxLoad")
  bool AutoMaxLoadNum = true;

  // 最大加载 Tiles 数量
  UPROPERTY(Config, EditAnywhere, Category = "Das3DTilesSetting | MaxLoad")
  float MaxLoadTilesScale = 1.0;

  // 每个 Tileset 的最大加载 Tiles 数量
  UPROPERTY(Config, EditAnywhere, Category = "Das3DTilesSetting")
  int MaxLoadTileNumPerTileset = 8;

  // 最大创建数量 
  UPROPERTY(Config, EditAnywhere, Category = "Das3DTilesSetting")
  int MaxCreateNum = 5;

  // 最大创建大小
  UPROPERTY(Config, EditAnywhere, Category = "Das3DTilesSetting")
  int64 MaxCreateSize = 20000000000;

  // 最大销毁数量
  UPROPERTY(Config, EditAnywhere, Category = "Das3DTilesSetting")
  int MaxDestroyNum = 5;

  // 最大销毁大小
  UPROPERTY(Config, EditAnywhere, Category = "Das3DTilesSetting")
  int64 MaxDestroySize = 20000000000;


  //tset
  UPROPERTY(Config, EditAnywhere, Category = "Das3DTilesSetting")
  int NumThreadManager = 1.0;
  UPROPERTY(Config, EditAnywhere, Category = "Das3DTilesSetting")
  int NumThreadPool = 1.0;
};
