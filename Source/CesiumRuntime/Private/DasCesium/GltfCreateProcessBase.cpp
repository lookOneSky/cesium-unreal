// Copyright DAS Team. All Rights Reserved.

#include "DasCesium/GltfCreateProcessBase.h"
#include "LoadGltfResult.h"
#include "CreateGltfOptions.h"
#include "CesiumGltf/AccessorView.h"
#include "CesiumGltf/Model.h"
#include "Cesium3DTilesSelection/Tile.h"
#include "CesiumGltfComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SceneComponent.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshResources.h"

GltfCreateProcessBase::GltfCreateProcessBase()
{
}

GltfCreateProcessBase::~GltfCreateProcessBase()
{
}

void GltfCreateProcessBase::LoadModelOffGameThread(
	LoadGltfResult::LoadedModelResult& result,
	const glm::dmat4x4& transform,
	const CreateGltfOptions::CreateModelOptions& options)
{
	// 默认实现为空，由派生类重写
}

void GltfCreateProcessBase::loadNode(
	std::vector<LoadGltfResult::LoadedNodeResult>& loadNodeResults,
	const glm::dmat4x4& transform,
	const glm::dmat4x4& nodeTransformForinstance,
	const CreateGltfOptions::CreateNodeOptions& options)
{
	// 默认实现为空，由派生类重写
}

bool GltfCreateProcessBase::customPrimitiveVertexOffGameThread(
	TArray<FStaticMeshBuildVertex>& StaticMeshBuildVertices,
	std::unordered_map<int32_t, uint32_t>& gltfToUnrealTexCoordMap,
	TArray<uint32>& indices,
	LoadGltfResult::LoadedPrimitiveResult& primitiveResult,
	const CreateGltfOptions::CreatePrimitiveOptions& options,
	bool duplicateVertices)
{
	// 默认不自定义，返回false
	return false;
}

void GltfCreateProcessBase::UpdatePrimitiveVertexOffGameThread(
	LoadGltfResult::LoadedPrimitiveResult& primitiveResult,
	std::optional<int>& nNumUVNew,
	const glm::dmat4x4& transform,
	const CreateGltfOptions::CreatePrimitiveOptions& options,
	TArray<FStaticMeshBuildVertex>& StaticMeshBuildVertices,
	TArray<uint32>& indices,
	CesiumGltf::AccessorView<FVector3f> normalAccessor,
	bool duplicateVertices)
{
	// 默认实现为空，由派生类重写
}

void GltfCreateProcessBase::LoadExPrimitiveFromRenderData(
	LoadGltfResult::LoadedPrimitiveResult& primitiveResult,
	const glm::dmat4x4& transform,
	const CreateGltfOptions::CreatePrimitiveOptions& options,
	TArray<FStaticMeshBuildVertex>& StaticMeshBuildVertices,
	TArray<uint32>& indices,
	CesiumGltf::AccessorView<FVector3f> normalAccessor,
	bool duplicateVertices)
{
	// 默认实现为空，由派生类重写
}

UCesiumGltfComponent* GltfCreateProcessBase::CreateGltfOnGameThread()
{
	// 默认返回nullptr，由派生类重写
	return nullptr;
}

USceneComponent* GltfCreateProcessBase::CreateRenderOnGameThread(
	const CesiumGltf::Model& model,
	UCesiumGltfComponent* pGltf,
	LoadGltfResult::LoadedPrimitiveResult& loadResult,
	const glm::dmat4x4& cesiumToUnrealTransform,
	const Cesium3DTilesSelection::Tile& tile,
	ACesium3DTileset* pTilesetActor,
	const FName& meshName)
{
	// 默认返回nullptr，由派生类重写
	return nullptr;
}

void GltfCreateProcessBase::AttachToRenderOnGameThread(
	LoadGltfResult::LoadedPrimitiveResult& result,
	USceneComponent* pComponent,
	const Cesium3DTilesSelection::Tile& tile,
	UStaticMesh* pStaticMesh,
	const glm::dmat4x4& cesiumToUnrealTransform)
{
	// 默认实现为空，由派生类重写
}

bool GltfCreateProcessBase::IsOnQuickFinish()
{
	// 默认不启用快速完成模式
	return false;
}

void GltfCreateProcessBase::SetMaterialParameter(
	UMaterialInstanceDynamic* pMaterial,
	UCesiumMaterialUserData* pCesiumData,
	LoadGltfResult::LoadedPrimitiveResult& loadResult,
	const glm::dmat4x4& cesiumToUnrealTransform)
{
	// 默认实现为空，由派生类重写
}

UMaterialInterface* GltfCreateProcessBase::SetBaseMaterial(
	UMaterialInterface* pBaseMaterial,
	int32_t meshMode)
{
	// 默认直接返回传入的材质
	return pBaseMaterial;
}

void GltfCreateProcessBase::SetBoundingVolume(const std::optional<Cesium3DTilesSelection::BoundingVolume>& boundingVolume)
{
	mBoundingVolume = boundingVolume;
}

const std::optional<Cesium3DTilesSelection::BoundingVolume>& GltfCreateProcessBase::GetBoundingVolume() const
{
	return mBoundingVolume;
}
