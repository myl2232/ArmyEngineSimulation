#pragma once

#include "Styling/SlateStyle.h"

class FSCTXRStyleSet : public FSlateStyleSet
{
public:
	FSCTXRStyleSet();

    void Initialize();
	void SetEngineContentRoot(const FString& InEngineContentRootDir);
	FString RootToEngineContentDir(const FString& RelativePath, const TCHAR* Extension);
    void SetupDrawPanelStyle();

    /** 设置颜色样式 */
    void SetColorStyle();

    /** 设置字体样式 */
    void SetFontStyle();

    /** 设置widget样式 */
    void SetWidgetStyle();

    /** 设置编辑器样式 */
    void SetEditorStyle();

private:
	FString EngineContentRoot;
};