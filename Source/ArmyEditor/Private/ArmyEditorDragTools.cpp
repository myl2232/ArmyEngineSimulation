// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ArmyEditorDragTools.h"
//#include "SnappingUtils.h"
#include "ArmyEditorViewportClient.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// FDragTool
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FArmyDragTool::FArmyDragTool(FArmyEditorModeTools* InModeTools)
	: bConvertDelta( true )
	, ModeTools( InModeTools )
	, Start( FVector::ZeroVector )
	, End( FVector::ZeroVector )
	, EndWk( FVector::ZeroVector )
	, bUseSnapping( false )
	, bIsDragging( false )
{
}

void FArmyDragTool::AddDelta( const FVector& InDelta )
{
	EndWk += InDelta;

	End = EndWk;

	// Snap to constraints.
// 	if( bUseSnapping )
// 	{
// 		const float GridSize = GEditor->GetGridSize();
// 		const FVector GridBase( GridSize, GridSize, GridSize );
// 		FSnappingUtils::SnapPointToGrid( End, GridBase );
// 	}
}

void FArmyDragTool::StartDrag(UArmyEditorViewportClient* InViewportClient, const FVector& InStart, const FVector2D& StartScreen)
{
	Start = InStart;
	bIsDragging = true;

	// Snap to constraints.
// 	if( bUseSnapping )
// 	{
// 		const float GridSize = GEditor->GetGridSize();
// 		const FVector GridBase( GridSize, GridSize, GridSize );
// 		FSnappingUtils::SnapPointToGrid( Start, GridBase );
// 	}
	End = EndWk = Start;

	// Store button state when the drag began.
	bAltDown = InViewportClient->IsAltPressed();
	bShiftDown = InViewportClient->IsShiftPressed();
	bControlDown = InViewportClient->IsCtrlPressed();
	bLeftMouseButtonDown = InViewportClient->Viewport->KeyState(EKeys::LeftMouseButton);
	bRightMouseButtonDown = InViewportClient->Viewport->KeyState(EKeys::RightMouseButton);
	bMiddleMouseButtonDown = InViewportClient->Viewport->KeyState(EKeys::MiddleMouseButton);
}

void FArmyDragTool::EndDrag()
{
	Start = End = EndWk = FVector::ZeroVector;
	bIsDragging = false;
}
