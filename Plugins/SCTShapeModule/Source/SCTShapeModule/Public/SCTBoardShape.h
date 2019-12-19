/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File SCTBoardShape.h
 * @Description 板件
 *
 * @Author 赵志强
 * @Date 2018年5月15日
 * @Version 1.0
 */
#pragma once

#include "SCTShape.h"

class FSCTOutline;

#define RefName_TW TEXT("TW")  //切角宽
#define RefName_TD TEXT("TD")  //切角深
#define RefName_SW TEXT("SW")  //斜角宽
#define RefName_SD TEXT("SD")  //斜角深
#define RefName_CW TEXT("CW")  //转角宽
#define RefName_CD TEXT("CD")  //转角深
#define RefName_TLEN TEXT("TLEN")   //点距长
#define RefName_ANGLE TEXT("ANGLE") //角度
#define RefName_SHX TEXT("SHX")     //方挖洞左上点X位置
#define RefName_SHY TEXT("SHY")     //方挖洞左上点Y位置
#define RefName_SHW TEXT("SHW")     //方挖洞宽
#define RefName_SHD TEXT("SHD")     //方挖洞深
#define RefName_SHX_LEFT TEXT("SHX_LEFT") // 矩形方洞-第一个洞上X坐标
#define RefName_SHY_LEFT TEXT("SHY_LEFT") // 矩形方洞-第一个洞Y坐标
#define RefName_SHW_LEFT TEXT("SHW_LEFT") // 矩形方洞-第一个洞W
#define RefName_SHD_LEFT TEXT("SHD_LEFT") // 矩形方洞-第一个洞D
#define RefName_SHX_RIGHT TEXT("SHX_RIGHT")	// 矩形方洞-第二个洞上X坐
#define RefName_SHY_RIGHT TEXT("SHY_RIGHT")	// 矩形方洞-第二个洞Y坐标
#define RefName_SHW_RIGHT TEXT("SHW_RIGHT")	// 矩形方洞-第二个洞W
#define RefName_SHD_RIGHT TEXT("SHD_RIGHT")	// 矩形方洞-第二个洞D

#define RefName_CHX TEXT("CHX")  //圆挖洞原点X位置
#define RefName_CHY TEXT("CHY")  //圆挖洞原点Y位置
#define RefName_CHR TEXT("CHR")  //圆挖洞半径
#define RefName_LTLEN TEXT("LTLEN")    //左圆弧点距长
#define RefName_LANGLE TEXT("LANGLE")  //左圆弧角度
#define RefName_RTLEN TEXT("RTLEN")    //右圆弧点距长
#define RefName_RANGLE TEXT("RANGLE")  //右圆弧角度

/** Pak资源所需的存储属性 */
struct FPakMetaData
{	
	int64 ID = 0;
	FString Name;
	FString Url;
	FString MD5;
	FString ThumbnailUrl;
	FString OptimizeParam;
};

// 板件型录
class SCTSHAPEMODULE_API FBoardShape : public FSCTShape
{
public:
	FBoardShape();
	virtual ~FBoardShape();

public:
	/** 解析JSon文件创建型录 */
	virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;

	/** 将型录转存为JSon文件 */
	virtual void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	/** 获取型录中的所有资源Url */
	virtual void GetResourceUrls(TArray<FString> &OutResourceUrls);
	/** 获取型录中的所有资源路径 */
	virtual void GetFileCachePaths(TArray<FString> &OutFileCachePaths);

	/** 拷贝复制型录 */
	virtual void CopyTo(FSCTShape* OutShape) override;

	/** 创建型录Actor */
	virtual ASCTShapeActor* SpawnShapeActor() override;
	/** 删除型录Actor */
	//virtual void DestroyShapeActor() override;
	/** 创建子型录用于点选的Actor */
	void SpawnActorsForSelected(FName InProfileName);
	/** 设置ProfileName */
	virtual void SetCollisionProfileName(FName InProfileName) override;

public:
	/** 设置板件宽度 */
	virtual bool SetShapeWidth(float InIntValue) override;
	virtual bool SetShapeWidth(const FString &InStrValue) override;
	/** 设置板件深度 */
	virtual bool SetShapeDepth(float InIntValue) override;
	virtual bool SetShapeDepth(const FString &InStrValue) override;

	/** 调整型录尺寸偏移量 */
	virtual bool ModifyShapeDimentionDelta(const FVector &InDelta) override;

	virtual float GetShapePosLeftSide() override;
	virtual float GetShapePosRightSide() override;
	virtual float GetShapePosFrontSide() override;
	//virtual float GetShapePosBackSide() override;
	virtual float GetShapePosTopSide() override;
	//virtual float GetShapePosBottomSide() override;
	virtual bool SetShapePosLeftSide(float InValue) override;
	virtual bool SetShapePosRightSide(float InValue) override;
	virtual bool SetShapePosFrontSide(float InValue) override;
	//virtual bool SetShapePosBackSide(float InValue) override;
	virtual bool SetShapePosTopSide(float InValue) override;
	//virtual bool SetShapePosBottomSide(float InValue) override;

public:
	/** 板件产品编码 */
	FORCEINLINE void SetProduceCode(const FString& InCode) { ProduceCode = InCode; }
	FORCEINLINE const FString& GetProduceCode() const { return ProduceCode; }

	/** 板件基材 */
	FORCEINLINE void SetSubstrateType(ESubstrateType InType) { SubstrateType = InType; }
	FORCEINLINE ESubstrateType GetSubstrateType() const { return SubstrateType; }
	FORCEINLINE void SetSubstrateID(int64 InID) { SubstrateId = InID; }
	FORCEINLINE int64 GetSubstrateID() const { return SubstrateId; }
	FORCEINLINE void SetSubstrateName(const FString& InName) { SubstrateName = InName; }
	FORCEINLINE const FString& GetSubstrateName() const { return SubstrateName; }
	const TSharedPtr<FShapeAttribute>& GetSubstrateListAttri() const;

	/** 板件材质相关接口 */
	bool SetMaterialData(const FPakMetaData & InData);
	const FPakMetaData & GetMaterialData() const { return MaterialData; }

	/** 封边相关接口 */
	bool SetEdgeBandingData(const FPakMetaData& InData);
	const FPakMetaData & GetEdgeBandingData() const;
	const TSharedPtr<FShapeAttribute>& GetEdgeMaterialTypeAttri() const;

	/** 包边相关接口 */
	bool SetCoveredEdgeMaterial(const FPakMetaData & InData);
	const FPakMetaData & GetCoveredEdgeMaterialValue() const;
	const TArray<int32> & GetLightCoveredEdges() const;
	const TSharedPtr<FShapeAttribute>& GetCoveredEdgeMaterialTypeAttri() const;

	/** 铝箔相关接口 */
	void SetUsingAluminumFild(bool InUsing);
	FORCEINLINE bool IsUsingAluminumFild() const { return bUsingAluminumFoil; }
	const FPakMetaData & GetVeneerInfoData() const { return VeneerInfoData; }
	void SetVeneerInfoData(const FPakMetaData & InPakMeta) { VeneerInfoData = InPakMeta; }

	/** 板件形状类型 */
	FORCEINLINE void SetBoardShapeType(EBoardShapeType InType) { BoardShapeType = InType; }
	FORCEINLINE EBoardShapeType GetBoardShapeType() const { return BoardShapeType; }

	/** 板件形状参数 */
	float GetShapeParamValue(int32 Index);
	float GetShapeParamValueByRefName(const FString &InRefName);
	bool AddShapeParamAttri(TArray<TSharedPtr<FShapeAttribute>> InAttriArray);
	const TSharedPtr<FShapeAttribute>& GetShapeParamAttri(int32 Index) const;
	TSharedPtr<FShapeAttribute> GetShapeParamAttriByRefName(const FString &InRefName) const;
	bool SetShapeParamValue(int32 Index, float InValue);
	bool SetShapeParamValueByRefName(const FString &InRefName, float InValue);
	bool SetShapeParamValue(int32 Index, const FString &InValue);	
	bool SetShapeParamValueByRefName(const FString &InRefName, const FString &InValue);
	FORCEINLINE int32 GetShapeParamCount() const { return ShapeParameters.Num(); }

	/** 计算板件角点坐标 */
	//OutEdgePoints 边集合
	//OutPoints 地面轮廓点集合
	//@note TArray<TArray<FVector>> ----> 边数组<顶点数组<顶点坐标>>
	void CalcBoardPoint(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints);

	/** 获取板件洞轮廓点，如果返回OutPoints为空，说明该板件不存在洞 */
	void CalcBoardHolePoints(TArray<TArray<FVector>>& OutPoints);

	bool IsNeedRebuilEdgeAndHolePointSet() const;
	/**
	* @brief 重新计算边集合
	* @param[out] OutOuterEdgePoints 外轮廓边集合	
	* @param[out]  OutInnerHolesPoints 内洞集合-可以存在多个
	* @note TArray<TArray<FVector>> ----> 边数组<顶点数组<顶点坐标>>
	*/
	void ReBuildEdge(TArray<TArray<FVector>> & OutOuterEdgePoints, TArray<TArray<FVector>> & OutInnerHolesPoints);

	/** 计算板件网格点列 */
	//void CalBoardPointsForCSGNode(TArray<FVector>& OutPoints);

	/** 设置板件轮廓 */
	void SetBoardOutline(TSharedPtr<FSCTOutline> InOutline);
	TSharedPtr<FSCTOutline> GetBoardOutline();

private:
	/** 计算板件顺时针点位 */
	//* @note TArray<TArray<FVector>> ----> 边数组<顶点数组<顶点坐标>>
	/**
	 * @brief 计算矩形板点位
	 * @param[out] OutEdgePoints 边集合
	 * @param[out] OutPoints 地面轮廓点集合
	 * @note TArray<TArray<FVector>> ----> 边数组<顶点数组<顶点坐标>>
	 */	 
	void CalcRectanglePoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints);

	/**
	 * @brief 计算左切角板点位
	 * @param[out] OutEdgePoints 边集合
	 * @param[out] OutPoints 地面轮廓点集合
	 * @note TArray<TArray<FVector>> ----> 边数组<顶点数组<顶点坐标>>
	 */
	void CalcLeftCutPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints);

   /**
	* @brief 计算右切角板点位
	* @param[out] OutEdgePoints 边集合
	* @param[out] OutPoints 地面轮廓点集合
	* @note TArray<TArray<FVector>> ----> 边数组<顶点数组<顶点坐标>>
	*/
	void CalcRightCutPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints);

	/**
	* @brief 计算左五角板点位
	* @param[out] OutEdgePoints 边集合
	* @param[out] OutPoints 地面轮廓点集合
	* @note TArray<TArray<FVector>> ----> 边数组<顶点数组<顶点坐标>>
	*/
	void CalcLeftObliquePoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints);

	/**
	* @brief 计算右五角板点位
	* @param[out] OutEdgePoints 边集合
	* @param[out] OutPoints 地面轮廓点集合
	* @note TArray<TArray<FVector>> ----> 边数组<顶点数组<顶点坐标>>
	*/
	void CalcRightObliquePoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints);

   /**
	* @brief 计算左圆弧板点位
	* @param[out] OutEdgePoints 边集合
	* @param[out] OutPoints 地面轮廓点集合
	* @note TArray<TArray<FVector>> ----> 边数组<顶点数组<顶点坐标>>
	*/
	void CalcLeftOutArcPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints);

   /**
	* @brief 计算右圆弧板点位
	* @param[out] OutEdgePoints 边集合
	* @param[out] OutPoints 地面轮廓点集合
	* @note TArray<TArray<FVector>> ----> 边数组<顶点数组<顶点坐标>>
	*/
	void CalcRightOutArcPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints);

	/**
	 * @brief 计算左圆弧转角板点位
	 * @param[out] OutEdgePoints 边集合
	 * @param[out] OutPoints 地面轮廓点集合
	 * @note TArray<TArray<FVector>> ----> 边数组<顶点数组<顶点坐标>>
	 */
	void CalcLeftInArcPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints);

   /**
	* @brief 计算右圆弧转角板点位
	* @param[out] OutEdgePoints 边集合
	* @param[out] OutPoints 地面轮廓点集合
	* @note TArray<TArray<FVector>> ----> 边数组<顶点数组<顶点坐标>>
	*/
	void CalcRightInArcPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints);

   /**
	* @brief 计算左转角板点位
	* @param[out] OutEdgePoints 边集合
	* @param[out] OutPoints 地面轮廓点集合
	* @note TArray<TArray<FVector>> ----> 边数组<顶点数组<顶点坐标>>
	*/
	void CalcLeftCornerPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints);

	/**
	* @brief 计算右转角板点位
	* @param[out] OutEdgePoints 边集合
	* @param[out] OutPoints 地面轮廓点集合
	* @note TArray<TArray<FVector>> ----> 边数组<顶点数组<顶点坐标>>
	*/
	void CalcRightCornerPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints);

   /**
	* @brief 计算右转角板点位
	* @param[out] OutEdgePoints 边集合
	* @param[out] OutPoints 地面轮廓点集合
	* @note TArray<TArray<FVector>> ----> 边数组<顶点数组<顶点坐标>>
	*/
	void CalcLeftCornerCutPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints);

	/**
	* @brief 计算右转角切角板点位
	* @param[out] OutEdgePoints 边集合
	* @param[out] OutPoints 地面轮廓点集合
	* @note TArray<TArray<FVector>> ----> 边数组<顶点数组<顶点坐标>>
	*/
	void CalcRightCornerCutPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints);

	/**
	 * @brief 计算左圆弧转角切角点位
	 * @param[out] OutEdgePoints 边集合
	 * @param[out] OutPoints 地面轮廓点集合
	 * @note TArray<TArray<FVector>> ----> 边数组<顶点数组<顶点坐标>>
	 */
	void CalcLeftInArcCutPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints);

	/**
	 * @brief 计算右圆弧转角切角点位
	 * @param[out] OutEdgePoints 边集合
	 * @param[out] OutPoints 地面轮廓点集合
	 * @note TArray<TArray<FVector>> ----> 边数组<顶点数组<顶点坐标>>
	 */
	void CalcRightInArcCutPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints);

	/**
	* @brief 计算双圆弧板点位
	* @param[out] OutEdgePoints 边集合
	* @param[out] OutPoints 地面轮廓点集合
	* @note TArray<TArray<FVector>> ----> 边数组<顶点数组<顶点坐标>>
	*/
	void CalcLeftRightOutArcPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints);

	//计算圆弧上的点位
	void CalcArcPoints(TArray<FVector>& OutPoints, 
		FVector ArcPnt0, FVector ArcPnt1, FVector CenterPnt, 
		float ArcAlph, bool bClockWise = true);

	/** 检查内部矩形挖洞板件参数是否自洽 */
	bool CheckInnerRectHoleValied(); // const;
	/** 检查内部圆挖洞板件参数是否自洽 */
	bool CheckInnerCircleHoleValied(); // cosnt;

	//板件见光判定
	TArray<int32> GetShapeDimensionMatchingTranslucent();
	bool IsPlaneCullFace(const FVector& PlaneCorePos, const FVector& Direction);

public:
	//各边延伸值(对外接口，内部考虑板件旋转)
	bool SetShapeLeftExpand(const float InIntValue);
	float GetShapeLeftExpand();
	bool SetShapeRightExpand(const float InIntValue);
	float GetShapeRightExpand();
	bool SetShapeTopExpand(const float InIntValue);
	float GetShapeTopExpand();
	bool SetShapeBottomExpand(const float InIntValue);
	float GetShapeBottomExpand();
	bool SetShapeFrontExpand(const float InIntValue);
	float GetShapeFrontExpand();
	bool SetShapeBackExpand(const float InIntValue);
	float GetShapeBackExpand();
	//获取板件宽度和深度方向延伸值和（用于更新板件尺寸）
	float GetWidthStretch();
	float GetDepthStretch();
	//获取各轴延伸值（用于更新板件位置）
	float GetAxisXStretch();
	float GetAxisYStretch();
	float GetAxisZStretch();
	//获取板件四个方向的外延值（内部接口，相对板件的延伸值，不考虑旋转）
	float GetLeftExtension() { return LeftExtension; }
	float GetRightExtension() { return RightExtension; }
	float GetBackExtension() { return BackExtension; }
	float GetFrontExtension() { return BackExtension; }

	//获取内缩值
	bool SetShapeContraction(const float InValue);
	float GetShapeContraction() const { return Contraction; }
	bool SetShapeConfluenceContraction(const float InValue);
	float GetShapeConfluenceContraction() const { return ConfluenceContraction; }
	//判定板件内缩方向
	FVector GetShapeContractionDire();

	//设置板件是否为活动板
	void SetBoardActive(bool bActive, float InValue = 18.0);
	bool IsBoardActive();
	void SetActiveRetractValue(float InValue);
	float GetActiveRetractValue();

	/** 材质旋转值 */
	float GetMaterialRotateValue() const { return MaterialRotateValue; }
	void SetMaterialRotateValue(const float InRotateValue) { MaterialRotateValue = InRotateValue; }

private:
	//板件外延值（尺寸修正）
	float LeftExtension = 0.0f;
	float RightExtension = 0.0f;
	float BackExtension = 0.0f;
	float FrontExtension = 0.0f;
	//板件内缩值（位置修正）
	float Contraction = 0.0f;
	float ConfluenceContraction = 0.0f; /*!< 受影响内缩值 门板压缩前板*/

	//标识是否为活动层板
	bool bActiveBoard = false;	
	float ActiveRetract = 0.0f;  //活层内缩值

private:
	//生产编号
	FString ProduceCode;

	//基材类型
	ESubstrateType SubstrateType;
	int64 SubstrateId = 0;
	FString SubstrateName;
	TSharedPtr<FShapeAttribute> SubstrateList;

	//材质数据
	FPakMetaData MaterialData;

	//封边数据
	FPakMetaData EdgeBandingData;
	float EdgeHeight;
	TSharedPtr<FShapeAttribute> EdgeMaterialList;

	//包边数据
	FPakMetaData CoverBandingData;
	TArray<int32> LightEdgeIndexArray;// 见光边索引
	TSharedPtr<FShapeAttribute> CoverdEdgeList;

	//铝箔数据
	bool bUsingAluminumFoil = false;
	FPakMetaData VeneerInfoData;

	//形状类型
	EBoardShapeType BoardShapeType;
	//板件形状参数
	TArray<TSharedPtr<FShapeAttribute>> ShapeParameters;

	//轮廓
	TSharedPtr<FSCTOutline> OutlineData;

	// 材质需要旋转的角度 
	float MaterialRotateValue = 0.0f;
};

