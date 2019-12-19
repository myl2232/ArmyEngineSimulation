#pragma once

#include "CoreMinimal.h"
#include "Engine/GameEngine.h"
#include "ArmyTypes.h"
#include "ArmyEditorEngine.generated.h"

class AArmyGroupActor;

UCLASS()
class ARMYEDITOR_API UArmyEditorEngine : public UGameEngine
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FVector UnsnappedClickLocation;

	UPROPERTY()
	FVector ClickLocation;

	UPROPERTY()
	FPlane ClickPlane;

public:
	virtual void Init(IEngineLoop* InEngineLoop);
	virtual void Tick(float DeltaSeconds, bool bIdleMode) override;
	virtual void RedrawViewports(bool bShouldPresent = true) override;

	void InitEditor(IEngineLoop* InEngineLoop);

	void CreateGameViewportWidget(UGameViewportClient* GameViewportClient) override;

	DECLARE_EVENT_OneParam (UArmyEditorEngine,FOnEndTransformObject,UObject&);

public:
	virtual void SelectActor(AActor* Actor, bool bInSelected, bool bNotify, bool bSelectEvenIfHidden = false, bool bForceRefresh = false);
	virtual void SelectNone(bool bNoteSelectionChange, bool bDeselectBSPSurfs, bool WarnAboutManyActors = true);
	/**@欧石楠 取消选中actor*/
	void DeselectActor(bool bNoteSelectionChange, bool bDeselectBSPSurfs, bool WarnAboutManyActors = true);

	virtual bool CanSelectActor(AActor* Actor, bool bInSelected, bool bSelectEvenIfHidden = false, bool bWarnIfLevelLocked = false) const { return true; }
	virtual void SelectGroup(AArmyGroupActor* InGroupActor, bool bForceSelection = false, bool bInSelected = true, bool bNotify = true);
	virtual void SelectComponent(class UActorComponent* Component, bool bInSelected, bool bNotify, bool bSelectEvenIfHidden = false) {}
	virtual void NoteSelectionChange();
	virtual void SetPivot(FVector NewPivot, bool bSnapPivotToGrid, bool bIgnoreAxis, bool bAssignPivot = false);

	class USelection* GetSelectedActors() const;
	class USelection* GetSelectedComponents() const;
	class USelection* GetSelectedObjects() const;
	void GetSelectedActors(TArray<AActor*>& OutActors);

	class FSelectionIterator GetSelectedActorIterator() const;
	class FSelectionIterator GetSelectedComponentIterator() const;

	int32 GetSelectedComponentCount() const;
	int32 GetSelectedActorCount() const;
	bool CanAllSelectedBeMoved();
	bool AllSelectedWithDetail();
	AActor* GetSingleSelectedActor();

	bool IsMaterialSelected();
	UMaterialInterface* SelectedMaterial;
	UStaticMeshComponent* SelectedMaterialOwnerSMC;
	UMaterialInterface* ClipboardMI;

	DECLARE_DELEGATE_RetVal(FBox, FOnDelegateVector);
	FOnDelegateVector GetSelectedActorsBoundingBoxDelegate;
	FBox GetSelectedActorsBox();

	virtual void SetActorSelectionFlags(AActor* InActor);
	void ApplyDeltaToActor(AActor* InActor, bool bDelta, const FVector* InTranslation, const FRotator* InRotation, const FVector* InScaling, bool bAltDown = false, bool bShiftDown = false, bool bControlDown = false) const;

	void EditorApplyTranslation(AActor* InActor, const FVector& DeltaTranslation, bool bAltDown, bool bShiftDown, bool bCtrlDown);
	void EditorApplyRotation(AActor* InActor, const FRotator& DeltaRotation, bool bAltDown, bool bShiftDown, bool bCtrlDown);
	void EditorApplyScale(AActor* InActor, const FVector& DeltaScale, const FVector* PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown);
	void EditorApplyMirror(AActor* InActor, const FVector& MirrorScale, const FVector& PivotLocation);
	void MirrorActors(const FVector& MirrorScale);
	
    /**
	*刘克祥，添加Actor到成组
	*/
	virtual AArmyGroupActor* CreateGroupActor(TArray<AActor*>& Actores);
	/**
	* Creates a new group from the current selection removing any existing groups.
	*/
	virtual AArmyGroupActor* edactRegroupFromSelected();

	/**
	* Disbands any groups in the current selection, does not attempt to maintain any hierarchy
	*/
	virtual void edactUngroupFromSelected();

	/**
	* Activates "Add to Group" mode which allows the user to select a group to append current selection
	*/
	virtual void edactAddToGroup();

	/**
	* Copy selected actors to the clipboard.  Does not copy PrefabInstance actors or parts of Prefabs.
	*
	* @param	InWorld					World context
	* @param	DestinationData			If != NULL, additionally copy data to string
	*/
	virtual void edactCopySelected(UWorld* InWorld, FString* DestinationData = NULL);

	/**
	* Paste selected actors from the clipboard.
	*
	* @param	InWorld					World context
	* @param	bDuplicate			Is this a duplicate operation (as opposed to a real paste)?
	* @param	bOffsetLocations	Should the actor locations be offset after they are created?
	* @param	bWarnIfHidden		If true displays a warning if the destination level is hidden
	* @param	SourceData			If != NULL, use instead of clipboard data
	*/
	virtual void edactPasteSelected(UWorld* InWorld, bool bDuplicate, bool bOffsetLocations, bool bWarnIfHidden, FString* SourceData = NULL);

	/**
	* Duplicates selected actors.
	*
	* @param	InLevel			Level to place duplicate
	* @param	bUseOffset		Should the actor locations be offset after they are created?
	*/
	virtual void edactDuplicateSelected(ULevel* InLevel, bool bOffsetLocations);

	/**
	* Deletes all selected actors
	*
	* @param		InWorld				World context
	* @param		bVerifyDeletionCanHappen	[opt] If true (default), verify that deletion can be performed.
	* @return									true unless the delete operation was aborted.
	*/
	virtual bool edactDeleteSelected(UWorld* InWorld, bool bVerifyDeletionCanHappen = true);

	/**
	* Iterate over all levels of the world and create a list of world infos, then
	* Iterate over selected actors and assemble a list of actors which can be deleted.
	*
	* @param	InWorld					The world we want to examine
	* @param	bStopAtFirst			Whether or not we should stop at the first deletable actor we encounter
	* @param	bLogUndeletable			Should we log all the undeletable actors
	* @param	OutDeletableActors		Can be NULL, provides a list of all the actors, from the selection, that are deletable
	* @return							true if any of the selection can be deleted
	*/
	bool CanDeleteSelectedActors(const UWorld* InWorld, const bool bStopAtFirst, const bool bLogUndeletable, TArray<AActor*>* OutDeletableActors = NULL) const;

	/**
	* Called to reset the editor's pivot (widget) location using the currently selected objects.  Usually
	* called when the selection changes.
	* @param bOnChange Set to true when we know for a fact the selected object has changed
	*/
	void UpdatePivotLocationForSelection(bool bOnChange = false);

	/**
	* Called when an actor or component has been translated, rotated, or scaled by the editor
	*
	* @param Object	The actor or component that moved
	*/
	void BroadcastEndObjectMovement (UObject& Object) const
	{
		OnEndObjectTransformEvent.Broadcast (Object);
	}

	/** Editor-only event triggered after actor or component has moved, rotated or scaled by an editor system */
	
	FOnEndTransformObject& OnEndObjectMovement ()
	{
		return OnEndObjectTransformEvent;
	}

	float GetGridSize();

	class UArmyThumbnailManager* GetThumbnailManager();

    /** 重置选中物体的尺寸，材质等 */
    void ResetSelectedActor();

    /** X轴镜像选中物体 */
    void MirrorXSelectedActor();

    /** Y轴镜像选中物体 */
    void MirrorYSelectedActor();

    /** 使选中物体快速落地 */
    void SnapSelectedActorToGround();
	
public:
	DECLARE_DELEGATE_TwoParams(FOnDelegateActorsInt, TArray<AActor*>, int);
	DECLARE_DELEGATE_ThreeParams(FOnDelegateActorMIInt, AActor*, UMaterialInterface*, int32);
	DECLARE_DELEGATE_TwoParams(FOnDelegateIntMI, int32, UMaterialInterface*);
	DECLARE_DELEGATE_OneParam(FOnDelegateUnGroup, TArray<AArmyGroupActor*>&);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDelegateDynamicActor, AActor*, NewActor);
	FObjectDelegate SelectionEditedEvent;
	FSimpleDelegate RightSelectedEvent;
	FSimpleDelegate SelectNonePreEvent;
	FOnDelegateDynamicActor OnLevelActorAdded;
	FActorDelegate OnLevelActorRemoved;
	FOnDelegateActorsInt OnActorsOperation;
	FOnDelegateActorMIInt OnAddActorMaterial;
	FOnDelegateIntMI OnAddSurfaceMaterial;
	/** Delegate broadcast when an actor or component has been moved, rotated, or scaled */
	FOnEndTransformObject OnEndObjectTransformEvent;

	FOnDelegateUnGroup SelectionUngroupEvent;

	//@欧石楠 Actor被拖拽时发出
	FActorDelegate OnLevelActorDragged;
	UPROPERTY()
	FVector MouseMovement;

    /** 等轴克隆actor的回调 */
	FMultiActorDelegate OnActorDuplicated;
	//FActorDelegate OnActorDuplicated;

public:
    bool bRipperIsStartScreenshot = false;

    bool bRipperIsScreenshot = false;

    bool bRipperIsExportJpg = false;

    int32 RipperResSize = 1;


	//欧石楠 添加选择过滤功能
public:
	DECLARE_DELEGATE_RetVal_OneParam(bool,FSelectFilter,AActor*)
	bool isUsingSelectFilter() 
	{
		return UsingSelectorFilter;
	};

	//开启过滤器，在开启之前必须绑定CanSelectFilter
	void OpenSelectFilter()
	{
		if (CanSelectFilter.IsBound())
		{
			UsingSelectorFilter = true;
		}
	};
	void CloseSelectFilter()
	{
		UsingSelectorFilter = false; 
		CanSelectFilter.Unbind();
	};
	//用于判断是否可以选择的代理
	FSelectFilter	CanSelectFilter;
protected:
	//是否使用选择过滤器，
	bool UsingSelectorFilter = false;
};

extern ARMYEDITOR_API class UArmyEditorEngine* GArmyEditor;
