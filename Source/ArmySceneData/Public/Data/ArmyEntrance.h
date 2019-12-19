/**
* Copyright 2018 ������Ƽ����޹�˾.
* All Rights Reserved.
* 
*
* @File FArmyEntrance.h
* @Description ʩ��ͼ��ڻ���
*
* @Author ������
* @Date 2018��11��1��
* @Version 1.0
*/

#pragma once
#include "ArmyObject.h"
#include "ArmyPolygon.h"
#include "Map.h"
/*
*              /|\---->����
*             / | \
*            /__|__\
*              |_|
*               =	--->�ײ�����
*/

#define UP_DIS   10.0f
#define DOWN_DIS 10.0f
#define LONG_DIS 25.0f
#define NEAR_DIS 20.0f
#define FIRSTLINE_DOWN_DIS 14.0f
#define SECONDLINE_DOWN_DIS 18.0f
#define THREELINE_DOWN_DIS 22.0f

class ARMYSCENEDATA_API FArmyEntrance : public FArmyObject
{
public:
	//���ͼ���������λ��
	struct ConstructHouse
	{
		FVector Top              = FVector(0, UP_DIS, 0);
		FVector Long_Left        = FVector(-LONG_DIS, 0, 0);
		FVector Long_Right       = FVector(LONG_DIS, 0, 0);
		FVector Middle_Left      = FVector(-NEAR_DIS, 0, 0);
		FVector Middle_Right     = FVector(NEAR_DIS, 0, 0);
		FVector Long_BottomLeft  = FVector(-NEAR_DIS, -DOWN_DIS, 0);
		FVector Long_BottomRight = FVector(NEAR_DIS, -DOWN_DIS, 0);
		FVector Bottom           = FVector(0, -DOWN_DIS, 0);
		
		FVector FirstLine_Left   = FVector(-NEAR_DIS, -FIRSTLINE_DOWN_DIS, 0);
		FVector FirstLine_Right  = FVector(NEAR_DIS, -FIRSTLINE_DOWN_DIS, 0);
		FVector SecondLine_Left  = FVector(-NEAR_DIS, -SECONDLINE_DOWN_DIS, 0);
		FVector SecondLine_Right = FVector(NEAR_DIS, -SECONDLINE_DOWN_DIS, 0);
		FVector ThreeLine_Left   = FVector(-NEAR_DIS, -THREELINE_DOWN_DIS, 0);
		FVector ThreeLine_Right  = FVector(NEAR_DIS, -THREELINE_DOWN_DIS, 0);
	};

public:
	FArmyEntrance();
	virtual ~FArmyEntrance();

	//���л�
	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	//�����л�
	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;

	//����
	virtual void Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View) override;

	//HUD����
	virtual void DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override;

	virtual void Refresh() override;

	virtual TSharedPtr<FArmyEditPoint> SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;

	virtual void SetState(EObjectState InState) override;

	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;

	virtual const FBox GetBounds() override;

	virtual TSharedPtr<FArmyEditPoint> HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;

	virtual void ApplyTransform(const FTransform& Trans) override;

private:
	//��ʼ��
	void Init();
	/**
	* @brief ��ȡ���������
	* @Param InVertices--> �����������µ�λ������
	* @Param OutBoundLines--> ����λ�������ȡ�õ���������������鲢OUT����
	*/
	static void GetOutLine(const TArray<FVector>& InVertices, TArray<TSharedPtr<FArmyLine>>& OutBoundLines);

public:
	/**
	* @brief ���û��Ƶ�λ��
	* @Param �����������µ�λ��
	* @Param �����������µķ���
	*/
	void SetWorldTransform(const FVector& Pos, const FVector& Rotate);

	/**
	* @brief ��ȡ����б�
	* @Param ���س�����б� KeyΪ�Ƿ�Ϊ��ɫ��䣬ValueΪ����ζ���
	*/
	void GetPolygon(TArray<TPair<bool, TSharedPtr<class FArmyPolygon>>>& OutPolygonMap) const
	{
		OutPolygonMap.Emplace(TPair<bool, TSharedPtr<class FArmyPolygon>>(true, WhiteArrows));
		OutPolygonMap.Emplace(TPair<bool, TSharedPtr<class FArmyPolygon>>(false, BlackArrows));
	}

	/**
	* @brief ��ȡԭʼ����б�
	* @Param ���س�����б� KeyΪ�Ƿ�Ϊ��ɫ��䣬ValueΪ����ζ���
	*/
	void GetOriginPolygon(TArray<TPair<bool, TSharedPtr<class FArmyPolygon>>>& OutPolygonMap) const
	{
		OutPolygonMap.Emplace(TPair<bool, TSharedPtr<class FArmyPolygon>>(true, OriginWhiteArrows));
		OutPolygonMap.Emplace(TPair<bool, TSharedPtr<class FArmyPolygon>>(false, OriginBlackArrows));
	}

	/**
	* @brief ��ȡ�뻧���߶��б�
	* @Param �߶��б���
	*/
	void GetBoundLineList(TArray< TSharedPtr<FArmyLine>>& OutLineList) const
	{
		OutLineList.Append(BoundList);
		//�ײ������߶�TODO
	}

	/**
	* @brief ��ȡԭʼ�뻧���߶��б�
	* @Param �߶��б���
	*/
	void GetOriginBoundLineList(TArray< TSharedPtr<FArmyLine>>& OutLineList) const
	{
		GetOutLine(WhiteVertices, OutLineList);
		GetOutLine(BlackVertices, OutLineList);
		OutLineList.Emplace(MakeShareable(new FArmyLine(CH.FirstLine_Left, CH.FirstLine_Right)));
		OutLineList.Emplace(MakeShareable(new FArmyLine(CH.SecondLine_Left, CH.SecondLine_Right)));
		OutLineList.Emplace(MakeShareable(new FArmyLine(CH.ThreeLine_Left, CH.ThreeLine_Right)));
	}

	/**
	* @brief ��ȡ�뻧������λ��
	*/
	FVector GetWorldPosition() const
	{
		return WorldPosition;
	}

	FVector GetOriginPosition() const
	{
		return WorldOriginPosition;
	}

	/**
	* @brief ��ȡ��λת��
	*/
	FTransform GetTransform() const
	{
		return Transform;
	}

private:
	//����һ��ķ���-->��ɫ
	TSharedPtr<FArmyPolygon> WhiteArrows;
	TSharedPtr<FArmyPolygon> OriginWhiteArrows;
	TArray<FVector> WhiteVertices;

	//����һ��ķ���-->��ɫ
	TSharedPtr<FArmyPolygon> BlackArrows;
	TSharedPtr<FArmyPolygon> OriginBlackArrows;
	TArray<FVector> BlackVertices;

	//���Ʒ��ݱ�־ͼ���Լ�������������
	TArray< TSharedPtr<FArmyLine>> BoundList;

	//���ݽṹ����
	ConstructHouse CH;

	//�༭��
	TSharedPtr<class FArmyEditPoint> EditPoint;

private:
	//ͼ���λ��
	FVector WorldPosition = FVector::ZeroVector;
	FVector WorldOriginPosition = FVector::ZeroVector;
	//ͼ�����ת��λ
	FVector WorldRotate   = FVector::ZeroVector;
	//��λת��
	FTransform Transform;
};
REGISTERCLASS(FArmyEntrance)