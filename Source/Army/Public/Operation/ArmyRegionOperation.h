
/**
* Copyright 2018 ������Ƽ����޹�˾.
* All Rights Reserved.
* 
*
* @File AliyunOssHttp.h
* @Description �������ɲ���
*
* @Author  fengxiaoyi
* @Date 2018��11��21��
* @Version 1.0
*/
#pragma once
#include "ArmyOperation.h"
#include "ArmyRegion.h"
#include "ArmySplitLine.h"
#include "ArmyRoom.h"
 

class  FArmyRegionOperation :public FArmyOperation
{
public:
	FArmyRegionOperation(EModelType InBelongModel);
	virtual ~FArmyRegionOperation() {}
	
	/** ��ʼ���򻮷ֲ��� */
	virtual void BeginOperation(XRArgument InArg = XRArgument()) override;
	
	/** �������򻮷ֲ��� */
	virtual void EndOperation() override;

	/** ���ƻ����� */
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;

	/** ÿ֡��׽Slide Line */
	virtual void Tick() override;
	
	/** ����������� */
	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event) override;

	/** ����ƶ� */
	virtual void MouseMove(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y) override;
	
	/** ѡ�еĹ��� */
	virtual void SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPrimitive) override;
private:
	/*------------------------------------------------------------------------------------
	*
	*	   @�Ƿ������ж��߼�:�ڴ˴����
	*
	-------------------------------------------------------------------------------------*/
	//��������
	bool IsPointInRegion(FVector vector, TSharedPtr<FArmyRegion> region);
	
	//����������
	bool IsPointOnRegionLine(FVector vector, TSharedPtr<FArmyRegion> region);
	
	//�ڷ�����
	bool IsPointOnInternelRoom(FVector vector, TSharedPtr<FArmyRoom> region);
	
	//�ڷ�������
	bool IsPointOnRoomLines(FVector vector, TSharedPtr<FArmyRoom> region);
	
	//���ཻ
	bool IsSelfIntersection(const FVector&Vector);
	
	//�������ص�
	bool IsOverlayWithRegion(const FVector&Vector, TSharedPtr<FArmyRegion> region);
	
	//�뷿���ص�
	bool IsOverlayWithRegion(const FVector&Vector, TSharedPtr<FArmyRoom> room);
	
	//���ϴε��غ�
	bool IsSameWithLastPoint(const FVector&Vector);
	
	//�����غ�
	bool IsOverlayWithSelf(const FVector&Vector);

	// �������
	bool IsSpanRegion(const FVector& vector, TArray<TWeakPtr<FArmyObject>> &RegionList);
protected:
	
	void ReDraw();

	// ɾ���ָ���
	void DeleteExecute();
	// ���������� 
	void ClearCacheData();
	
	//���ʵʱ�����
	bool AddCachePoint(const FVector& vector);
	
	//���ݻ�����ѯ����
	TSharedPtr<FArmyRegion> SearchRegionByCachePoint(const FVector& vector , TArray<TWeakPtr<FArmyObject>> &RegionList);
	
	//���ݻ�����ѯ����
	TSharedPtr<FArmyRoom> SearchRoomByCachePoint(const FVector& vector, TArray<TWeakPtr<FArmyObject>> &RoomList);

	//����з���
	void ProduceSplitLine(FString spaceId);
	
	//��ȡ������,��䵽����
	void FillRegionLinesToArray(TArray<TPair<FArmyLine, bool>>&LinesArray, TSharedPtr<FArmyRegion> room);
	
	//��ȡ������,��䵽����
	void FillRegionLinesToArray(TArray<TPair<FArmyLine, bool>>&LinesArray, TSharedPtr<FArmyRoom> room);
	
	//�߶γ��Ȼص�
	void OnLineInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);
 
	//����ĳ�����·ָ��ߵĸ���
	 int  CalculateNumberOfSplitLinesOnRegion(FString SpaceId);
	
	 //�ռ���ͬΨһ��ʶ���ŵ����зָ��߲����ݷָ��ߵ���������,�õ���˳��ķָ�
	 TArray<TSharedPtr<FArmySplitLine>> SearchSplitLinesWithSameSpaceIdOrderByIndex(FString SpaceId);
	
	 //�ռ���ͬΨһ��ʶ���µ��������ݼ���
	 TArray<TSharedPtr<FArmyRegion>>	SearchRegionWithSameSpaceId(FString SpaceId);
	
	 //����и���
	 void AddSplitLine(TSharedPtr<FArmySplitLine> SplitLine,FString SpaceId);

	 //ɾ���и���
	 void DeleteSplitLineAndSort(TSharedPtr<FArmySplitLine> SplitLine);

	 //��ȡ�и������������
	 TArray<TSharedPtr<FArmySplitLine>> GetRelativeSplitLines(TSharedPtr<FArmySplitLine> SplitLine);

	 //������������
	 void SearchRelativeSplitLines(TSet<int> &Lines,TSharedPtr<FArmySplitLine> SplitLine, TArray<TSharedPtr<FArmySplitLine>>&  LinesArray);
	  
	//�Ƴ�һ�����������е�����
	 void RemoveAllRegionInRoom(const TArray<TSharedPtr<FArmyRegion>> &RegionList);
	
	 //ˢ��ָ������
	 void RealTimeFlushRegionList(TSharedPtr<FArmyRoom> WhichRoom);
	
	 //�ӷ������ɻ�������
	 void ProduceCacheRegionByRoom(TSharedPtr<FArmyRoom> WhichRoom, TArray<TSharedPtr<FArmyRegion>> &RegionList,TSharedPtr<FArmySplitLine> Lines);
	
	 //�����������ɻ�������
	 void ProduceCacheRegionByRegionList(TArray<TSharedPtr<FArmyRegion>> &RegionList, TSharedPtr<FArmySplitLine> Lines);
	
	 //����з��ߵ�����
	 int SearchSplitLinesRegion(TArray<TSharedPtr<FArmyRegion>> &RegionList, TSharedPtr<FArmySplitLine> Lines);
	 
	 //�Ƚ����������,��������
	 void CompareSignature(TArray<TSharedPtr<FArmyRegion>> &Src, TArray<TSharedPtr<FArmyRegion>> &Dest);
	 
	 // �Ƿ���ͬ����
	 bool IsSameRegion( TSharedPtr<FArmyRegion>  &Src,  TSharedPtr<FArmyRegion> &Dest);
	
	 //�Ƿ���ͬ�����ж�
	 bool IsSameRegionPrecisely(TSharedPtr<FArmyRegion>  &Src, TSharedPtr<FArmyRegion> &Dest);

	 //���»���������
	 void UpdateRegionList(FString id);
	 
	 //����������ָ��ߵĹ�ϵ
	 void RelativeSplitLinesWithRegions(TArray<TSharedPtr<FArmyRegion>>&Regions, TArray<TSharedPtr<FArmySplitLine>>&SplitLines);

private:
	FLinearColor Color;

	// �����ɾ�������ķָ���
	TArray<TSharedPtr<FArmySplitLine>> RelativeSplitLines;
	// ѡ�еķָ���
	TSharedPtr<FArmySplitLine> SelectedSplitLine;
	//����һ�β������еĵ����� 
	TArray<FVector> CachedPoints;
	//��겶׽�� 
	TSharedPtr<class FArmyLine>		 SolidLine;
	//����
	XRArgument CurrentArg;
	//���
	TSharedPtr<class FArmyRulerLine>	RulerLine;
};