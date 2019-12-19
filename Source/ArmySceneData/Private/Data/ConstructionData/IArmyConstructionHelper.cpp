#include "IArmyConstructionHelper.h"
#include "ArmyRoom.h"
int32 IArmyConstructionHelper::GetRoomSpaceID()
{
	auto Room = GetRoom();

	if (Room.IsValid())
	{
		return Room->GetSpaceId();
	}
	return -1;
}
