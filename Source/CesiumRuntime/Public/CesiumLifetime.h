// Copyright 2020-2024 CesiumGS, Inc. and Contributors

#pragma once
#include "Components/SceneComponent.h"
#include "Containers/Array.h"
#include "Tickable.h"
#include "UObject/WeakObjectPtrTemplates.h"

class UObject;
class UTexture;

class AmortizedDestructor : FTickableGameObject {
public:
  void Tick(float DeltaTime) override;
  ETickableTickType GetTickableTickType() const override;
  bool IsTickableWhenPaused() const override;
  bool IsTickableInEditor() const override;
  TStatId GetStatId() const;
  void destroy(UObject* pObject);

#pragma region Das
  void addToPending(UObject* pObject);
  void ProcessLimitTime(float fRemainTime);
public:
	bool mbForbitDefualtGC = false;
#pragma endregion

private:
  bool runDestruction(UObject* pObject) const;
  void processPending();
  void finalizeDestroy(UObject* pObject) const;

  TArray<TWeakObjectPtr<UObject>> _pending;
  TArray<TWeakObjectPtr<UObject>> _nextPending;
};

class CESIUMRUNTIME_API CesiumLifetime {
public:
  static void destroy(UObject* pObject);
  static void destroyComponentRecursively(USceneComponent* pComponent);
#pragma region Das
  static void ForbitDefualtGC(bool bForbit);
  static void TickLimitTime(float fRemainTime);
#pragma endregion

private:
  static AmortizedDestructor amortizedDestructor;
};
