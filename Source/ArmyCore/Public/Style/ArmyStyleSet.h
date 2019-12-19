#pragma once

#include "Styling/SlateStyle.h"

class FArmyStyleSet : public FSlateStyleSet
{
public:
    FArmyStyleSet();

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

    /** 设置原始户型UICommand样式 */
    void SetHomeModeStyle();

    /** 设置拆改户型UICommand样式 */
    void SetLayoutModeStyle();

    /** 设置硬装模式UICommand样式 */
    void SetHardModeStyle();

    /** 设置硬装地面模式UICommand样式 */
    void SetHardModeFloorStyle();

    /** 设置软装模式UICommand样式 */
    void SetSoftModeStyle();

    /**	设置图纸模式UICommand样式 */
    void SetConstructionModeStyle();

    /**	设置水电模式UICommand样式 */
    void SetHydropowerModeStyle();

    void SetWHCModeStyle();

private:
	FString EngineContentRoot;
};