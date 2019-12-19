#pragma once


#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GTEngineLibrary.generated.h"


UCLASS()
class SCTTESS_API UGTEngineLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:	
	/**
	* @brief 三角网化、 破洞处理 -- 使用单外轮廓，单内部洞,返回破洞处理之后的顶点集合以及对应的三角网索引列表
	* @param[in] InOuterVertexArray 轮廓点集合
	* @param[in] InInnerVertexArray 内洞点集合
	* @param[out] OutVertexArray，   返回破洞之后的顶点集合
	* @param[out] OutVertexIndexArray 返回破洞之后的差集顶点集合索引
	* @return 如果破洞成功，返回true，否则返回false
	* @note 只适合平面挖洞，虽然输入参数中坐标点是三维的，但是只取用其中的xy坐标
	*/
	static bool TriangulatePoly(const TArray<FVector> & InOuterVertexArray, const TArray<FVector> & InInnerVertexArray,
		TArray<FVector> & OutVertexArray, TArray<uint16> & OutVertexIndexArray);


	/**
	* @brief 三角网化 -- 根据输入的顶点集合，返回三角网之后的顶点集合以及顶点索引列表
	* @param[in]  InVertexArray 顶点轮廓点集合
	* @param[out] OutVertexIndexArray 返回三角网化之后的顶点索引列表
	* @return 如果三角网化成功，则返回true，否则返回false
	* @note 只适合平面挖洞，虽然输入参数中坐标点是三维的，但是只取用其中的xy坐标
	*/
	static bool TriangulatePoly(const TArray<FVector> & InVertexArray, TArray<uint16> & OutVertexIndexArray);


	/**
	* @brief 三角网化，并缓存数据
	* @param[in] UUID 缓冲ID，可以通过该ID，直接查询缓存列表
	* @copydoc static bool TriangulatePoly(const TArray<FVector> & InOuterVertexArray, const TArray<FVector> & InInnerVertexArray,
	*	TArray<FVector> & OutVertexArray, TArray<uint16> & OutVertexIndexArray);
	* @note 只适合平面挖洞，虽然输入参数中坐标点是三维的，但是只取用其中的xy坐标
	*/
	static bool TriangulatePolyWithDataCache(const FString & UUID, const TArray<FVector> & InOuterVertexArray, const TArray<FVector> & InInnerVertexArray,
		TArray<FVector> & OutVertexArray, TArray<uint16> & OutVertexIndexArray);

	/**
	* @brief 三角网化，并缓存数据
	* @param[in] UUID 缓冲ID，可以通过该ID，直接查询缓存列表
	* @copydoc static bool TriangulatePoly(const TArray<FVector> & InVertexArray,TArray<uint16> & OutVertexIndexArray);
	* @note 只适合平面挖洞，虽然输入参数中坐标点是三维的，但是只取用其中的xy坐标
	*/
	static bool TriangulatePolyWithDataCache(const FString & UUID, const TArray<FVector> & InVertexArray, TArray<uint16> & OutVertexIndexArray);


	/**
	* @brief 获取不带三角网缓存数据
	* @param[in] InUUID
	* @param[out] OutIndexArray 存储顶点索引列表
	* @return 如果已经缓冲则返回true，否则返回false	
	*/
	static bool GetCacheData(const FString & InUUID, TArray<uint16> & OutIndexArray);


	/**
	*@brief 获取带洞三角网存数据
	*@param[in] InUUID
	*@param[out] OutIndexArray 存储顶点索引列表
	*@param[out] OutVertexArray  存储顶点坐标列表
	*/
	static bool GetCacheData(const FString & InUUID, TArray<FVector> & OutVertexArray, TArray<uint16> & OutIndexArray);



	/**
	* @brief 三角网化 -- 根据输入的顶点集合，返回三角网之后的顶点集合以及顶点索引列表
	* @param[in]  InOuterlineVertexArray 顶点外轮廓点集合
	* @param[in]  InHoles 内洞集合
	* @param[out] OutVertices 将轮廓与洞整合成一个整体的数组返回
	* @param[out] OutVertexIndexArray 索引集合
	* @param[out] InRetIndeciesClockWise 为true，则返回的索引为顺时针环绕，为false，返回的索引为逆时针环绕
	* @return 如果三角网化成功，则返回true，否则返回false
	* @note 只适合平面挖洞，虽然输入参数中坐标点是三维的，但是只取用其中的xy坐标
	*/
	static bool TriangulatePolyWithFastEarcut(const TArray<FVector> & InOuterlineVertexArray, const TArray<TArray<FVector>> & InHoles,
	  TArray<FVector> & OutVertices,TArray<uint16> & OutVertexIndexArray,const bool InRetIndeciesClockWise = true);


	/**
	 *@brief 计算两个闭合多边形的差集
	 *@param[in]  InFirstVertexArray 第一个多边形点集合
	 *@param[in]  InSecondVertexArray 第二个多边形点集合
	 *@param[out] OutVertexArray 差集点集合
	 *@param[in] InRetClockWise 返回的点集合是否需要时顺时针，默认为true
	 *@return 如果执行成功，则，发挥true，否则返回false
	 *@note 该函数应该扩展成，一个多边形附带多个内洞，并且返回的差集多边形也可以是多个的情况
	 */
	static bool  CalPloygonDiffenceSet(const TArray<FVector> & InFirstVertexArray, const TArray<FVector> & InSecondVertexArray,
		TArray<FVector> & OutVertexArray, const bool InRetClockWise = true);



};