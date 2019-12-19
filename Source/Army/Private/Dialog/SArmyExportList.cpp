//#include "SArmyExportList.h"
//#include "SBox.h"
//#include "SlateApplication.h"
//#include "Runtime/WebBrowser/Public/SWebBrowser.h"
//
//void SArmyExportList::Construct(const FArguments & InArgs)
//{
//	Width = InArgs._Width;
//	Height = InArgs._Height;
//	Url = InArgs._Url;
//
//	ChildSlot
//		[
//			SNew(SBox)
//			.WidthOverride(Width)
//			.HeightOverride(Height)
//			[
//				SAssignNew(WebBrowser, SWebBrowser)
//				.InitialURL(Url)
//				.ViewportSize(FVector2D(Width, Height))
//				.ShowControls(false)
//			]
//		];
//	ITextInputMethodSystem* const TextInputMethodSystem = FSlateApplication::Get().GetTextInputMethodSystem();
//	WebBrowser->BindInputMethodSystem(TextInputMethodSystem);
//}
