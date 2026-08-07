#include "UnrealPrepareRendererResources.h"
#include "Cesium3DTileset.h"
#include "CesiumGltfComponent.h"
#include "CesiumLifetime.h"
#include "CesiumRasterOverlay.h"
#include "CesiumRuntime.h"
#include "CreateGltfOptions.h"
#include "ExtensionImageAssetUnreal.h"
#include <Cesium3DTilesSelection/Tile.h>
#include <Cesium3DTilesSelection/TileLoadResult.h>
#include <CesiumAsync/AsyncSystem.h>
#include <CesiumGeospatial/Ellipsoid.h>
#include <glm/mat4x4.hpp>

#pragma region Das
THIRD_PARTY_INCLUDES_START
#include "CesiumUtility/Uri.h"
THIRD_PARTY_INCLUDES_END
#include "DasCesium/GltfCreateProcessBase.h"
#pragma endregion


UnrealPrepareRendererResources::UnrealPrepareRendererResources(
    ACesium3DTileset* pActor)
    : _pActor(pActor) {}

CesiumAsync::Future<Cesium3DTilesSelection::TileLoadResultAndRenderResources>
UnrealPrepareRendererResources::prepareInLoadThread(
    const CesiumAsync::AsyncSystem& asyncSystem,
    Cesium3DTilesSelection::TileLoadResult&& tileLoadResult,
    const glm::dmat4& transform,
    const std::any& rendererOptions) {
  CreateGltfOptions::CreateModelOptions options(std::move(tileLoadResult));
  if (!options.pModel) {
    return asyncSystem.createResolvedFuture(
        Cesium3DTilesSelection::TileLoadResultAndRenderResources{
            std::move(options.tileLoadResult),
            nullptr});
  }

  options.alwaysIncludeTangents = this->_pActor->GetAlwaysIncludeTangents();
  options.createPhysicsMeshes = this->_pActor->GetCreatePhysicsMeshes();

  options.ignoreKhrMaterialsUnlit = this->_pActor->GetIgnoreKhrMaterialsUnlit();

#pragma region jiangs
  options.allowMeshCPUAccess = this->_pActor->GetAllowMeshCPUAccess();
#pragma endregion

  if (this->_pActor->_featuresMetadataDescription) {
    options.pFeaturesMetadataDescription =
        &(*this->_pActor->_featuresMetadataDescription);
  } else if (this->_pActor->_metadataDescription_DEPRECATED) {
    options.pEncodedMetadataDescription_DEPRECATED =
        &(*this->_pActor->_metadataDescription_DEPRECATED);
  }

  const CesiumGeospatial::Ellipsoid& ellipsoid = tileLoadResult.ellipsoid;

#pragma region Das
  CesiumGltf::Model& model = *options.pModel;
  std::string name = "glTF";
  const auto urlIt = model.extras.find("Cesium3DTiles_TileUrl");
  FString strTileID;
  if (urlIt != model.extras.end())
  {
    name = urlIt->second.getStringOrDefault("glTF");
    std::string nameCh = CesiumUtility::Uri::uriPathToNativePath(name);
    strTileID = UTF8_TO_TCHAR(nameCh.c_str());

#ifdef SHOW_3DTILES_LOAD_TIME
    FString strFileName = FPaths::GetCleanFilename(strTileID);  
    {
      std::lock_guard<std::mutex> lock(_pActor->mmutex);

      if (this->_pActor->mmapTile2TimeBeignLoad.Contains(strFileName)) {
        float fTimeSpan = (FDateTime::Now() -
                           this->_pActor->mmapTile2TimeBeignLoad[strFileName])
                .GetTotalMilliseconds();

        if (fTimeSpan > 10000)
        {
          int a = 0;
          a++;
        }

        UE_LOG(
            LogCesium,
            Log,
            TEXT("prepareInLoadThread: %s, time span %f"),
            *FString(strFileName),
            fTimeSpan);
        _pActor->mmapTile2TimeBeignLoad.Remove(strFileName);

      } else {
        int a = 0;
        a++;
      }
    }

		{
			std::lock_guard<std::mutex> lock(_pActor->mmutex);
			this->_pActor->mmapTile2LoadFinish.Add(strFileName, FDateTime::Now());
		}
#endif
	}

	FDateTime timeBefore = FDateTime::Now();

  //add Das
  GltfCreateProcessBase* pProcess = this->_pActor->CreateGltfCreateProcess(tileLoadResult);
  // 设置包围盒到GltfCreateProcessBase
  if (pProcess)
  {
    pProcess->SetBoundingVolume(tileLoadResult.tileBoundingVolume);
  }
#pragma endregion

	CesiumAsync::Future<UCesiumGltfComponent::CreateOffGameThreadResult>
		pHalfFuture = UCesiumGltfComponent::CreateOffGameThread(
			asyncSystem,
			transform,
			std::move(options),
#pragma region Das
			ellipsoid,
			pProcess
      );
#pragma endregion

#pragma region Das
	float fTimeSpan = (FDateTime::Now() - timeBefore).GetTotalMilliseconds();
	if (fTimeSpan > _pActor->mnMillSecondSlow)
	{
		_pActor->AddSlowTileThread(strTileID);
	}
#pragma endregion

  return MoveTemp(pHalfFuture)
      .thenImmediately(
          [](UCesiumGltfComponent::CreateOffGameThreadResult&& result)
              -> Cesium3DTilesSelection::TileLoadResultAndRenderResources {
            return Cesium3DTilesSelection::TileLoadResultAndRenderResources{
                std::move(result.TileLoadResult),
                result.HalfConstructed.Release()};
          });
}

void* UnrealPrepareRendererResources::prepareInMainThread(
    Cesium3DTilesSelection::Tile& tile,
    void* pLoadThreadResult) {
  Cesium3DTilesSelection::TileContent& content = tile.getContent();
  if (content.isRenderContent()) {
    TUniquePtr<UCesiumGltfComponent::HalfConstructed> pHalf(
        reinterpret_cast<UCesiumGltfComponent::HalfConstructed*>(
            pLoadThreadResult));
    Cesium3DTilesSelection::TileRenderContent& renderContent =
        *content.getRenderContent();
    return UCesiumGltfComponent::CreateOnGameThread(
        renderContent.getModel(),
        this->_pActor,
        std::move(pHalf),
        _pActor->GetCesiumTilesetToUnrealRelativeWorldTransform(),
        this->_pActor->GetMaterial(),
        this->_pActor->GetTranslucentMaterial(),
        this->_pActor->GetWaterMaterial(),
        this->_pActor->GetCustomDepthParameters(),
        tile,
        this->_pActor->GetCreateNavCollision());
  }
  // UE_LOG(LogCesium, VeryVerbose, TEXT("No content for tile"));
  return nullptr;
}

void UnrealPrepareRendererResources::free(
    Cesium3DTilesSelection::Tile& tile,
    void* pLoadThreadResult,
    void* pMainThreadResult) noexcept {
#pragma region Das
	TRACE_CPUPROFILER_EVENT_SCOPE(UnrealPrepareRendererResources::free);
#pragma endregion
  if (pLoadThreadResult) {
    UCesiumGltfComponent::HalfConstructed* pHalf =
        reinterpret_cast<UCesiumGltfComponent::HalfConstructed*>(
            pLoadThreadResult);
    delete pHalf;
  } else if (pMainThreadResult) {
    UCesiumGltfComponent* pGltf =
        reinterpret_cast<UCesiumGltfComponent*>(pMainThreadResult);

#pragma region Das
		//TileID在Cesium3Dtile中，msetSetSlows存在的话打印日志
		const std::string* pTileId = std::get_if<std::string>(&tile.getTileID());
		if (pTileId) {
			FString TileID = UTF8_TO_TCHAR(pTileId->c_str());

			// 检查 TileID 是否存在于 msetSetSlows
			if (_pActor->msetSetSlows.Contains(TileID)) {
				UE_LOG(
					LogCesium,
					Warning,
					TEXT("TileID %s exists in msetSetSlows."),
					*TileID);
			}
		}
#pragma endregion

    CesiumLifetime::destroyComponentRecursively(pGltf);
  }
}

void* UnrealPrepareRendererResources::prepareRasterInLoadThread(
    CesiumGltf::ImageAsset& image,
    const std::any& rendererOptions) {
  auto ppOptions =
      std::any_cast<FRasterOverlayRendererOptions*>(&rendererOptions);
  check(ppOptions != nullptr && *ppOptions != nullptr);
  if (ppOptions == nullptr || *ppOptions == nullptr) {
    return nullptr;
  }

  auto pOptions = *ppOptions;

  if (pOptions->useMipmaps) {
    std::optional<std::string> errorMessage =
        CesiumGltfReader::ImageDecoder::generateMipMaps(image);
    if (errorMessage) {
      UE_LOG(
          LogCesium,
          Warning,
          TEXT("%s"),
          UTF8_TO_TCHAR(errorMessage->c_str()));
    }
  }

  // TODO: sRGB should probably be configurable on the raster overlay.
  bool sRGB = true;

  const ExtensionImageAssetUnreal& extension =
      ExtensionImageAssetUnreal::getOrCreate(
          CesiumAsync::AsyncSystem(nullptr), // TODO
          image,
          sRGB,
          pOptions->useMipmaps,
          std::nullopt);

  // Because raster overlay images are never shared (at least currently!), the
  // future should already be resolved by the time we get here.
  check(extension.getFuture().isReady());

  auto texture = CesiumTextureUtility::loadTextureAnyThreadPart(
      image,
      TextureAddress::TA_Clamp,
      TextureAddress::TA_Clamp,
      pOptions->filter,
      pOptions->useMipmaps,
      pOptions->group,
      sRGB,
      std::nullopt);

  return texture.Release();
}

void* UnrealPrepareRendererResources::prepareRasterInMainThread(
    CesiumRasterOverlays::RasterOverlayTile& rasterTile,
    void* pLoadThreadResult) {
  TUniquePtr<CesiumTextureUtility::LoadedTextureResult> pLoadedTexture{
      static_cast<CesiumTextureUtility::LoadedTextureResult*>(
          pLoadThreadResult)};

  if (!pLoadedTexture) {
    return nullptr;
  }

  CesiumUtility::IntrusivePointer<
      CesiumTextureUtility::ReferenceCountedUnrealTexture>
      pTexture =
          CesiumTextureUtility::loadTextureGameThreadPart(pLoadedTexture.Get());
  if (!pTexture) {
    return nullptr;
  }

  // Don't let this ReferenceCountedUnrealTexture be destroyed when the
  // intrusive pointer goes out of scope.
  pTexture->addReference();
  return pTexture.get();
}

void UnrealPrepareRendererResources::freeRaster(
    const CesiumRasterOverlays::RasterOverlayTile& rasterTile,
    void* pLoadThreadResult,
    void* pMainThreadResult) noexcept {
  if (pLoadThreadResult) {
    CesiumTextureUtility::LoadedTextureResult* pLoadedTexture =
        static_cast<CesiumTextureUtility::LoadedTextureResult*>(
            pLoadThreadResult);
    delete pLoadedTexture;
  }

  if (pMainThreadResult) {
    CesiumTextureUtility::ReferenceCountedUnrealTexture* pTexture =
        static_cast<CesiumTextureUtility::ReferenceCountedUnrealTexture*>(
            pMainThreadResult);
    pTexture->releaseReference();
  }
}

void UnrealPrepareRendererResources::attachRasterInMainThread(
    const Cesium3DTilesSelection::Tile& tile,
    int32_t overlayTextureCoordinateID,
    const CesiumRasterOverlays::RasterOverlayTile& rasterTile,
    void* pMainThreadRendererResources,
    const glm::dvec2& translation,
    const glm::dvec2& scale) {
  const Cesium3DTilesSelection::TileContent& content = tile.getContent();
  const Cesium3DTilesSelection::TileRenderContent* pRenderContent =
      content.getRenderContent();
  if (pMainThreadRendererResources != nullptr && pRenderContent != nullptr) {
    UCesiumGltfComponent* pGltfContent =
        reinterpret_cast<UCesiumGltfComponent*>(
            pRenderContent->getRenderResources());
    if (pGltfContent) {
      pGltfContent->AttachRasterTile(
          tile,
          rasterTile,
          static_cast<CesiumTextureUtility::ReferenceCountedUnrealTexture*>(
              pMainThreadRendererResources)
              ->getUnrealTexture(),
          translation,
          scale,
          overlayTextureCoordinateID);
    }
  }
}

void UnrealPrepareRendererResources::detachRasterInMainThread(
    const Cesium3DTilesSelection::Tile& tile,
    int32_t overlayTextureCoordinateID,
    const CesiumRasterOverlays::RasterOverlayTile& rasterTile,
    void* pMainThreadRendererResources) noexcept {
  const Cesium3DTilesSelection::TileContent& content = tile.getContent();
  const Cesium3DTilesSelection::TileRenderContent* pRenderContent =
      content.getRenderContent();
  if (pRenderContent) {
    UCesiumGltfComponent* pGltfContent =
        reinterpret_cast<UCesiumGltfComponent*>(
            pRenderContent->getRenderResources());
    if (pMainThreadRendererResources != nullptr && pGltfContent != nullptr) {
      pGltfContent->DetachRasterTile(
          tile,
          rasterTile,
          static_cast<CesiumTextureUtility::ReferenceCountedUnrealTexture*>(
              pMainThreadRendererResources)
              ->getUnrealTexture());
    }
  }
}
