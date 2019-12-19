#pragma once

#include "ArmyHardware.h"
#include "ArmyCommonTypes.h"
#include "IArmyReplace.h"
#include "FArmyConstructionItemInterface.h"
#include "IArmyConstructionHelper.h"

class AXRBrush;
class AStaticMeshActor;
class FArmyEditPoint;

/**
 * 门
 */
class ARMYSCENEDATA_API FArmySingleDoor : public FArmyHardware, public IArmyReplace, public IArmyConstructionHelper
{
public:
    FArmySingleDoor();
    FArmySingleDoor(FArmySingleDoor* Copy);
	virtual ~FArmySingleDoor();

    //~ Begin FObject2D Interface
	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
    virtual void SetState(EObjectState InState) override;
    virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
    virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
    virtual bool Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
    virtual void ApplyTransform(const FTransform& Trans) override;
    virtual void Destroy() override;
    //~ End FObject2D Interface

    //~ Begin FArmyHardware Interface
	/*获取户型模式下Draw的线段集合*/
	virtual void GetCadLineArray(TArray<TPair<FVector, FVector>>& OutLineList) const override;
    virtual void Update() override;
	virtual void Generate(UWorld* InWorld) override;
	virtual void Delete() override;
    //~ End FArmyHardware Interface

    //~ Begin IArmyReplace Interface
    virtual void ReplaceGoods(FContentItemPtr InContentItem, UWorld* InWorld) override;
    //~ End IArmyReplace Interface

    /** 是否能生成 */
    bool IsGenerative() const;

    /** 获取门的包围盒 */
    TArray<FVector> GetBoundingBox();
	 
	/** @纪仁泽 获得门的立面投影 */
	TArray<struct FLinesInfo> GetFacadeBox();
	
    /** bsp生成裁剪 */
    TArray<FVector> GetClipingBox();

    /** 获取门的包围盒 */
    const FBox GetBounds();

    int32 GetDoorSaleID() { return SaleID; }
	int GetDoorType() { return DoorType; }
    int32 GetOpenDirectionID() const { return OpenDirectionID; }
    FString GetThumbnailUrl() const { return ThumbnailUrl; }

    void SetDoorSaleID(int32 InValue) { SaleID = InValue; }
	void SetDoorType(int InType) { DoorType = InType; }
    void SetOpenDirectionID(int32 InOpenDirectionID) { OpenDirectionID = InOpenDirectionID; }
    void SetThumbnailUrl(FString InThumbnailUrl) { ThumbnailUrl = InThumbnailUrl; }

	void UpdateDoorHole(EModelType InModelType = E_ModifyModel);

	TSharedPtr<class FArmyCircle> GetDoorTrack() const
	{
		return DoorTrack;
	}
	
	/**@欧石楠 计算周长和面积*/
	void GetModifyWallAreaAndPerimeter(float& OutArea, float& OutPerimeter);

	void GetOriginPassArea(float &OutArea);

	// CAD弧线
	virtual TSharedPtr<FArmyArcLine> GetDoorArcLine() const;

	/** @欧石楠 计算链接的门洞是否需要更新施工项*/
	void UpdataConnectedDoorHole();

	/**@欧石楠 获取选中的材质ID*/
	int GetSelectedMaterialID() { return SelectedMaterialID; }
	void SetMatType(FString InType);
	FString GetMatType() { return MatType; }

public:

	//@郭子阳
	//有没有赋予商品
	bool HasGoods() { return SaleID > 0; };

    /** 门模型 */
    AActor* DoorActor;

	/**@欧石楠 直接放门导致拆改的拆除区域*/
	TSharedPtr<FArmyRect> ModifyWallRect;

	/**@欧石楠 是否链接了门洞*/
	bool bHasConnectPass = false;	

	/**@欧石楠 施工项相关数据*/
	TSharedPtr<FArmyConstructionItemInterface> ConstructionItemData;

	/** @欧石楠 链接的门洞*/
	TWeakPtr<FArmyPass> ConnectedPass;
protected:		
	virtual void OnFirstLineLInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);
	
	virtual void OnFirstLineRInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);
	
	virtual void OnSecondLineLInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);
	
	virtual void OnSecondLineRInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

protected:
	FString MatType = TEXT("轻体砖");
	int SelectedMaterialID = 2;//轻体砖

	/** @欧石楠 记录是否处在反序列化过程中*/
	bool bDeserialization = false;

	/**@欧石楠 拆除墙材质*/
	UMaterialInstanceDynamic* MI_ModifyWall;

    /** 门的材质 */
    UMaterialInstanceDynamic* MI_SingleDoor;

    /** 门板 */
    TSharedPtr<class FArmyRect> DoorPlank;

    /** 门的开关轨迹 */
    TSharedPtr<class FArmyCircle> DoorTrack;

	/** 门的商品ID*/
	int32 SaleID = -2;

    /** 缩略图url */
    FString ThumbnailUrl;

	/** 门类型1 标准门 2 防盗门 3 推拉门*/
	int DoorType = 1;

    /** 门的开启方向 1 内开左开 2 内开右开 3 外开左开 4 外开右开 */
    int32 OpenDirectionID = 0;

	/**@欧石楠 顶视图绘制数据*/
    TSharedPtr<FArmyRect> TopViewFillingRect;
    TSharedPtr<class FArmyPolygon> TopViewTrackPolygon;
	TSharedPtr<FArmyRect> TopViewRectImagePanel;
	TSharedPtr<FArmyRect> TopViewDoorPlank;
    TSharedPtr<FArmyCircle> TopViewDoorTrack;

    /** 拆改墙填充材质 */
	UMaterialInstanceDynamic* MI_RectModifyWall;

    bool bShouldUpdateWall = false;


	//@郭子阳
	//*********3D模式施工项相关**********
public:
	TSharedPtr<struct ConstructionPatameters> GetConstructionParameter();
	//获取施工项查询Key
	TSharedPtr<struct ObjectConstructionKey> GetConstructionKey();

	//输出窗台石施工项
	void ConstructionData(TArray<struct FArmyGoods>& ArtificialData);

	//获取窗户对应的房间
	TSharedPtr<FArmyRoom> GetRoom() override;

	virtual void OnRoomSpaceIDChanged(int32 NewSpaceID) override;

};
REGISTERCLASS(FArmySingleDoor)