#include "Das3DTilesSetting.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "JsonObjectConverter.h"


void UDas3DTilesSetting::SaveSettings()
{
  FString strPath = GetSettingsFilePath();
 #if 0
    //SaveConfig打包之后无法再修复
  this->SaveConfig(CPF_Config, *strPath);
 #else
  // 创建一个 JSON 对象
  TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

  // 将当前对象的属性序列化到 JSON 对象中
  FJsonObjectConverter::UStructToJsonObject(
      this->GetClass(),
      this,
      JsonObject.ToSharedRef(),
      0,
      0);

  // 将 JSON 对象转换为字符串
  FString OutputString;
  TSharedRef<TJsonWriter<>> Writer =
      TJsonWriterFactory<>::Create(&OutputString);
  FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

  // 将字符串写入文件
  FFileHelper::SaveStringToFile(OutputString, *strPath);
 #endif
}

bool UDas3DTilesSetting::LoadSettings()
{
  FString strPath = GetSettingsFilePath();
  //文件是否存在
  bool bFile = FPaths::FileExists(strPath);
  if (!bFile)
  {
    return false;
  }

#if 0
  this->LoadConfig(UDas3DTilesSetting::StaticClass(), *strPath);
#else
  // 从文件中读取 JSON 字符串
  FString JsonString;
  if (!FFileHelper::LoadFileToString(JsonString, *strPath)) {
    return false;
  }

  // 将 JSON 字符串解析为 JSON 对象
  TSharedPtr<FJsonObject> JsonObject;
  TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
  if (!FJsonSerializer::Deserialize(Reader, JsonObject) ||
      !JsonObject.IsValid()) {
    return false;
  }

  // 将 JSON 对象反序列化到当前对象
  FJsonObjectConverter::JsonObjectToUStruct(
      JsonObject.ToSharedRef(),
      this->GetClass(),
      this,
      0,
      0);
#endif

  return true;
}

FString UDas3DTilesSetting::GetSettingsFilePath() const {
  FString strPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectConfigDir()) /
      TEXT("DasCustomSettings.json");
  return strPath;
}
