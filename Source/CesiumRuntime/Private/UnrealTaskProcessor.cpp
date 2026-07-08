// Copyright 2020-2024 CesiumGS, Inc. and Contributors

#include "UnrealTaskProcessor.h"
#include "Async/Async.h"
#include "Misc/QueuedThreadPool.h"

#pragma region Das
#include "CesiumAsync/ThreadPool.h"
#include "CesiumRuntime.h"
#include "CesiumAsync/AsyncSystem.h"
#include "Custom/Das3DTilesSetting.h"

#if 0
class CESIUMRUNTIME_API UnrealTaskProcessorReal
	: public CesiumAsync::ITaskProcessor {

public:
	UnrealTaskProcessorReal() {}

public:
	virtual void startTask(std::function<void()> f)
	{
		AsyncTask(ENamedThreads::Type::AnyBackgroundThreadNormalTask, [f, this]() {
			TRACE_CPUPROFILER_EVENT_SCOPE(Cesium::AsyncTask)
				f();
			});
	}

#pragma region Das
	TSharedPtr<CesiumAsync::AsyncSystem> mpAsync = nullptr;
#pragma endregion

};

UnrealTaskProcessor::UnrealTaskProcessor()
{
	mpAsync = TSharedPtr<CesiumAsync::AsyncSystem>(new
		CesiumAsync::AsyncSystem(std::make_shared<UnrealTaskProcessorReal>()));
	mpThreadPool = TSharedPtr<CesiumAsync::ThreadPool>(new CesiumAsync::ThreadPool(1));
}

void UnrealTaskProcessor::startTask(std::function<void()> f)
{
	mpAsync->runInThreadPool(*mpThreadPool, f);
}
#else
UnrealTaskProcessor::UnrealTaskProcessor()
{
#pragma region Das
	int nCore = FGenericPlatformMisc::NumberOfCoresIncludingHyperthreads();
  UDas3DTilesSetting* pSetting = GetMutableDefault<UDas3DTilesSetting>();
  double dScale = 1.0;
  int nThread = nCore;
  if (pSetting->LoadSettings())
  {
    dScale = pSetting->MaxLoadTilesScale;
    nThread *= dScale;
    pSetting->SaveSettings();
  }

  pSetting->NumThreadPool = nThread;
  mpThreadPool = TSharedPtr<CesiumAsync::ThreadPool>(new CesiumAsync::ThreadPool(nThread));
#pragma endregion
}

void UnrealTaskProcessor::startTask(std::function<void()> f) {
#pragma region Das
  getAsyncSystem().runInThreadPool(*mpThreadPool, [f]() {
			FPlatformProcess::SetThreadPriority(TPri_Lowest);
			f();
		});
	//AsyncTask(ENamedThreads::Type::AnyBackgroundThreadNormalTask, [f]() {
	//	TRACE_CPUPROFILER_EVENT_SCOPE(Cesium::AsyncTask)
	//		f();
	//	});
#pragma endregion
}
#endif

#pragma endregion



