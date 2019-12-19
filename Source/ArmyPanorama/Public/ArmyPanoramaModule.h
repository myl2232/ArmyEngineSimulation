/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @Module XRPanoramaModule
* @Description 用于截取房间六个方向的六张图
*
* @Author 梁晓菲
* @Date 2018年9月5日
* @Version 1.0
*/
#pragma once

#include "ModuleInterface.h"
#include "ArmyPanoramaMgr.h"
#include "ArmyPanoramaController.h"
#include "Modules/ModuleInterface.h"

class FArmyPanoramaModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	XRPANORAMA_API static FArmyPanoramaModule& Get();
	XRPANORAMA_API inline TSharedPtr<FArmyPanoramaController> GetPanoramaController();

private:
	TSharedPtr<FArmyPanoramaController> PanoramaController;
	static FArmyPanoramaModule* Singleton;
};