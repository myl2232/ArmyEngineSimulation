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

    /** ������ɫ��ʽ */
    void SetColorStyle();

    /** ����������ʽ */
    void SetFontStyle();

    /** ����widget��ʽ */
    void SetWidgetStyle();

    /** ���ñ༭����ʽ */
    void SetEditorStyle();

private:
	FString EngineContentRoot;
};