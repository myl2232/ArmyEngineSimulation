#pragma once

#include "ArmyCameraPhotography.h"

DECLARE_DELEGATE_OneParam(StartData, bool);

class FArmyCameraParamModule : public ICameraPhotographyModule
{
public:
	/**
	* Singleton-like access to this module's interface.  This is just for convenience!
	* Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	*
	* @return Returns singleton instance, loading the module on demand if needed
	*/
	static inline FArmyCameraParamModule& Get()
	{
		return FModuleManager::LoadModuleChecked< FArmyCameraParamModule >("ArmyCameraParam");
	}

	/**
	* Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	*
	* @return True if the module is loaded and ready to use
	*/
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("ArmyCameraParam");
	}

	ARMYCAMERAPARAM_API void StartCamera(bool BShutDown);

	ARMYCAMERAPARAM_API void SetCameraLocationAndRotation(FVector Location,FRotator Rotation);

	//true获取数据，false结束
	void GetScreenData(bool BEnd);
	StartData StartDataDelegate;

private:
	/**
	* Attempts to create a new photography interface
	*
	* @return Interface to the photography implementation, if we were able to successfully create one
	*/
	virtual TSharedPtr< class ICameraPhotography > CreateCameraPhotography() override;
};