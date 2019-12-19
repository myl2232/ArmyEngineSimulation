#include "SCTStringTool.h"

bool FSCTStringTool::VerifyStringContainNumberOnly(const FString& InStrValue)
{
	bool bAllNumber = true;
	InStrValue.Replace(TEXT(" "), TEXT(""));//移除其中的空格
	const TCHAR* CharArray = InStrValue.GetCharArray().GetData();
	int32 nLen = InStrValue.Len();
	for (int32 i = 0; i < nLen; ++i)
	{
		//首字符可以为‘-’或‘+’
		if (i == 0)
		{
			if (CharArray[i] == TEXT('-') || CharArray[i] == TEXT('+'))
			{
				if (nLen > 1)
				{
					continue;
				}
			}
		}
		//字符为数字
		if (CharArray[i] >= TEXT('0') && CharArray[i] <= TEXT('9'))
		{
			continue;
		}
		//字符为小数点
		if (CharArray[i] == TEXT('.'))
		{
			continue;
		}
		
		bAllNumber = false;
		break;
	}

	return bAllNumber;
}