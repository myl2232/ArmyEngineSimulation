/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRConstructionModeController.h
 * @Description 户型绘制视图控制器
 *
 * @Author 欧石楠
 * @Date 2018年6月7日
 * @Version 1.0
 */

#pragma once

#include "ArmyDesignModeController.h"
#include "SArmyObjectOutliner.h"
#include "ArmyResourceService.h"
#include "ArmyConstructionLayerManager.h"

#include "ArmyConstructionIndexSubModeController.h"

//导出CAD
#include "ArmyZipInterface.h"
#include "ZipFileFunctionLibrary.h"
struct HomeModelInfo
{
#define INFO_ARGUMENT( ArgType, ArgName ) \
		ArgType _##ArgName; \
		HomeModelInfo& ArgName( ArgType InArg ) \
		{ \
			_##ArgName = InArg; \
			return *this; \
		}
	
	HomeModelInfo(EObjectType ArgObjType=EObjectType::OT_None) :_ArgObjType(ArgObjType) 
	{
		_ArgPosition = FVector::ZeroVector;
		_ArgBox = FBox(EForceInit::ForceInitToZero);
	}
	INFO_ARGUMENT(EObjectType, ArgObjType);
	INFO_ARGUMENT(FVector, ArgPosition);
	INFO_ARGUMENT(FBox, ArgBox);

	FORCEINLINE bool operator==(const HomeModelInfo& InOtherModelInfo) const
	{
		return
			_ArgObjType  == InOtherModelInfo._ArgObjType  &&
			(_ArgPosition - InOtherModelInfo._ArgPosition).Size()<0.001 &&
			     _ArgBox == InOtherModelInfo._ArgBox;
	}
};

class FArmyConstructionModeController : public FArmyDesignModeController
{
public:
	enum EConstrctionOperation
	{
		HO_Dimensions,
		HO_DownLeadLabel,
		HO_ObjectOperation,
		HO_Compass,
		HO_DimensionDownLead,
		HO_ARCLINE,
		HO_TextLabel,
		HO_BoxChoose,
		HO_BoradSplit
	};
	struct FAutoObjectInfo
	{
		FObjectWeakPtr ObjPtr;
		FString ObjTag;
		FName LayerName;
		FAutoObjectInfo(const FObjectWeakPtr InObj) :ObjPtr(InObj), ObjTag(TEXT("AUTO")) {}
		FAutoObjectInfo(const FObjectWeakPtr InObj, const FString& InTag,const FName& InLayerName) :ObjPtr(InObj), ObjTag(InTag),LayerName(InLayerName) {}
		bool operator==(const FAutoObjectInfo& InOther) const
		{
			return ObjPtr == InOther.ObjPtr;
		}
	};
	struct FLayerInfo
	{
		FString ObjTag;
		FName LayerName;
		bool AutoData = false;
		FLayerInfo(const FName& InLayerName,const FString& InTag,bool InAuto) : ObjTag(InTag), LayerName(InLayerName), AutoData(InAuto){}
		bool operator==(const FLayerInfo& InOther) const
		{
			return LayerName == InOther.LayerName;
		}
	};
    ~FArmyConstructionModeController();

    //~ Begin FArmyDesignModeController Interface
    virtual const EModelType GetDesignModelType() override;
    virtual void Init() override;
	virtual void InitOperations() override;
    virtual void BeginMode() override;
    virtual bool EndMode() override;
	virtual void EndOperation() override;
	virtual void Clear() override;
    virtual TSharedPtr<SWidget> MakeLeftPanelWidget() override;
    virtual TSharedPtr<SWidget> MakeRightPanelWidget() override;
    virtual TSharedPtr<SWidget> MakeToolBarWidget() override;
    virtual TSharedPtr<SWidget> MakeSettingBarWidget() override;
    //~ End FArmyDesignModeController Interface
	virtual void CollectAllLinesAndPoints(TArray< TSharedPtr<FArmyLine> >& InOutLines, TArray< TSharedPtr<FArmyPoint> >& InOutPoints, TArray<TSharedPtr<FArmyCircle>>& InOutCircles) override;
	virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI);

	virtual void Draw(FViewport* InViewport, const FSceneView* View, FCanvas* SceneCanvas);

	virtual bool InputKey(FViewport* Viewport, FKey Key, EInputEvent Event) override;

	virtual void MouseMove(FViewport* Viewport, int32 X, int32 Y) override;
	virtual bool MouseDrag(FViewport* Viewport, FKey Key) override;

	virtual void Tick(float DeltaSeconds);

	/** 加载数据 */
	virtual void Load(TSharedPtr<FJsonObject> Data) override;

	/** 保存数据 */
	virtual bool Save(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter) override;

	virtual void Delete() override;

	TWeakPtr<FArmyLayer> DelegateNewLayer(const FName& InLayerName, bool IsStaticLayer, int32 InLayerType = -1);

	TWeakPtr<FArmyLayer> NewLayer(const FName& InLayerName, bool IsStaticLayer = true);

	bool DeleteLayer(const FName& InLayerName);

	void InitClassVisible(TWeakPtr<FArmyLayer> InLayer, ObjectClassType InClassType, bool bVisible = true);

	void InitLayers(const TSharedPtr<FJsonObject> InJsonObj);
	void OnObjectOperation(const XRArgument& InArg, FObjectPtr InObj,bool bTransaction=false);

	void GetCurrentObjects(TArray<FObjectWeakPtr>& OutObjArray);

	void InitConstructionFrame(TSharedPtr<FArmyConstructionFrame> InConstructionFrame, int32 InFrameType = 0);
	void OnLoadFrameLogo(const FString& InFilePath);
	void LoadConstructionFrame(const FString& InFilePath);
	void FillConstructionFrame();

	/** @马云龙 自动调节当前Viewport缩放，显示所有内容 */
	void AdjustViewportToShowAllContent(TWeakPtr<FArmyLayer> InCurrentLayer);

	void ShotCommand(bool InStart);
	void OnLayerChanged(const FName& InLayerName);
	void OnScreenShot(FViewport* InViewport);

	void OnBaseDataChanged(int32 InDataType);
	void OnResetDismensionCommand();
	void OnUpdateDismensionCommand();

	//移动压缩的CAD文件
	void MoveZipToDir(const FString& InDirPath,const FString& From, const FString& To);

	const FString From;
	const FString To;

private:
	void DeleteSelectedObjects();
	void ClearAutoObjects();
	void ClearAutoObjectsWithClassType(TArray<ObjectClassType> ClassTypeList);
	//保存
	void OnSaveCommand();
	void OnDeleteCommand();
	//@梁晓菲 撤销
	void OnUndoCommand();
	//@梁晓菲 重做
	void OnRedoCommand();
	void OnLoadFrameCommand();
	void OnSaveImageCommand();
	void OnGenFacadeIndex();
	void IndexSubCommand(int32 InType);
	//导出CAD
	void OnExportCAD(const TArray<FName>& CADChosenList);

	/** 点击导出CAD弹出的选择框*/
	void OnExportCADDialog();

	//唤出windows选择目录窗口
	void MigratePackages();

	void OnUploadImageStep(const int32 InStep, const FString& InLayerName);
	void OnCancelUploadImage();
	void OnGeneratePrintStart(const TArray<FName>& InLayerArray);

	void OnAutoRulerCommand(int32 InType);
	bool IsAutoRulerChecked(int32 InType);

	TSharedRef<SWidget> GenerateTextTypeComboItem(TSharedPtr<FString> InItem);
	void HandleTextTypeComboChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);
	FText GetTextTypeComboText() const;

	TSharedRef<SWidget> GenerateFontSizeComboItem(TSharedPtr<int32> InItem);
	void HandleFontSizeComboChanged(TSharedPtr<int32> NewSelection, ESelectInfo::Type SelectInfo);
	FText GetFontSizeComboText() const;

	EVisibility VisibleFontSize() const;
	FReply OnFontSizeSet();
	FReply OnFontSizeRevert();

	TSharedRef<SWidget> OnAutoRulerSelected();
	TSharedRef<SWidget> CreateLayerOutliner();
	void OnAddCompass();
	void OnCommandOperation(EConstrctionOperation InOp, XRArgument InArg = XRArgument());
	void OnCustomHeight(EConstrctionOperation InOp, XRArgument InArg = XRArgument());
	void OnLayerSelect(const SArmyObjectTreeItemPtr, bool InV);
	void OnSetFontHoverd();
	void OnSetFontUnHoverd();

	TMap<unsigned long long, HomeModelInfo> GetAllObjectWithoutConstruct() const;
	TArray<EObjectType> CheckDifferentObj(TMap<unsigned long long, HomeModelInfo> LastObjList, TMap<unsigned long long, HomeModelInfo> CurrentObjList) const;
	TArray<ObjectClassType> CalcNeedChangeType(TArray<EObjectType> ObjTypeList) const;
	void ShowUpdateTitle();



private:
	bool BaseDataChanged = false;
	/** 已经更新过*/
	bool  BeenUpdated = true;
	/**	开启截屏*/
	bool StartShotScreen = false;

	int32 CurrentShotIndex = 0;

	FString FrameLogoFilePath;
	TSharedPtr<FArmyFurniture> FrameLogo;

	FName CurrentLayerName;
	TArray<FName> ShotLayerNames;

	/* 可以被捕获的object集合*/
	TArray<FObjectWeakPtr> CanSelected;

	TSharedPtr<FString> CurrentTextStyle;
	TArray<TSharedPtr<FString>> TextTypes;
	TArray<TSharedPtr<int32>> TextSizeList;
	EVisibility FontSizeVisible = EVisibility::Collapsed;
	TSharedPtr<SWidget> FontSizeSetWidget;
	TSharedPtr<class SArmyDynamicFontSize> DynamicFontSizeSetWidget;
	TSharedPtr<class SArmyProgressBar> ProgressBarDialog;
	TWeakPtr<class FArmyConstructionFrame> CurrentFrame;
	TMap<FName, TWeakPtr<class FArmyConstructionFrame>> LayerFrameMap;//每个图层的图框，可能不一样（例如：橱柜台面图框，以后会有立面图图框等）
	TMap<FObjectWeakPtr, FLayerInfo> LocalDatas;//FName is layer name,if empty then location all layers FString is parameter
	TSharedPtr<SArmyObjectOutliner> LayerOutLiner;
	TSharedPtr<FArmyConstructionLayerManager> LayerManager;
	TSharedPtr<FArmyResourceService> LocalModelService;
	TSharedPtr<FArmyConstructionIndexSubModeController> IndexSubController;

	TSharedPtr<class FArmyConstructionFunction> LocalFunction;

	TMap<EObjectType, TArray<FAutoObjectInfo>> LocalAutoObjectMap;

	TSharedPtr<STextBlock> SetFontTextBlock;
	/* @梁晓菲 可被捕捉的物体类型集合*/
	TArray<EObjectType> SelectedObjArray;
	//标注对象集合
	TArray<FObjectPtr> DemensionObjList;
	//视口大小
	FVector2D ViewportSize;
	//记录鼠标按下抬起的位置（用于判别点选或者框选）
	FVector2D PressPos;
	FVector2D ReleasePos;
	bool IsPressCtl=false;
	bool IsBoxChoose = false;
	//存储上一次切换图纸时候的全部OBJ
	TMap<unsigned long long, HomeModelInfo> LastObjMap;
	//更新弹窗
	SOverlay::FOverlaySlot* UpdateOverlay;

	//管理压缩生命周期
	TSharedPtr<FArmyCADZipInterface> DelegateInterface = MakeShareable(new FArmyCADZipInterface());
	//调用压缩方法
	TSharedPtr<FZipFileFunctionLibrary> ZipLibrary = MakeShareable(new FZipFileFunctionLibrary());

	//导出CAD唤出windows窗口选择的路径
	FString DestinationFolder;

};