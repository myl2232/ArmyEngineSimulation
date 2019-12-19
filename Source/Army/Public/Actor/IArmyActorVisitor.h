/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File IArmyActorVisitor.h
 * @Description Actor访问管理器统一接口
 *
 * @Author 欧石楠
 * @Date 2018年12月7日
 * @Version 1.0
 */

#pragma once

class IArmyActorVisitor
{
public:
    virtual ~IArmyActorVisitor() {}

    /** 根据Actor的Tag去匹配需要查询的Actor */
    virtual bool IsActorTagsMatched(class AActor* Actor) = 0;

    TArray<AActor*>& Get() { return Actors; }

    /** Actor显示/隐藏 */
    virtual void Show(bool bShow);

    /** Actor锁定/解锁 */
    void Lock(bool bLock);

    /** Actor清空 */
    void Clear();

    /** 重新获取Actor数组 */
    void Refresh();

protected:
    /** 是否已经获取了Actors */
    bool IsAlreadyGetActors() { return Actors.Num() > 0; }

protected:
    TArray<AActor*> Actors;

    TArray<FName> Tags;
};