#pragma once

#include "ArmyHardware.h"
#include "IArmyReplace.h"
#include "FArmyConstructionItemInterface.h"
#include "IArmyConstructionHelper.h"
class AStaticMeshActor;

/**
 * 推拉门
 */
class ARMYSCENEDATA_API FArmySlidingDoor : public FArmyHardware, public IArmyReplace,  public IArmyConstructionHelper
{
public:
    FArmySlidingDoor();
    FArmySlidingDoor(FArmySlidingDoor* Copy);
    virtual ~FArmySlidingDoor();

    //~ Begin FArmyObject Interface
	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
    virtual void SetState(EObjectState InState) override;
    virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
    virtual bool IsSelected(const FVector& Pos, UArmyEditorViewportClient* InViewportClient) override;
    virtual bool Hover(const FVector& Pos, UArmyEditorViewportClient* InViewportClient) override;
    virtual const FBox GetBounds() override;
    virtual void Destroy() override;
    //~ End FArmyObject Interface

    //~ Begin FArmyHardware Interface
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
	
	/** 生成立面投影 */
	TArray<struct FLinesInfo> GetFacadeBox();

    /** bsp生成裁剪 */
    TArray<FVector> GetClipingBox();

	void UpdateDoorHole(EModelType InModelType = E_ModifyModel);

    int32 GetDoorSaleID() { return SaleID; }
    int32 GetOpenDirectionID() const { return OpenDirectionID; }
    FString GetThumbnailUrl() const { return ThumbnailUrl; }

    void SetDoorSaleID(int32 InValue) { SaleID = InValue; }
    void SetOpenDirectionID(int32 InOpenDirectionID) { OpenDirectionID = InOpenDirectionID; }
    void SetThumbnailUrl(FString InThumbnailUrl) { ThumbnailUrl = InThumbnailUrl; }

	/**@欧石楠 计算周长和面积*/
	void GetModifyWallAreaAndPerimeter(float& OutArea, float& OutPerimeter);

	void GetOriginPassArea(float &OutArea);

	/** @欧石楠 计算链接的门洞是否需要更新施工项*/
	void UpdataConnectedDoorHole();

	/**@欧石楠 获取选中的材质ID*/
	int GetSelectedMaterialID() { return SelectedMaterialID; }
	void SetMatType(FString InType);
	FString GetMatType() { return MatType; }
public:
    TSharedPtr<class FArmyRect> StartDoorPlank;
    TSharedPtr<class FArmyRect> EndDoorPlank;
	TSharedPtr<class FArmyLine> LeftLine;
	TSharedPtr<class FArmyLine> RightLine;
	//@郭子阳
	//有没有赋予商品
	bool HasGoods() { return SaleID > 0; };

    /** 推拉门模型 */
    AActor* SlidingDoorActor;

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

    /** 门的开启方向 1 内开左开 2 内开右开 3 外开左开 4 外开右开 */
    int32 OpenDirectionID = 0;

    /** 缩略图url */
    FString ThumbnailUrl;

	/** @欧石楠 记录是否处在反序列化过程中*/
	bool bDeserialization = false;
private:
	/**@欧石楠 拆除墙材质*/
	UMaterialInstanceDynamic* MI_ModifyWall;

	/**@欧石楠 顶视图绘制数据*/
	TSharedPtr<class FArmyRect> TopViewRectImagePanel;
	TSharedPtr<class FArmyRect> TopViewStartDoorPlank;
	TSharedPtr<class FArmyRect> TopViewEndDoorPlank;
	TSharedPtr<class FArmyLine> TopViewLeftLine;
	TSharedPtr<class FArmyLine> TopViewRightLine;
	bool bShouldUpdateWall = false;	

	/** 填充材质 */
	UMaterialInstanceDynamic* MI_RectModifyWall;

    /** 门的商品ID*/
    int32 SaleID = -1;

	//@郭子阳
	//*********3D模式施工项相关**********
public:
	TSharedPtr<struct ConstructionPatameters> GetConstructionParameter();
	//获取施工项查询Key
	TSharedPtr<struct ObjectConstructionKey> GetConstructionKey();

	//输出门施工项
	void ConstructionData(TArray<struct FArmyGoods>& ArtificialData);
	//获取门所在的房间
	TSharedPtr<FArmyRoom> GetRoom() override;

	void OnRoomSpaceIDChanged(int32 NewSpaceID);
};

REGISTERCLASS(FArmySlidingDoor)