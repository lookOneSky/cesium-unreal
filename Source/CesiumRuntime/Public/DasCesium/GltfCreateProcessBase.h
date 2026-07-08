// Copyright DAS Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Cesium3DTilesSelection/BoundingVolume.h>
#include <glm/mat4x4.hpp>
#include <optional>
#include <unordered_map>
#include <vector>

// UE前置声明
class UCesiumGltfComponent;
class ACesium3DTileset;
class UCesiumMaterialUserData;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class USceneComponent;
class UStaticMesh;
struct FStaticMeshBuildVertex;

namespace Cesium3DTilesSelection {
	class Tile;
}

namespace LoadGltfResult {
	struct LoadedModelResult;
	struct LoadedNodeResult;
	struct LoadedPrimitiveResult;
}

namespace CreateGltfOptions {
	struct CreateModelOptions;
	struct CreateNodeOptions;
	struct CreatePrimitiveOptions;
}

/**
 * Gltf模型创建过程的基类
 * 扩展UCesiumGltfComponent的解析和创建过程，提供自定义接口
 * 每个Cesium3DTileset对应一个实例，每次加载时创建
 */
class CESIUMRUNTIME_API GltfCreateProcessBase
{
public:
	GltfCreateProcessBase();
	virtual ~GltfCreateProcessBase();

	// ================================
	// 离线程接口 (Off Game Thread)
	// ================================

	/**
	 * 在离线程加载模型
	 * 对应GltfComponent的LoadModelAnyThread流程
	 * @param result 加载结果输出
	 * @param transform 变换矩阵
	 * @param options 创建选项
	 */
	virtual void LoadModelOffGameThread(
		LoadGltfResult::LoadedModelResult& result,
		const glm::dmat4x4& transform,
		const CreateGltfOptions::CreateModelOptions& options);

	/**
	 * 加载节点
	 * @param LoadedNodeResults 节点加载结果列表
	 * @param transform 变换矩阵
	 * @param nodeTransformForinstance 实例节点变换矩阵
	 * @param options 创建选项
	 */
	virtual void loadNode(
		std::vector<LoadGltfResult::LoadedNodeResult>& LoadedNodeResults,
		const glm::dmat4x4& transform,
		const glm::dmat4x4& nodeTransformForinstance,
		const CreateGltfOptions::CreateNodeOptions& options);

	/**
	 * 自定义图元顶点数据，不再创建默认UV
	 * @param StaticMeshBuildVertices 静态网格顶点数组
	 * @param gltfToUnrealTexCoordMap Gltf到UE的纹理坐标映射
	 * @param indices 索引数组
	 * @param primitiveResult 图元加载结果
	 * @param options 创建选项
	 * @param duplicateVertices 是否复制顶点
	 * @return 是否自定义成功
	 */
	virtual bool customPrimitiveVertexOffGameThread(
		TArray<FStaticMeshBuildVertex>& StaticMeshBuildVertices,
		std::unordered_map<int32_t, uint32_t>& gltfToUnrealTexCoordMap,
		TArray<uint32>& indices,
		LoadGltfResult::LoadedPrimitiveResult& primitiveResult,
		const CreateGltfOptions::CreatePrimitiveOptions& options,
		bool duplicateVertices);

	/**
	 * RenderData创建之前的接口，用于补充顶点、法线、UV等数据
	 * @param primitiveResult 图元加载结果
	 * @param nNumUVNew 新UV数量
	 * @param transform 变换矩阵
	 * @param options 创建选项
	 * @param StaticMeshBuildVertices 静态网格顶点数组
	 * @param indices 索引数组
	 * @param normalAccessor 法线访问器
	 * @param duplicateVertices 是否复制顶点
	 */
	virtual void UpdatePrimitiveVertexOffGameThread(
		LoadGltfResult::LoadedPrimitiveResult& primitiveResult,
		std::optional<int>& nNumUVNew,
		const glm::dmat4x4& transform,
		const CreateGltfOptions::CreatePrimitiveOptions& options,
		TArray<FStaticMeshBuildVertex>& StaticMeshBuildVertices,
		TArray<uint32>& indices,
		CesiumGltf::AccessorView<FVector3f> normalAccessor,
		bool duplicateVertices);

	/**
	 * 从RenderData加载图元顶点数据的接口
	 * @param primitiveResult 图元加载结果
	 * @param transform 变换矩阵
	 * @param options 创建选项
	 * @param StaticMeshBuildVertices 静态网格顶点数组
	 * @param indices 索引数组
	 * @param normalAccessor 法线访问器
	 * @param duplicateVertices 是否复制顶点
	 */
	virtual void LoadExPrimitiveFromRenderData(
		LoadGltfResult::LoadedPrimitiveResult& primitiveResult,
		const glm::dmat4x4& transform,
		const CreateGltfOptions::CreatePrimitiveOptions& options,
		TArray<FStaticMeshBuildVertex>& StaticMeshBuildVertices,
		TArray<uint32>& indices,
		CesiumGltf::AccessorView<FVector3f> normalAccessor,
		bool duplicateVertices);

	// ================================
	// 游戏线程接口 (On Game Thread)
	// ================================

	/**
	 * 在游戏线程创建Gltf组件
	 * @return 创建的Gltf组件
	 */
	virtual UCesiumGltfComponent* CreateGltfOnGameThread();

	/**
	 * 在游戏线程创建渲染组件
	 * @param model Gltf模型
	 * @param pGltf Gltf组件
	 * @param loadResult 加载结果
	 * @param cesiumToUnrealTransform Cesium到UE的变换矩阵
	 * @param tile Tile数据
	 * @param pTilesetActor Tileset Actor
	 * @param meshName 网格名称
	 * @return 创建的场景组件
	 */
	virtual USceneComponent* CreateRenderOnGameThread(
		const CesiumGltf::Model& model,
		UCesiumGltfComponent* pGltf,
		LoadGltfResult::LoadedPrimitiveResult& loadResult,
		const glm::dmat4x4& cesiumToUnrealTransform,
		const Cesium3DTilesSelection::Tile& tile,
		ACesium3DTileset* pTilesetActor,
		const FName& meshName);

	/**
	 * 设置每个PrimitiveComponent使用的基础材质
	 * @param pBaseMaterial 基础材质
	 * @param meshMode 网格模式
	 * @return 设置后的材质接口
	 */
	virtual UMaterialInterface* SetBaseMaterial(
		UMaterialInterface* pBaseMaterial,
		int32_t meshMode);

	/**
	 * 设置材质参数
	 * @param pMaterial 动态材质实例
	 * @param pCesiumData Cesium材质用户数据
	 * @param loadResult 加载结果
	 * @param cesiumToUnrealTransform Cesium到UE的变换矩阵
	 */
	virtual void SetMaterialParameter(
		UMaterialInstanceDynamic* pMaterial,
		UCesiumMaterialUserData* pCesiumData,
		LoadGltfResult::LoadedPrimitiveResult& loadResult,
		const glm::dmat4x4& cesiumToUnrealTransform);

	/**
	 * 附加到渲染组件
	 * 用途：
	 * 1. 在已有的渲染对象上添加新的绘制对象，例如覆冰
	 * 2. 增加一些自定义渲染属性，例如EVStencil
	 * @param result 图元加载结果
	 * @param pParentComponent 父组件
	 * @param tile Tile数据
	 * @param pStaticMesh 静态网格
	 * @param cesiumToUnrealTransform Cesium到UE的变换矩阵
	 */
	virtual void AttachToRenderOnGameThread(
		LoadGltfResult::LoadedPrimitiveResult& result,
		USceneComponent* pParentComponent,
		const Cesium3DTilesSelection::Tile& tile,
		UStaticMesh* pStaticMesh,
		const glm::dmat4x4& cesiumToUnrealTransform);

	/**
	 * 是否启用快速完成模式
	 * 用于优化3DTiles移除的效率
	 * @return 是否快速完成
	 */
	virtual bool IsOnQuickFinish();

	/**
	 * 设置包围盒
	 * @param boundingVolume 包围盒
	 */
	virtual void SetBoundingVolume(const std::optional<Cesium3DTilesSelection::BoundingVolume>& boundingVolume);

	/**
	 * 获取包围盒
	 * @return 包围盒
	 */
	const std::optional<Cesium3DTilesSelection::BoundingVolume>& GetBoundingVolume() const;

	friend class ACesium3DTileset;

private:
	std::optional<Cesium3DTilesSelection::BoundingVolume> mBoundingVolume; // Tile的包围盒
};
