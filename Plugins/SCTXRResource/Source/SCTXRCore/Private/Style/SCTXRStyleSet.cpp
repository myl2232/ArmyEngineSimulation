  #include "SCTXRStyleSet.h"
#include "Styling/SlateTypes.h"
#include "CoreStyle.h"

#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(FPaths::ProjectContentDir() / "Slate" / RelativePath + TEXT(".png"), __VA_ARGS__)
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush(RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush(RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo(RootToEngineContentDir(RelativePath, TEXT(".ttf")), __VA_ARGS__)
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo(RootToEngineContentDir(RelativePath, TEXT(".otf")), __VA_ARGS__)

const FVector2D Icon1x1(1.f, 1.f);
const FVector2D Icon8x8(8.f, 8.0f);
const FVector2D Icon12x12(12.f, 12.0f);
const FVector2D Icon14x14(14.f, 14.0f);
const FVector2D Icon16x16(16.f, 16.0f);
const FVector2D Icon18x18(18.f, 18.0f);

const FVector2D Icon24x24(24.f, 24.0f);
const FVector2D Icon32x32(32.f, 32.0f);
const FVector2D Icon56x56(56.f, 56.0f);
const FVector2D Icon64x64(64.0f, 64.0f);
const FVector2D Icon72x72(72.0f, 72.0f);
const FVector2D Icon128x128(128.0f, 128.0f);
const FVector2D Icon156x156(156.f, 156.f);
const FVector2D Icon420x420(420.f, 420.f);

FSCTXRStyleSet::FSCTXRStyleSet()
    : FSlateStyleSet("SCTXRStyle")
{
}

void FSCTXRStyleSet::Initialize()
{
	SetContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));
	SetEngineContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

    SetupDrawPanelStyle();
}

void FSCTXRStyleSet::SetEngineContentRoot(const FString& InContentRootDir)
{
	EngineContentRoot = InContentRootDir;
}

FString FSCTXRStyleSet::RootToEngineContentDir(const FString& RelativePath, const TCHAR* Extension)
{
	return (EngineContentRoot / RelativePath) + Extension;
}
void FSCTXRStyleSet::SetupDrawPanelStyle()
{
    SetColorStyle();
    SetFontStyle();
    SetWidgetStyle();
    SetEditorStyle();

    Set("Launcher.LoginView", new IMAGE_BRUSH("Icons/T_LoginView", FVector2D(812, 720)));
    Set("Launcher.LoginBackground", new IMAGE_BRUSH("Icons/T_LoginBackground", FVector2D(1920, 1061)));
    Set("Launcher.LoginBackground1", new IMAGE_BRUSH("Icons/T_LoginBackground1", FVector2D(468, 768)));
    Set("Launcher.Logo", new IMAGE_BRUSH("Icons/T_Logo", FVector2D(328, 60)));
    Set("MainPage.Logo", new IMAGE_BRUSH("Icons/T_Logo", FVector2D(260, 48)));
    Set("Icon.Username", new IMAGE_BRUSH("Icons/T_Username", Icon24x24));
    Set("Icon.Password", new IMAGE_BRUSH("Icons/T_Pwd", Icon24x24));
    Set("Icon.Throbber", new IMAGE_BRUSH("Icons/T_Throbber", Icon24x24));
    Set("Icon.NewProject", new IMAGE_BRUSH("Icons/T_NewProject", Icon420x420));
    Set("Icon.DefaultImage", new IMAGE_BRUSH("Icons/T_DefaultImage", Icon420x420));
    Set("Icon.Refresh", new IMAGE_BRUSH("Icons/T_Refresh", Icon24x24));
    Set("Icon.UpArrow", new IMAGE_BRUSH("Icons/T_UpArrow", Icon12x12));
    Set("Icon.DownArrow", new IMAGE_BRUSH("Icons/T_DownArrow", Icon12x12));
	Set("Icon.Newest", new IMAGE_BRUSH("Icons/Newest", Icon32x32));
	Set("Icon.ResourceReady", new IMAGE_BRUSH("Icons/ResourceReady", Icon56x56));
	Set("Icon.Close", new IMAGE_BRUSH("Icons/T_Close", Icon32x32));
	Set("Icon.Error", new IMAGE_BRUSH("Icons/T_Error", Icon32x32));
	Set("Icon.Left_Top", new IMAGE_BRUSH("Icons/Left_Top", Icon24x24));
	Set("Icon.Mid_Top", new IMAGE_BRUSH("Icons/Mid_Top", Icon24x24));
	Set("Icon.Right_Top", new IMAGE_BRUSH("Icons/Right_Top", Icon24x24));
	Set("Icon.Left_Mid", new IMAGE_BRUSH("Icons/Left_Mid", Icon24x24));
	Set("Icon.Mid_Mid", new IMAGE_BRUSH("Icons/Mid_Mid", Icon24x24));
	Set("Icon.Right_Mid", new IMAGE_BRUSH("Icons/Right_Mid", Icon24x24));
	Set("Icon.Left_Down", new IMAGE_BRUSH("Icons/Left_Down", Icon24x24));
	Set("Icon.Mid_Down", new IMAGE_BRUSH("Icons/Mid_Down", Icon24x24));
	Set("Icon.Right_Down", new IMAGE_BRUSH("Icons/Right_Down", Icon24x24));
	Set("Icon.None", new IMAGE_BRUSH("Icons/White", Icon24x24, GetColor("Color.None")));

	//地板拼花临时数据加载，待服务器数据构造完成可删除此部分
	Set("Icon.Space_400_600", new IMAGE_BRUSH("Icons/Thumbnail/Tile/Interlace/Space_400_600", Icon128x128));
	Set("Icon.Texture", new IMAGE_BRUSH("Icons/Thumbnail/Tile/Continue/Texture", Icon128x128));

    {
        Set("Icon.CompanyLabrary.Normal", new IMAGE_BRUSH("Icons/T_CompanyLabrary", Icon24x24, GetColor("Color.FF9D9FA5")));
        Set("Icon.CompanyLabrary.Hovered", new IMAGE_BRUSH("Icons/T_CompanyLabrary", Icon24x24, GetColor("Color.FFFD9800")));

        Set("Icon.ProjectLabrary.Normal", new IMAGE_BRUSH("Icons/T_ProjectLabrary", Icon24x24, GetColor("Color.FF9D9FA5")));
        Set("Icon.ProjectLabrary.Hovered", new IMAGE_BRUSH("Icons/T_ProjectLabrary", Icon24x24, GetColor("Color.FFFD9800")));

        Set("Icon.PersonalLabrary.Normal", new IMAGE_BRUSH("Icons/T_PersonalLabrary", Icon24x24, GetColor("Color.FF9D9FA5")));
        Set("Icon.PersonalLabrary.Hovered", new IMAGE_BRUSH("Icons/T_PersonalLabrary", Icon24x24, GetColor("Color.FFFD9800")));

        Set("Icon.Favorite.Normal", new IMAGE_BRUSH("Icons/T_Favorite", Icon24x24, GetColor("Color.FF9D9FA5")));
        Set("Icon.Favorite.Hovered", new IMAGE_BRUSH("Icons/T_Favorite", Icon24x24, GetColor("Color.FFFD9800")));
    }

    Set("Splitter.Thin", new IMAGE_BRUSH("Icons/White", FVector2D(1, 1), GetColor("Color.FF404040")));
    Set("Splitter.Thin1", new IMAGE_BRUSH("Icons/White", FVector2D(1, 1), GetColor("Color.FF28292B")));
    Set("Splitter.Thin.V", new IMAGE_BRUSH("Icons/White", FVector2D(2, 1), GetColor("Color.FF191A1D")));
    Set("Splitter.Thin.H", new IMAGE_BRUSH("Icons/White", FVector2D(1, 2), GetColor("Color.FF191A1D")));

    // Levels General
    {
        Set("Level.VisibleIcon16x", new IMAGE_BRUSH("Icons/icon_levels_visible_16px", Icon16x16));
        Set("Level.VisibleHighlightIcon16x", new IMAGE_BRUSH("Icons/icon_levels_visible_hi_16px", Icon16x16));
        Set("Level.NotVisibleIcon16x", new IMAGE_BRUSH("Icons/icon_levels_invisible_16px", Icon16x16));
        Set("Level.NotVisibleHighlightIcon16x", new IMAGE_BRUSH("Icons/icon_levels_invisible_hi_16px", Icon16x16));
    }
}

void FSCTXRStyleSet::SetColorStyle()
{
    // 常用颜色
    Set("Color.None", FLinearColor(FColor(0X00000000)));
    Set("Color.FF191A1D", FLinearColor(FColor(0XFF191A1D)));
    Set("Color.FF28292B", FLinearColor(FColor(0XFF28292B)));
    Set("Color.FF303133", FLinearColor(FColor(0XFF303133)));
    Set("Color.FF343539", FLinearColor(FColor(0XFF343539)));
    Set("Color.FF353535", FLinearColor(FColor(0XFF353535)));
    Set("Color.FF37383C", FLinearColor(FColor(0XFF37383C)));
    Set("Color.FF4C4D52", FLinearColor(FColor(0XFF4C4D52)));
    Set("Color.FF656870", FLinearColor(FColor(0XFF656870)));
    Set("Color.FF979797", FLinearColor(FColor(0XFF979797)));
    Set("Color.489D9FA5", FLinearColor(FColor(0X489D9FA5)));
    Set("Color.FF9D9FA5", FLinearColor(FColor(0XFF9D9FA5)));
    Set("Color.FFB5B7BE", FLinearColor(FColor(0XFFB5B7BE)));
    Set("Color.FFFD9800", FLinearColor(FColor(0XFFFD9800)));
    Set("Color.FFFF5A5A", FLinearColor(FColor(0XFFFF5A5A)));
    Set("InputBox.Gray", FLinearColor(FColor(0X21FFFFFF)));
    Set("Text.DarkGray", FLinearColor(FColor(0XFF666666)));
    Set("Color.FF404040", FLinearColor(FColor(0XFF404040)));
    Set("Color.Gray.FF111111", FLinearColor(FColor(0XFF111111)));
}

void FSCTXRStyleSet::SetFontStyle()
{
    //常用字号的字体
    const FTextBlockStyle NormalText = FTextBlockStyle()
        .SetFont(TTF_FONT("Fonts/Roboto-Regular", 9))
        .SetColorAndOpacity(FSlateColor(FLinearColor::White))
        .SetShadowOffset(FVector2D::ZeroVector);

    const FTextBlockStyle XRText_8 = FTextBlockStyle(NormalText)
        .SetFont(TTF_FONT("Fonts/Roboto-Regular", 8))
        .SetColorAndOpacity(FSlateColor(FLinearColor::White))
        .SetShadowOffset(FVector2D::ZeroVector);

    const FTextBlockStyle XRText_10 = FTextBlockStyle(NormalText)
        .SetFont(TTF_FONT("Fonts/Roboto-Regular", 10))
        .SetColorAndOpacity(FSlateColor(FLinearColor::White))
        .SetShadowOffset(FVector2D::ZeroVector);

    const FTextBlockStyle XRText_12 = FTextBlockStyle(NormalText)
        .SetFont(TTF_FONT("Fonts/Roboto-Regular", 12))
        .SetColorAndOpacity(FSlateColor(FLinearColor::White))
        .SetShadowOffset(FVector2D::ZeroVector);

    const FTextBlockStyle XRText_14 = FTextBlockStyle(NormalText)
        .SetFont(TTF_FONT("Fonts/Roboto-Regular", 14))
        .SetColorAndOpacity(FSlateColor(FLinearColor::White))
        .SetShadowOffset(FVector2D::ZeroVector);

    const FTextBlockStyle XRText_16 = FTextBlockStyle(NormalText)
        .SetFont(TTF_FONT("Fonts/Roboto-Regular", 16))
        .SetColorAndOpacity(FSlateColor(FLinearColor::White))
        .SetShadowOffset(FVector2D::ZeroVector);

    const FEditableTextBoxStyle XRText_18 = FEditableTextBoxStyle();

    Set("ArmyText_8", XRText_8);
    Set("ArmyText_9", NormalText);
    Set("ArmyText_10", XRText_10);
    Set("ArmyText_12", XRText_12);
    Set("ArmyText_14", XRText_14);
    Set("ArmyText_16", XRText_16);
    Set("ArmyText_18", XRText_18);
}

void FSCTXRStyleSet::SetWidgetStyle()
{
    // EditableTextBoxStyle
    {
        Set("ArmyEditableTextBox", FEditableTextBoxStyle()
            .SetBackgroundImageNormal(FSlateNoResource())
            .SetBackgroundImageHovered(FSlateNoResource())
            .SetBackgroundImageFocused(FSlateNoResource())
            .SetBackgroundImageReadOnly(FSlateNoResource())
            .SetPadding(FMargin(0))
            .SetFont(TTF_FONT("Fonts/Roboto-Regular", 10))
            .SetForegroundColor(GetColor("Color.FFB5B7BE"))
        );

        Set("EditableTextBox.DarkGray", FEditableTextBoxStyle()
            .SetBackgroundImageNormal(FSlateNoResource())
            .SetBackgroundImageHovered(FSlateNoResource())
            .SetBackgroundImageFocused(FSlateNoResource())
            .SetBackgroundImageReadOnly(FSlateNoResource())
            .SetPadding(FMargin(0))
            .SetFont(TTF_FONT("Fonts/Roboto-Regular", 12))
            .SetForegroundColor(GetColor("Text.DarkGray"))
        );

        Set("EditableTextBox.Gray", FEditableTextBoxStyle()
            .SetBackgroundImageNormal(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.FF37383C")))
            .SetBackgroundImageHovered(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.FF28292B")))
            .SetBackgroundImageFocused(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.FF37383C")))
            .SetBackgroundImageReadOnly(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.FF37383C")))
            .SetFont(TTF_FONT("Fonts/Roboto-Regular", 10))
            .SetForegroundColor(GetColor("Color.FF9D9FA5"))
        );
    }

    // CheckBoxStyle
    {
        Set("CheckBox.Orange", FCheckBoxStyle()
            .SetUncheckedImage(IMAGE_BRUSH("Icons/T_Unchecked", Icon14x14))
            .SetUncheckedHoveredImage(IMAGE_BRUSH("Icons/T_Unchecked", Icon14x14))
            .SetUncheckedPressedImage(IMAGE_BRUSH("Icons/T_Unchecked", Icon14x14))
            .SetCheckedImage(IMAGE_BRUSH("Icons/T_Checked", Icon14x14))
            .SetCheckedHoveredImage(IMAGE_BRUSH("Icons/T_Checked", Icon14x14))
            .SetCheckedPressedImage(IMAGE_BRUSH("Icons/T_Checked", Icon14x14))
            .SetPadding(FMargin(0))
        );

        Set("CheckBox.Normal", FCheckBoxStyle()
            .SetUncheckedImage(IMAGE_BRUSH("Icons/T_Layer_Unchecked", Icon12x12))
            .SetUncheckedHoveredImage(IMAGE_BRUSH("Icons/T_Layer_Unchecked", Icon12x12))
            .SetUncheckedPressedImage(IMAGE_BRUSH("Icons/T_Layer_Unchecked", Icon12x12))
            .SetCheckedImage(IMAGE_BRUSH("Icons/T_Layer_Checked", Icon12x12))
            .SetCheckedHoveredImage(IMAGE_BRUSH("Icons/T_Layer_Checked", Icon12x12))
            .SetCheckedPressedImage(IMAGE_BRUSH("Icons/T_Layer_Checked", Icon12x12))
            .SetPadding(FMargin(0))
        );
    }

    // ComboBoxStyle
    {
        Set("ComboBox.Button.Gray", FButtonStyle()
            .SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF37383C")))
            .SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF28292B")))
            .SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF37383C")))
            .SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF37383C")))
        );
    }

    // TableRowStyle
    {
        Set("TableRow.Gray", FTableRowStyle()
            .SetSelectorFocusedBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.None")))
            .SetActiveHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF28292B")))
            .SetActiveBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF28292B")))
            .SetInactiveHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF28292B")))
            .SetInactiveBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.None")))
            .SetEvenRowBackgroundHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF28292B")))
            .SetEvenRowBackgroundBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.None")))
            .SetOddRowBackgroundHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF28292B")))
            .SetOddRowBackgroundBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.None")))
            .SetTextColor(GetColor("Color.FF9D9FA5"))
            .SetSelectedTextColor(FLinearColor::White)
        );
    }

    // ButtonStyle
    {
        Set("Button.Orange.Pressed", FLinearColor(FColor(0XFFFC8300)));
        Set("Button.Orange", FButtonStyle()
            .SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FFFD9800")))
            .SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Button.Orange.Pressed")))
            .SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Button.Orange.Pressed")))
            .SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FFFD9800")))
        );

        Set("Button.None", FButtonStyle()
            .SetNormal(FSlateNoResource())
            .SetHovered(FSlateNoResource())
            .SetPressed(FSlateNoResource())
            .SetDisabled(FSlateNoResource())
        );

        Set("Button.FF4C4D52", FButtonStyle()
            .SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF4C4D52")))
            .SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF656870")))
            .SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF4C4D52")))
            .SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF4C4D52")))
        );

        Set("Button.FF343539", FButtonStyle()
            .SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF343539")))
            .SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF343539")))
            .SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF343539")))
            .SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF343539")))
        );

        Set("Button.FF28292B", FButtonStyle()
            .SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF28292B")))
            .SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF353535")))
            .SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF353535")))
            .SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF28292B")))
        );

		Set("Button.PureFF303133", FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF303133")))
			.SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF303133")))
			.SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF303133")))
			.SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF303133")))
		);

        Set("Button.FF303133", FButtonStyle()
            .SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF303133")))
            .SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF28292B")))
            .SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF303133")))
            .SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF303133")))
		);

		Set("Button.FF9D9FA5", FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF9D9FA5")))
			.SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF9D9FA5")))
			.SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF9D9FA5")))
			.SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF9D9FA5")))
		);

		Set("Button.FF191A1D", FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FFFD9800")))
			.SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF191A1D")))
			.SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF191A1D")))
			.SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF191A1D")))
		);

        Set("Button.Search", FButtonStyle()
            .SetNormal(IMAGE_BRUSH("Icons/T_Search", Icon16x16, GetColor("Color.FF9D9FA5")))
            .SetHovered(IMAGE_BRUSH("Icons/T_Search", Icon16x16, GetColor("Color.Gray.FF111111")))
            .SetPressed(IMAGE_BRUSH("Icons/T_Search", Icon16x16, GetColor("Color.FF9D9FA5")))
            .SetDisabled(IMAGE_BRUSH("Icons/T_Search", Icon16x16, GetColor("Color.FF9D9FA5")))
        );

		Set("Button.Close", FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/T_Close", Icon16x16, GetColor("Color.FF9D9FA5")))
			.SetHovered(IMAGE_BRUSH("Icons/T_Close", Icon16x16, GetColor("Color.Gray.FF111111")))
			.SetPressed(IMAGE_BRUSH("Icons/T_Close", Icon16x16, GetColor("Color.FF9D9FA5")))
			.SetDisabled(IMAGE_BRUSH("Icons/T_Close", Icon16x16, GetColor("Color.FF9D9FA5")))
		);
    }

    // ScrollBarStyle
    {
        Set("ScrollBar.Style.Gray", FScrollBoxStyle()
            .SetTopShadowBrush(BOX_BRUSH("Common/ScrollBoxShadowTop", FVector2D(16, 8), FMargin(0.5, 1, 0.5, 0), FLinearColor(FColor(0X320B0B0C))))
            .SetBottomShadowBrush(BOX_BRUSH("Common/ScrollBoxShadowBottom", FVector2D(16, 8), FMargin(0.5, 0, 0.5, 1), FLinearColor(FColor(0X040B0B0C))))
        );

        Set("ScrollBar.BarStyle.Gray", FScrollBarStyle()
            .SetVerticalTopSlotImage(FSlateNoResource())
            .SetVerticalBottomSlotImage(FSlateNoResource())
            .SetNormalThumbImage(IMAGE_BRUSH("Icons/White", FVector2D(4, 4), GetColor("Color.FF191A1D")))
            .SetHoveredThumbImage(IMAGE_BRUSH("Icons/White", FVector2D(4, 4), GetColor("Color.FF9D9FA5")))
            .SetDraggedThumbImage(IMAGE_BRUSH("Icons/White", FVector2D(4, 4), GetColor("Color.FF9D9FA5")))
        );
    }

    // SpinBoxStyle
    {
        Set("SpinBoxStyle.Gray", FSpinBoxStyle()
            .SetBackgroundBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF28292B")))
            .SetHoveredBackgroundBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF353535")))
            .SetActiveFillBrush(IMAGE_BRUSH("Icons/SpinBoxUnderLine", Icon24x24, GetColor("Color.FFFD9800")))
            .SetInactiveFillBrush(IMAGE_BRUSH("Icons/SpinBoxUnderLine", Icon24x24, GetColor("Color.FFFD9800")))
            .SetArrowsImage(IMAGE_BRUSH("Icons/SpinArrows", Icon12x12, GetColor("Color.FF9D9FA5")))
            .SetTextPadding(FMargin(2))
        );
    }

	{
		FEditableTextBoxStyle InlineEditableTextBlockEditable = FEditableTextBoxStyle()
			.SetFont(TTF_FONT("Fonts/Roboto-Regular", 8))
			.SetBackgroundImageNormal(BOX_BRUSH("Common/TextBox", FMargin(4.0f / 16.0f)))
			.SetBackgroundImageHovered(BOX_BRUSH("Common/TextBox_Hovered", FMargin(4.0f / 16.0f)))
			.SetBackgroundImageFocused(BOX_BRUSH("Common/TextBox_Hovered", FMargin(4.0f / 16.0f)))
			.SetBackgroundImageReadOnly(BOX_BRUSH("Common/TextBox_ReadOnly", FMargin(4.0f / 16.0f)))
			.SetScrollBarStyle(FCoreStyle::Get().GetWidgetStyle<FScrollBarStyle>("Scrollbar"));

		FInlineEditableTextBlockStyle InlineEditableTextBlockStyle = FInlineEditableTextBlockStyle()
			.SetTextStyle(GetWidgetStyle<FTextBlockStyle>("ArmyText_8"))
			.SetEditableTextBoxStyle(InlineEditableTextBlockEditable);

		Set("InlineEditableTextBlockStyle", InlineEditableTextBlockStyle);
	}
}

void FSCTXRStyleSet::SetEditorStyle()
{
    // 场景编辑器工具栏
    Set("ArmyLevelEditor.ClearLevel", new IMAGE_BRUSH("Icons/Toolbar/ClearLevel", Icon24x24));
    Set("ArmyLevelEditor.NewHomeInLevel", new IMAGE_BRUSH("Icons/Toolbar/CreateHome", Icon24x24));
    Set("ArmyLevelEditor.EditLevelHome", new IMAGE_BRUSH("Icons/Toolbar/EditHome", Icon24x24));
    Set("ArmyLevelEditor.ImportHomeToLevel", new IMAGE_BRUSH("Icons/Toolbar/ImportHome", Icon24x24));
    Set("ArmyLevelEditor.Panoramas", new IMAGE_BRUSH("Icons/Toolbar/Panorama", Icon24x24));
    Set("ArmyLevelEditor.SaveLevel", new IMAGE_BRUSH("Icons/Toolbar/SavePlan", Icon24x24));
    Set("ArmyLevelEditor.VRMode", new IMAGE_BRUSH("Icons/Toolbar/VR", Icon24x24));
    Set("ArmyLevelEditor.DesignSketch", new IMAGE_BRUSH("Icons/Toolbar/VR", Icon24x24));

	Set("FHomeEditor.TestCommandInfo", new IMAGE_BRUSH("Icons/Newest", Icon32x32));

	Set("FHomeEditor.CommandClassDrawWall",  new IMAGE_BRUSH("Icons/Component/Base_Line", Icon72x72));
	Set("FHomeEditor.CommandClassDrawRectWall", new IMAGE_BRUSH("Icons/Component/Base_RectLine", Icon72x72));

	Set("FHomeEditor.CommandClassDrawArcWall", new IMAGE_BRUSH("Icons/Component/Base_ArcWall", Icon72x72));
	Set("FHomeEditor.CommandClassBeam", new IMAGE_BRUSH("Icons/Component/Beam", Icon72x72));
	Set("FHomeEditor.CommandClassAddWall", new IMAGE_BRUSH("Icons/Component/Base_AddWall", Icon72x72));
	Set ("FHomeEditor.CommandClassFlue",new IMAGE_BRUSH ("Icons/Component/FLue",Icon72x72));

	Set("FHomeEditor.CommandClassDrawSpace",  new IMAGE_BRUSH("Icons/Component/Base_SpaceLine", Icon72x72));
	Set("FHomeEditor.CommandClassDrawMainWallSpace", new IMAGE_BRUSH("Icons/Component/Base_MainWallSpace", Icon72x72));

	Set("FHomeEditor.CommandClassDrawRectSpace", new IMAGE_BRUSH("Icons/Component/Base_RectSpaceLine", Icon72x72));
	Set("FHomeEditor.CommandClassDrawCrossDoorStone",new IMAGE_BRUSH("Icons/Component/CrossDoorStone",Icon72x72));

	Set("FHomeEditor.CommandClassStandardDoor",  new IMAGE_BRUSH("Icons/Component/Base_Door", Icon72x72));
    Set("FHomeEditor.CommandClassSlidingDoor", new IMAGE_BRUSH("Icons/Component/T_SlidingDoor", Icon72x72));
    Set("FHomeEditor.CommandClassStandardPass", new IMAGE_BRUSH("Icons/Component/T_Pass", Icon72x72));
	Set("FHomeEditor.CommandClassStandardWindow",  new IMAGE_BRUSH("Icons/Component/Base_Window", Icon72x72));
	Set("FHomeEditor.CommandClassFloorWindow", new IMAGE_BRUSH("Icons/Component/Base_FloorWindow", Icon72x72));
	Set("FHomeEditor.CommandClassArcWindow", new IMAGE_BRUSH("Icons/Component/Base_ArcWindow", Icon72x72));

	Set("FHomeEditor.CommandClassRectBayWindow", new IMAGE_BRUSH("Icons/Component/Rect_BayWindow", Icon72x72));
	Set("FHomeEditor.CommandClassTrapeBayWindow", new IMAGE_BRUSH("Icons/Component/Trape_BayWindow", Icon72x72));
	Set("FHomeEditor.CommandClassCornerBayWindow", new IMAGE_BRUSH("Icons/Component/Corner_BayWindow", Icon72x72));

	Set("FHomeEditor.CommandClassSkirtingLine",  new IMAGE_BRUSH("Icons/Component/Base_SkirtingLine", Icon72x72));
	Set("FHomeEditor.CommandClassDelWall",  new IMAGE_BRUSH("Icons/Component/Base_DelWall", Icon72x72));
	Set("FHomeEditor.CommandClassSocket_Point",  new IMAGE_BRUSH("Icons/Component/Socket", Icon72x72));

	Set("FHomeEditor.CommandClassSocket_Point", new IMAGE_BRUSH("Icons/Component/Socket", Icon72x72));
	Set("FHomeEditor.CommandClassSocketFive_Point", new IMAGE_BRUSH("Icons/Component/Socket_Five", Icon72x72));
	Set("FHomeEditor.CommandClassSocketFiveAndOn_Point", new IMAGE_BRUSH("Icons/Component/Socket_Five_On", Icon72x72));
	Set("FHomeEditor.CommandClassSocketFour_Point", new IMAGE_BRUSH("Icons/Component/Socket_Four", Icon72x72));
	Set("FHomeEditor.CommandClassSocketFourAndOn_Point", new IMAGE_BRUSH("Icons/Component/Socket_Four_On", Icon72x72));
	Set("FHomeEditor.CommandClassSocketIT_Point", new IMAGE_BRUSH("Icons/Component/Socket_IT", Icon72x72));
	Set("FHomeEditor.CommandClassSocketThreeAndOn_Point", new IMAGE_BRUSH("Icons/Component/Socket_On", Icon72x72));
	Set("FHomeEditor.CommandClassSocketTP_Point", new IMAGE_BRUSH("Icons/Component/Socket_TP", Icon72x72));
	Set("FHomeEditor.CommandClassSocketTV_Point", new IMAGE_BRUSH("Icons/Component/Socket_TV", Icon72x72));
	Set("FHomeEditor.CommandClassSocketTVAndIT_Point", new IMAGE_BRUSH("Icons/Component/Socket_TVAndIT", Icon72x72));


	Set("FHomeEditor.CommandClassEleBoxL_Point",  new IMAGE_BRUSH("Icons/Component/EleBoxL_Point", Icon72x72));//弱电箱
	Set("FHomeEditor.CommandClassEleBoxH_Point",  new IMAGE_BRUSH("Icons/Component/EleBoxH_Point", Icon72x72));//强电箱
	Set("FHomeEditor.CommandClassElectricWire",  new IMAGE_BRUSH("Icons/Component/ElectricWire", Icon72x72));//电线
	Set("FHomeEditor.CommandClassSwitch3O2_Point",  new IMAGE_BRUSH("Icons/Component/Switch3O2_Point", Icon72x72));//三联双控开关
	Set("FHomeEditor.CommandClassSwitch3O1_Point",  new IMAGE_BRUSH("Icons/Component/Switch3O1_Point", Icon72x72));//三联单控开关
	Set("FHomeEditor.CommandClassSwitch2O2_Point",  new IMAGE_BRUSH("Icons/Component/Switch2O2_Point", Icon72x72));//双联双控开关
	Set("FHomeEditor.CommandClassSwitch2O1_Point",  new IMAGE_BRUSH("Icons/Component/Switch2O1_Point", Icon72x72));//双联单控开关
	Set("FHomeEditor.CommandClassSwitch1O2_Point",  new IMAGE_BRUSH("Icons/Component/Switch1O2_Point", Icon72x72));//单连双控开关
	Set("FHomeEditor.CommandClassSwitch1O1_Point",  new IMAGE_BRUSH("Icons/Component/Switch1O1_Point", Icon72x72));//单连单控开关
	Set("FHomeEditor.CommandClassLamp_Pendant_Point",  new IMAGE_BRUSH("Icons/Component/Lamp_Pendant_Point", Icon72x72));//吊灯
	Set("FHomeEditor.CommandClassLamp_Flower_Point",  new IMAGE_BRUSH("Icons/Component/ssLamp_Flower_Point", Icon72x72));//花灯
	Set("FHomeEditor.CommandClassLamp_Down_Point",  new IMAGE_BRUSH("Icons/Component/Lamp_Down_Point", Icon72x72));//筒灯
	Set("FHomeEditor.CommandClassLamp_Ceiling_Point",  new IMAGE_BRUSH("Icons/Component/Lamp_Ceiling_Point", Icon72x72));//吸顶灯
	Set("FHomeEditor.CommandClassWater_Hot_Point",  new IMAGE_BRUSH("Icons/Component/Water_Hot_Point", Icon72x72));//热水点位
	Set("FHomeEditor.CommandClassWater_Chilled_Point",  new IMAGE_BRUSH("Icons/Component/Water_Chilled_Point", Icon72x72));//冷水点位
	Set("FHomeEditor.CommandClassFloorDrain_Point",  new IMAGE_BRUSH("Icons/Component/FloorDrain_Point", Icon72x72));//地漏
	Set("FHomeEditor.CommandClassDewatering_Point",  new IMAGE_BRUSH("Icons/Component/Dewatering_Point", Icon72x72));//排水点位
	Set("FHomeEditor.CommandClassFurniture_TVStand",  new IMAGE_BRUSH("Icons/Component/Furniture_TVStand", Icon72x72));//电视柜
	Set("FHomeEditor.CommandClassFurniture_TV",  new IMAGE_BRUSH("Icons/Component/Furniture_TV", Icon72x72));//电视
	Set("FHomeEditor.CommandClassFurniture_TeaTable",  new IMAGE_BRUSH("Icons/Component/Furniture_TeaTable", Icon72x72));//茶几
	Set("FHomeEditor.CommandClassFurniture_SquattingPan",  new IMAGE_BRUSH("Icons/Component/Furniture_SquattingPan", Icon72x72));//蹲便器
	Set("FHomeEditor.CommandClassFurniture_SofaSet",  new IMAGE_BRUSH("Icons/Component/Furniture_SofaSet", Icon72x72));//沙发组
	Set("FHomeEditor.CommandClassFurniture_Sofas",  new IMAGE_BRUSH("Icons/Component/Furniture_Sofas", Icon72x72));//多人沙发
	Set("FHomeEditor.CommandClassFurniture_Sink",  new IMAGE_BRUSH("Icons/Component/Furniture_Sink", Icon72x72));//厨房水槽
	Set("FHomeEditor.CommandClassFurniture_SingleBed",  new IMAGE_BRUSH("Icons/Component/Furniture_SingleBed", Icon72x72));//单人床
	Set("FHomeEditor.CommandClassFurniture_ShowerHead",  new IMAGE_BRUSH("Icons/Component/Furniture_ShowerHead", Icon72x72));//花洒
	Set("FHomeEditor.CommandClassFurniture_PedestalPan",  new IMAGE_BRUSH("Icons/Component/Furniture_PedestalPan", Icon72x72));//坐便器
	Set("FHomeEditor.CommandClassFurniture_FloorCabinet",  new IMAGE_BRUSH("Icons/Component/Furniture_FloorCabinet", Icon72x72));//地柜
	Set("FHomeEditor.CommandClassFurniture_DoubleSofa",  new IMAGE_BRUSH("Icons/Component/Furniture_DoubleSofa", Icon72x72));//双人沙发
	Set("FHomeEditor.CommandClassFurniture_DoubleBed18X20",  new IMAGE_BRUSH("Icons/Component/Furniture_DoubleBed1.8X2.0", Icon72x72));//双人床1800*200
	Set("FHomeEditor.CommandClassFurniture_DoubleBed15X19",  new IMAGE_BRUSH("Icons/Component/Furniture_DoubleBed1.5X1.9", Icon72x72));//双人沙发1500*1
	Set("FHomeEditor.CommandClassFurniture_DiningTable",  new IMAGE_BRUSH("Icons/Component/Furniture_DiningTable", Icon72x72));//餐桌
	Set("FHomeEditor.CommandClassFurniture_CommonCabinet",  new IMAGE_BRUSH("Icons/Component/Furniture_CommonCabinet", Icon72x72));//通用柜
	Set("FHomeEditor.CommandClassFurniture_Closet",  new IMAGE_BRUSH("Icons/Component/Furniture_Closet", Icon72x72));//衣柜
	Set("FHomeEditor.CommandClassFurniture_Chair",  new IMAGE_BRUSH("Icons/Component/Furniture_Chair", Icon72x72));//椅子
	Set("FHomeEditor.CommandClassFurniture_Botany",  new IMAGE_BRUSH("Icons/Component/Furniture_Botany", Icon72x72));//植物
	Set("FHomeEditor.CommandClassFurniture_BedSideTable",  new IMAGE_BRUSH("Icons/Component/Furniture_BedSideTable", Icon72x72));//床头柜
	Set("FHomeEditor.CommandClassFurniture_BedChair",  new IMAGE_BRUSH("Icons/Component/Furniture_BedChair", Icon72x72));//床尾凳
	Set("FHomeEditor.CommandClassFurniture_Bathtub",  new IMAGE_BRUSH("Icons/Component/Furniture_Bathtub", Icon72x72));//浴缸
	Set("FHomeEditor.CommandClassFurniture_BathroomCabinet",  new IMAGE_BRUSH("Icons/Component/Furniture_BathroomCabinet", Icon72x72));//浴室柜
	Set("FHomeEditor.CommandClassFurniture_Armchair",  new IMAGE_BRUSH("Icons/Component/Furniture_Armchair", Icon72x72));//单人沙发
	Set("FHomeEditor.CommandClassFurniture_GasAppliance",  new IMAGE_BRUSH("Icons/Component/Funiture_GasAppliance", Icon72x72));//灶具

    Set("FHomeEditor.CommandClassFurniture_AirConditioner", new IMAGE_BRUSH("Icons/Component/Furniture_AirConditioner", Icon72x72)); // 空调
    Set("FHomeEditor.CommandClassFurniture_CasualSofa", new IMAGE_BRUSH("Icons/Component/Furniture_CasualSofa", Icon72x72)); // 休闲沙发
    Set("FHomeEditor.CommandClassFurniture_CornerTable", new IMAGE_BRUSH("Icons/Component/Furniture_CornerTable", Icon72x72)); // 角几
    Set("FHomeEditor.CommandClassFurniture_Curtains", new IMAGE_BRUSH("Icons/Component/Furniture_Curtains", Icon72x72)); // 窗帘
    Set("FHomeEditor.CommandClassFurniture_SingleSink", new IMAGE_BRUSH("Icons/Component/Furniture_SingleSink", Icon72x72)); // 水槽(单槽)
    Set("FHomeEditor.CommandClassFurniture_Fridge", new IMAGE_BRUSH("Icons/Component/Furniture_Fridge", Icon72x72)); // 冰箱
    Set("FHomeEditor.CommandClassFurniture_WashingMachine", new IMAGE_BRUSH("Icons/Component/Furniture_WashingMachine", Icon72x72)); // 洗衣机
    Set("FHomeEditor.CommandClassFurniture_WritingChair", new IMAGE_BRUSH("Icons/Component/Furniture_WritingChair", Icon72x72)); // 书椅
    Set("FHomeEditor.CommandClassFurniture_WritingDesk", new IMAGE_BRUSH("Icons/Component/Furniture_WritingDesk", Icon72x72)); // 书桌

	Set("FHomeEditor.CommandClassGas_Point",  new IMAGE_BRUSH("Icons/Component/Gas_Point", Icon72x72));//燃气点位

	Set("FHomeEditor.Command_OperationDimension", new IMAGE_BRUSH("Icons/Component/Dimension", Icon72x72));//两点标尺
	Set("FHomeEditor.Command_OperationSeriesDimension", new IMAGE_BRUSH("Icons/Component/SeriesDimension", Icon72x72));//连续标尺
	Set("FHomeEditor.Command_OperationSpaceHeightLabel", new IMAGE_BRUSH("Icons/Component/SpaceHeightLabel", Icon72x72));//空间高度

	Set("FHomeEditor.CommandClassBase", new IMAGE_BRUSH("Icons/Base", Icon24x24));
	Set("FHomeEditor.CommandClassDoor", new IMAGE_BRUSH("Icons/Door", Icon24x24));
	Set("FHomeEditor.CommandClassWindow", new IMAGE_BRUSH("Icons/Window", Icon24x24));
	Set("FHomeEditor.CommandClassSwitch", new IMAGE_BRUSH("Icons/Switch", Icon24x24));
	Set("FHomeEditor.CommandClassSocket", new IMAGE_BRUSH("Icons/Socket", Icon24x24));
	Set("FHomeEditor.CommandClassElectric", new IMAGE_BRUSH("Icons/Electric", Icon24x24));
	Set("FHomeEditor.CommandClassLamp", new IMAGE_BRUSH("Icons/Lamp", Icon24x24));
	Set("FHomeEditor.CommandClassWaterRoute", new IMAGE_BRUSH("Icons/WaterRoute", Icon24x24));
	Set("FHomeEditor.CommandClassFurniture", new IMAGE_BRUSH("Icons/Furniture", Icon24x24));
	Set("FHomeEditor.CommandClassOtherComponent", new IMAGE_BRUSH("Icons/Other", Icon24x24));
	Set("FHomeEditor.CommandClassAnnotation", new IMAGE_BRUSH("Icons/Annotation", Icon24x24));

	Set("FHomeEditor.CommandToolSave", new IMAGE_BRUSH("Icons/Save", Icon24x24));
	Set("FHomeEditor.CommandToolClear", new IMAGE_BRUSH("Icons/Clear", Icon24x24));
    Set("FHomeEditor.CommandToolCenterDisplay", new IMAGE_BRUSH("Icons/T_CenterDisplay", Icon24x24));
	Set("FHomeEditor.CommandToolImport", new IMAGE_BRUSH("Icons/Import", Icon24x24));
	Set("FHomeEditor.CommandToolClearBaseImage", new IMAGE_BRUSH("Icons/ClearBaseImage", Icon24x24));
	Set("FHomeEditor.CommandToolScaler", new IMAGE_BRUSH("Icons/Scaler", Icon24x24));
	Set("FHomeEditor.CommandToolRuler", new IMAGE_BRUSH("Icons/Ruler", Icon24x24));
	Set("FHomeEditor.CommandToolFillet", new IMAGE_BRUSH("Icons/Fillet", Icon24x24));
	Set("FHomeEditor.CommandToolFilletClip", new IMAGE_BRUSH("Icons/Clip", Icon24x24));
	Set("FHomeEditor.CommandToolCopyOffset", new IMAGE_BRUSH("Icons/CopyOffset", Icon24x24));
	Set("FHomeEditor.CommandToolMove", new IMAGE_BRUSH("Icons/Move", Icon24x24));
	Set("FHomeEditor.CommandToolRotation", new IMAGE_BRUSH("Icons/Rotation", Icon24x24));
	Set("FHomeEditor.CommandToolScale", new IMAGE_BRUSH("Icons/Scale", Icon24x24));
	Set("FHomeEditor.CommandToolUndo", new IMAGE_BRUSH("Icons/Undo", Icon24x24));
	Set("FHomeEditor.CommandToolRedo", new IMAGE_BRUSH("Icons/Redo", Icon24x24));

	Set("FHomeEditor.CommandToolHouseType2D", new IMAGE_BRUSH("Icons/CommandToolHouseType2D", Icon24x24));
	Set("FHomeEditor.CommandToolHouseType3D", new IMAGE_BRUSH("Icons/CommandToolHouseType3D", Icon24x24));
	Set("FHomeEditor.CommandToolConstruction", new IMAGE_BRUSH("Icons/CommandToolConstruction", Icon24x24));

	Set("FArmyConstructionEditor.CommandToolSave", new IMAGE_BRUSH("Icons/Save", Icon24x24));
	Set("FArmyConstructionEditor.CommandToolSaveImage", new IMAGE_BRUSH("Icons/SaveImage", Icon24x24));
	Set("FArmyConstructionEditor.CommandToolLoadFrame", new IMAGE_BRUSH("Icons/Frame", Icon24x24));

	Set("FArmyConstructionEditor.DimensionInSideWall_CommandInfo", new IMAGE_BRUSH("Icons/Component/DimensionInSideWall", Icon64x64));
	Set("FArmyConstructionEditor.DimensionOutSideWall_CommandInfo", new IMAGE_BRUSH("Icons/Component/DimensionOutSideWall", Icon64x64));
	Set("FArmyConstructionEditor.DimensionCommon_CommandInfo", new IMAGE_BRUSH("Icons/Component/Dimension", Icon64x64));
	Set("FArmyConstructionEditor.DimensionSeries_CommandInfo", new IMAGE_BRUSH("Icons/Component/SeriesDimension", Icon64x64));
	Set("FArmyConstructionEditor.DownLeadLabel_CommandInfo", new IMAGE_BRUSH("Icons/Component/DownLeadLabel", Icon64x64));

	Set("FArmyConstructionEditor.CommandToolHouseType2D", new IMAGE_BRUSH("Icons/CommandToolHouseType2D", Icon24x24));
	Set("FArmyConstructionEditor.CommandToolHouseType3D", new IMAGE_BRUSH("Icons/CommandToolHouseType3D", Icon24x24));
	Set("FArmyConstructionEditor.CommandToolConstruction", new IMAGE_BRUSH("Icons/CommandToolConstruction", Icon24x24));

	Set("FArmyFloorTextureEditor.CommandClassBaseTextureRect", new IMAGE_BRUSH("Icons/TextureEditor/FloorEditor/Base_RectEditor", Icon72x72));
	Set("FArmyFloorTextureEditor.CommandClassBaseTextureCircle", new IMAGE_BRUSH("Icons/TextureEditor/FloorEditor/Base_Circle", Icon72x72));
	Set("FArmyFloorTextureEditor.CommandClassBaseTexturePolygon", new IMAGE_BRUSH("Icons/TextureEditor/FloorEditor/Base_Polygon", Icon72x72));
	
	Set("FArmyFloorTextureEditor.CommandClassTileTextureContinue", new IMAGE_BRUSH("Icons/Component/Base_Window", Icon72x72));
	Set("FArmyFloorTextureEditor.CommandClassTileTextureWorker", new IMAGE_BRUSH("Icons/Component/Base_FloorWindow", Icon72x72));
	Set("FArmyFloorTextureEditor.CommandClassTileTextureWind", new IMAGE_BRUSH("Icons/Component/Switch3O2_Point", Icon72x72));
	Set("FArmyFloorTextureEditor.CommandClassTileTextureSpace", new IMAGE_BRUSH("Icons/Component/T_SlidingDoor", Icon72x72));
	Set("FArmyFloorTextureEditor.CommandClassTileTextureInterlace", new IMAGE_BRUSH("Icons/Component/Furniture_SofaSet", Icon72x72));

	Set("FArmyFloorTextureEditor.CommandClassFloorTextureContinue", new IMAGE_BRUSH("Icons/Component/Base_Window", Icon72x72));
	Set("FArmyFloorTextureEditor.CommandClassFloorTextureWorker", new IMAGE_BRUSH("Icons/Component/Base_FloorWindow", Icon72x72));
	Set("FArmyFloorTextureEditor.CommandClassFloorTextureWind", new IMAGE_BRUSH("Icons/Component/Switch3O2_Point", Icon72x72));
	Set("FArmyFloorTextureEditor.CommandClassFloorTextureSpace", new IMAGE_BRUSH("Icons/Component/T_SlidingDoor", Icon72x72));
	Set("FArmyFloorTextureEditor.CommandClassFloorTextureInterlace", new IMAGE_BRUSH("Icons/Component/Furniture_SofaSet", Icon72x72));

	Set("FArmyFloorTextureEditor.CommandToolSave", new IMAGE_BRUSH("Icons/Save", Icon24x24));
	Set("FArmyFloorTextureEditor.CommandToolClear", new IMAGE_BRUSH("Icons/Clear", Icon24x24));
	Set("FArmyFloorTextureEditor.CommandToolDrive", new IMAGE_BRUSH("Icons/T_DownArrow", Icon24x24));
	Set("FArmyFloorTextureEditor.CommandToolCopy", new IMAGE_BRUSH("Icons/CopyOffset", Icon24x24));
	Set("FArmyFloorTextureEditor.CommandToolUndo", new IMAGE_BRUSH("Icons/Undo", Icon24x24));
	Set("FArmyFloorTextureEditor.CommandToolRedo", new IMAGE_BRUSH("Icons/Redo", Icon24x24));

	Set("FArmyFloorTextureEditor.CommandToolGroundType2D", new IMAGE_BRUSH("Icons/T_CompanyLabrary", Icon24x24));
	Set("FArmyFloorTextureEditor.CommandToolHouseType2D", new IMAGE_BRUSH("Icons/CommandToolHouseType2D", Icon24x24));
	Set("FArmyFloorTextureEditor.CommandToolHouseType3D", new IMAGE_BRUSH("Icons/CommandToolHouseType3D", Icon24x24));
	Set("FArmyFloorTextureEditor.CommandToolConstruction", new IMAGE_BRUSH("Icons/CommandToolConstruction", Icon24x24));


	Set ("FArmyHydropowerEditor.CommandToolHouseType2D",new IMAGE_BRUSH ("Icons/CommandToolHouseType2D",Icon24x24));
	Set ("FArmyHydropowerEditor.CommandToolHouseType3D",new IMAGE_BRUSH ("Icons/CommandToolHouseType3D",Icon24x24));
	Set ("FArmyHydropowerEditor.CommandToolConstruction",new IMAGE_BRUSH ("Icons/CommandToolConstruction",Icon24x24));

	//Tool Bar
	{
		// SButton defaults...
        const FButtonStyle TestButton = FButtonStyle()
            .SetNormal(IMAGE_BRUSH("Icons/White", Icon32x32, GetColor("Color.FF303133")))
            .SetHovered(IMAGE_BRUSH("Icons/White", Icon32x32, GetColor("Color.FF28292B")))
            .SetPressed(IMAGE_BRUSH("Icons/White", Icon32x32, GetColor("Color.FF303133")))
            .SetNormalPadding(FMargin(10, 2, 10, 2))
            .SetPressedPadding(FMargin(10, 3, 10, 1));

		Set("ToolBar.Background", new BOX_BRUSH("Icons/White", FMargin(4.0f / 16.0f), GetColor("Color.None")));
		Set("ToolBar.Icon", new IMAGE_BRUSH("Icons/icon_tab_toolbar_16px", Icon16x16));
		Set("ToolBar.Expand", new IMAGE_BRUSH("Icons/toolbar_expand_16x", Icon16x16));
		Set("ToolBar.SubMenuIndicator", new IMAGE_BRUSH("Common/SubmenuArrow", Icon8x8));
		Set("ToolBar.SToolBarComboButtonBlock.Padding", FMargin(4.0f, 0.0f));
		Set("ToolBar.SToolBarButtonBlock.Padding", FMargin(4.0f, 0.0f));
		Set("ToolBar.SToolBarCheckComboButtonBlock.Padding", FMargin(4.0f, 0.0f));
		Set("ToolBar.SToolBarButtonBlock.CheckBox.Padding", FMargin(4.0f, 0.0f));
		Set("ToolBar.SToolBarComboButtonBlock.ComboButton.Color", FLinearColor(0.72f, 0.72f, 0.72f, 1.f));

		Set("ToolBar.Block.IndentedPadding", FMargin(18.0f, 2.0f, 4.0f, 4.0f));
		Set("ToolBar.Block.Padding", FMargin(2.0f, 2.0f, 4.0f, 4.0f));

        Set("ToolBar.Separator", new BOX_BRUSH("Old/White", 4.0f / 32.0f, GetColor("Color.489D9FA5")));
        Set("ToolBar.Separator.Padding", FMargin(0));

		Set("ToolBar.Label", GetWidgetStyle<FTextBlockStyle>("ArmyText_9"));
		//Set("ToolBar.EditableText", FEditableTextBoxStyle(NormalEditableTextBoxStyle).SetFont(TTF_FONT("Fonts/Roboto-Regular", 9)));
		Set("ToolBar.Keybinding", GetWidgetStyle<FTextBlockStyle>("ArmyText_8"));

		//Set("ToolBar.Heading", FTextBlockStyle(NormalText)
		//	.SetFont(TTF_FONT("Fonts/Roboto-Regular", 8))
		//	.SetColorAndOpacity(FLinearColor(0.4f, 0.4, 0.4f, 1.0f)));

		Set("ToolBar.Button", FButtonStyle(TestButton)
			.SetNormal(FSlateNoResource())
			.SetPressed(BOX_BRUSH("Old/White", 4.0f / 16.0f, GetColor("Color.FF28292B")))
			.SetHovered(BOX_BRUSH("Old/White", 4.0f / 16.0f, GetColor("Color.FF28292B")))
        );

		//Set("ToolBar.Button.Normal", new FSlateNoResource());
		//Set("ToolBar.Button.Pressed", new BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, FLinearColor(FColor(52, 53, 57))));
		//Set("ToolBar.Button.Hovered", new BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, FLinearColor(FColor(52, 53, 57))));
	}

	 //Class Bar
	{
		// SButton defaults...
		const FButtonStyle Button = FButtonStyle()
			.SetNormal(BOX_BRUSH("Common/Button", FVector2D(32, 32), 8.0f / 32.0f))
			.SetHovered(BOX_BRUSH("Common/Button_Hovered", FVector2D(32, 32), 8.0f / 32.0f))
			.SetPressed(BOX_BRUSH("Common/Button_Pressed", FVector2D(32, 32), 8.0f / 32.0f))
			.SetNormalPadding(FMargin(2, 20, 2, 2))
			.SetPressedPadding(FMargin(2, 21, 2, 1));

		Set("ClassToolBar.Background", new BOX_BRUSH("Icons/White", FMargin(4.0f / 16.0f), GetColor("Color.None")));
		Set("ClassToolBar.Icon", new IMAGE_BRUSH("Icons/icon_tab_toolbar_16px", Icon16x16));
		Set("ClassToolBar.Expand", new IMAGE_BRUSH("Icons/toolbar_expand_16x", Icon16x16));
		Set("ClassToolBar.SubMenuIndicator", new IMAGE_BRUSH("Common/SubmenuArrow", Icon8x8));
        Set("ClassToolBar.SToolBarComboButtonBlock.Padding", FMargin(0));
        Set("ClassToolBar.SToolBarButtonBlock.Padding", FMargin(0));
        Set("ClassToolBar.SToolBarCheckComboButtonBlock.Padding", FMargin(0));
        Set("ClassToolBar.SToolBarButtonBlock.CheckBox.Padding", FMargin(0, 16));
		Set("ClassToolBar.SToolBarComboButtonBlock.ComboButton.Color", FLinearColor(0.72f, 0.72f, 0.72f, 1.f));
		Set("ClassToolBar.Block.IndentedPadding", FMargin(0));
		Set("ClassToolBar.Block.Padding", FMargin(0));

		//Set("ClassToolBar.Separator", new BOX_BRUSH("Old/Button", 4.0f / 32.0f));
		//Set("ClassToolBar.Separator.Padding", FMargin(0.5f));

		Set("ClassToolBar.Label", GetWidgetStyle<FTextBlockStyle>("ArmyText_9"));
		//Set("ClassToolBar.EditableText", FEditableTextBoxStyle(NormalEditableTextBoxStyle).SetFont(TTF_FONT("Fonts/Roboto-Regular", 9)));
		Set("ClassToolBar.Keybinding", GetWidgetStyle<FTextBlockStyle>("ArmyText_8"));

		//Set("ClassToolBar.Heading", FTextBlockStyle(NormalText)
		//	.SetFont(TTF_FONT("Fonts/Roboto-Regular", 8))
		//	.SetColorAndOpacity(FLinearColor(0.4f, 0.4, 0.4f, 1.0f)));

		/* Create style for "ToolBar.CheckBox" widget ... */
		const FCheckBoxStyle ToolBarCheckBoxStyle = FCheckBoxStyle()
			.SetUncheckedImage(IMAGE_BRUSH("Common/SmallCheckBox", Icon14x14))
			.SetCheckedImage(IMAGE_BRUSH("Common/SmallCheckBox_Checked", Icon14x14))
			.SetUncheckedHoveredImage(IMAGE_BRUSH("Common/SmallCheckBox_Hovered", Icon14x14))
			.SetCheckedImage(IMAGE_BRUSH("Common/SmallCheckBox_Checked_Hovered", Icon14x14))
			.SetUncheckedPressedImage(IMAGE_BRUSH("Common/SmallCheckBox_Hovered", Icon14x14, FLinearColor(0.5f, 0.5f, 0.5f)))
			.SetCheckedPressedImage(IMAGE_BRUSH("Common/SmallCheckBox_Checked_Hovered", Icon14x14, FLinearColor(0.5f, 0.5f, 0.5f)))
			.SetUndeterminedImage(IMAGE_BRUSH("Common/CheckBox_Undetermined", Icon14x14))
			.SetUndeterminedHoveredImage(IMAGE_BRUSH("Common/CheckBox_Undetermined_Hovered", Icon14x14))
			.SetUndeterminedPressedImage(IMAGE_BRUSH("Common/CheckBox_Undetermined_Hovered", Icon14x14, FLinearColor(0.5f, 0.5f, 0.5f)));
		/* ... and add new style */
		Set("ClassToolBar.CheckBox", ToolBarCheckBoxStyle);


		/* Read-only checkbox that appears next to a menu item */
		/* Set images for various SCheckBox states associated with read-only toolbar check box items... */
		const FCheckBoxStyle BasicToolBarCheckStyle = FCheckBoxStyle()
			.SetUncheckedImage(IMAGE_BRUSH("Icons/Empty_14x", Icon14x14))
			.SetUncheckedHoveredImage(IMAGE_BRUSH("Icons/Empty_14x", Icon14x14))
			.SetUncheckedPressedImage(IMAGE_BRUSH("Common/SmallCheckBox_Hovered", Icon14x14))
			.SetCheckedImage(IMAGE_BRUSH("Common/SmallCheck", Icon14x14))
			.SetCheckedHoveredImage(IMAGE_BRUSH("Common/SmallCheck", Icon14x14))
			.SetCheckedPressedImage(IMAGE_BRUSH("Common/SmallCheck", Icon14x14))
			.SetUndeterminedImage(IMAGE_BRUSH("Icons/Empty_14x", Icon14x14))
			.SetUndeterminedHoveredImage(FSlateNoResource())
			.SetUndeterminedPressedImage(FSlateNoResource());
		Set("ClassToolBar.Check", BasicToolBarCheckStyle);

		/* Create style for "ToolBar.ToggleButton" widget ... */
		const FCheckBoxStyle ToolBarToggleButtonCheckBoxStyle = FCheckBoxStyle()
			.SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
			.SetUncheckedImage(FSlateNoResource())
			.SetUncheckedPressedImage(BOX_BRUSH("Icons/White", 4.0f / 16.0f, GetColor("Color.FF28292B")))
			.SetUncheckedHoveredImage(BOX_BRUSH("Icons/White", 4.0f / 16.0f, GetColor("Color.FF28292B")))
			.SetCheckedImage(BOX_BRUSH("Icons/White", 4.0f / 16.0f, GetColor("Color.FF28292B")))
			.SetCheckedHoveredImage(BOX_BRUSH("Icons/White", 4.0f / 16.0f, GetColor("Color.FF28292B")))
			.SetCheckedPressedImage(BOX_BRUSH("Icons/White", 4.0f / 16.0f, GetColor("Color.FF28292B")));
		/* ... and add new style */

		Set("ClassToolBar.ToggleButton", ToolBarToggleButtonCheckBoxStyle);

        Set("ClassToolBar.Button", GetWidgetStyle<FButtonStyle>("Button.FF303133"));
	}

	//Tool Bar Alone
	{
		// SButton defaults...
		const FButtonStyle Button = GetWidgetStyle<FButtonStyle>("Button.FF303133");

		Set("ToolBarAlone.Background", new BOX_BRUSH("Icons/White", FMargin(4.0f / 16.0f), GetColor("Color.None")));
		Set("ToolBarAlone.Icon", new IMAGE_BRUSH("Icons/icon_tab_toolbar_16px", Icon16x16));
		Set("ToolBarAlone.Expand", new IMAGE_BRUSH("Icons/toolbar_expand_16x", Icon16x16));
		Set("ToolBarAlone.SubMenuIndicator", new IMAGE_BRUSH("Common/SubmenuArrow", Icon8x8));
		Set("ToolBarAlone.SToolBarComboButtonBlock.Padding", FMargin(4.0f, 0.0f));
		Set("ToolBarAlone.SToolBarButtonBlock.Padding", FMargin(4.0f, 0.0f));
		Set("ToolBarAlone.SToolBarCheckComboButtonBlock.Padding", FMargin(4.0f, 0.0f));
		Set("ToolBarAlone.SToolBarButtonBlock.CheckBox.Padding", FMargin(4.0f, 0.0f));
		Set("ToolBarAlone.SToolBarComboButtonBlock.ComboButton.Color", FLinearColor(0.72f, 0.72f, 0.72f, 1.f));

		Set("ToolBarAlone.Block.IndentedPadding", FMargin(18.0f, 2.0f, 4.0f, 4.0f));
		Set("ToolBarAlone.Block.Padding", FMargin(2.0f, 2.0f, 4.0f, 4.0f));

		Set("ToolBarAlone.Label", GetWidgetStyle<FTextBlockStyle>("ArmyText_9"));
		//Set("ToolBarAlone.EditableText", FEditableTextBoxStyle(NormalEditableTextBoxStyle).SetFont(TTF_FONT("Fonts/Roboto-Regular", 9)));
		Set("ToolBarAlone.Keybinding", GetWidgetStyle<FTextBlockStyle>("ArmyText_8"));

		/* Create style for "ToolBarAlone.CheckBox" widget ... */
		const FCheckBoxStyle ToolBarCheckBoxStyle = FCheckBoxStyle()
			.SetUncheckedImage(IMAGE_BRUSH("Common/SmallCheckBox", Icon14x14))
			.SetCheckedImage(IMAGE_BRUSH("Common/SmallCheckBox_Checked", Icon14x14))
			.SetUncheckedHoveredImage(IMAGE_BRUSH("Common/SmallCheckBox_Hovered", Icon14x14))
			.SetCheckedImage(IMAGE_BRUSH("Common/SmallCheckBox_Checked_Hovered", Icon14x14))
			.SetUncheckedPressedImage(IMAGE_BRUSH("Common/SmallCheckBox_Hovered", Icon14x14, FLinearColor( 0.701f, 0.225f, 0.003f )))
			.SetCheckedPressedImage(IMAGE_BRUSH("Common/SmallCheckBox_Checked_Hovered", Icon14x14, FLinearColor( 0.701f, 0.225f, 0.003f )))
			.SetUndeterminedImage(IMAGE_BRUSH("Common/CheckBox_Undetermined", Icon14x14))
			.SetUndeterminedHoveredImage(IMAGE_BRUSH("Common/CheckBox_Undetermined_Hovered", Icon14x14))
			.SetUndeterminedPressedImage(IMAGE_BRUSH("Common/CheckBox_Undetermined_Hovered", Icon14x14, FLinearColor( 0.701f, 0.225f, 0.003f )));
		/* ... and add new style */
		Set("ToolBarAlone.CheckBox", ToolBarCheckBoxStyle);

		/* Read-only checkbox that appears next to a menu item */
		/* Set images for various SCheckBox states associated with read-only toolbar check box items... */
		const FCheckBoxStyle BasicToolBarCheckStyle = FCheckBoxStyle()
			.SetUncheckedImage(IMAGE_BRUSH("Icons/Empty_14x", Icon14x14))
			.SetUncheckedHoveredImage(IMAGE_BRUSH("Icons/Empty_14x", Icon14x14))
			.SetUncheckedPressedImage(IMAGE_BRUSH("Common/SmallCheckBox_Hovered", Icon14x14))
			.SetCheckedImage(IMAGE_BRUSH("Common/SmallCheck", Icon14x14))
			.SetCheckedHoveredImage(IMAGE_BRUSH("Common/SmallCheck", Icon14x14))
			.SetCheckedPressedImage(IMAGE_BRUSH("Common/SmallCheck", Icon14x14))
			.SetUndeterminedImage(IMAGE_BRUSH("Icons/Empty_14x", Icon14x14))
			.SetUndeterminedHoveredImage(FSlateNoResource())
			.SetUndeterminedPressedImage(FSlateNoResource());
		Set("ToolBarAlone.Check", BasicToolBarCheckStyle);

		/* Create style for "ToolBarAlone.ToggleButton" widget ... */
		const FCheckBoxStyle ToolBarToggleButtonCheckBoxStyle = FCheckBoxStyle()
			.SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
			.SetUncheckedImage(FSlateNoResource())
			.SetUncheckedPressedImage(BOX_BRUSH("Icons/White", 4.0f / 16.0f, GetColor("Color.FFFD9800")))
			.SetUncheckedHoveredImage(BOX_BRUSH("Icons/White", 4.0f / 16.0f, GetColor("Color.FFFD9800")))
			.SetCheckedImage(BOX_BRUSH("Icons/White", 4.0f / 16.0f, GetColor("Color.FFFD9800")))
			.SetCheckedHoveredImage(BOX_BRUSH("Icons/White", 4.0f / 16.0f, GetColor("Color.FFFD9800")))
			.SetCheckedPressedImage(BOX_BRUSH("Icons/White", 4.0f / 16.0f, GetColor("Color.FFFD9800")));
		/* ... and add new style */
		Set("ToolBarAlone.ToggleButton", ToolBarToggleButtonCheckBoxStyle);
	}
}
