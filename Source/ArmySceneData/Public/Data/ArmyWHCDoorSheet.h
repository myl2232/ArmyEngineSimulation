#pragma once
#include "Data/XRFurniture.h"
#include "Primitive2D/XRLine.h"

class ARMYSCENEDATA_API FArmyWHCDoorSheet final : public FArmyObject
{
public:
	//block box 
	//block property 1、xscale 2、yscale 3、xyscale 4、static
	//current xlenth current ylenth

	struct FDxfSpliteBlock :public TSharedFromThis<FDxfSpliteBlock>
	{
		enum EBlockModel
		{
			BM_STATIC,
			BM_XSCALE,
			BM_YSCALE,
			BM_XYSCALE
		};
		bool XChanged = false;
		bool YChanged = false;
		float XScale = 1;
		float YScale = 1;
		EBlockModel BlockModel;
		FBox2D BlockBoxBase;
		FBox2D BlockBoxPost;

		TArray<int32> ContainIndexArray;
	};
	struct FBlockRelationship
	{
		TSharedPtr<FDxfSpliteBlock> MainBlock;
		TArray<TSharedPtr<FDxfSpliteBlock>> XFrontBorderArray;
		TArray<TSharedPtr<FDxfSpliteBlock>> YFrontBorderArray;

		FBlockRelationship(const TSharedPtr<FDxfSpliteBlock> InBlock) :MainBlock(InBlock) {}

		bool operator==(const FBlockRelationship& Other) const
		{
			return MainBlock == Other.MainBlock;
		}
	};

    FArmyWHCDoorSheet();

    virtual void Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View) override;

	/** 移动位置 */
	virtual void ApplyTransform(const FTransform& Trans)  override;

	virtual void GetTransForm(FRotator& InRot, FVector& InTran, FVector& InScale) const  override;

	/** 获取包围盒*/
	virtual const FBox GetBounds() override;

	/** 获取原始包围盒（未进行变换的包围盒）*/
	virtual const FBox GetPreBounds()  override;

	/**	获取基点*/
	virtual const FVector GetBasePos()  override;

	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;

	void SetDataAddBlocks(const TSharedPtr<FArmyFurniture> InObj, const TArray<TSharedPtr<FDxfSpliteBlock>>& InSplitBlockArray);

	void SetCurrentSize(float InXlength,float InYLength);
	// 获得绘制CAD的相关数据
	TArray<struct FSimpleElementVertex> GetDrawArray() { return DrawArray; };
private:
	void Init();
	void Update();
	void UpdateDrawVertex();
	void UpdateRelationship();
	void XBorderScaleFun(const FBlockRelationship& InCurrentBlock,TArray<TSharedPtr<FDxfSpliteBlock>>& OutXBlockArray);
	void YBorderScaleFun(const FBlockRelationship& InCurrentBlock,TArray<TSharedPtr<FDxfSpliteBlock>>& OutYBlockArray);
private:
	FBox BoundingBox;//包围盒
	FTransform LocalTransform; 

	float CurrentXLenth = 0;
	float CurrentYLenth = 0;

	TArray<struct FSimpleElementVertex> BaseArray;
	TArray<struct FSimpleElementVertex> DrawArray;

	TArray<TSharedPtr<FDxfSpliteBlock>> BlockArray;
	TArray<FBlockRelationship> BlockRelationshipArray;
};
REGISTERCLASS(FArmyWHCDoorSheet)