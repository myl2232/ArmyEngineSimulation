#include "ArmyStyleSet.h"
#include "Styling/SlateTypes.h"
#include "CoreStyle.h"

#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(FPaths::ProjectContentDir() / "Slate" / RelativePath + TEXT(".png"), __VA_ARGS__)
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush(RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush(RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo(RootToEngineContentDir(RelativePath, TEXT(".ttf")), __VA_ARGS__)
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo(RootToEngineContentDir(RelativePath, TEXT(".otf")), __VA_ARGS__)

const FVector2D Icon1x1(1.f, 1.f);
const FVector2D Icon8x8(8.f, 8.0f);
const FVector2D Icon10x10(10.f,10.0f);
const FVector2D Icon10x6(10.f, 6.f);
const FVector2D Icon12x12(12.f, 12.0f);
const FVector2D Icon14x14(14.f, 14.0f);
const FVector2D Icon16x16(16.f, 16.0f);
const FVector2D Icon18x18(18.f, 18.0f);
const FVector2D Icon6x4(6.f, 4.0f);

const FVector2D Icon24x24(24.f, 24.0f);
const FVector2D Icon32x32(32.f, 32.0f);
const FVector2D Icon44x44(44.0f, 44.0f);
const FVector2D Icon40x40(40.0f, 40.0f);
const FVector2D Icon56x56(56.0f, 56.0f);
const FVector2D Icon64x24(64.0f, 24.0f);
const FVector2D Icon64x64(64.0f, 64.0f);
const FVector2D Icon72x72(72.0f, 72.0f);
const FVector2D Icon120x120(120.0f, 120.0f);
const FVector2D Icon128x128(128.0f, 128.0f);
const FVector2D Icon180x180(180.0f, 180.0f);
const FVector2D Icon156x156(156.f, 156.f);
const FVector2D Icon160x160(160.f, 160.f);
const FVector2D Icon182x182(182.f, 182.f);
const FVector2D Icon420x420(420.f, 420.f);

FArmyStyleSet::FArmyStyleSet()
    : FSlateStyleSet("ArmyStyle")
{
}

void FArmyStyleSet::Initialize()
{
	SetContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));
	SetEngineContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

    SetupDrawPanelStyle();
}

void FArmyStyleSet::SetEngineContentRoot(const FString& InContentRootDir)
{
	EngineContentRoot = InContentRootDir;
}

FString FArmyStyleSet::RootToEngineContentDir(const FString& RelativePath, const TCHAR* Extension)
{
	return (EngineContentRoot / RelativePath) + Extension;
}

void FArmyStyleSet::SetupDrawPanelStyle()
{
    SetColorStyle();
    SetFontStyle();
    SetWidgetStyle();
    SetEditorStyle();
    SetHomeModeStyle();
    SetLayoutModeStyle();
	SetHardModeStyle();
	SetHardModeFloorStyle();
    SetSoftModeStyle();
	SetConstructionModeStyle();
    SetHydropowerModeStyle();
	SetWHCModeStyle();

    Set("Launcher.LoginView1", new IMAGE_BRUSH("Icons/T_LoginView_1", FVector2D(812, 768)));
	Set("Launcher.LoginView2", new IMAGE_BRUSH("Icons/T_LoginView_2", FVector2D(812, 768)));
	
    Set("Launcher.LoginBackground", new IMAGE_BRUSH("Icons/T_LoginBackground", FVector2D(1920, 1061)));
    Set("Launcher.LoginBackground1", new IMAGE_BRUSH("Icons/T_LoginBackground1", FVector2D(468, 768)));
    Set("Launcher.Logo", new IMAGE_BRUSH("Icons/T_Logo", FVector2D(360, 100)));
    Set("MainPage.Logo", new IMAGE_BRUSH("Icons/T_Logo", FVector2D(260, 48)));
	Set("Icon.Application", new IMAGE_BRUSH("Icons/Application", FVector2D(100, 20)));
    Set("Icon.MinimizeWindow", new IMAGE_BRUSH("Icons/MinimizeWindow", FVector2D(76, 22)));
    Set("Icon.CloseApplication", new IMAGE_BRUSH("Icons/CloseApplication", FVector2D(76, 22)));
    Set("Icon.Username", new IMAGE_BRUSH("Icons/T_Username", Icon24x24));
    Set("Icon.Password", new IMAGE_BRUSH("Icons/T_Pwd", Icon24x24));
    Set("Icon.Throbber", new IMAGE_BRUSH("Icons/T_Throbber", Icon24x24));
	Set("Icon.NewProject", new IMAGE_BRUSH("Icons/T_NewProject", FVector2D(210, 210)));
	Set("Icon.NewPlan", new IMAGE_BRUSH("Icons/NewPlan", FVector2D(72, 72)));
	Set("Icon.DefaultImage", new IMAGE_BRUSH("Icons/DefaultImage", FVector2D(210, 210)));
	Set("Icon.MasterImgDefault", new IMAGE_BRUSH("Icons/MasterImgDefault", FVector2D(48, 48)));
	Set("Icon.DefaultImage_QRCode", new IMAGE_BRUSH("Icons/DefaultImage_QRCode", FVector2D(10, 10)));
	Set("Icon.DefaultImage_ContentBrowser", new IMAGE_BRUSH("Icons/DefaultImage_ContentBrowser", FVector2D(112, 112)));
	Set("Icon.Lable_Goods", new IMAGE_BRUSH("Icons/Lable_Goods", FVector2D(32, 18)));
	Set("Icon.Newest", new IMAGE_BRUSH("Icons/Newest", Icon32x32));
	Set("Icon.ResourceReady", new IMAGE_BRUSH("Icons/ResourceReady", Icon32x32));
	Set("Icon.information", new IMAGE_BRUSH("Icons/information_hover", Icon12x12));
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
    Set("Icon.DownArrow_Gray", new IMAGE_BRUSH("Icons/DownArrow_Gray", Icon16x16));
	Set("Icon.DownArrow_Orange", new IMAGE_BRUSH("Icons/DownArrow_Orange", Icon16x16));
    Set("Icon.DownArrow_White", new IMAGE_BRUSH("Icons/DownArrow_White", Icon16x16));
    Set("Icon.UpArrow_Orange", new IMAGE_BRUSH("Icons/UpArrow_Orange", Icon16x16));
	Set("Icon.UpArrow_White", new IMAGE_BRUSH("Icons/UpArrow_White", Icon16x16));
	Set("Icon.None", new IMAGE_BRUSH("Icons/White", Icon24x24, GetColor("Color.None")));
	Set("Icon.NoResourceImage", new IMAGE_BRUSH("Icons/NoResourceImage", Icon180x180));
	Set("Icon.NoResourceImage_Goods", new IMAGE_BRUSH("Icons/NoResourceImage", Icon120x120));
    Set("Icon.NoResourceImage_Replace", new IMAGE_BRUSH("Icons/NoResourceImage", Icon160x160));
	Set("Icon.alert_success", new IMAGE_BRUSH("Icons/alert_success", Icon32x32));
	Set("Icon.alert_fail", new IMAGE_BRUSH("Icons/alert_fail", Icon32x32));
	Set("Icon.delete", new IMAGE_BRUSH("Icons/delete", Icon16x16));
    Set("Icon.Alert", new IMAGE_BRUSH("Icons/Alert", Icon10x10));
	Set("Icon.Triangle", new IMAGE_BRUSH("Icons/Triangle", Icon120x120));
	Set("Icon.Update", new IMAGE_BRUSH("Icons/Update", Icon44x44));
	Set("Icon.StaticLightingQuality_Preview", new IMAGE_BRUSH("Icons/StaticLightingQuality_Preview", Icon182x182));
	Set("Icon.StaticLightingQuality_Standard", new IMAGE_BRUSH("Icons/StaticLightingQuality_Standard", Icon182x182));
	Set("Icon.StaticLightingQuality_HD", new IMAGE_BRUSH("Icons/StaticLightingQuality_HD", Icon182x182));
	Set("Icon.ToolTip", new IMAGE_BRUSH("Icons/information", Icon12x12));
	Set("Icon.BlackTriangle", new IMAGE_BRUSH("Icons/BlackTriangle", Icon24x24));
	//@欧石楠 下载
	Set("Icon.DownloadStyle_BlackCircle", new IMAGE_BRUSH("Icons/DownloadStyle_BlackCircle", Icon44x44));
	Set("Icon.DownloadStyle_Downloading", new IMAGE_BRUSH("Icons/DownloadStyle_Downloading", Icon44x44));
	Set("Icon.DownloadStyle_success", new IMAGE_BRUSH("Icons/DownloadStyle_success", Icon44x44));
	Set("Icon.DownloadStyle_fail", new IMAGE_BRUSH("Icons/DownloadStyle_fail", Icon44x44));
	Set("Icon.DownloadStyle_failcircle", new IMAGE_BRUSH("Icons/DownloadStyle_failcircle", Icon44x44));
	Set("Icon.Spinbox_UpNormalArrow", new IMAGE_BRUSH("Icons/Spinbox_UpNormalArrow", Icon6x4));
	Set("Icon.Spinbox_DownNormalArrow", new IMAGE_BRUSH("Icons/Spinbox_DownNormalArrow", Icon6x4));
	Set("Icon.Spinbox_UpSelectedArrow", new IMAGE_BRUSH("Icons/Spinbox_UpSelectedArrow", Icon6x4));
	Set("Icon.Spinbox_DownSelectedArrow", new IMAGE_BRUSH("Icons/Spinbox_DownSelectedArrow", Icon6x4));
	Set("Icon.Replace", new IMAGE_BRUSH("Icons/T_Replace", Icon16x16));
	Set("Icon.ModalDialog_Warning", new IMAGE_BRUSH("Icons/ModalDialog_Warning", Icon32x32));

	//@甘文俊 智能设计
	Set("Icon.Close_AutoDesign", new IMAGE_BRUSH("Icons/AutoDesign/Close", Icon24x24)); 
	Set("Icon.CloseBig_AutoDesign", new IMAGE_BRUSH("Icons/AutoDesign/ic_offnew", Icon32x32));
	Set("Icon.SelectType_Default_AutoDesign", new IMAGE_BRUSH("Icons/AutoDesign/SelectType_Default", Icon16x16));
	Set("Icon.Radio_Normal_AutoDesign", new IMAGE_BRUSH("Icons/AutoDesign/radio_normal@2x",Icon14x14));
	Set("Icon.Radio_Selected_AutoDesign", new IMAGE_BRUSH("Icons/AutoDesign/radio_select@2x", Icon14x14));
	Set("Icon.Information_Normal_AutoDesign", new IMAGE_BRUSH("Icons/AutoDesign/information@2x",Icon12x12));
	Set("Icon.LType_Clicked_AutoDesign", new IMAGE_BRUSH("Icons/AutoDesign/LType_Clicked", Icon56x56));
	Set("Icon.HLType_Normal_AutoDesign", new IMAGE_BRUSH("Icons/AutoDesign/HLType_Default", Icon56x56));
	Set("Icon.UType_Normal_AutoDesign", new IMAGE_BRUSH("Icons/AutoDesign/UType_Default", Icon56x56));
	Set("Icon.DLType_Normal_AutoDesign", new IMAGE_BRUSH("Icons/AutoDesign/DLType_Default", Icon56x56));
	Set("Icon.KitchenType_Default_AutoDesign", new IMAGE_BRUSH("Icons/AutoDesign/KitchenTypeDefault", Icon56x56));

	Set("Button.FF383A3D", FButtonStyle()
		.SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF383A3D")))
		.SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF383A3D")))
		.SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FFFF8400")))
		.SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FFFD9800")))
	);

	//@ 大师方案
	Set("Icon.masterPlan", new IMAGE_BRUSH("Icons/masterPlan", Icon24x24));
	//企业素材
	Set("Icon.companyCategry", new IMAGE_BRUSH("Icons/companyCategry", Icon24x24));

	Set("Icon.Back", new IMAGE_BRUSH("Icons/Toolbar/back", Icon24x24));

	Set("Icon.Note", new IMAGE_BRUSH("Icons/Nav_Note", Icon24x24));
	Set("Icon.Ruler", new IMAGE_BRUSH("Icons/Nav_Ruler", Icon24x24));
	Set("Icon.Other", new IMAGE_BRUSH("Icons/Nav_Other", Icon24x24));

    Set("Icon.Success", new IMAGE_BRUSH("Icons/Success", Icon24x24));
	Set("Icon.Warning", new IMAGE_BRUSH("Icons/Warning", Icon24x24));
	Set("Icon.Normal", new IMAGE_BRUSH("Icons/Normal", Icon24x24));
	Set("Icon.White", new IMAGE_BRUSH("Icons/White", Icon1x1));

    // 返回主页
    Set("Icon.HomePage.Normal", new IMAGE_BRUSH("Icons/HomePage_Normal", Icon16x16));
    Set("Icon.HomePage.Hovered", new IMAGE_BRUSH("Icons/HomePage_Hovered", Icon16x16));

	//方案估价
	Set("Icon.PlanPrice.Normal", new IMAGE_BRUSH("Icons/PlanPrice_Normal", Icon16x16));
	Set("Icon.PlanPrice.Hovered", new IMAGE_BRUSH("Icons/PlanPrice_Hovered", Icon16x16));

    Set("Border.None", new BORDER_BRUSH("Icons/White", FMargin(2), GetColor("Color.None")));
	Set("Border.Orange", new BORDER_BRUSH("Icons/White", FMargin(2), GetColor("Color.FFFD9800")));
    Set("Border.Gray.FFC8C9CC", new BORDER_BRUSH("Icons/White", FMargin(1), GetColor("Color.Gray.FFC8C9CC")));

    Set("Icon.VersionUpdated", new IMAGE_BRUSH("Icons/VersionUpdated", FVector2D(420, 180)));
	//吸管图标
	Set("Icon.Straw", new IMAGE_BRUSH("Icons/Cursor_Straw", Icon40x40));

    // @欧石楠 起铺点
    Set("Icon.LayingPoint.Normal", new IMAGE_BRUSH("Icons/LayingPoint/LayingPoint_Normal", Icon16x16));
    Set("Icon.LayingPoint.Hovered", new IMAGE_BRUSH("Icons/LayingPoint/LayingPoint_Hovered", Icon16x16));
    Set("Icon.LayingPoint.Selected", new IMAGE_BRUSH("Icons/LayingPoint/LayingPoint_Selected", Icon16x16));
    Set("Icon.LayingPoint.Captured", new IMAGE_BRUSH("Icons/LayingPoint/LayingPoint_Captured", Icon16x16));
	
	
    
    Set(TEXT("原始户型"), new IMAGE_BRUSH("Icons/TitleBar/HomeMode", Icon24x24));
	Set(TEXT("拆改户型"), new IMAGE_BRUSH("Icons/TitleBar/Dismantle", Icon24x24));
	Set(TEXT("立面模式"), new IMAGE_BRUSH("Icons/TitleBar/HardMode", Icon24x24));
    Set(TEXT("软装模式"), new IMAGE_BRUSH("Icons/TitleBar/SoftMode", Icon24x24));
	Set(TEXT("水电模式"),new IMAGE_BRUSH("Icons/TitleBar/HydropowerMode",Icon24x24));
    Set(TEXT("图纸模式"), new IMAGE_BRUSH("Icons/TitleBar/ConstructionMode", Icon24x24));
	Set(TEXT("木作模式"), new IMAGE_BRUSH("Icons/TitleBar/WHCMode", Icon24x24));

    Set(TEXT("原始户型_Active"), new IMAGE_BRUSH("Icons/TitleBar/HomeMode_Active", Icon24x24));
	Set(TEXT("拆改户型_Active"), new IMAGE_BRUSH("Icons/TitleBar/Dismantle_Active", Icon24x24));
    Set(TEXT("立面模式_Active"), new IMAGE_BRUSH("Icons/TitleBar/HardMode_Active", Icon24x24));
    Set(TEXT("软装模式_Active"), new IMAGE_BRUSH("Icons/TitleBar/SoftMode_Active", Icon24x24));
    Set(TEXT("水电模式_Active"), new IMAGE_BRUSH("Icons/TitleBar/HydropowerMode_Active", Icon24x24));
    Set(TEXT("图纸模式_Active"), new IMAGE_BRUSH("Icons/TitleBar/ConstructionMode_Active", Icon24x24));
	Set(TEXT("木作模式_Active"), new IMAGE_BRUSH("Icons/TitleBar/WHCMode_Active", Icon24x24));

	Set(TEXT("后台管理"), new IMAGE_BRUSH("Icons/TitleBar/Setup", Icon12x12));
	Set(TEXT("注销"), new IMAGE_BRUSH("Icons/TitleBar/Logout", Icon12x12));
	Set(TEXT("退出"), new IMAGE_BRUSH("Icons/TitleBar/Quit", Icon12x12));

	/**@欧石楠 解锁图标*/
	Set("Icon.Unlock", new IMAGE_BRUSH("Icons/Toolbar/Unlock", Icon24x24));

    Set("Splitter.Thin", new IMAGE_BRUSH("Icons/White", FVector2D(1, 1), GetColor("Color.FF404040")));
	Set("Splitter.White", new IMAGE_BRUSH("Icons/White", FVector2D(1, 1), FLinearColor::White));
	Set("Splitter.FF9D9FA5", new IMAGE_BRUSH("Icons/White", FVector2D(1, 1), GetColor("Color.FF9D9FA5"))); 
    Set("Splitter.Orange.FFFD9800", new IMAGE_BRUSH("Icons/White", FVector2D(1, 1), GetColor("Color.FFFD9800")));
	Set("Splitter.FF46474D", new IMAGE_BRUSH("Icons/White", FVector2D(1, 1), GetColor("Color.FF46474D")));
	Set("Splitter.FF343538", new IMAGE_BRUSH("Icons/White", FVector2D(1, 1), GetColor("Color.FF343538")));
	Set("Splitter.FF54555C",new IMAGE_BRUSH("Icons/White",FVector2D(1,1),GetColor("Color.FF54555C")));
    Set("Splitter.Gray.FF424447", new IMAGE_BRUSH("Icons/White", FVector2D(1, 1), GetColor("Color.Gray.FF424447")));

	/**@欧石楠 选择户型生成方式*/
	Set("NewPlanWay.New", new IMAGE_BRUSH("Icons/NewPlanWay_New", Icon56x56));
	Set("NewPlanWay.Import", new IMAGE_BRUSH("Icons/NewPlanWay_Import", Icon56x56));

    // Levels General
    {
        Set("Level.VisibleIcon16x", new IMAGE_BRUSH("Icons/icon_levels_visible_16px", Icon16x16));
        Set("Level.VisibleHighlightIcon16x", new IMAGE_BRUSH("Icons/icon_levels_visible_hi_16px", Icon16x16));
        Set("Level.NotVisibleIcon16x", new IMAGE_BRUSH("Icons/icon_levels_invisible_16px", Icon16x16));
        Set("Level.NotVisibleHighlightIcon16x", new IMAGE_BRUSH("Icons/icon_levels_invisible_hi_16px", Icon16x16));

		Set("Level.lock_hover", new IMAGE_BRUSH("Icons/lock_hover", Icon16x16));
		Set("Level.lock_normal", new IMAGE_BRUSH("Icons/lock_normal", Icon16x16));
		Set("Level.unlock_hover", new IMAGE_BRUSH("Icons/unlock_hover", Icon16x16));
		Set("Level.unlock_normal", new IMAGE_BRUSH("Icons/unlock_normal", Icon16x16));
    }

    Set("TreeArrow_Collapsed", new IMAGE_BRUSH("Common/TreeArrow_Collapsed", Icon16x16));
    Set("TreeArrow_Collapsed_Hovered", new IMAGE_BRUSH("Common/TreeArrow_Collapsed_Hovered", Icon16x16));
    Set("TreeArrow_Expanded", new IMAGE_BRUSH("Common/TreeArrow_Expanded", Icon16x16));
    Set("TreeArrow_Expanded_Hovered", new IMAGE_BRUSH("Common/TreeArrow_Expanded_Hovered", Icon16x16));

	Set("Icon.Replace", new IMAGE_BRUSH("Icons/T_Replace", Icon16x16));
}

void FArmyStyleSet::SetColorStyle()
{
    // 常用颜色
    Set("Color.None", FLinearColor(FColor(0X00000000)));
    Set("Color.FF191A1D", FLinearColor(FColor(0XFF191A1D)));
    Set("Color.FF28292B", FLinearColor(FColor(0XFF28292B))); 
    Set("Color.FF343539", FLinearColor(FColor(0XFF343539)));
    Set("Color.FF353535", FLinearColor(FColor(0XFF353535)));
    Set("Color.FF4C4D52", FLinearColor(FColor(0XFF4C4D52)));
    Set("Color.FF656870", FLinearColor(FColor(0XFF656870)));
    Set("Color.489D9FA5", FLinearColor(FColor(0X489D9FA5)));
	Set("Color.FF9D9FA5", FLinearColor(FColor(0XFF9D9FA5)));
	Set("Color.FF313236", FLinearColor(FColor(0XFF313236)));
    Set("Color.FFFD9800", FLinearColor(FColor(0XFFFD9800)));
    Set("Color.FFFF5A5A", FLinearColor(FColor(0XFFFF5A5A)));
    Set("InputBox.Gray", FLinearColor(FColor(0X21FFFFFF)));
    Set("Color.FF404040", FLinearColor(FColor(0XFF404040)));
	Set("Color.FF353638", FLinearColor(FColor(0XFF353638))); 
	Set("Color.FF3F4042", FLinearColor(FColor(0XFF3F4042)));
	Set("Color.FF494A4D", FLinearColor(FColor(0XFF494A4D)));
    Set("Color.FF2F3033", FLinearColor(FColor(0XFF2F3033)));
    Set("Color.FF252629", FLinearColor(FColor(0XFF252629)));
    Set("Color.FF1C1D1F", FLinearColor(FColor(0XFF1C1D1F)));
    Set("Color.FF262729", FLinearColor(FColor(0XFF262729)));
    Set("Color.FF46474D", FLinearColor(FColor(0XFF46474D)));
    Set("Color.99000000", FLinearColor(FColor(0X99000000)));
    Set("Color.Gray.FF212224", FLinearColor(FColor(0XFF212224)));
	Set("Color.Gray.FF2A2B2E", FLinearColor(FColor(0XFF2A2B2E)));
	Set("Color.FF1C1D1E", FLinearColor(FColor(0XFF1C1D1E)));
    Set("Color.FF343538", FLinearColor(FColor(0XFF343538)));
    Set("Color.FFB5B7BE", FLinearColor(FColor(0XFFB5B7BE)));
    Set("Color.Gray.FFC8C9CC", FLinearColor(FColor(0XFFC8C9CC)));
    Set("Color.FFFF9800", FLinearColor(FColor(0XFFFF9800)));
    Set("Color.FF7AAAEF", FLinearColor(FColor(0XFF7AAAEF)));
    Set("Color.FFF6DDDD", FLinearColor(FColor(0XFFF6DDDD)));
	Set("Color.FF383A3D", FLinearColor(FColor(0XFF383A3D)));
	Set("Color.FF35373B", FLinearColor(FColor(0XFF35373B)));
	Set("Color.FF2E2F32", FLinearColor(FColor(0XFF2E2F32))); 
	Set("Color.FF969799", FLinearColor(FColor(0XFF969799))); 
	Set("Color.FF232529", FLinearColor(FColor(0XFF232529))); 
	Set("Color.FF26272B", FLinearColor(FColor(0XFF26272B)));
	Set("Color.FF1D1E21", FLinearColor(FColor(0XFF1D1E21)));
    Set("Color.FF36373B", FLinearColor(FColor(0XFF36373B)));
	Set("Color.FF333438", FLinearColor(FColor(0XFF333438))); 
	Set("Color.FF3C3D42", FLinearColor(FColor(0XFF3C3D42))); 
	Set("Color.FF4D4E52", FLinearColor(FColor(0XFF4D4E52)));
	Set("Color.FF65676B", FLinearColor(FColor(0XFF65676B)));
	Set("Color.FF3C3D42", FLinearColor(FColor(0XFF3C3D42)));
    Set("Color.Capture", FLinearColor(FColor(0XFF00FFFF)));
	Set("Color.FF7D7F85", FLinearColor(FColor(0XFF7D7F85))); 
	Set("Color.FF8BC7A2", FLinearColor(FColor(0XFF8BC7A2)));
	Set("Color.FF2A2E2F", FLinearColor(FColor(0XFF2A2E2F)));
	Set("Color.FFFF8080", FLinearColor(FColor(0XFFFF8080))); 
	Set("Color.FF2E2C2F", FLinearColor(FColor(0XFF2E2C2F)));
	Set("Color.FF5D5F66", FLinearColor(FColor(0XFF5D5F66)));
	Set("Color.FFFF8C00", FLinearColor(FColor(0XFFFF8C00)));
	Set("Color.FFFF8400", FLinearColor(FColor(0XFFFF8400)));
	Set("Color.FFE6E6E6", FLinearColor(FColor(0XFFE6E6E6)));
	Set("Color.FFCB00FF", FLinearColor(FColor(0XFFCB00FF)));
	Set("Color.E5E6E8FF", FLinearColor(FColor(0XE5E6E8FF)));
	Set("Color.FF54555C", FLinearColor(FColor(0XFF54555C)));
	Set("Color.FF666666", FLinearColor(FColor(0XFF666666)));
    Set("Color.FF7D7E80", FLinearColor(FColor(0XFF7D7E80)));
    Set("Color.FF38393D", FLinearColor(FColor(0XFF38393D)));
    Set("Color.FF43444A", FLinearColor(FColor(0XFF43444A)));
    Set("Color.FF0072D6", FLinearColor(FColor(0XFF0072D6)));
    Set("Color.Gray.FF424447", FLinearColor(FColor(0XFF424447)));
    Set("Color.FF585B61", FLinearColor(FColor(0XFF585B61)));
	Set("Color.FFFF4457", FLinearColor(FColor(0XFFFF4457)));
	Set("Color.FF4B4D52", FLinearColor(FColor(0xFF4B4D52)));
    Set("Color.FFF4AD16", FLinearColor(FColor(0XFFF4AD16)));
    Set("Color.Gray.FF111111", FLinearColor(FColor(0XFF111111)));
    Set("Color.FFDC143C", FLinearColor(FColor(0XFFDC143C)));
    Set("Color.Translucent.7F000000", FLinearColor(FColor(0X7F000000)));
	Set("Color.FF212226", FLinearColor(FColor(0XFF212226)));
	Set("Color.FF212224", FLinearColor(FColor(0XFF222124)));
	Set("Color.FF090A0A", FLinearColor(FColor(0XFF090A0A)));
	Set("Color.FFFFC266", FLinearColor(FColor(0XFFFFC266)));
	Set("Color.FF646566", FLinearColor(FColor(0XFF646566)));

}

void FArmyStyleSet::SetFontStyle()
{
    //常用字号的字体
    const FTextBlockStyle NormalText = FTextBlockStyle()
        .SetFont(TTF_FONT("Fonts/Roboto-Regular", 9))
        .SetColorAndOpacity(FSlateColor(FLinearColor::White))
        .SetShadowOffset(FVector2D::ZeroVector);

    const FTextBlockStyle ArmyText_8 = FTextBlockStyle(NormalText)
        .SetFont(TTF_FONT("Fonts/Roboto-Regular", 8))
        .SetColorAndOpacity(FSlateColor(FLinearColor::White))
        .SetShadowOffset(FVector2D::ZeroVector);

    const FTextBlockStyle ArmyText_10 = FTextBlockStyle(NormalText)
        .SetFont(TTF_FONT("Fonts/Roboto-Regular", 10))
        .SetColorAndOpacity(FSlateColor(FLinearColor::White))
        .SetShadowOffset(FVector2D::ZeroVector);

    const FTextBlockStyle ArmyText_12 = FTextBlockStyle(NormalText)
        .SetFont(TTF_FONT("Fonts/Roboto-Regular", 12))
        .SetColorAndOpacity(FSlateColor(FLinearColor::White))
        .SetShadowOffset(FVector2D::ZeroVector);

	const FTextBlockStyle ArmyText_12_Menu = FTextBlockStyle(NormalText)
		.SetFont(TTF_FONT("Fonts/Roboto-Regular", 12))
		.SetColorAndOpacity(FLinearColor(FColor(0XFFC8C9CC)));

	const FTextBlockStyle ArmyText_12_Keybinding = FTextBlockStyle(NormalText)
		.SetFont(TTF_FONT("Fonts/Roboto-Regular", 12))
		.SetColorAndOpacity(FLinearColor(FColor(0XFF646466)));

	const FTextBlockStyle ArmyText_10_DownLead = FTextBlockStyle(NormalText)
		.SetFont(TTF_FONT("Fonts/Roboto-Regular", 10))
		.SetColorAndOpacity(FSlateColor(FColor(0XFF333333)))
		.SetShadowOffset(FVector2D::ZeroVector);

    const FTextBlockStyle ArmyText_14 = FTextBlockStyle(NormalText)
        .SetFont(TTF_FONT("Fonts/Roboto-Regular", 14))
        .SetColorAndOpacity(FSlateColor(FLinearColor::White))
        .SetShadowOffset(FVector2D::ZeroVector);

    const FTextBlockStyle ArmyText_16 = FTextBlockStyle(NormalText)
        .SetFont(TTF_FONT("Fonts/Roboto-Regular", 16))
        .SetColorAndOpacity(FSlateColor(FLinearColor::White))
        .SetShadowOffset(FVector2D::ZeroVector);

    const FTextBlockStyle ArmyText_18 = FTextBlockStyle(NormalText)
		.SetFont(TTF_FONT("Fonts/Roboto-Regular", 18))
		.SetColorAndOpacity(FSlateColor(FLinearColor::White))
		.SetShadowOffset(FVector2D::ZeroVector);

	const FTextBlockStyle ArmyText_24 = FTextBlockStyle(NormalText)
		.SetFont(TTF_FONT("Fonts/Roboto-Regular", 24))
		.SetColorAndOpacity(FSlateColor(FLinearColor::White))
		.SetShadowOffset(FVector2D::ZeroVector);

	const FTextBlockStyle ArmyText_24_Bold = FTextBlockStyle(NormalText)
		.SetFont(TTF_FONT("Fonts/Roboto-Bold", 24))
		.SetColorAndOpacity(FSlateColor(FLinearColor::White))
		.SetShadowOffset(FVector2D::ZeroVector);

	const FTextBlockStyle ArmyText_14_Bold = FTextBlockStyle(NormalText)
		.SetFont(TTF_FONT("Fonts/Roboto-Bold", 14))
		.SetColorAndOpacity(FSlateColor(FLinearColor::White))
		.SetShadowOffset(FVector2D::ZeroVector);

    Set("ArmyText_8", ArmyText_8);
    Set("ArmyText_9", NormalText);
    Set("ArmyText_10", ArmyText_10);
    Set("ArmyText_12", ArmyText_12);
	Set("ArmyText_12_Menu", ArmyText_12_Menu);
	Set("ArmyText_12_Keybinding", ArmyText_12_Keybinding);
	Set("ArmyText_10_DownLead", ArmyText_10_DownLead);
    Set("ArmyText_14", ArmyText_14);
    Set("ArmyText_16", ArmyText_16);
    Set("ArmyText_18", ArmyText_18);
	Set("ArmyText_24", ArmyText_24);
    Set("ArmyText_24_Bold", ArmyText_24_Bold);
	Set("ArmyText_14_Bold", ArmyText_14_Bold);

	//@欧石楠
	//富文本样式

	//方案估价界面价格
	Set("ArmyRichTextBlock.Price", FTextBlockStyle(ArmyText_24)
		.SetColorAndOpacity(FLinearColor(FColor(0xFF, 0x98, 0))));
		//.SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f)));
}

void FArmyStyleSet::SetWidgetStyle()
{
    // EditableText
    {
        Set("EditableText", FEditableTextStyle()
            .SetColorAndOpacity(FLinearColor::Black)
            .SetFont(TTF_FONT("Fonts/Roboto-Regular", 12)));
    }

    // EditableTextBoxStyle
    {
        Set("EditableTextBox", FEditableTextBoxStyle()
            .SetBackgroundImageNormal(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.FF38393D")))
            .SetBackgroundImageHovered(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.FF43444A")))
            .SetBackgroundImageFocused(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.FF43444A")))
            .SetBackgroundImageReadOnly(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.FF2F3033")))
            .SetPadding(FMargin(8, 0))
            .SetFont(TTF_FONT("Fonts/Roboto-Regular", 12))
            .SetForegroundColor(GetColor("Color.FFFFFFFF"))
            .SetReadOnlyForegroundColor(GetColor("Color.FF7D7E80"))
        );

        Set("EditableTextBox.Gray", FEditableTextBoxStyle()
            .SetBackgroundImageNormal(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.Gray.FF212224")))
            .SetBackgroundImageHovered(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.FF28292B")))
            .SetBackgroundImageFocused(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.Gray.FF212224")))
            .SetBackgroundImageReadOnly(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.Gray.FF212224")))
            .SetFont(TTF_FONT("Fonts/Roboto-Regular", 12))
            .SetForegroundColor(GetColor("Color.FF9D9FA5"))
        );

        Set("EditableTextBox.FF2F3033", FEditableTextBoxStyle()
            .SetBackgroundImageNormal(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.FF2F3033")))
            .SetBackgroundImageHovered(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.FF3C3D42")))
            .SetBackgroundImageFocused(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.FF2F3033")))
            .SetBackgroundImageReadOnly(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.FF2F3033")))
            .SetFont(TTF_FONT("Fonts/Roboto-Regular", 12))
            .SetForegroundColor(GetColor("Color.FF9D9FA5"))
        );

        Set("EditableTextBox.FF212224", FEditableTextBoxStyle()
            .SetBackgroundImageNormal(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.Gray.FF212224")))
            .SetBackgroundImageHovered(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.Gray.FF212224")))
            .SetBackgroundImageFocused(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.Gray.FF212224")))
            .SetBackgroundImageReadOnly(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.Gray.FF212224")))
            .SetFont(TTF_FONT("Fonts/Roboto-Regular", 12))
            .SetForegroundColor(GetColor("Color.FF9D9FA5"))
        );

        Set("EditableTextBox.FF2E2F32", FEditableTextBoxStyle()
            .SetBackgroundImageNormal(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.FF2E2F32")))
            .SetBackgroundImageHovered(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.FF2E2F32")))
            .SetBackgroundImageFocused(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.FF2E2F32")))
            .SetBackgroundImageReadOnly(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.FF2E2F32")))
            .SetFont(TTF_FONT("Fonts/Roboto-Regular", 12))
            .SetForegroundColor(GetColor("Color.FF969799"))
        );

        Set("EditableTextBox.FFFFFFFF", FEditableTextBoxStyle()
            .SetBackgroundImageNormal(BOX_BRUSH("Icons/TestWhite", FVector2D(16, 16), FMargin(0), GetColor("Color.FFFFFFFF")))
            .SetBackgroundImageHovered(BOX_BRUSH("Icons/TestWhite", FVector2D(16, 16), FMargin(0), GetColor("Color.FFFFFFFF")))
            .SetBackgroundImageFocused(BOX_BRUSH("Icons/TestWhite", FVector2D(16, 16), FMargin(0), GetColor("Color.FFFFFFFF")))
            .SetBackgroundImageReadOnly(BOX_BRUSH("Icons/TestWhite", FVector2D(16, 16), FMargin(0), GetColor("Color.FFFFFFFF")))
            .SetFont(TTF_FONT("Fonts/Roboto-Regular", 12))
            .SetForegroundColor(GetColor("Color.FFFFFFFF"))
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

		Set("CheckBox.Yellow",FCheckBoxStyle()
			.SetUncheckedImage(IMAGE_BRUSH("Icons/checkbox_normal",Icon14x14))
			.SetUncheckedHoveredImage(IMAGE_BRUSH("Icons/checkbox_hover",Icon14x14))
			.SetUncheckedPressedImage(IMAGE_BRUSH("Icons/Checkbox_hover",Icon14x14))
			.SetCheckedImage(IMAGE_BRUSH("Icons/checkbox_active",Icon14x14)) 
			.SetCheckedHoveredImage(IMAGE_BRUSH("Icons/checkbox_active",Icon14x14))
			.SetCheckedPressedImage(IMAGE_BRUSH("Icons/checkbox_active",Icon14x14))
			.SetPadding(FMargin(0))
		);

		Set("CheckBox.AddDelete",FCheckBoxStyle()
			.SetUncheckedImage(IMAGE_BRUSH("Icons/add",Icon14x14))
			.SetUncheckedHoveredImage(IMAGE_BRUSH("Icons/add",Icon14x14))
			.SetUncheckedPressedImage(IMAGE_BRUSH("Icons/add",Icon14x14))
			.SetCheckedImage(IMAGE_BRUSH("Icons/done",Icon14x14))
			.SetCheckedHoveredImage(IMAGE_BRUSH("Icons/done",Icon14x14))
			.SetCheckedPressedImage(IMAGE_BRUSH("Icons/done",Icon14x14))
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

		Set("SingleCheckBox", FCheckBoxStyle()
            .SetCheckedImage(IMAGE_BRUSH("Icons/Checkbox_active", Icon14x14))
            .SetCheckedPressedImage(IMAGE_BRUSH("Icons/Checkbox_active", Icon14x14))
            .SetCheckedHoveredImage(IMAGE_BRUSH("Icons/Checkbox_active", Icon14x14))
            .SetUncheckedImage(IMAGE_BRUSH("Icons/Checkbox_normal", Icon14x14))
            .SetUncheckedPressedImage(IMAGE_BRUSH("Icons/Checkbox_hover", Icon14x14))
            .SetUncheckedHoveredImage(IMAGE_BRUSH("Icons/Checkbox_hover", Icon14x14))
			.SetPadding(FMargin(0))
        );

		//@   郭子陽
		Set("CheckBox.RadioBox", FCheckBoxStyle()
			.SetCheckedImage(IMAGE_BRUSH("Icons/SingleCheckBox_Checked", Icon14x14))
			.SetCheckedPressedImage(IMAGE_BRUSH("Icons/SingleCheckBox_Checked", Icon14x14))
			.SetCheckedHoveredImage(IMAGE_BRUSH("Icons/SingleCheckBox_Checked", Icon14x14))
			.SetUncheckedImage(IMAGE_BRUSH("Icons/SingleCheckBox_Unchecked", Icon14x14))
			.SetUncheckedPressedImage(IMAGE_BRUSH("Icons/SingleCheckBox_Hover", Icon14x14))
			.SetUncheckedHoveredImage(IMAGE_BRUSH("Icons/SingleCheckBox_Hover", Icon14x14))
			.SetPadding(FMargin(0))
		);

		//用于添加的水电模式checkbox
		Set("CheckBox.AddBox", FCheckBoxStyle()
			.SetCheckedImage(IMAGE_BRUSH("Icons/Confirm_Normal", Icon14x14))
			.SetCheckedPressedImage(IMAGE_BRUSH("Icons/Confirm_Hover", Icon14x14))
			.SetCheckedHoveredImage(IMAGE_BRUSH("Icons/Confirm_Hover", Icon14x14))
			.SetUncheckedImage(IMAGE_BRUSH("Icons/Add_Normal", Icon14x14))
			.SetUncheckedPressedImage(IMAGE_BRUSH("Icons/Add_Hover", Icon14x14))
			.SetUncheckedHoveredImage(IMAGE_BRUSH("Icons/Add_Hover", Icon14x14))
			.SetPadding(FMargin(0))
		);


		Set("AutoDesignCheckBox", FCheckBoxStyle()
			.SetCheckedImage(IMAGE_BRUSH("Icons/Checkbox_active", Icon14x14))
			.SetCheckedPressedImage(IMAGE_BRUSH("Icons/Checkbox_active", Icon14x14))
			.SetCheckedHoveredImage(IMAGE_BRUSH("Icons/Checkbox_active", Icon14x14))
			.SetUncheckedImage(IMAGE_BRUSH("Icons/Checkbox_normal", Icon14x14))
			.SetUncheckedPressedImage(IMAGE_BRUSH("Icons/Checkbox_hover", Icon14x14))
			.SetUncheckedHoveredImage(IMAGE_BRUSH("Icons/Checkbox_hover", Icon14x14))
			.SetPadding(FMargin(0))
		
		);
    }

    // ComboBoxStyle
	{
		Set("ComboBox.Button.FF212224", FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.Gray.FF212224")))
			.SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF232529")))
			.SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF232529")))
			.SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF2F3033")))
		);

		Set("HardModeExit.Button", FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF2F3033")))
			.SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.Gray.FF2A2B2E")))
			.SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF232529")))
			.SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF2F3033")))
		);

        Set("ComboBox.Button.Gray", FButtonStyle()
            .SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF2F3033")))
            .SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF1C1D1F")))
            .SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF2F3033")))
            .SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF2F3033")))
        );

		Set("ComboBox.Button.Gray1",FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/White",Icon12x12,GetColor("Color.FF2F3033")))
			.SetHovered(IMAGE_BRUSH("Icons/White",Icon12x12,GetColor("Color.FF2F3033")))
			.SetPressed(IMAGE_BRUSH("Icons/White",Icon12x12,GetColor("Color.FF2F3033")))
			.SetDisabled(IMAGE_BRUSH("Icons/White",Icon12x12,GetColor("Color.FF2F3033")))
		);

        FComboButtonStyle ComboButtonStyleNone = FComboButtonStyle()
            .SetDownArrowImage(FSlateNoResource())
            .SetMenuBorderBrush(FSlateNoResource())
            .SetMenuBorderPadding(FMargin(0));

        Set("ComboButton.None", ComboButtonStyleNone);

        Set("ComboBox.None", FComboBoxStyle()
            .SetComboButtonStyle(ComboButtonStyleNone)
        );

		FComboButtonStyle ComboButtonStyleGray = FComboButtonStyle()
			.SetButtonStyle(GetWidgetStyle<FButtonStyle>("ComboBox.Button.Gray1"))
			.SetDownArrowImage(IMAGE_BRUSH("Icons/DownArrow_White", Icon16x16, FLinearColor::White))
			.SetMenuBorderBrush(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.Gray.FF212224")))
			.SetMenuBorderPadding(FMargin(0));

		Set("ComboBox.Gray", FComboBoxStyle()
			.SetComboButtonStyle(ComboButtonStyleGray)
		);

		const FButtonStyle ComButtonBlack = FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/White", Icon32x32, GetColor("Color.Gray.FF212224")))
			.SetHovered(IMAGE_BRUSH("Icons/White", Icon32x32, GetColor("Color.Gray.FF212224")))
			.SetPressed(IMAGE_BRUSH("Icons/White", Icon32x32, GetColor("Color.Gray.FF212224")))
			.SetNormalPadding(FMargin(0))
			.SetPressedPadding(FMargin(0));

		FComboButtonStyle ComboButtonStyleBlack = FComboButtonStyle()
			.SetButtonStyle(ComButtonBlack)
			.SetDownArrowImage(IMAGE_BRUSH("Icons/DownArrow_White", Icon16x16, FLinearColor::White))
			.SetMenuBorderBrush(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.Gray.FF212224")))
			.SetMenuBorderPadding(FMargin(0));

		Set("ComboBox.Black", FComboBoxStyle()
			.SetComboButtonStyle(ComboButtonStyleBlack)
		);

		const FButtonStyle ComButton = FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/White", Icon32x32, GetColor("Color.FF38393D")))
			.SetHovered(IMAGE_BRUSH("Icons/White", Icon32x32, GetColor("Color.FF43444A")))
			.SetPressed(IMAGE_BRUSH("Icons/White", Icon32x32, GetColor("Color.FF43444A")))
            .SetDisabled(IMAGE_BRUSH("Icons/White", Icon32x32, GetColor("Color.FF2F3033")))
			.SetNormalPadding(FMargin(8, 0))
			.SetPressedPadding(FMargin(8, 0));

		FComboButtonStyle ComboButtonStyle = FComboButtonStyle()
			.SetButtonStyle(ComButton)
			.SetDownArrowImage(IMAGE_BRUSH("Icons/DownArrow_White", Icon16x16))  // , GetColor("Color.FF9D9FA5")
			.SetMenuBorderBrush(BOX_BRUSH("Icons/White", FVector2D(4, 4), FMargin(0), GetColor("Color.Gray.FF424447")))
			.SetMenuBorderPadding(FMargin(0));

		Set("ComboBox", FComboBoxStyle()
			.SetComboButtonStyle(ComboButtonStyle)
		);

		FButtonStyle BaseButtonStyle;
		BaseButtonStyle
			.SetNormal(FSlateImageBrush(FPaths::EngineContentDir() / TEXT("Slate/Old/White.png"), FVector2D(1.0f, 1.0f), FLinearColor::White, ESlateBrushTileType::Both))
			.SetHovered(FSlateImageBrush(FPaths::EngineContentDir() / TEXT("Slate/Old/White.png"), FVector2D(1.0f, 1.0f), FLinearColor::White, ESlateBrushTileType::Both))
			.SetPressed(FSlateImageBrush(FPaths::EngineContentDir() / TEXT("Slate/Old/White.png"), FVector2D(1.0f, 1.0f), FLinearColor::White, ESlateBrushTileType::Both))
			.SetDisabled(FSlateImageBrush(FPaths::EngineContentDir() / TEXT("Slate/Old/White.png"), FVector2D(1.0f, 1.0f), FLinearColor::White, ESlateBrushTileType::Both));
		Set("BaseButtonStyle.White", BaseButtonStyle);
    }

    // TableRowStyle
    {
        Set("TableRow.Gray", FTableRowStyle()
            .SetSelectorFocusedBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.None")))
            .SetActiveHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF383A3D")))
            .SetActiveBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF383A3D")))
            .SetInactiveHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF383A3D")))
            .SetInactiveBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.None")))
            .SetEvenRowBackgroundHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF383A3D")))
            .SetEvenRowBackgroundBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.None")))
            .SetOddRowBackgroundHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF383A3D")))
            .SetOddRowBackgroundBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.None")))
            .SetTextColor(GetColor("Color.Gray.FFC8C9CC"))
            .SetSelectedTextColor(GetColor("Color.FFFF9800"))
        );
	}
	{
		//可获得焦点的TableRowStyle  欧石楠
		Set("TableRow.GrayWithFocused", FTableRowStyle()
			.SetSelectorFocusedBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF383A3D")))
			.SetActiveHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF383A3D")))
			.SetActiveBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF383A3D")))
			.SetInactiveHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF383A3D")))
			.SetInactiveBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF383A3D")))
			.SetEvenRowBackgroundHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF383A3D")))
			.SetEvenRowBackgroundBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.None")))
			.SetOddRowBackgroundHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF383A3D")))
			.SetOddRowBackgroundBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.None")))
			.SetTextColor(GetColor("Color.Gray.FFC8C9CC"))
			.SetSelectedTextColor(GetColor("Color.FFFF9800"))
		);
	}



	{
		Set("TableRow.FF28292B",FTableRowStyle()
			.SetSelectorFocusedBrush(IMAGE_BRUSH("Icons/White",Icon1x1,GetColor("Color.FF28292B")))
			.SetActiveHoveredBrush(IMAGE_BRUSH("Icons/White",Icon1x1,GetColor("Color.FF28292B")))
			.SetActiveBrush(IMAGE_BRUSH("Icons/White",Icon1x1,GetColor("Color.FF28292B")))
			.SetInactiveHoveredBrush(IMAGE_BRUSH("Icons/White",Icon1x1,GetColor("Color.FF28292B")))
			.SetInactiveBrush(IMAGE_BRUSH("Icons/White",Icon1x1,GetColor("Color.FF28292B")))
			.SetEvenRowBackgroundHoveredBrush(IMAGE_BRUSH("Icons/White",Icon1x1,GetColor("Color.FF28292B")))
			.SetEvenRowBackgroundBrush(IMAGE_BRUSH("Icons/White",Icon1x1,GetColor("Color.FF28292B")))
			.SetOddRowBackgroundHoveredBrush(IMAGE_BRUSH("Icons/White",Icon1x1,GetColor("Color.FF28292B")))
			.SetOddRowBackgroundBrush(IMAGE_BRUSH("Icons/White",Icon1x1,GetColor("Color.FF28292B")))
			.SetTextColor(GetColor("Color.FF38393D"))
			.SetSelectedTextColor(GetColor("Color.FF38393D"))
		);
	}

	// TableRowStyle
	{
		Set("TableRow.Layers", FTableRowStyle()
			.SetSelectorFocusedBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.Gray.FF2A2B2E")))
			.SetActiveHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF333438")))
			.SetActiveBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF333438")))
			.SetInactiveHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF333438")))
			.SetInactiveBrush(IMAGE_BRUSH("Icons/White", Icon1x1, MakeShareable(new FLinearColor(GetColor("Color.FF333438")))))
			.SetEvenRowBackgroundHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF333438")))
			.SetEvenRowBackgroundBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.Gray.FF2A2B2E")))
			.SetOddRowBackgroundHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF333438")))
			.SetOddRowBackgroundBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.Gray.FF2A2B2E")))
			.SetTextColor(GetColor("Color.Gray.FFC8C9CC"))
			.SetSelectedTextColor(GetColor("Color.FFFF9800"))
		);

		Set("TableRow.Normal", FTableRowStyle()
			.SetSelectorFocusedBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.Gray.FF2A2B2E")))
			.SetActiveHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF333438")))
			.SetActiveBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF333438")))
			.SetInactiveHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF333438")))
			.SetInactiveBrush(IMAGE_BRUSH("Icons/White", Icon1x1, MakeShareable(new FLinearColor(GetColor("Color.FF333438")))))
			.SetEvenRowBackgroundHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF333438")))
			.SetEvenRowBackgroundBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.Gray.FF2A2B2E")))
			.SetOddRowBackgroundHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF333438")))
			.SetOddRowBackgroundBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.Gray.FF2A2B2E")))
			.SetTextColor(GetColor("Color.Gray.FFC8C9CC"))
			.SetSelectedTextColor(GetColor("Color.FFFFFFFF"))
		);
	}

	// TableRowStyle
	/**@欧石楠 模式切换*/
	{
		Set("TableRow.ModeChange", FTableRowStyle()
			.SetSelectorFocusedBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF343538")))
			.SetActiveHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF343538")))
			.SetActiveBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF383A3D")))
			.SetInactiveHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF343538")))
			.SetInactiveBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF383A3D")))
			.SetEvenRowBackgroundHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF343538")))
			.SetEvenRowBackgroundBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF383A3D")))
			.SetOddRowBackgroundHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF343538")))
			.SetOddRowBackgroundBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF383A3D")))
			.SetTextColor(GetColor("Color.Gray.FFC8C9CC"))
			.SetSelectedTextColor(GetColor("Color.FFFF9800"))
		);
	}

	// TableRowStyle
	/**@欧石楠 属性下拉*/
	{
		Set("TableRow.PropertyCombobox", FTableRowStyle()
			.SetSelectorFocusedBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF26272B")))
			.SetActiveHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF26272B")))
			.SetActiveBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF1D1E21")))
			.SetInactiveHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF26272B")))
			.SetInactiveBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF1D1E21")))
			.SetEvenRowBackgroundHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF26272B")))
			.SetEvenRowBackgroundBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF1D1E21")))
			.SetOddRowBackgroundHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF26272B")))
			.SetOddRowBackgroundBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF1D1E21")))
			.SetTextColor(GetColor("Color.FFFF9800"))
			.SetSelectedTextColor(GetColor("Color.FFFF9800"))
		);
	
        Set("ComboBox.Button.Property", FButtonStyle()
            .SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF38393D")))
            .SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF43444A")))
            .SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF43444A")))
            .SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF2F3033")))
        );

		Set("TableRow.Property", FTableRowStyle()
            .SetSelectorFocusedBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.Gray.FF424447")))
            .SetActiveHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF585B61")))
            .SetActiveBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF585B61")))
            .SetInactiveHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF585B61")))
            .SetInactiveBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.Gray.FF424447")))
            .SetEvenRowBackgroundHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF585B61")))
            .SetEvenRowBackgroundBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.Gray.FF424447")))
            .SetOddRowBackgroundHoveredBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF585B61")))
            .SetOddRowBackgroundBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.Gray.FF424447")))
            .SetTextColor(GetColor("Color.FFFFFFFF"))
            .SetSelectedTextColor(GetColor("Color.FFFF9800"))
		);
	}

    // ButtonStyle
    {
		Set("Button.Orange.Hover", FLinearColor(FColor(0XFFFF8C00)));
		Set("Button.Orange.Pressed", FLinearColor(FColor(0XFFFF8400)));

		/**@欧石楠 轮播控件小按钮样式*/
		Set("Button.CarouseButton", FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/PurePoint", Icon16x16, GetColor("Color.FFFFFFFF")))
			.SetHovered(IMAGE_BRUSH("Icons/PurePoint", Icon16x16, GetColor("Color.FFFFFFFF")))
			.SetPressed(IMAGE_BRUSH("Icons/PurePoint", Icon16x16, GetColor("Color.FFFFFFFF")))
			.SetDisabled(IMAGE_BRUSH("Icons/PurePoint", Icon16x16, GetColor("Color.FFFD9800")))
		);

        Set("Button.Orange", FButtonStyle()
            .SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FFFD9800")))
            .SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Button.Orange.Hover")))
            .SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Button.Orange.Pressed")))
            .SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FFFD9800")))
        );

		Set("Button.FFFF9800", FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FFFF9800")))
			.SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FFFF8C00")))
			.SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FFFF8400")))
			.SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FFFD9800")))
		);

        Set("Button.None", FButtonStyle()
            .SetNormal(FSlateNoResource())
            .SetHovered(FSlateNoResource())
            .SetPressed(FSlateNoResource())
            .SetDisabled(FSlateNoResource())
        );

        Set("Button.FF2E2F32", FButtonStyle()
            .SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF2E2F32")))
            .SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF36373B")))
            .SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF36373B")))
            .SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF2E2F32")))
        );

		Set("Button.MasterPlanGray", FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FFFFC266")))
			.SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FFFFC266")))
			.SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FFFFC266")))
			.SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FFFFC266")))
		);

        Set("Button.FF4C4D52", FButtonStyle()
            .SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF4C4D52")))
            .SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF656870")))
            .SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF4C4D52")))
            .SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF4C4D52")))
        );

        Set("Button.FF2A2B2E", FButtonStyle()
            .SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.Gray.FF2A2B2E")))
            .SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF1C1D1F")))
            .SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF1C1D1F")))
            .SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.Gray.FF2A2B2E")))
		);

		Set("Button.MasterPreview", FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/preview_normal", Icon24x24, GetColor("Color.FFFFFFFF")))
			.SetHovered(IMAGE_BRUSH("Icons/preview_hover", Icon24x24, GetColor("Color.FFFFFFFF")))
			.SetPressed(IMAGE_BRUSH("Icons/preview_hover", Icon24x24, GetColor("Color.FFFFFFFF")))
			.SetDisabled(IMAGE_BRUSH("Icons/preview_normal", Icon24x24, GetColor("Color.FFFFFFFF")))
		);

		Set("Button.Translucent.7F000000", FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.None")))
			.SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.Translucent.7F000000")))
			.SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.Translucent.7F000000")))
			.SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.Translucent.7F000000")))
		);

        Set("Button.FF343538", FButtonStyle()
            .SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF343538")))
            .SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF1C1D1F")))
            .SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF343538")))
            .SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF343538")))
		);

		Set("Button.Gray.FF353638", FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF353638")))
			.SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF3F4042")))
			.SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF494A4D")))
			.SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF353638")))
		);

		Set("Button.GrayToOrange", FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF353638")))
			.SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FFFF9800")))
			.SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FFFF9800")))
			.SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FFFF9800")))
		);

        Set("Button.Search", FButtonStyle()
            .SetNormal(IMAGE_BRUSH("Icons/T_Search", Icon16x16, GetColor("Color.FF9D9FA5")))
            .SetHovered(IMAGE_BRUSH("Icons/T_Search", Icon16x16, GetColor("Color.Gray.FF111111")))
            .SetPressed(IMAGE_BRUSH("Icons/T_Search", Icon16x16, GetColor("Color.FF9D9FA5")))
            .SetDisabled(IMAGE_BRUSH("Icons/T_Search", Icon16x16, GetColor("Color.FF9D9FA5")))
        );

        Set("Button.Minimize", FButtonStyle()
            .SetNormal(IMAGE_BRUSH("Icons/Minimize", Icon24x24, FLinearColor(FColor(0XFFC8C9CC))))//@欧石楠 最小化颜色
            .SetHovered(IMAGE_BRUSH("Icons/Minimize", Icon24x24, FLinearColor::White))
            .SetPressed(IMAGE_BRUSH("Icons/Minimize", Icon24x24, FLinearColor::White))
            .SetDisabled(IMAGE_BRUSH("Icons/Minimize", Icon24x24, FLinearColor::White))
        );

        Set("Button.CloseApplication", FButtonStyle()
            .SetNormal(IMAGE_BRUSH("Icons/CloseApplication", Icon24x24, FLinearColor(FColor(0XFFC8C9CC))))//@欧石楠 关闭颜色
            .SetHovered(IMAGE_BRUSH("Icons/CloseApplication", Icon24x24, FLinearColor::White))
            .SetPressed(IMAGE_BRUSH("Icons/CloseApplication", Icon24x24, FLinearColor::White))
            .SetDisabled(IMAGE_BRUSH("Icons/CloseApplication", Icon24x24, FLinearColor::White))
        );

		Set("Button.Close", FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/T_Close", Icon16x16, GetColor("Color.FF9D9FA5")))
			.SetHovered(IMAGE_BRUSH("Icons/T_Close", Icon16x16, GetColor("Color.Gray.FF111111")))
			.SetPressed(IMAGE_BRUSH("Icons/T_Close", Icon16x16, GetColor("Color.FF9D9FA5")))
			.SetDisabled(IMAGE_BRUSH("Icons/T_Close", Icon16x16, GetColor("Color.FF9D9FA5")))
		);

		Set("Button.FF4D4E52", FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF4D4E52")))
			.SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF65676B")))
			.SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF7D7F85")))
			.SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF4D4E52")))
		);

		Set("Button.FFDC143C", FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/information", Icon12x12))
			.SetPressed(IMAGE_BRUSH("Icons/information_hover", Icon12x12))
			.SetHovered(IMAGE_BRUSH("Icons/information_hover", Icon12x12))

        );

		Set("Button.Del",FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/del",Icon32x32,FColor::White))
			.SetPressed(IMAGE_BRUSH("Icons/del_press",Icon32x32,FColor::White))
			.SetHovered(IMAGE_BRUSH("Icons/del_hover",Icon32x32,FColor::White))

		);

		Set("Button.GreatPlanBack",FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/slide_left_normal", Icon32x32))
			.SetPressed(IMAGE_BRUSH("Icons/slide_left_hover", Icon32x32))
			.SetHovered(IMAGE_BRUSH("Icons/slide_left_hover", Icon32x32))
		);

		Set("Button.GreatPlanForward", FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/slide_right_normal", Icon32x32))
			.SetPressed(IMAGE_BRUSH("Icons/slide_right_hover", Icon32x32))
			.SetHovered(IMAGE_BRUSH("Icons/slide_right_hover", Icon32x32))
		);


		Set("Button.ContentBrowerLeftRoot", FButtonStyle()
			.SetNormal(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF212224")))
			.SetHovered(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.FF212224")))
			.SetPressed(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.Gray.FF2A2B2E")))
			.SetDisabled(IMAGE_BRUSH("Icons/White", Icon12x12, GetColor("Color.Gray.FF2A2B2E")))
		);
		
    }

	/**@欧石楠 登录界面 scrollbox  */
	{
		Set("LoginScrollBox", FScrollBoxStyle()
		.SetTopShadowBrush(BOX_BRUSH("Common/ScrollBoxShadowTop", FVector2D(16, 8), FMargin(0, 0, 0, 0), FLinearColor(FColor(1, 1, 1, 0))))
		.SetBottomShadowBrush(BOX_BRUSH("Common/ScrollBoxShadowBottom", FVector2D(16, 8), FMargin(0, 0, 0, 0), FLinearColor(FColor(1, 1, 1, 0))))
		.SetLeftShadowBrush(BOX_BRUSH("Common/ScrollBoxShadowLeft", FVector2D(8, 16), FMargin(0, 0, 0, 0), FLinearColor(FColor(1, 1, 1, 0))))
		.SetRightShadowBrush(BOX_BRUSH("Common/ScrollBoxShadowRight", FVector2D(8, 16), FMargin(0, 0, 0, 0), FLinearColor(FColor(1, 1, 1, 0))))
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

		Set("ScrollBar.BarStyle.FF262729", FScrollBarStyle()
			.SetVerticalTopSlotImage(IMAGE_BRUSH("Icons/White", FVector2D(4, 4), GetColor("Color.FFFF9800")))
			.SetVerticalBottomSlotImage(IMAGE_BRUSH("Icons/White", FVector2D(4, 4), GetColor("Color.FFFF9800")))
			.SetNormalThumbImage(IMAGE_BRUSH("Icons/White", FVector2D(4, 4), GetColor("Color.FFFF9800")))
			.SetHoveredThumbImage(IMAGE_BRUSH("Icons/White", FVector2D(4, 4), GetColor("Color.FFFF9800")))
			.SetDraggedThumbImage(IMAGE_BRUSH("Icons/White", FVector2D(4, 4), GetColor("Color.FFFF9800")))
		);
    }

    // SpinBoxStyle
    {
        Set("SpinBoxStyle", FSpinBoxStyle()
            .SetBackgroundBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF38393D")))
            .SetHoveredBackgroundBrush(IMAGE_BRUSH("Icons/White", Icon1x1, GetColor("Color.FF43444A")))
            .SetActiveFillBrush(IMAGE_BRUSH("Icons/SpinBoxUnderLine", Icon24x24, GetColor("Color.FF43444A")))
            .SetInactiveFillBrush(IMAGE_BRUSH("Icons/SpinBoxUnderLine", Icon24x24, GetColor("Color.FF2F3033")))
            .SetArrowsImage(IMAGE_BRUSH("Icons/SpinArrows", Icon12x12, GetColor("Color.FF9D9FA5")))
            .SetForegroundColor(GetColor("Color.FFFFFFFF"))
            .SetTextPadding(FMargin(8, 0))
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
			.SetTextStyle(GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
			.SetEditableTextBoxStyle(InlineEditableTextBlockEditable);

		Set("InlineEditableTextBlockStyle", InlineEditableTextBlockStyle);
	}

	//
	{
		/* Create style for "ToolBar.ToggleButton" widget ... */
		const FCheckBoxStyle ToolBarToggleButtonCheckBoxStyle = FCheckBoxStyle()
			.SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
			.SetUncheckedImage(FSlateNoResource())
			.SetUncheckedPressedImage(BOX_BRUSH("Old/White", 4.0f / 16.0f, GetColor("Color.FF28292B")))
			.SetUncheckedHoveredImage(BOX_BRUSH("Old/White", 4.0f / 16.0f, GetColor("Color.FF28292B")))
			.SetCheckedImage(BOX_BRUSH("Old/White", 4.0f / 16.0f, FLinearColor(FColor(0XFF1C1D1F))))
			.SetCheckedHoveredImage(BOX_BRUSH("Old/White", 4.0f / 16.0f, FLinearColor(FColor(0XFF1C1D1F))))
			.SetCheckedPressedImage(BOX_BRUSH("Old/White", 4.0f / 16.0f, FLinearColor(FColor(0XFF1C1D1F))))			
			;
		/* ... and add new style */
		Set("ToolBar.ToggleButton", ToolBarToggleButtonCheckBoxStyle);
	}
}

void FArmyStyleSet::SetEditorStyle()
{
	//Tool Bar
	{
		// SButton defaults...
        const FButtonStyle TestButton = FButtonStyle()
            .SetNormal(IMAGE_BRUSH("Icons/White", Icon32x32, GetColor("Color.FF2F3033")))
            .SetHovered(IMAGE_BRUSH("Icons/White", Icon32x32, GetColor("Color.FF1C1D1F")))
            .SetPressed(IMAGE_BRUSH("Icons/White", Icon32x32, GetColor("Color.Gray.FF2A2B2E")))
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

		Set("ToolBar.Label", GetWidgetStyle<FTextBlockStyle>("ArmyText_12"));
		//Set("ToolBar.EditableText", FEditableTextBoxStyle(NormalEditableTextBoxStyle).SetFont(TTF_FONT("Fonts/Roboto-Regular", 9)));
		Set("ToolBar.Keybinding", GetWidgetStyle<FTextBlockStyle>("ArmyText_12"));

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

		Set("ClassToolBar.Label", GetWidgetStyle<FTextBlockStyle>("ArmyText_12"));
		//Set("ClassToolBar.EditableText", FEditableTextBoxStyle(NormalEditableTextBoxStyle).SetFont(TTF_FONT("Fonts/Roboto-Regular", 9)));
		Set("ClassToolBar.Keybinding", GetWidgetStyle<FTextBlockStyle>("ArmyText_12"));

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

        Set("ClassToolBar.Button", GetWidgetStyle<FButtonStyle>("Button.FF343538"));
	}

    // MenuBar
    {
        Set("Menu.Background", new BOX_BRUSH("Icons/White", FMargin(8.0f / 64.0f), GetColor("Color.FF35373B")));//欧石楠，背景色
        Set("Menu.Icon", new IMAGE_BRUSH("Icons/icon_tab_toolbar_16px", Icon16x16));
        Set("Menu.Expand", new IMAGE_BRUSH("Icons/toolbar_expand_16x", Icon16x16));
        Set("Menu.SubMenuIndicator", new IMAGE_BRUSH("Common/SubmenuArrow", Icon8x8));
        Set("Menu.SToolBarComboButtonBlock.Padding", FMargin(4.0f));
        Set("Menu.SToolBarButtonBlock.Padding", FMargin(4.0f));
        Set("Menu.SToolBarCheckComboButtonBlock.Padding", FMargin(4.0f));
        Set("Menu.SToolBarButtonBlock.CheckBox.Padding", FMargin(0.0f));
        //Set("Menu.SToolBarComboButtonBlock.ComboButton.Color", DefaultForeground);

        Set("Menu.Block.IndentedPadding", FMargin(18.0f, 2.0f, 4.0f, 4.0f));
        Set("Menu.Block.Padding", FMargin(2.0f, 2.0f, 4.0f, 4.0f));

		
        Set("Menu.Separator", new BOX_BRUSH("Old/Button", 4.0f / 32.0f));
        Set("Menu.Separator.Padding", FMargin(0.f));/**@欧石楠 去掉分割线*/

        Set("Menu.Label", GetWidgetStyle<FTextBlockStyle>("ArmyText_12_Menu"));
        Set("Menu.Keybinding", GetWidgetStyle<FTextBlockStyle>("ArmyText_12_Keybinding"));

        //Set("Menu.Heading", FTextBlockStyle(NormalText)
        //    .SetFont(TTF_FONT("Fonts/Roboto-Regular", 8))
        //    .SetColorAndOpacity(FLinearColor(0.4f, 0.4, 0.4f, 1.0f)));

        /* Set images for various SCheckBox states associated with menu check box items... */
        const FCheckBoxStyle BasicMenuCheckBoxStyle = FCheckBoxStyle()
            .SetUncheckedImage(IMAGE_BRUSH("Common/MenuBar_Unchecked", Icon14x14))
            .SetUncheckedHoveredImage(IMAGE_BRUSH("Common/MenuBar_Unchecked", Icon14x14))
            .SetUncheckedPressedImage(IMAGE_BRUSH("Common/MenuBar_Unchecked", Icon14x14, FLinearColor(0.5f, 0.5f, 0.5f)))
            .SetCheckedImage(IMAGE_BRUSH("Common/MenuBar_Checked", Icon14x14))
            .SetCheckedHoveredImage(IMAGE_BRUSH("Common/MenuBar_Checked", Icon14x14))
            .SetCheckedPressedImage(IMAGE_BRUSH("Common/MenuBar_Checked", Icon14x14, FLinearColor(0.5f, 0.5f, 0.5f)))
            .SetUndeterminedImage(IMAGE_BRUSH("Common/MenuBar_Unchecked", Icon14x14))
            .SetUndeterminedHoveredImage(IMAGE_BRUSH("Common/MenuBar_Unchecked", Icon14x14))
            .SetUndeterminedPressedImage(IMAGE_BRUSH("Common/MenuBar_Unchecked", Icon14x14, FLinearColor(0.5f, 0.5f, 0.5f)));

        /* ...and add the new style */
        Set("Menu.CheckBox", BasicMenuCheckBoxStyle);

        /* Read-only checkbox that appears next to a menu item */
        /* Set images for various SCheckBox states associated with read-only menu check box items... */
        const FCheckBoxStyle BasicMenuCheckStyle = FCheckBoxStyle()
            .SetUncheckedImage(IMAGE_BRUSH("Common/MenuBar_Unchecked", Icon14x14))
            .SetUncheckedHoveredImage(IMAGE_BRUSH("Common/MenuBar_Unchecked", Icon14x14))
            .SetUncheckedPressedImage(IMAGE_BRUSH("Common/MenuBar_Unchecked", Icon14x14))
            .SetCheckedImage(IMAGE_BRUSH("Common/MenuBar_Checked", Icon14x14))
            .SetCheckedHoveredImage(IMAGE_BRUSH("Common/MenuBar_Checked", Icon14x14))
            .SetCheckedPressedImage(IMAGE_BRUSH("Common/MenuBar_Checked", Icon14x14))
            .SetUndeterminedImage(IMAGE_BRUSH("Common/MenuBar_Unchecked", Icon14x14))
            .SetUndeterminedHoveredImage(IMAGE_BRUSH("Common/MenuBar_Unchecked", Icon14x14))
            .SetUndeterminedPressedImage(IMAGE_BRUSH("Common/MenuBar_Unchecked", Icon14x14));

        /* ...and add the new style */
        Set("Menu.Check", BasicMenuCheckStyle);



        /* This radio button is actually just a check box with different images */
        /* Set images for various Menu radio button (SCheckBox) states... */
		FLinearColor SelectionColor(1.0f, 1.0f, 0.0f);
		FLinearColor SelectionColor_Pressed(1.0f, 1.0f, 0.0f);
		const FCheckBoxStyle BasicMenuRadioButtonStyle = FCheckBoxStyle()
            .SetUncheckedImage(IMAGE_BRUSH("Common/RadioButton_Unselected_16x", Icon16x16))
            .SetUncheckedHoveredImage(IMAGE_BRUSH("Common/RadioButton_Unselected_16x", Icon16x16))
            .SetUncheckedPressedImage(IMAGE_BRUSH("Common/RadioButton_Unselected_16x", Icon16x16))
            .SetCheckedImage(IMAGE_BRUSH("Common/RadioButton_Selected_16x", Icon16x16))
            .SetCheckedHoveredImage(IMAGE_BRUSH("Common/RadioButton_Selected_16x", Icon16x16, SelectionColor))
            .SetCheckedPressedImage(IMAGE_BRUSH("Common/RadioButton_Unselected_16x", Icon16x16, SelectionColor_Pressed))
            .SetUndeterminedImage(IMAGE_BRUSH("Common/RadioButton_Unselected_16x", Icon16x16))
            .SetUndeterminedHoveredImage(IMAGE_BRUSH("Common/RadioButton_Unselected_16x", Icon16x16, SelectionColor))
            .SetUndeterminedPressedImage(IMAGE_BRUSH("Common/RadioButton_Unselected_16x", Icon16x16, SelectionColor_Pressed));

        /* ...and set new style */
        Set("Menu.RadioButton", BasicMenuRadioButtonStyle);

        /* Create style for "Menu.ToggleButton" widget ... */
        //const FCheckBoxStyle MenuToggleButtonCheckBoxStyle = FCheckBoxStyle()
        //    .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        //    .SetUncheckedImage(FSlateNoResource())
        //    .SetUncheckedPressedImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor_Pressed))
        //    .SetUncheckedHoveredImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor))
        //    .SetCheckedImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor_Pressed))
        //    .SetCheckedHoveredImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor_Pressed))
        //    .SetCheckedPressedImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor));
        ///* ... and add new style */
        //Set("Menu.ToggleButton", MenuToggleButtonCheckBoxStyle);

        Set("Menu.Button", FButtonStyle()
            .SetNormal(BOX_BRUSH("Icons/White", 4.0f / 16.0f, GetColor("Color.FF2F3033")))//
            .SetPressed(BOX_BRUSH("Icons/White", 4.0f / 16.0f, GetColor("Color.FF252629")))
            .SetHovered(BOX_BRUSH("Icons/White", 4.0f / 16.0f, GetColor("Color.FF252629")))
			.SetDisabled(BOX_BRUSH("Icons/White", 4.0f / 16.0f, GetColor("Color.FF2F3033")))
            .SetNormalPadding(FMargin(0, 1))
            .SetPressedPadding(FMargin(0, 2, 0, 0))
        );

        //Set("Menu.Button.Checked", new BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor_Pressed));
        //Set("Menu.Button.Checked_Hovered", new BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor_Pressed));
        //Set("Menu.Button.Checked_Pressed", new BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor));

        /* The style of a menu bar button when it has a sub menu open */
        Set("Menu.Button.SubMenuOpen", new BORDER_BRUSH("Common/Selection", FMargin(4.f / 16.f), FLinearColor(0.10f, 0.10f, 0.10f)));
    }
}

void FArmyStyleSet::SetHomeModeStyle()
{
    Set("Category.Building", new IMAGE_BRUSH("Icons/Categorys/T_Building", Icon24x24));
	Set("Category.Window", new IMAGE_BRUSH("Icons/Categorys/Window", Icon24x24));
	Set("Category.Component", new IMAGE_BRUSH("Icons/Categorys/Component", Icon24x24));
	Set("Category.Construction", new IMAGE_BRUSH("Icons/Construction", Icon16x16));
	Set("Category.Component18", new IMAGE_BRUSH("Icons/Categorys/Component", Icon18x18));
    Set("Category.PointLocation", new IMAGE_BRUSH("Icons/Categorys/PointLocation", Icon24x24));

    /** @欧石楠 ToolBar Icon*/
    Set("HomeMode.CommandSave", new IMAGE_BRUSH("Icons/Toolbar/Save", Icon24x24));
    Set("HomeMode.CommandUndo", new IMAGE_BRUSH("Icons/Toolbar/Undo", Icon24x24));
    Set("HomeMode.CommandRedo", new IMAGE_BRUSH("Icons/Toolbar/Redo", Icon24x24));
    Set("HomeMode.CommandImport", new IMAGE_BRUSH("Icons/Toolbar/Import", Icon24x24));
    Set("HomeMode.CommandHouseType", new IMAGE_BRUSH("Icons/Toolbar/Import", Icon24x24));
    Set("HomeMode.CommandEmpty", new IMAGE_BRUSH("Icons/Toolbar/Clear", Icon24x24));
    Set("HomeMode.CommandDelete", new IMAGE_BRUSH("Icons/Toolbar/Delete", Icon24x24));
    Set("HomeMode.CommandDisplay", new IMAGE_BRUSH("Icons/Toolbar/Display", Icon24x24));
    Set("HomeMode.CommandOffset", new IMAGE_BRUSH("Icons/Toolbar/Shift", Icon24x24));
    Set("HomeMode.CommandMirror", new IMAGE_BRUSH("Icons/Clear", Icon24x24));
    Set("HomeMode.CommandGroup", new IMAGE_BRUSH("Icons/Clear", Icon24x24));
    Set("HomeMode.CommandCreateOutWall", new IMAGE_BRUSH("Icons/Toolbar/OutWall", Icon24x24));
    Set("HomeMode.CommandDeleteOutWall", new IMAGE_BRUSH("Icons/Toolbar/DeleteOutWall", Icon24x24));    

    /**@欧石楠  建筑*/
    Set("HomeMode.CommandInternalWall", new IMAGE_BRUSH("Icons/Operations/Wall", Icon64x64));
    Set("HomeMode.CommandOriginalWall", new IMAGE_BRUSH("Icons/Operations/OriginalWall", Icon64x64));
    Set("HomeMode.CommandNewWall", new IMAGE_BRUSH("Icons/Operations/NewWall", Icon64x64));
    Set("HomeMode.CommandDismantleWall", new IMAGE_BRUSH("Icons/Operations/DismantleWall", Icon64x64));
    Set("HomeMode.CommandBearingWall", new IMAGE_BRUSH("Icons/Operations/BearingWall", Icon64x64));
    Set("HomeMode.CommandSpaceLine", new IMAGE_BRUSH("Icons/Operations/SpaceLine", Icon64x64));
    Set("HomeMode.CommandBalconyPunch", new IMAGE_BRUSH("Icons/Operations/Punch", Icon64x64));
	Set("HomeMode.CommandPackPipe", new IMAGE_BRUSH("Icons/Operations/PackPipe", Icon64x64));


    Set("HomeMode.CommandDoor", new IMAGE_BRUSH("Icons/Operations/Door", Icon64x64));
    Set("HomeMode.CommandSecurityDoor", new IMAGE_BRUSH("Icons/Operations/Door", Icon64x64));
    Set("HomeMode.CommandSlidingDoor", new IMAGE_BRUSH("Icons/Operations/SlidingDoor", Icon64x64));
	Set("HomeMode.CommandPass", new IMAGE_BRUSH("Icons/Operations/DoorWay", Icon64x64));

    Set("HomeMode.CommandWindow", new IMAGE_BRUSH("Icons/Operations/Window", Icon64x64));
    Set("HomeMode.CommandFloorWindow", new IMAGE_BRUSH("Icons/Operations/FloorWindow", Icon64x64));
    Set("HomeMode.CommandBayWindow", new IMAGE_BRUSH("Icons/Operations/BayWindow", Icon64x64));
    Set("HomeMode.CommandCornerBayWindow", new IMAGE_BRUSH("Icons/Operations/CornerBayWindow", Icon64x64));
    Set("HomeMode.CommandTrapezoidBayWindow", new IMAGE_BRUSH("Icons/Operations/TrapezoidBayWindow", Icon64x64));
    Set("HomeMode.CommandArcWindow", new IMAGE_BRUSH("Icons/Operations/ArcWindow", Icon64x64));
	Set("HomeMode.CommandAirLouver", new IMAGE_BRUSH("Icons/Operations/AirLouver", Icon64x64));

    Set("HomeMode.CommandPillar", new IMAGE_BRUSH("Icons/Operations/Pillar", Icon64x64));        
    Set("HomeMode.CommandAirFlue", new IMAGE_BRUSH("Icons/Operations/AirFlue", Icon64x64));    
    Set("HomeMode.CommandBeam", new IMAGE_BRUSH("Icons/Operations/Beam", Icon64x64));        


    Set("HomeMode.CommandDiversityWaterDevice", new IMAGE_BRUSH("Icons/Operations/PointPosition/DiversityWaterDevice", Icon64x64));
    Set("HomeMode.CommandDrainPoint", new IMAGE_BRUSH("Icons/Operations/PointPosition/DrainPoint", Icon64x64));
    Set("HomeMode.CommandWaterSupply", new IMAGE_BRUSH("Icons/Operations/PointPosition/WaterSupply", Icon64x64));
    Set("HomeMode.CommandDewateringPipe", new IMAGE_BRUSH("Icons/Operations/PointPosition/DewateringPipe", Icon64x64));
    Set("HomeMode.CommandRainPipe", new IMAGE_BRUSH("Icons/Operations/PointPosition/RainPipe", Icon64x64));
    Set("HomeMode.CommandClosestool", new IMAGE_BRUSH("Icons/Operations/PointPosition/Closestool", Icon64x64));
    Set("HomeMode.CommandGasMeter", new IMAGE_BRUSH("Icons/Operations/PointPosition/GasMeter", Icon64x64));
    Set("HomeMode.CommandGasMainPipe", new IMAGE_BRUSH("Icons/Operations/PointPosition/GasMainPipe", Icon64x64));
    Set("HomeMode.CommandHighElvBox", new IMAGE_BRUSH("Icons/Operations/PointPosition/HighElvBox", Icon64x64));
	Set("HomeMode.CommandLowElvBox", new IMAGE_BRUSH("Icons/Operations/PointPosition/LowElvBox", Icon64x64));
	Set("HomeMode.CommandBasin", new IMAGE_BRUSH("Icons/Operations/PointPosition/Basin", Icon64x64));
	Set("HomeMode.CommandWaterChilledPoint", new IMAGE_BRUSH("Icons/Operations/PointPosition/WaterChilledPoint", Icon64x64));
	Set("HomeMode.CommandWaterNormalPoint", new IMAGE_BRUSH("Icons/Operations/PointPosition/WaterNormalPoint", Icon64x64));
	Set("HomeMode.CommandWaterHotPoint", new IMAGE_BRUSH("Icons/Operations/PointPosition/WaterHotPoint", Icon64x64));
	Set("HomeMode.CommandWaterSeparatorPoint", new IMAGE_BRUSH("Icons/Operations/PointPosition/WaterSeparatorPoint", Icon64x64));
	Set("HomeMode.CommandEnterRoomWater", new IMAGE_BRUSH("Icons/Operations/PointPosition/EnterRoomWater", Icon64x64));
}

void FArmyStyleSet::SetLayoutModeStyle()
{
    Set("Category.Dismantle", new IMAGE_BRUSH("Icons/Categorys/Dismantle", Icon24x24));
    Set("Category.Door", new IMAGE_BRUSH("Icons/Categorys/Door", Icon24x24));

    Set("LayoutMode.CommandSave", new IMAGE_BRUSH("Icons/Toolbar/Save", Icon24x24));
    Set("LayoutMode.CommandUndo", new IMAGE_BRUSH("Icons/Toolbar/Undo", Icon24x24));
    Set("LayoutMode.CommandRedo", new IMAGE_BRUSH("Icons/Toolbar/Redo", Icon24x24));
    Set("LayoutMode.CommandEmpty", new IMAGE_BRUSH("Icons/Toolbar/Clear", Icon24x24));
    Set("LayoutMode.CommandDelete", new IMAGE_BRUSH("Icons/Toolbar/Delete", Icon24x24));
	Set("LayoutMode.CommandDismantleCombo", new IMAGE_BRUSH("Icons/Toolbar/DismantleState", Icon24x24));    

	Set("LayoutMode.CommandDismantle", new IMAGE_BRUSH("Icons/Toolbar/DismantleState", Icon24x24));
	Set("LayoutMode.CommandAfterDismantle", new IMAGE_BRUSH("Icons/Toolbar/DismantleState", Icon24x24));

    Set("LayoutMode.CommandNewWall", new IMAGE_BRUSH("Icons/Operations/NewWall", Icon64x64));
    Set("LayoutMode.CommandDismantleWall", new IMAGE_BRUSH("Icons/Operations/DismantleWall", Icon64x64));
	Set("LayoutMode.CommandNewDoorHole", new IMAGE_BRUSH("Icons/Operations/DoorWay", Icon64x64));
	Set("LayoutMode.CommandCurtainBox", new IMAGE_BRUSH("Icons/Operations/CurtainBox", Icon64x64));
	Set("LayoutMode.CommandPackPipe", new IMAGE_BRUSH("Icons/Operations/PackPipe", Icon64x64));
	Set("LayoutMode.CommandNewPass", new IMAGE_BRUSH("Icons/Operations/Pass", Icon64x64));

    Set("LayoutMode.CommandDoor", new IMAGE_BRUSH("Icons/Operations/Door", Icon64x64));
    Set("LayoutMode.CommandSlidingDoor", new IMAGE_BRUSH("Icons/Operations/SlidingDoor", Icon64x64));

	/** @纪仁泽  区域分割 （需要更新icon） */
	Set("Category.SplitRegion", new IMAGE_BRUSH("Icons/Categorys/RegionLayout", Icon24x24));
	Set("LayoutMode.CommandSplitRegion", new IMAGE_BRUSH("Icons/Operations/AreaSegmentation", Icon64x64));
}

void FArmyStyleSet::SetHardModeStyle()
{
	Set("HardMode.CommandSave", new IMAGE_BRUSH("Icons/Toolbar/Save", Icon24x24));
	Set("HardMode.CommandUndo", new IMAGE_BRUSH("Icons/Toolbar/Undo", Icon24x24));
	Set("HardMode.CommandRedo", new IMAGE_BRUSH("Icons/Toolbar/Redo", Icon24x24));
	Set("HardMode.CommandReplace", new IMAGE_BRUSH("Icons/Toolbar/Replace", Icon24x24));
	Set("HardMode.CommandCopy", new IMAGE_BRUSH("Icons/CopyOffset", Icon24x24));
	Set("HardMode.CommandEmpty", new IMAGE_BRUSH("Icons/Toolbar/ClearAll", Icon24x24));
	Set("HardMode.CommandDelete", new IMAGE_BRUSH("Icons/Toolbar/Delete", Icon24x24));
	Set("HardMode.CommandReturnViewAngle", new IMAGE_BRUSH("Icons/Toolbar/back", Icon24x24));
	Set("HardMode.OnAreaCommand", new IMAGE_BRUSH("Icons/Toolbar/topbar_area", Icon24x24));
	Set("HardMode.OnLightCommand", new IMAGE_BRUSH("Icons/Toolbar/topbar_light", Icon24x24));
	Set("HardMode.CommandBuildStaticLighting", new IMAGE_BRUSH("Icons/Toolbar/BuildStaticLighting", Icon64x24));

	Set("HardMode.CommandMaterialBrush", new IMAGE_BRUSH("Icons/Toolbar/MaterialBrush", Icon24x24));

	/**@欧石楠 平面模式*/
	Set("HardMode.CommandViewCombo", new IMAGE_BRUSH("Icons/Toolbar/topdown_view", Icon24x24));
	Set("HardMode.CommandHardOverallView", new IMAGE_BRUSH("Icons/Toolbar/perspective_view", Icon24x24));
	// zhx AutoDesign show package warning
	Set("HardMode.showpackagewarningIcon", new IMAGE_BRUSH("Icons/PowerSystem/namespace", FVector2D(24, 24)));
	Set("HardMode.showpackagewarningInfo", new IMAGE_BRUSH("Icons/showpackagewarningInfo", FVector2D(300, 170)));

}

void FArmyStyleSet::SetHardModeFloorStyle()
{
	Set("HardModeFloor.CommandSave", new IMAGE_BRUSH("Icons/Toolbar/Save", Icon24x24));
	Set("HardModeFloor.CommandUndo", new IMAGE_BRUSH("Icons/Undo", Icon24x24));
	Set("HardModeFloor.CommandRedo", new IMAGE_BRUSH("Icons/Redo", Icon24x24));
	Set("HardModeFloor.CommandReplace", new IMAGE_BRUSH("Icons/Toolbar/Replace", Icon24x24));
	Set("HardModeFloor.CommandCopy", new IMAGE_BRUSH("Icons/CopyOffset", Icon24x24)); 
	Set("HardModeFloor.CommandEmpty", new IMAGE_BRUSH("Icons/Toolbar/Clear", Icon24x24));
	Set("HardModeFloor.CommandDelete", new IMAGE_BRUSH("Icons/Toolbar/Delete", Icon24x24));
	Set("HardModeFloor.CommandReturnViewAngle", new IMAGE_BRUSH("Icons/Toolbar/back", Icon24x24));
	Set("HardMode.PlanLayout", new IMAGE_BRUSH("Icons/Planelayout", Icon72x72));
	Set("HardMode.HardWear", new IMAGE_BRUSH("Icons/HardWear", Icon72x72));
	Set("HardMode.Softwear", new IMAGE_BRUSH("Icons/Softwear", Icon72x72));
}

void FArmyStyleSet::SetSoftModeStyle()
{
    Set("SoftMode.CommandSave", new IMAGE_BRUSH("Icons/Toolbar/Save", Icon24x24));
    Set("SoftMode.CommandUndo", new IMAGE_BRUSH("Icons/Undo", Icon24x24));
    Set("SoftMode.CommandRedo", new IMAGE_BRUSH("Icons/Redo", Icon24x24));
    Set("SoftMode.CommandClear", new IMAGE_BRUSH("Icons/Toolbar/Clear", Icon24x24));
    Set("SoftMode.CommandReplace", new IMAGE_BRUSH("Icons/Toolbar/Replace", Icon24x24));
    Set("SoftMode.CommandMirror", new IMAGE_BRUSH("Icons/Clear", Icon24x24));
    Set("SoftMode.CommandRotate", new IMAGE_BRUSH("Icons/Clear", Icon24x24));
    Set("SoftMode.CommandFavorite", new IMAGE_BRUSH("Icons/Clear", Icon24x24));
    Set("SoftMode.CommandGroup", new IMAGE_BRUSH("Icons/Clear", Icon24x24));
    Set("SoftMode.CommandDelete", new IMAGE_BRUSH("Icons/Toolbar/Delete", Icon24x24));
    Set("SoftMode.CommandPointLight", new IMAGE_BRUSH("Icons/Clear", Icon24x24));
    Set("SoftMode.CommandSpotLight", new IMAGE_BRUSH("Icons/Clear", Icon24x24));
    Set("SoftMode.CommandReflectionCapture", new IMAGE_BRUSH("Icons/Clear", Icon24x24));
}

void FArmyStyleSet::SetConstructionModeStyle()
{
	/**	toolbar icon*/
	Set("ConstructionMode.CommandSave", new IMAGE_BRUSH("Icons/Toolbar/Save", Icon24x24));
	Set("ConstructionMode.CommandDelete", new IMAGE_BRUSH("Icons/Toolbar/Delete", Icon24x24));
	Set("ConstructionMode.CommandLoadFrame", new IMAGE_BRUSH("Icons/Toolbar/Frame", Icon24x24));
	Set("ConstructionMode.CommandSaveImage", new IMAGE_BRUSH("Icons/Toolbar/SaveImage", Icon24x24));
	Set("ConstructionMode.CommandExportCAD", new IMAGE_BRUSH("Icons/Toolbar/Export", Icon24x24));
	Set("ConstructionMode.CommandReset", new IMAGE_BRUSH("Icons/Toolbar/Reset", Icon24x24));
	Set("ConstructionMode.CommandUpdate", new IMAGE_BRUSH("Icons/Toolbar/Update", Icon24x24));
	Set("ConstructionMode.CommandFacadeIndex", new IMAGE_BRUSH("Icons/Toolbar/FacadeIndex", Icon24x24));
	Set("ConstructionMode.OnAutoRulerCommand", new IMAGE_BRUSH("Icons/Toolbar/Annotation", Icon24x24));

	Set("ConstructionMode.Class_Base", new IMAGE_BRUSH("Icons/T_ModelEditor", Icon24x24));
	Set("ConstructionMode.CommandDimension", new IMAGE_BRUSH("Icons/Operations/Dimension", Icon64x64));
	Set("ConstructionMode.CommandDownLeadLabel_Size", new IMAGE_BRUSH("Icons/Operations/DownLeadLabel_Size", Icon64x64));
	Set("ConstructionMode.CommandDownLeadLabel_Material", new IMAGE_BRUSH("Icons/Operations/DownLeadLabel_MT", Icon64x64));
	Set("ConstructionMode.CommandDownLeadLabel_Craft", new IMAGE_BRUSH("Icons/Operations/DownLeadLabel_Craft", Icon64x64));
	Set("ConstructionMode.CommandDownLeadLabel_Height", new IMAGE_BRUSH("Icons/Operations/DownLeadLabel_Height", Icon64x64));
	Set("ConstructionMode.CommandDownLeadLabel_Text", new IMAGE_BRUSH("Icons/Operations/DownLeadLabel_Text", Icon64x64));
	Set("ConstructionMode.CommandDownLeadLabel_CustomHeight", new IMAGE_BRUSH("Icons/Operations/TextLabel_CustomHeight", Icon64x64));

	Set("ConstructionMode.CommandCompass", new IMAGE_BRUSH("Icons/Operations/Compass", Icon64x64));
	Set("ConstructionMode.CommandBoradSplitLine", new IMAGE_BRUSH("Icons/Operations/SplitLine", Icon64x64));
	//@欧石楠
	Set("ConstructionMode.CommandUndo", new IMAGE_BRUSH("Icons/Toolbar/Undo", Icon24x24));
	Set("ConstructionMode.CommandRedo", new IMAGE_BRUSH("Icons/Toolbar/Redo", Icon24x24));
}

void FArmyStyleSet::SetHydropowerModeStyle()
{
    Set("HydropowerMode.CommandSave", new IMAGE_BRUSH("Icons/Toolbar/Save", Icon24x24));
    Set("HydropowerMode.CommandUndo", new IMAGE_BRUSH("Icons/Toolbar/Undo", Icon24x24));
    Set("HydropowerMode.CommandRedo", new IMAGE_BRUSH("Icons/Toolbar/Redo", Icon24x24));
    Set("HydropowerMode.CommandEmpty", new IMAGE_BRUSH("Icons/Toolbar/Clear", Icon24x24));
    Set("HydropowerMode.CommandReplace", new IMAGE_BRUSH("Icons/Toolbar/Replace", Icon24x24));
    Set("HydropowerMode.CommandDelete", new IMAGE_BRUSH("Icons/Toolbar/Delete", Icon24x24));
    Set("HydropowerMode.CommandCopy", new IMAGE_BRUSH("Icons/Toolbar/Delete", Icon24x24));
    Set("HydropowerMode.CommandUndo", new IMAGE_BRUSH("Icons/Toolbar/Undo", Icon24x24));
    Set("HydropowerMode.CommandRedo", new IMAGE_BRUSH("Icons/Toolbar/Redo", Icon24x24));
    Set("HydropowerMode.CommandGroup", new IMAGE_BRUSH("Icons/Toolbar/Group", Icon24x24));
	Set("HydropowerMode.CommandSwitchConnect",new IMAGE_BRUSH("Icons/Toolbar/dengkong",Icon24x24));
	Set("HydropowerMode.CommandExitSwitchConnect",new IMAGE_BRUSH("Icons/Toolbar/dengkong",Icon24x24));
	Set("HydropowerMode.CommandPowerSystem",new IMAGE_BRUSH("Icons/Toolbar/PowerSystem",Icon24x24));
	Set("HydropowerMode.CommandAutoDesignLine",new IMAGE_BRUSH("Icons/Toolbar/autodesignline",Icon24x24));
	Set("HydropowerMode.CommandAutoDesignPoint", new IMAGE_BRUSH("Icons/Toolbar/autodesignpoint", Icon24x24));

	/**@欧石楠 视图模式*/
	Set("HydropowerMode.CommandTopView", new IMAGE_BRUSH("Icons/Toolbar/topdown_view", Icon24x24));
	Set("HydropowerMode.CommandOverallView", new IMAGE_BRUSH("Icons/Toolbar/perspective_view", Icon24x24));

	Set("Icon.Add",new IMAGE_BRUSH("Icons/PowerSystem/add",Icon10x10));
	Set("Icon.PowerLeft",new IMAGE_BRUSH("Icons/PowerSystem/line_short",FVector2D(395,21)));
	Set("PowerLine.Normal",new IMAGE_BRUSH("Icons/PowerSystem/line_normal",FVector2D(708,21)));
	Set("PowerLine.Yellow",new IMAGE_BRUSH("Icons/PowerSystem/line_yellow",FVector2D(708,21))); 

	Set("HydropowerMode.eleh",new IMAGE_BRUSH("Icons/PowerSystem/eleh",FVector2D(24,24)));
	Set("HydropowerMode.elehInfo",new IMAGE_BRUSH("Icons/PowerSystem/elehInfo",FVector2D(300,170)));

	Set("HydropowerMode.Loop",new IMAGE_BRUSH("Icons/PowerSystem/Loop",FVector2D(24,24)));
	Set("HydropowerMode.LoopInfo",new IMAGE_BRUSH("Icons/PowerSystem/LoopInfo",FVector2D(300,170)));

	Set("HydropowerMode.namespace",new IMAGE_BRUSH("Icons/PowerSystem/namespace",FVector2D(24,24)));
	Set("HydropowerMode.namespaceInfo",new IMAGE_BRUSH("Icons/PowerSystem/namespaceInfo",FVector2D(300,170)));

	Set("HydropowerMode.socket",new IMAGE_BRUSH("Icons/PowerSystem/socket",FVector2D(24,24)));
	Set("HydropowerMode.socketInfo",new IMAGE_BRUSH("Icons/PowerSystem/socketInfo",FVector2D(300,170)));
	
	Set("HydropowerMode.autowater", new IMAGE_BRUSH("Icons/PowerSystem/autowater", FVector2D(64, 64)));
	Set("HydropowerMode.autoswitch", new IMAGE_BRUSH("Icons/PowerSystem/autoswitch", FVector2D(64, 64)));
	Set("HydropowerMode.autosocket", new IMAGE_BRUSH("Icons/PowerSystem/autosocket", FVector2D(64, 64)));
}

void FArmyStyleSet::SetWHCModeStyle()
{
	Set("WHCMode.SaveCommand", new IMAGE_BRUSH("Icons/Toolbar/Save", Icon24x24));
    Set("WHCMode.DeleteCommand", new IMAGE_BRUSH("Icons/Toolbar/Delete", Icon24x24));
    Set("WHCMode.ClearSceneCommand", new IMAGE_BRUSH("Icons/Toolbar/Clear", Icon24x24));
    Set("WHCMode.HideCommand", new IMAGE_BRUSH("Icons/Toolbar/topbar_display", Icon24x24));
    Set("WHCMode.StyleCommand", new IMAGE_BRUSH("Icons/Toolbar/ic_style", Icon24x24));
    Set("WHCMode.RulerCommand", new IMAGE_BRUSH("Icons/Toolbar/topbar_measure_line", Icon24x24));
    Set("WHCMode.GeneratingCommand", new IMAGE_BRUSH("Icons/Toolbar/topbar_generate", Icon24x24));
    Set("WHCMode.TopdownViewCommand", new IMAGE_BRUSH("Icons/Toolbar/topdown_view", Icon24x24));
    Set("WHCMode.RegularViewCommand", new IMAGE_BRUSH("Icons/Toolbar/perspective_view", Icon24x24));
	
	Set("WHCMode.Cabinet", new IMAGE_BRUSH("Icons/Categorys/cabinet", Icon24x24));
	Set("WHCMode.Wardrobe", new IMAGE_BRUSH("Icons/Categorys/wardrobe", Icon24x24));
	Set("WHCMode.Other", new IMAGE_BRUSH("Icons/Categorys/other", Icon24x24));

	Set("WHCMode.Replace", new IMAGE_BRUSH("Icons/replace", Icon24x24));
	Set("WHCMode.Menu", new IMAGE_BRUSH("Icons/del_copy_2", Icon24x24));

	Set("WHCMenu.Background", new BOX_BRUSH("Icons/White", FMargin(8.0f / 64.0f), GetColor("Color.FF35373B")));//欧石楠，背景色
	Set("WHCMenu.Icon", new IMAGE_BRUSH("Icons/icon_tab_toolbar_16px", Icon16x16));
	Set("WHCMenu.Expand", new IMAGE_BRUSH("Icons/toolbar_expand_16x", Icon16x16));
	Set("WHCMenu.SubMenuIndicator", new IMAGE_BRUSH("Common/SubmenuArrow", Icon8x8));
	Set("WHCMenu.SToolBarComboButtonBlock.Padding", FMargin(4.0f));
	Set("WHCMenu.SToolBarButtonBlock.Padding", FMargin(4.0f));
	Set("WHCMenu.SToolBarCheckComboButtonBlock.Padding", FMargin(4.0f));
	Set("WHCMenu.SToolBarButtonBlock.CheckBox.Padding", FMargin(0.0f));

	Set("WHCMenu.Block.IndentedPadding", FMargin(18.0f, 2.0f, 4.0f, 4.0f));
	Set("WHCMenu.Block.Padding", FMargin(2.0f, 2.0f, 4.0f, 4.0f));

	
	Set("WHCMenu.Separator", new BOX_BRUSH("Old/Button", 4.0f / 32.0f));
	Set("WHCMenu.Separator.Padding", FMargin(0.f));

	Set("WHCMenu.Label", GetWidgetStyle<FTextBlockStyle>("ArmyText_12_Menu"));
	Set("WHCMenu.Keybinding", GetWidgetStyle<FTextBlockStyle>("ArmyText_12_Keybinding"));

	/* Set images for various SCheckBox states associated with menu check box items... */
	const FCheckBoxStyle BasicMenuCheckBoxStyle = FCheckBoxStyle()
		.SetUncheckedImage(IMAGE_BRUSH("Icons/Checkbox_normal", Icon14x14))
		.SetUncheckedHoveredImage(IMAGE_BRUSH("Icons/Checkbox_hover", Icon14x14))
		.SetUncheckedPressedImage(IMAGE_BRUSH("Icons/Checkbox_hover", Icon14x14, FLinearColor(0.5f, 0.5f, 0.5f)))
		.SetCheckedImage(IMAGE_BRUSH("Icons/Checkbox_active", Icon14x14))
		.SetCheckedHoveredImage(IMAGE_BRUSH("Icons/Checkbox_active", Icon14x14))
		.SetCheckedPressedImage(IMAGE_BRUSH("Icons/Checkbox_active", Icon14x14, FLinearColor(0.5f, 0.5f, 0.5f)))
		.SetUndeterminedImage(IMAGE_BRUSH("Icons/Checkbox_normal", Icon14x14))
		.SetUndeterminedHoveredImage(IMAGE_BRUSH("Icons/Checkbox_hover", Icon14x14))
		.SetUndeterminedPressedImage(IMAGE_BRUSH("Icons/Checkbox_hover", Icon14x14, FLinearColor(0.5f, 0.5f, 0.5f)));

	/* ...and add the new style */
	Set("WHCMenu.CheckBox", BasicMenuCheckBoxStyle);

	/* Read-only checkbox that appears next to a menu item */
	/* Set images for various SCheckBox states associated with read-only menu check box items... */
	const FCheckBoxStyle BasicMenuCheckStyle = FCheckBoxStyle()
		.SetUncheckedImage(IMAGE_BRUSH("Icons/Checkbox_normal", Icon14x14))
		.SetUncheckedHoveredImage(IMAGE_BRUSH("Icons/Checkbox_hover", Icon14x14))
		.SetUncheckedPressedImage(IMAGE_BRUSH("Icons/Checkbox_hover", Icon14x14, FLinearColor(0.5f, 0.5f, 0.5f)))
		.SetCheckedImage(IMAGE_BRUSH("Icons/Checkbox_active", Icon14x14))
		.SetCheckedHoveredImage(IMAGE_BRUSH("Icons/Checkbox_active", Icon14x14))
		.SetCheckedPressedImage(IMAGE_BRUSH("Icons/Checkbox_active", Icon14x14, FLinearColor(0.5f, 0.5f, 0.5f)))
		.SetUndeterminedImage(IMAGE_BRUSH("Icons/Checkbox_normal", Icon14x14))
		.SetUndeterminedHoveredImage(IMAGE_BRUSH("Icons/Checkbox_hover", Icon14x14))
		.SetUndeterminedPressedImage(IMAGE_BRUSH("Icons/Checkbox_hover", Icon14x14, FLinearColor(0.5f, 0.5f, 0.5f)));

	/* ...and add the new style */
	Set("WHCMenu.Check", BasicMenuCheckStyle);

	Set("WHCMenu.Button", FButtonStyle()
		.SetNormal(BOX_BRUSH("Icons/White", 4.0f / 16.0f, GetColor("Color.FF2F3033")))//
		.SetPressed(BOX_BRUSH("Icons/White", 4.0f / 16.0f, GetColor("Color.FF252629")))
		.SetHovered(BOX_BRUSH("Icons/White", 4.0f / 16.0f, GetColor("Color.FF252629")))
		.SetDisabled(BOX_BRUSH("Icons/White", 4.0f / 16.0f, GetColor("Color.FF2F3033")))
		.SetNormalPadding(FMargin(0, 1))
		.SetPressedPadding(FMargin(0, 2, 0, 0))
	);

	/* The style of a menu bar button when it has a sub menu open */
	Set("WHCMenu.Button.SubMenuOpen", new BORDER_BRUSH("Common/Selection", FMargin(4.f / 16.f), FLinearColor(0.10f, 0.10f, 0.10f)));
}