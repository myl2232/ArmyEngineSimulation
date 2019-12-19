#include "ArmyUpdatedNotes.h"
#include "SBorder.h"
#include "SBox.h"
#include "SBoxPanel.h"
#include "SButton.h"
#include "SImage.h"
#include "SMultiLineEditableText.h"
#include "SOverlay.h"
#include "SScrollBox.h"
#include "STextBlock.h"
#include "ArmyStyle.h"
#include "ArmyTypes.h"
#include "ArmyUser.h"

void SArmyUpdatedNotes::Construct(const FArguments& InArgs)
{
  //  const FString UpdatedContent =
  //      FString(TEXT("家装BIM v1.8版本更新说明（20190612）\n\n")) +
  //      FString(TEXT("本期新增功能：\n")) +
  //      FString(TEXT("一、基础模块\n")) +
  //      FString(TEXT("1.支持在客户端快速查看当前设计方案的预估报价。\n")) +
  //      FString(TEXT("二、施工算量\n")) +
  //      FString(TEXT("1.支持对门、垭口套、波打线配置施工工艺并算量。\n")) +
  //      FString(TEXT("2.针对开门洞、封门洞，支持根据墙体材质，匹配出不同的施工项。\n")) +
  //      FString(TEXT("三、户型模式\n")) +
  //      FString(TEXT("1.支持对门洞垂直方向进行拆除/修补、配置施工工艺并算量。\n")) +
  //      FString(TEXT("四、立面模式\n")) +
  //      FString(TEXT("1.支持分段删除顶角线、灯槽，从而实现只在部分位置安装顶角线、灯槽。\n")) +
  //      FString(TEXT("五、木作模式\n")) +
  //      FString(TEXT("1.柜体库新增烟机柜种类：\n")) +
  //      FString(TEXT("1）烟机柜有“中空宽度”的参数，与烟机的排风管适配，确保烟机与烟机柜匹配；\n")) +
  //      FString(TEXT("2）烟机柜调整宽度时，中空宽度保持不变，两侧储物区同步缩放；\n")) +
  //      FString(TEXT("3）烟机柜替换烟机时，柜子的中空宽度会自动调整并适配替换后的烟机；\n")) +
  //      FString(TEXT("4）用户在后台可以给烟机柜配置可替换的烟机，并能够在前端调整烟机的显隐；\n")) +
  //      FString(TEXT("5）烟机柜可以与灶具进行对齐，对齐后移动灶具，烟机柜同步移动；\n")) +
  //      FString(TEXT("6）支持生成烟机柜、烟机的平面图/立面图；\n")) +
  //      FString(TEXT("7）支持生成烟机柜、烟机的算量清单；\n")) +
  //      FString(TEXT("2.见光板新增了质替换功能。\n")) +
  //      FString(TEXT("3.支持针对每个其他柜，单独生成台面。\n")) +
  //      FString(TEXT("六、后台管理\n")) + 
  //      FString(TEXT("（一）企业后台\n")) + 
  //      FString(TEXT("1.支持用户配置 立面索引图 上需要显示哪些内容。\n")) + 
  //      FString(TEXT("2.支持用户设置每张施工图纸上是否显示热水、冷水、中水点位。\n")) + 
  //      FString(TEXT("（二）木作管理后台\n")) + 
  //      FString(TEXT("1.新增了厨房水槽台上、台中、台下三种安装方式的参数设置；\n")) + 
  //      FString(TEXT("2.新增了水盆/水槽台面开孔截面图上传功能，以便于呈现正确的异形水槽台面开孔效果；\n")) + 
  //      FString(TEXT("3.调整板新增了L型和U型参数设置；\n")) + 
  //      FString(TEXT("4.新增了移门型材参数设置，避免了型材移门拼接时的错位效果。\n\n")) +
  //      FString(TEXT("另外，我们还优化/修复了如下问题：\n")) + 
  //      FString(TEXT("1.修复了计算新建/拆除墙体时，没有扣除门洞/窗洞面积的问题。\n")) + 
  //      FString(TEXT("2.修复了智能设计时，新增对墙面、顶面区域施工项的自动复用。\n")) + 
  //      FString(TEXT("3.优化了见光板的默认材质规则：优先取最近的门板材质，无门板时取柜体材质。\n")) + 
  //      FString(TEXT("4.橱柜平/立面图中，不再显示衣柜和其他柜体，橱柜信息更突出。\n")) + 
  //      FString(TEXT("5.优化了施工图中矮墙的自动标注，便于跟整墙进行区分。\n")) + 
  //      FString(TEXT("6.优化了施工图标注的引线生成规则：文字长度大于标尺长度时才出现引线，更加简洁明了。\n")) + 
  //      FString(TEXT("7.优化了施工图中窗标注与窗户距离过大的问题。\n")) + 
  //      FString(TEXT("8.优化了地漏下水的图例与模型，以显示正确的地漏下水管道。\n")) + 
  //      FString(TEXT("9.优化了燃气表的图例样式。\n")) + 
		//FString(TEXT("10.优化了燃气主管道图例过小的问题。\n")) +
		//FString(TEXT("11.优化了热水、冷水、中水、入户水点位的图例过大的问题。\n")) +
		//FString(TEXT("12.优化了强弱电、给排水点位的图例线型，解决了导出CAD后打印看不清点位文字的问题。\n")) +
  //      FString(TEXT("13.优化了房间信息文字/符号过大的问题。\n")) +
  //      FString(TEXT("14.优化了图框内文字的太小的问题，解决了打印后显示不清晰的问题。"));

    ChildSlot
    [
        SNew(SBorder)
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        .Padding(0)
        .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.99000000"))
        .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
        [
            SNew(SBox)
            .WidthOverride(420)
            .HeightOverride(530)
            [
                SNew(SBorder)
                .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
                .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
		        .Padding(FMargin(0))
                [
                    SNew(SVerticalBox)

                    // 标题
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        CreateTitleWidget()
                    ]
                
                    // 更新内容
                    + SVerticalBox::Slot()
                    .FillHeight(1.f)
                    [
                        SNew(SScrollBox)
                        .Orientation(Orient_Vertical)
                        .Style(FArmyStyle::Get(), "ScrollBar.Style.Gray")
                        .ScrollBarStyle(FArmyStyle::Get(), "ScrollBar.BarStyle.Gray")

                        + SScrollBox::Slot()
                        .HAlign(HAlign_Fill)
                        .VAlign(VAlign_Fill)
                        .Padding(40, 24, 40, 0)
                        [
                            SNew(SMultiLineEditableText)
                            .TextStyle(FArmyStyle::Get(), "ArmyText_12")
                            .IsReadOnly(true)
                            .Text(FText::FromString(InArgs._Content))
                            .AutoWrapText(true)
                            .LineHeightPercentage(1.5f)
                            .AllowContextMenu(false)
                        ]
                    ]

                    // 分割线
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(SImage)
                        .Image(FArmyStyle::Get().GetBrush("Splitter.FF343538"))
                    ]

                    // 确认按钮
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign(HAlign_Right)
                    .VAlign(VAlign_Center)
                    .Padding(0, 20, 20, 20)
                    [
                        SNew(SBox)
                        .WidthOverride(80)
                        .HeightOverride(30)
                        [
                            SNew(SButton)
                            .ButtonStyle(FArmyStyle::Get(), "Button.Gray.FF353638")
                            .HAlign(HAlign_Center)
                            .VAlign(VAlign_Center)
                            .OnClicked(InArgs._OnConfirm)
                            [
                                SNew(STextBlock)
                                .Text(MAKE_TEXT("我知道了"))
                                .TextStyle(FArmyStyle::Get(), "ArmyText_12")
                            ]
                        ]
                    ]
                ]
            ]
        ]
    ];
}

TSharedRef<SWidget> SArmyUpdatedNotes::CreateTitleWidget()
{
    return
        SNew(SOverlay)

        + SOverlay::Slot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            SNew(SImage)
            .Image(FArmyStyle::Get().GetBrush("Icon.VersionUpdated"))
        ]
                    
        + SOverlay::Slot()
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Top)
        .Padding(40, 48, 0, 0)
        [
            SNew(SVerticalBox)

            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(STextBlock)
                .TextStyle(FArmyStyle::Get(), "ArmyText_18")
                .Text(MAKE_TEXT("版本更新说明"))
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0, 16, 0, 0)
            [
                SNew(STextBlock)
                .TextStyle(FArmyStyle::Get(), "ArmyText_12")
                .Text(FText::FromString(TEXT("v ") + FArmyUser::Get().GetApplicationVersion()))
            ]
        ];
}