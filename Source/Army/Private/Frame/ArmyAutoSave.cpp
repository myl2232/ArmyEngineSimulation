#include "ArmyAutoSave.h"
#include "ArmyGameInstance.h"

/** @欧石楠 每隔10分钟提示一次 */
const static int32 DeltaTime = 10;

void FArmyAutoSave::StartTimer()
{
    GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateRaw(this, &FArmyAutoSave::RemindSave), DeltaTime * 60.f, true);
}

void FArmyAutoSave::StopTimer()
{
    GetTimerManager().ClearTimer(TimerHandle);
}

void FArmyAutoSave::RemindSave()
{
    ++TimeCounts;
    GGI->Window->ShowMessage(EXRMessageType::MT_Normal, GetRemindsMessage());
}

FString FArmyAutoSave::GetRemindsMessage()
{
    FString TimeString;

    int32 Time = TimeCounts * DeltaTime;
    if (Time > 0 && Time < 60)
    {
        TimeString = FString::Printf(TEXT("%d分钟"), Time);
    }
    else if (Time >= 60 && Time < 24 * 60)
    {
        TimeString = FString::Printf(TEXT("%d小时"), Time / 60);
    }
    else if (Time >= 24 * 60)
    {
        TimeString = FString::Printf(TEXT("%d天"), Time / 24 * 60);
    }

    return FString::Printf(TEXT("您已经超过%s没有保存啦，保存一下，继续加油！"), *TimeString);
}

class FTimerManager& FArmyAutoSave::GetTimerManager() const
{
    return GGI->GetWorld()->GetTimerManager();
}
