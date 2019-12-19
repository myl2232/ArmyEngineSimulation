#include "ArmyViewController.h"

FArmyViewController::~FArmyViewController()
{
}

void FArmyViewController::Init()
{
    TitleBarWidget = MakeCustomTitleBarWidget();
    ContentWidget = MakeContentWidget();
	AlreadyInited = true;
}
bool FArmyViewController::Inited()
{
	return AlreadyInited;
}