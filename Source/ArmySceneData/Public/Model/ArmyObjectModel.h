#pragma once

#include "ArmyTypes.h"
#include "ArmyObject.h"
#include "ArmyEngineModule.h"
/**
 * 基础业务层逻辑接口
 */
class ARMYSCENEDATA_API XRObjectModel
{
public:
    XRObjectModel() {}
    ~XRObjectModel() {}

    /** 添加 */
    void Add(FObjectPtr Object);

    /** 删除 */
    void Delete(FObjectPtr Object);

    /** 清空 */
    void Empty();

	/** 选择 */
	FObjectPtr Select(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

	/** 高亮 */
	FObjectPtr Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

    /** 获得所有的物体数据 */
    const TArray<FObjectPtr>& GetAllObjects() const;
protected:
    TArray<FObjectPtr> Objects;
};
