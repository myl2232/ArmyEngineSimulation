#pragma once
#include "ArmyOperation.h"
#include "Engine/Canvas.h"
#include "SContentItem.h"
#include "ArmyCommonTypes.h"
class FArmyReplaceTextureOperation :public FArmyOperation
{
public:
	FArmyReplaceTextureOperation(EModelType InBelongModel);
	~FArmyReplaceTextureOperation() {}

	//~ Begin FArmyController Interface
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event) override;
	virtual  void ProcessClickInViewPortClient(FViewport* InViewPort, FKey Key, EInputEvent Event)override;
    virtual void ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;
	virtual void BeginOperation(XRArgument InArg = XRArgument());
	virtual void EndOperation() override;
	virtual void Tick() override;
	//~ End FArmyController Interface
	virtual void MouseMove(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y) override;
	
	static void SetCurrentItem(TSharedPtr<FContentItemSpace::FContentItem> item);
	
	//@郭子阳
	//指定铺贴时的样式
	static void SetCurrentMatDataSource(TSharedPtr<FArmyBaseEditStyle> NewMatDataSource) {
		MatDataSource = NewMatDataSource;
	};
	
	//void SetCurrentSelectItem(TSharedPtr<class SContentItem> item);
	
	//@郭子阳 设置Thumbnail
	static void SetThumbnail(FString ThumbnailUrl);


	void SetCurrentEditSurface(TSharedPtr<class FArmyRoomSpaceArea> RoomSpaceArea);

	void SetEditMode(uint32 mode);

	//@郭子阳 复制墙面的style
	 static void CopyEditAreaSytle(TSharedPtr<FArmyBaseArea> SourceEditArea,TSharedPtr<FArmyBaseArea> DesitinationEditArea);

protected:
	//@郭子阳
	//铺贴时的参考样式，如果这个样式不为空，铺贴时就会从中复制数据
	static TSharedPtr<FArmyBaseEditStyle> MatDataSource;

	bool ReplaceTextureOperation();
	TSharedPtr<FArmyBaseArea>	PickOperation(const FVector& pos, class UArmyEditorViewportClient* InViewportClient);
	void UpdateEditAreaSytle(TSharedPtr<FArmyBaseArea> editArea);
	static TSharedPtr<FContentItemSpace::FContentItem> ContentItem;
	uint32 CurrentState = 0;

	//水刀拼花材质
	UMaterialInstanceDynamic* MI_WaterKnifeTextureMat;
	//矩形
	TSharedPtr<class FArmyRect> OwnerRect;

private:

	static TSharedPtr<SImage>  ThumbnailImage; //拖拽时的缩略图

	//瀑布流项，用于设置缩略图
//	TSharedPtr<SContentItem> SContemtItemWidget;
	TSharedPtr<class FArmyRoomSpaceArea> CurrentEditSurface;

	// 0 是自由空间，1 表示墙顶地
	uint32 CurrentEditState = 0;

	/* @ 梁晓菲 放置扣条Hover时是否Hover了单边*/
	bool bHoverBuckleEdge = false;
	FVector HoverEdgeStart = FVector::ZeroVector;
	FVector HoverEdgeEnd = FVector::ZeroVector;

	/** @梁晓菲 请求施工项相关数据*/
	void ReqConstructionData(int32 GoodsId);
	void ResConstructionData(FArmyHttpResponse Response, int32 GoodsId);
#ifdef WITH_EDITOR
	TArray<FVector> TestSurface;

	TSharedPtr<FArmyRoomSpaceArea> TestSelectedRoompSapce;
#endif
};