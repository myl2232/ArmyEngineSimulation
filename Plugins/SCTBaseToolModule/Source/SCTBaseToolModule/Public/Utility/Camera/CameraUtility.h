#pragma once

#include "CoreMinimal.h"

namespace SCTUtility
{

	/**摄像机操作相关的一些辅助函数，不要去实例化该类*/
	class SCTBASETOOLMODULE_API FCameraUtility
	{
	public :
		/**
		 * @brief 用于计算正交摄像机的宽高尺寸
		 * @param[in] InW 聚焦物体的宽度
		 * @param[in] InD 聚焦物体的深度
		 * @param[in] InH 聚焦物体的高度
		 * @param[in] InCameraTransform 摄像机的变换矩阵
		 * @parma[out]	OutW 正交摄像机裁剪区域-宽
		 * @parma[OutH]	OutW 正交摄像机裁剪区域-宽高
		 * @note 前提条件： 聚焦物体处于的坐标原点为左下后，并且处于世界坐标原点
		 */
		static void CalOrthoProjcetionClipWidthAndHeight(const float InW, const float InD,const float InH, const FTransform & InCameraTransform, float & OutW, float & OutH);


	private:
		FCameraUtility() = delete;

	};

}