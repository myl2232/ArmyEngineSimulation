/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
 *
 * @File SCTStringTool.h
 * @Description 字符串操作工具
 *
 * @Author 赵志强
 * @Date 2018年6月25日
 * @Version 1.0
 */
#pragma once

#include "UnrealString.h"

class SCTBASETOOLMODULE_API FSCTStringTool
{
public:
	/** 验证字符串中是否只包含数字 */
	static bool VerifyStringContainNumberOnly(const FString& InStrValue);

};