#pragma once

#include "ArmyObject.h"
#include "ArmyTypes.h"

const float G_MinRoomSpacing = 1.f;//单位cm
const float G_MaxRoomSpacing = 100.f;

class FArmyRoom;
DECLARE_DELEGATE_OneParam(RoomSpaceIDChangeDelegate,TSharedPtr<FArmyRoom>)

class ARMYSCENEDATA_API FArmyRoom :public FArmyObject
{
public:

	FArmyRoom();
	~FArmyRoom(){}
	FArmyRoom(FArmyRoom* Copy);

	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	virtual void SetState(EObjectState InState) override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual bool Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	/** 高亮操作点 */
	virtual TSharedPtr<FArmyEditPoint> HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual TSharedPtr<FArmyLine> HoverLine(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual void GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs = false) override;
	virtual const FBox GetBounds();
	virtual const FBox GetBounds3D();
	virtual void RemoveLine(TSharedPtr<FArmyLine> InLine);
	virtual void Delete() override;	

	virtual void Modify() override;

	void GetHardwareListRelevance(TArray< TSharedPtr<class FArmyHardware> >& OutObjects) const;

	/** @欧石楠 当被清空时的调用，与上面的删除不一致*/
	virtual void OnDelete() override;


	//@郭子阳 房间ID变动时发生
	void OnSpaceIDChanged();

	/**	获取所有顶点*/
	virtual void GetVertexes(TArray<FVector>& OutVertexes) override;

	void RemovePoint(TSharedPtr<FArmyEditPoint> InPoint);

	void AddLine(TSharedPtr<class FArmyWallLine> InLine);

	bool IsHasLine(TSharedPtr<class FArmyWallLine> InLine);

	TSharedPtr<class FArmyEditPoint> GetEditPoint(const FVector& InPoint);

	const TArray<TSharedPtr<class FArmyEditPoint>>& GetPoints();

	//获取区域世界坐标点集(参数表示是否按着顺时针排序)
	TArray<FVector> GetWorldPoints(bool bSort = false);

	//区域是否闭合
	bool IsClosed();

	void Clear();

    /** 获得房屋面积和周长 */
    void GetRoomAreaAndPerimeter(float& OutArea, float& OutPerimeter);

    /** 判断一个3D点是否在房间内 */
    bool IsPointInRoom(const FVector& Point);

    /***@郭子阳  收集房间里的商品 */
    void CollectItemsInRoom(TArray< struct FArmyGoods>& ArtificialData);
	//@郭子阳 根据房间ID修改房间内所有物品的施工项的数据
	void ChangedItemsConstructionDataInRoom();

	/**@欧石楠 判断某条线是不是房间内的的线*/
	bool CheckHasLine(TSharedPtr<FArmyLine> InLine);

	/*@欧石楠 空间名称相关操作**/
	FString GetSpaceName()const { return SpaceName; }
	void SetSpaceName(FString InStr);

    const int32 GetSpaceId() const { return SpaceId; }
	void SetSpaceId(const int32 InSpaceId);

	FString GetRoomId()const { return UniqueId; }	

	const TSharedPtr<class FArmyRulerLine>& GetRulerLine();

	void UpdateSpacing();

	/*@梁晓菲 获取房间唯一ID*/
	int32 GetUniqueIdOfRoom()const { return UniqueIdOfRoom; }

	void CombineCollinearLines();

	TArray<TSharedPtr<class FArmyWallLine>> GetWallLines() const;

	FVector GetWallLineNormal(TSharedPtr<class FArmyWallLine> InWallLine);

	void GetObjectsRelevance(TArray<TWeakPtr<FArmyObject>>& InObjects);
	// @zengy 通过物体轮廓来查找物体和房间的关系，只要物体有任意一个点在房间内，就认为物体和房间有关系
	void GetObjectsRelevanceByObjectVertices(TArray<TWeakPtr<FArmyObject>>& InObjects);

	//@ 删除房间关联的对象
	bool DeleteRelatedObj();

	//@ 获取和设置空间名称位置
	FVector GetTextPos() const { return SpaceNamePos; }
	void SetTextPos(FVector InSpaceNamePosPos) { SpaceNamePos = InSpaceNamePosPos; }

	/** @欧石楠 更新空间名称显示的位置 */
	bool UpdateSpaceNamePos();

    /** @欧石楠 生成空间名称标注 */
    bool GenerateSpaceNameLabel(EModelType InModelType = E_HomeModel);
	
	//得到和获取空间标注
	TWeakPtr<class FArmyTextLabel> GetSpaceNameLabel() const { return SpaceNameLabel; }
	void SetSpaceNameLabel(TSharedPtr<class FArmyTextLabel> InSpaceNameAreaLabel) { SpaceNameLabel = InSpaceNameAreaLabel; }

	/**
	 * @ 计算点到空间的最近投影点
	 * @param Pos - FVector - 输入点
	 * @param _FirstProjPos - FVector & - 输出距离空间最近线的投影点
	 * @param _SecondProjPos - FVector & - 输出从中心到第一投影点方向上的另一空间线投影点 
	 * @return bool - true表示计算成功，否则失败
	 */
	bool CalPointToRoomLineMinDis(FVector Pos, FVector & _FirstProjPos, FVector & _SecondProjPos);

	void HiddenRulerLine();

	//删除空间名称
	bool DeleteSpaceNameObj(bool bModify = false);

	/**
	 * 更新与修改同类型的空间名称
	 * @param ModifyBefore - FString - 新的空间名称
	 * @return bool - 
	 */
	bool ModifySpaceName(FString InNewSpaceName);
	/**
	* 找到公共点的墙体
	*/
	void GetSamePointWalls(const FVector SamePoint, TArray<TSharedPtr<class FArmyWallLine>>& OutLines);

    /** 设置墙线颜色 */
    void SetWallLinesColor(const FLinearColor InColor);
	
	/** @欧石楠 获取原始面高度*/
	float GetOriginGroundHeight()const { return OriginGroundHeight; }

	/** @欧石楠 获取完成面高度*/
	float GetFinishGroundHeight()const { return FinishGroundHeight; }

	/** @欧石楠 设置原始面高度*/
	void SetOriginGroundHeight(float InValue) { OriginGroundHeight = InValue; }

	/** @欧石楠 设置完成面高度*/
	void SetFinishGroundHeight(float InValue) { FinishGroundHeight = InValue; }

	/** @欧石楠 获取因删除线条产生的另一个空间的点的数据*/
	TArray<FVector>& GetAnotherRoomPoints() { return CachedOtherRoomPonits; }

	/** @欧石楠 清除另一个房间的点*/
	void ClearAnotherRoomPoints() { CachedOtherRoomPonits.Empty(); }

    /** @欧石楠 通过一条墙线获取与之相连的两条墙线 */
    void GetRelatedWallLines(TSharedPtr<FArmyWallLine> SourceWallLine, TArray< TSharedPtr<FArmyWallLine> >& OutWallLines);

	RoomSpaceIDChangeDelegate SpaceIDChanged;
	
private:

	void SortLines(TSharedPtr<class FArmyWallLine> InLine, TArray< TSharedPtr<class FArmyWallLine> >& OutLines);

	void GenerateSortPoints(TArray<FVector>& InPoints);

	void OnLineInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction, TSharedPtr<SWidget> InThisWidget);
	
	bool IsParallel(TSharedPtr<FArmyLine> Line1, TSharedPtr<FArmyLine> Line2);
    bool IsParallel(const FVector& S0, const FVector& E0, const FVector& S1, const FVector& E1);

	bool OffsetRoom(FVector InDirect, float InDistance);

	//@ 删除门窗对象
	bool DeleteRelatedHardWareObj();
	//删除房间内的对象，如原始点位、梁、柱子、风道
	bool DeleteRelatedInRoomObj();
	//删除承重墙
	bool DeleteRelatedBearingObj();

	/**
	* 修改空间名称后更新所有空间名称
	* @param ModifyBefore - FString - 修改前的名称
	* @param ModifyAfter - FString - 修改后的名称
	* @return bool -
	*/
	bool UpdateSpaceName(FString ModifyBefore, FString ModifyAfter);

	/**
	* 删除空间名称，更新同类型空间名称
	* @param InSpaceName - FString - 输入删除的空间名称
	*/
	bool UpdateDeleteSpaceName(FString InSpaceName);	

protected:
	UMaterialInstanceDynamic* MI_Line;
	FLinearColor Color;

	TArray< TSharedPtr<class FArmyWallLine> > Lines;
	TArray< TSharedPtr<class FArmyEditPoint> > Points;

	/** @欧石楠 记录当被删除了某些线生成了两个房间时，另一个房间的数据*/
	TArray<FVector> CachedOtherRoomPonits;

	/*@欧石楠 空间名称**/
	FString SpaceName;

	int32 SpaceId;

	//@梁晓菲 每个Room的唯一ID，效果图全景图使用
	int32 UniqueIdOfRoom;

	//基于Brush的ActorLabel给到Room的唯一标识，暂时供软硬装模块使用
	FString UniqueId;

	TSharedPtr<class FArmyPolygon> SelectedRoomSpace;

	/**@欧石楠 选中线段的标尺线*/
	TSharedPtr<class FArmyRulerLine> RulerLine;

	/**@欧石楠 选中空间显示全部的标尺线*/
	TArray<TSharedPtr<class FArmyRulerLine>> AllRulerLines;

	struct FSpacingEditData
	{
		float PreDistance;
		FVector StartPoint;
		FVector MoveDirection;

		FVector LineStart;
		FVector LineEnd;
	};
	TMap<TSharedPtr<class FArmyRulerLine>, FSpacingEditData> LineInputBoxList;

	TMap<TSharedPtr<class FArmyLine>, TSharedPtr<FArmyRoom>> CatchLines;//出去本身空间的线集合

public:
	bool bModified = true;

	bool bPreModified = true;//原始户型下是否有改动（包括空间内的梁柱移动）	

private:
	FVector SpaceNamePos = FVector::ZeroVector;
	TSharedPtr<class FArmyTextLabel> SpaceNameLabel;

	/** @欧石楠 原始面高度*/
	float OriginGroundHeight = 0.f;

	/** @欧石楠 完成面高度*/
	float FinishGroundHeight = 0.f;

public:
	/* @梁晓菲 施工项属性结构体*/
	struct ConstructionProperty
	{
		int32 propertyId;
		int32 quotaProperties;
		ConstructionProperty(int32 InpropertyId, int32 InquotaProperties)
		{
			propertyId = InpropertyId;
			quotaProperties = InquotaProperties;
		}
		ConstructionProperty()
		{
			propertyId = -1;
			quotaProperties = -1;
		}
		friend bool operator==(const ConstructionProperty& temp1, const ConstructionProperty& temp2)
		{
			if (temp1.propertyId == temp2.propertyId && temp1.quotaProperties == temp2.quotaProperties)
			{
				return true;
			}
			return false;
		}
		friend bool operator!=(const ConstructionProperty& temp1, const ConstructionProperty& temp2)
		{
			if (temp1.propertyId == temp2.propertyId && temp1.quotaProperties == temp2.quotaProperties)
			{
				return false;
			}
			return true;
		}
		friend uint32 GetTypeHash(const ConstructionProperty& temp)
		{
			return FCrc::MemCrc_DEPRECATED(&temp, sizeof(ConstructionProperty));
		}
	};
	/* @梁晓菲 施工项结构体*/
	struct ItemIDCheckId
	{
		int32 ItemID;
		int32 QuotaID;
		TArray<ConstructionProperty> PropertyList;

		ItemIDCheckId(int32 InItemID, int32 InQuotaid, TArray<ConstructionProperty> InPropertyList)
		{
			ItemID = InItemID;
			QuotaID = InQuotaid;
			PropertyList = InPropertyList;
		}
		ItemIDCheckId()
		{
			ItemID = -1;
			QuotaID = -1;
			PropertyList.Empty();
		}
		friend bool operator==(const ItemIDCheckId& temp1, const ItemIDCheckId& temp2)
		{
			if (temp1.ItemID == temp2.ItemID && temp1.QuotaID == temp2.QuotaID && temp1.PropertyList == temp2.PropertyList)
			{
				return true;
			}
			return false;
		}
		friend bool operator!=(const ItemIDCheckId& temp1, const ItemIDCheckId& temp2)
		{
			if (temp1.ItemID == temp2.ItemID && temp1.QuotaID == temp2.QuotaID && temp1.PropertyList == temp2.PropertyList)
			{
				return false;
			}
			return true;
		}
		friend uint32 GetTypeHash(const ItemIDCheckId& temp)
		{
			return FCrc::MemCrc_DEPRECATED(&temp, sizeof(ItemIDCheckId));
		}
	};
};
REGISTERCLASS(FArmyRoom)