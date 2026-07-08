// Copyright 2020-2024 CesiumGS, Inc. and Contributors

#pragma once

#include "CesiumAsync/ITaskProcessor.h"
#include "HAL/Platform.h"

#pragma region Das
namespace CesiumAsync {
	class AsyncSystem;
	class ThreadPool;
}
#pragma endregion


class CESIUMRUNTIME_API UnrealTaskProcessor
    : public CesiumAsync::ITaskProcessor {

public:
	UnrealTaskProcessor();

public:
  virtual void startTask(std::function<void()> f) override;

#pragma region Das
  TSharedPtr<CesiumAsync::AsyncSystem> mpAsync = nullptr;
  TSharedPtr<CesiumAsync::ThreadPool> mpThreadPool = nullptr;
#pragma endregion

};
