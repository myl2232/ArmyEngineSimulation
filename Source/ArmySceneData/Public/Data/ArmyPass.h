#pragma once

#include "ArmyHardware.h"
#include "IArmyReplace.h"
#include "FArmyConstructionItemInterface.h"
#include "ArmyPolyline.h"
/**
 * 垭口
 */
class ARMYSCENEDATA_API FArmyPass : public FArmyHardware, public IArmyReplace
{
public:
	FArmyPass();
	FArmyPass(FArmyPass* Copy);
	virtual ~FArmyPass();

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

	/**	设置属性标识*/
	virtual void SetPropertyFlag(PropertyType InType, bool InUse) override;

	//~ Begin FArmyHardware Interface
	virtual void Update() override;
	//~ End FArmyHardware Interface
	virtual void Generate(UWorld* InWorld) override;

	virtual void SetPos(const FVector& InPos) override;
	virtual void SetHeight(const float& InHeight) override;
	virtual void SetDirection(const FVector& InDirection) override;

	virtual void ReplaceGoods(FContentItemPtr InContentItem, UWorld* InWorld) override;

	/*@欧石楠单独更新start point**/
	virtual void UpdateStartPoint();

	/*@欧石楠单独更新end point**/
	virtual void UpdateEndPoint();

	/** @纪仁泽 获得立面投影 */
	TArray<struct FLinesInfo> GetFacadeBox();

	/** bsp生成裁剪 */
	TArray<FVector> GetClipingBox();

	void UpdatePostWall();

	FORCEINLINE float GetThickness() { return Thickness; }
	FORCEINLINE float GetOuterWidth() { return OuterWidth; }
	FORCEINLINE float GetTotalWidth() { return Width + OuterWidth * 2.f; }

	void SetThickness(const float InThickness);
	void SetOuterWidth(const float InOuterWidth);

	const void GetAlonePoints(TArray< TSharedPtr<FArmyPoint> >& OutPoints) override;

	void SetIfGeneratePassModel(bool bGenerate);
	bool GetIfGeneratePassModel() { return bIfGenerateModel; }

	void SetIfFillPass(bool bFillPass);
	bool GetIfFillPass() { return bIfFillPass; }

	void UpdateWallHole(TSharedPtr<FArmyObject> InHardwareObj, bool bIntersect);

	void ResetAddModifyWall();

	/** @欧石楠 重置垂直拆改*/
	void RestVerticalState();

	void SetAddDoorHoleMatType(FText InStr);
	FText GetAddDoorHoleMatType() { return AddDoorHoleMatType; }

	void SetModifyDoorHoleMatType(FText InStr);
	FText GetModifyDoorHoleMatType() { return ModifyDoorHoleMatType; }

	/** 提供新增墙和拆改墙的标尺信息 */
	const FVector& GetLastStart() const {return LastModifyStart; };
	const FVector& GetLastEnd()const { return LastModifyEnd; };
	const TSharedPtr<FArmyRect> GetLeftAddWall() const { return LeftAddWallRect; }
	const TSharedPtr<FArmyRect> GetRightAddWall() const { return RightAddWallRect; }
	const TSharedPtr<FArmyRect> GetLeftDeleteWall() const { return LeftModifyWallRect; }
	const TSharedPtr<FArmyRect> GetRightDeleteWall() const { return RightModifyWallRect; }
	const TSharedPtr<FArmyRect> GetFillPassWall() const { return FillPassRect; }

	/**@欧石楠 请求材质商品编号*/
	void ReqAddMaterialSaleID();
	void ResAddMaterialSaleID(struct FArmyHttpResponse Response);

	void ReqModifyMaterialSaleID();
	void ResModifyMaterialSaleID(struct FArmyHttpResponse Response);

	/**@欧石楠 计算周长和面积*/
	void GetAddWallAreaAndPerimeter(float& LeftOutArea, float& LeftOutPerimeter, float& RightOutArea, float& RightOutPerimeter);

	void GetAddWallAreaAndPerimeter(float& OutArea, float& OutPerimeter);

	void GetModifyWallAreaAndPerimeter(float& OutArea, float& OutPerimeter);

	void GetOriginPassArea(float &OutArea);

    /** 计算填补门洞的面积和周长 */
    void GetFillAreaAndPerimeter(float& OutArea, float& OutPerimeter);

	/**@欧石楠 获取材质商品ID*/
	int GetAddMaterialSaleID() { return AddMaterialSaleID; }

	/**@欧石楠 获取材质商品ID*/
	int GetModifyMaterialSaleID() { return ModifyMaterialSaleID; }

	/**@欧石楠 获取选中的材质ID*/
	int GetSelectedAddMaterialID() { return SelectedAddMaterialID; }

	/**@欧石楠 获取选中的材质ID*/
	int GetSelectedModifyMaterialID() { return SelectedModifyMaterialID; }

	virtual	void GetAttachHoleWallInfo(TArray<struct FBSPWallInfo>& OutAttachWallInfo);

	/** @欧石楠 获取拆除长度*/
	float GetModifyLength();

	/** @欧石楠 获取新建长度*/
	float GetAddLength();

	/**@欧石楠 在拆改模式下更新拆补门洞*/
	void UpdateModifyPass();

	/**@欧石楠 清除拆补门洞信息*/
	void RestModifyPass();

	/**@欧石楠 检查是否还可以移动位置*/
	bool CheckIfCanMoveModifyPass(FVector InPos);

	/** @欧石楠 更新垂直拆改状态*/
	void UpdateVerticalState();

	/**@欧石楠 获取选中的材质ID*/
	int GetSelectedMaterialID() { return SelectedMaterialID; }

	void SetMatType(FString InType);
	FString GetMatType() { return MatType; }

	/** @欧石楠 获取原始高度*/
	float GetOriginHeight() { return OriginHeight; }

	/** @欧石楠 获取原始长度*/
	float GetOriginLength() { return OriginLength; }	

	/** @欧石楠 获取水平新建长度*/
	float GetHorizantalAddLength();

	/** @欧石楠 获取水平拆除长度*/
	float GetHorizantalModifyLength();

	int32 GetSaleID() const { return SaleID; }
    FString GetThumbnailUrl() const { return ThumbnailUrl; }

    void SetThumbnailUrl(FString InThumbnailUrl) { ThumbnailUrl = InThumbnailUrl; }
	TSharedPtr<FArmyPolyline>	GetStartPass() { return StartPass; }
	TSharedPtr<FArmyPolyline> GetEndPass() { return EndPass; }
private:
	void DrawOrigin(FPrimitiveDrawInterface* PDI, const FSceneView* View, bool InEdge = true);
	void DrawNormal(FPrimitiveDrawInterface* PDI, const FSceneView* View, bool InEdge = true);
	void DrawTopView(FPrimitiveDrawInterface* PDI, const FSceneView* View);
	void DrawOnlyDelete(FPrimitiveDrawInterface* PDI, const FSceneView* View);
	void DrawOnlyAdd(FPrimitiveDrawInterface* PDI, const FSceneView* View);
	void DrawAll(FPrimitiveDrawInterface* PDI, const FSceneView* View);
	void DrawPost(FPrimitiveDrawInterface* PDI, const FSceneView* View);
public:
	/**@欧石楠 施工项相关数据*/
	TSharedPtr<FArmyConstructionItemInterface> ConstructionItemData;

	TSharedPtr<class FArmyPolyline> StartPass;
	TSharedPtr<class FArmyPolyline> EndPass;
	TSharedPtr<class FArmyRect> Spacing;

	class AArmyExtrusionActor* ExtrusionActor;

	//TSharedPtr<class FArmyRect> RectImagePanel;

	TSharedPtr<FArmyHardware> ConnectHardwareObj;

	FContentItemPtr PassContentItem;//商品信息（默认是空）

	bool bHasAdd = false;
	bool bHasModify = false;
	bool bModifyType = false;

	bool bPassHasAdd = false;
	bool bPassHasModify = false;
	bool bPassHasVerticalAdd = false;
	bool bPassHasVerticalModify = false;

	//@郭子阳
	//有没有赋予商品
	bool HasGoods() { return SaleID > 0; };
protected:
	FString MatType = TEXT("轻体砖");	
	int SelectedMaterialID = 2;//轻体砖

private:
	/** 垭口厚度 */
	float Thickness;

	/** 垭口外边宽 */
	float OuterWidth;

	/**@欧石楠 是否生成包边*/
	bool bIfGenerateModel = false;

	/**@欧石楠 是否填补*/
	bool bIfFillPass = false;

	/** @欧石楠 记录是否处在反序列化过程中*/
	bool bDeserialization = false;

	/** @欧石楠 标记是否处于更新门洞自动拆改的过程中*/
	bool bIsAutoCalcPassState = false;		

	/**@欧石楠 记录原始户型下的门洞数据，在拆改模式下用于直接拖拽拆补*/	

	/**是否进行了拆补门洞 如果进行拆补了 则不接受放门*/
	bool bHasModifyPass = false;
	/**原始长度*/
	float OriginLength;
	/**原始高度*/
	float OriginHeight;
	/**原始起始点*/
	FVector OriginStartPoint;
	/**原始结束点*/
	FVector OriginEndPoint;
	/**原始位置点*/
	FVector OriginPoint;
	/**原始区域*/
	TSharedPtr<class FArmyRect> OriginRect;
	TSharedPtr<class FArmyPolyline> OriginStartPass;
	TSharedPtr<class FArmyPolyline> OriginEndPass;

	/**@欧石楠 顶视图下显示的额外数据*/
	TSharedPtr<class FArmyRect> TopViewRectImagePanel;
	TSharedPtr<class FArmyPolyline> TopViewStartPass;
	TSharedPtr<class FArmyPolyline> TopViewEndPass;
	TSharedPtr<class FArmyRect> TopViewSpacing;

	UMaterialInstanceDynamic* MI_AddWall;
	UMaterialInstanceDynamic* MI_ModifyWall;

	/**@欧石楠 填补门洞的填充材质*/
	UMaterialInstanceDynamic* MI_FillPass;

	FVector LastModifyStart;
	FVector LastModifyEnd;

	TSharedPtr<FArmyRect> LeftAddWallRect;
	TSharedPtr<FArmyRect> RightAddWallRect;
	TSharedPtr<FArmyRect> LeftModifyWallRect;
	TSharedPtr<FArmyRect> RightModifyWallRect;

	TSharedPtr<FArmyLine> PostModifyStartLine;
	TSharedPtr<FArmyLine> PostModifyEndLine;
	TSharedPtr<FArmyLine> PostModifyTopLine;
	TSharedPtr<FArmyLine> PostModifyBotLine;

	/**@欧石楠 填门洞填充*/
	TSharedPtr<FArmyRect> FillPassRect;

	bool bModify = false;//是否被新增或者拆改墙

	FText AddDoorHoleMatType = FText::FromString(TEXT("大芯板"));
	FText ModifyDoorHoleMatType = FText::FromString(TEXT("轻体砖"));

	int AddMaterialSaleID;
	int SelectedAddMaterialID = 6;//大芯板

	int ModifyMaterialSaleID;
	int SelectedModifyMaterialID = 2;//轻体砖

	/** 垭口的商品ID*/
	int32 SaleID = -1;

    /** 缩略图url */
    FString ThumbnailUrl;

	//@郭子阳
	//*********3D模式施工项相关**********
public:
	TSharedPtr<struct ConstructionPatameters> GetConstructionParameter();
	//获取施工项查询Key
	TSharedPtr<struct ObjectConstructionKey> GetConstructionKey();
	//输出窗台石施工项
	void ConstructionData(TArray<struct FArmyGoods>& ArtificialData);
};
REGISTERCLASS(FArmyPass)