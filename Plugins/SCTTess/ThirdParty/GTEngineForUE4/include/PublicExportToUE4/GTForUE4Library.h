#pragma once

#include "Prerequisites/PrerequisitesUtil.h"

namespace GTForUE4
{

	/** 前向声明 */
	class Vector2f;


	/**
	 * @brief 工具类，常用的工具性函数整合,目前包含如下功能
	 *	三角网化：支持单一破洞
	 * @note
	 *  坚守谁开辟，谁释放的原则，GTEngineForUE4暂时不会为返回数组开辟空间，所以如果有
	 *  需要通过数组来接受返回数组的情况，则需要提前开辟足够的空间来接纳返回数据，当然了，数据的有效
	 *  数量会跟着一块返回。	 
	 *
	 */
	class GTFORUE4_EXPORT UGTEngineLibrary
	{		
	public:
		/**
		* @brief 三角网化、 破洞处理 -- 使用单外轮廓，单内部洞,返回破洞处理之后的顶点集合以及对应的三角网索引列表
		*	@param[in] InOuterVertexArray 轮廓点集合
		*	@param[in] InOuterVertexCount 轮廓点集合个数
		*	@param[in] InInnerVertexArray 内洞点集合
		*   @param[in] InInnerVertexCount 内洞点个数
		*	@param[out] OutVertexArray，   返回破洞之后的顶点集合
		*	@param[out] OutVertexCount，   返回破洞之后的顶点个数
		*	@param[out] OutVertexIndexArray 返回破洞之后的差集顶点集合索引
		*	@param[out] OutVertexIndexCount 返回破洞之后的差集顶点索引个数
		*	@return 如果破洞成功，返回true，否则返回false
		*   @note 外轮廓与内洞点集合都采用顺时针环绕，切勿使用不同的环绕方式
		*/
		static bool TriangulatePoly(const Vector2f * InOuterVertexArray,const size_t InOuterVertexCount, const Vector2f * InInnerVertexArray,
			const size_t InInnerVertexCount,
			Vector2f * OutVertexArray, size_t * OutVertexCount, uint16_t * OutVertexIndexArray, size_t * OutVertexIndexCount);


		/**
		* @brief 三角网化 -- 根据输入的顶点集合，返回三角网之后的顶点集合以及顶点索引列表
		* @param[in]  InVertexArray 顶点轮廓点集合
		* @param[in]  InOuterVertexCount 顶点个数
		* @param[out] OutVertexIndexArray 返回三角网化之后的顶点索引列表
		* @param[out] OutVertexIndexCount 返回三角网化之后的顶点索引个数		
		* @return 如果三角网化成功，则返回true，否则返回false
		* @note 轮廓点采用顺时针环绕，逆时针环绕暂时没有经过验证
		*/
		static bool TriangulatePoly(const Vector2f * InVertexArray, const size_t InOuterVertexCount,
			uint16_t * OutVertexIndexArray, size_t * OutVertexIndexCount);

		/**
		* @brief 三角网化、 破洞处理 支持一个外轮廓，多个内洞，无论是外轮廓还是内洞支持任何环绕方式
		*	@param[in] InOuterLineVertexArray 外轮廓点集合
		*	@param[in] InVertexCount 外轮廓点集合个数
		*	@param[in] InInnerHoleVertexArray 内洞集合
		*	@param[in] InInnerHolesSizeArray  各个内洞分别所包含的顶点个数
		*   @param[in] InHolesCount   有多少个内洞
		*	@param[out] OutVertexArray 返回外轮廓与内洞整合到一起的点集合
		*	@param[out] OutVertexArrayCount 		
		*   @param[out] OutVertexIndexArray   索引集合 -- 无论轮廓点集合是何种环绕方式，返回的顶点索引都是顺时针环绕的
		*   @param[out] OutVertexIndexCount  
		*	@return 如果破洞成功，返回true，否则返回false
		*   @note Earcut Github地址 https://github.com/mapbox/earcut.hpp/blob/master/include/mapbox/earcut.hpp
		*/
		static bool TriangulatePolyWithFastEarCut(const Vector2f * InOuterLineVertexArray,
			const size_t InOuterLineVertexCount,
			const Vector2f * InInnerHoleVertexArray,
			const size_t* InInnerHolesSizeArray,
			const size_t InHolesCount,
			Vector2f * OutVertexArray, size_t * OutVertexArrayCount,
			uint16_t * OutVertexIndexArray, size_t * OutVertexIndexCount);

		/**
		*@brief 闭合多边形求差集
		*@param[in] InFirstPolygonPointSet 第一个多边形点集合
		*@param[in] InFirstPolygonPointCount 第一个多边形点集个数
		*@param[in] InSecondPolygonPointSet 第二个多边形点集
		*@param[in] InSecondPolygonPointCount 第二个多边形点集个数
		*@param[in] InFloatToIntFactor 浮点数变换为整型需要乘以的系数
		*@param[out] OutDstPolygonPointSet 第一个多边形与第二个多边形求差集之后的点集合
		*@param[out] OutDstPolygonPointCount 第一个多边形与第二个多边形求差集之后的点个数
		*@param[in]  InRetClockWise 返回的点集合是否是顺时针，默认返回的点集合是逆时针
		*@return 成功返回true，否则返回false
		*@note 虽然输入的点以及输出的点都是浮点型的，但是计算的时候会转换成整型进行计算，
		* 所以在明确项目精度的时候，先做一步转换，如果精度是0.1，则InFloatToIntFactor应该传入10
		* 后期应将此方法扩展成：输入情况为一个外轮廓，包含多个内洞，输出的点集合也有可能是多个
		*/
		static bool CalPloygonDiffenceSet(
			const Vector2f * InFirstPolygonPointSet, const size_t InFirstPolygonPointCount,
			const Vector2f * InSecondPolygonPointSet, const size_t InSecondPolygonPointCount,
			const int InFloatToIntFactor,
			Vector2f * OutDstPolygonPointSet, size_t * OutDstPolygonPointCount,
			const bool InRetClockWise = false
		);

	};

	

}
