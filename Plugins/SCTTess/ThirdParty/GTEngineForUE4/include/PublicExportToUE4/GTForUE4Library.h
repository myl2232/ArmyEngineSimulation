#pragma once

#include "Prerequisites/PrerequisitesUtil.h"

namespace GTForUE4
{

	/** ǰ������ */
	class Vector2f;


	/**
	 * @brief �����࣬���õĹ����Ժ�������,Ŀǰ�������¹���
	 *	����������֧�ֵ�һ�ƶ�
	 * @note
	 *  ����˭���٣�˭�ͷŵ�ԭ��GTEngineForUE4��ʱ����Ϊ�������鿪�ٿռ䣬���������
	 *  ��Ҫͨ�����������ܷ�����������������Ҫ��ǰ�����㹻�Ŀռ������ɷ������ݣ���Ȼ�ˣ����ݵ���Ч
	 *  ���������һ�鷵�ء�	 
	 *
	 */
	class GTFORUE4_EXPORT UGTEngineLibrary
	{		
	public:
		/**
		* @brief ���������� �ƶ����� -- ʹ�õ������������ڲ���,�����ƶ�����֮��Ķ��㼯���Լ���Ӧ�������������б�
		*	@param[in] InOuterVertexArray �����㼯��
		*	@param[in] InOuterVertexCount �����㼯�ϸ���
		*	@param[in] InInnerVertexArray �ڶ��㼯��
		*   @param[in] InInnerVertexCount �ڶ������
		*	@param[out] OutVertexArray��   �����ƶ�֮��Ķ��㼯��
		*	@param[out] OutVertexCount��   �����ƶ�֮��Ķ������
		*	@param[out] OutVertexIndexArray �����ƶ�֮��Ĳ���㼯������
		*	@param[out] OutVertexIndexCount �����ƶ�֮��Ĳ������������
		*	@return ����ƶ��ɹ�������true�����򷵻�false
		*   @note ���������ڶ��㼯�϶�����˳ʱ�뻷�ƣ�����ʹ�ò�ͬ�Ļ��Ʒ�ʽ
		*/
		static bool TriangulatePoly(const Vector2f * InOuterVertexArray,const size_t InOuterVertexCount, const Vector2f * InInnerVertexArray,
			const size_t InInnerVertexCount,
			Vector2f * OutVertexArray, size_t * OutVertexCount, uint16_t * OutVertexIndexArray, size_t * OutVertexIndexCount);


		/**
		* @brief �������� -- ��������Ķ��㼯�ϣ�����������֮��Ķ��㼯���Լ����������б�
		* @param[in]  InVertexArray ���������㼯��
		* @param[in]  InOuterVertexCount �������
		* @param[out] OutVertexIndexArray ������������֮��Ķ��������б�
		* @param[out] OutVertexIndexCount ������������֮��Ķ�����������		
		* @return ������������ɹ����򷵻�true�����򷵻�false
		* @note ���������˳ʱ�뻷�ƣ���ʱ�뻷����ʱû�о�����֤
		*/
		static bool TriangulatePoly(const Vector2f * InVertexArray, const size_t InOuterVertexCount,
			uint16_t * OutVertexIndexArray, size_t * OutVertexIndexCount);

		/**
		* @brief ���������� �ƶ����� ֧��һ��������������ڶ��������������������ڶ�֧���κλ��Ʒ�ʽ
		*	@param[in] InOuterLineVertexArray �������㼯��
		*	@param[in] InVertexCount �������㼯�ϸ���
		*	@param[in] InInnerHoleVertexArray �ڶ�����
		*	@param[in] InInnerHolesSizeArray  �����ڶ��ֱ��������Ķ������
		*   @param[in] InHolesCount   �ж��ٸ��ڶ�
		*	@param[out] OutVertexArray �������������ڶ����ϵ�һ��ĵ㼯��
		*	@param[out] OutVertexArrayCount 		
		*   @param[out] OutVertexIndexArray   �������� -- ���������㼯���Ǻ��ֻ��Ʒ�ʽ�����صĶ�����������˳ʱ�뻷�Ƶ�
		*   @param[out] OutVertexIndexCount  
		*	@return ����ƶ��ɹ�������true�����򷵻�false
		*   @note Earcut Github��ַ https://github.com/mapbox/earcut.hpp/blob/master/include/mapbox/earcut.hpp
		*/
		static bool TriangulatePolyWithFastEarCut(const Vector2f * InOuterLineVertexArray,
			const size_t InOuterLineVertexCount,
			const Vector2f * InInnerHoleVertexArray,
			const size_t* InInnerHolesSizeArray,
			const size_t InHolesCount,
			Vector2f * OutVertexArray, size_t * OutVertexArrayCount,
			uint16_t * OutVertexIndexArray, size_t * OutVertexIndexCount);

		/**
		*@brief �պ϶������
		*@param[in] InFirstPolygonPointSet ��һ������ε㼯��
		*@param[in] InFirstPolygonPointCount ��һ������ε㼯����
		*@param[in] InSecondPolygonPointSet �ڶ�������ε㼯
		*@param[in] InSecondPolygonPointCount �ڶ�������ε㼯����
		*@param[in] InFloatToIntFactor �������任Ϊ������Ҫ���Ե�ϵ��
		*@param[out] OutDstPolygonPointSet ��һ���������ڶ����������֮��ĵ㼯��
		*@param[out] OutDstPolygonPointCount ��һ���������ڶ����������֮��ĵ����
		*@param[in]  InRetClockWise ���صĵ㼯���Ƿ���˳ʱ�룬Ĭ�Ϸ��صĵ㼯������ʱ��
		*@return �ɹ�����true�����򷵻�false
		*@note ��Ȼ����ĵ��Լ�����ĵ㶼�Ǹ����͵ģ����Ǽ����ʱ���ת�������ͽ��м��㣬
		* ��������ȷ��Ŀ���ȵ�ʱ������һ��ת�������������0.1����InFloatToIntFactorӦ�ô���10
		* ����Ӧ���˷�����չ�ɣ��������Ϊһ������������������ڶ�������ĵ㼯��Ҳ�п����Ƕ��
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
