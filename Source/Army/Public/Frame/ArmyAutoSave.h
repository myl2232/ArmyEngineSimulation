/**
 * Copyright 2019 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRAutoSave.h
 * @Description 自动保存，目前只有保存提示
 *
 * @Author 欧石楠
 * @Date 2019年4月8日
 * @Version 1.0
 */

#pragma once

#include "TimerManager.h"

class FArmyAutoSave
{
public:
    /** @欧石楠 重置自动保存提示的计时 */
    void StartTimer();

    /** @欧石楠 停止自动保存提示的计时 */
    void StopTimer();

private:
    /** @欧石楠 显示保存提示 */
    void RemindSave();

    /** @欧石楠 构建提示内容 */
    FString GetRemindsMessage();

    FTimerManager& GetTimerManager() const;

private:
    FTimerHandle TimerHandle;

    int32 TimeCounts = 0;
};
