/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XR3DManager.h
 * @Description 3D场景中的物体管理器
 *
 * @Author 欧石楠
 * @Date 2018年10月24日
 * @Version 1.0
 */

#pragma once

#include "CoreMinimal.h"
#include "Army3DManager.generated.h"

typedef TSharedPtr<class FArmyObject> FObjectPtr;
UCLASS()
class UXR3DManager : public UObject
{
	GENERATED_BODY()

public:
	enum EXRActorType
	{
		AT_None,
		AT_CEILING
	};

    UXR3DManager();

	UFUNCTION()
	void OnOwnerDestroyed(AActor* DestroyedActor);

	struct HAdsorbDetect CaptureWallInfo(FVector MousePoint);
	struct HAdsorbDetect CaptureWallInfoRelace(FVector MousePoint,float releace = 0.1f);

	void OnXRBrushVisibleChange(AActor* InActor);

	void SetActorVisibleByType(EXRActorType InType, bool InVisible);


	void BindObjectAndActor(FObjectPtr ObjectPtr,class AActor* InActor);

	//创建平面视图
	bool CreateSurfaceComponent(AActor* InActor,float attibute);

	// 关联Actor和XRObject
	bool AssociatedObjectAndActor(FObjectPtr ObjectPtr,AActor* InActor,FString Tag);

	// 关联3d水管模型
	bool AssociatedObjectAndPipeLine(FObjectPtr ObjectPtr);

	/**判断点是否在房间内*/
	bool IsPointInRoom(FVector& InPoint);

private:
	/** Called by the engine when an actor is added to the world. */
	void OnLevelActorsAdded(AActor* InActor);

	UFUNCTION()
	void OnActorDestroyed(AActor* InActor);

private:
	TMultiMap<EXRActorType, AActor*> TypeActorMap;
};

extern XR_API UXR3DManager* G3DM;