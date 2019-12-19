#include "AutoDesignEngine.h"
#include "AutoDesignModel.h"
#include "Paths.h"

AutoDesignEngine::AutoDesignEngine()
{

}
AutoDesignEngine::~AutoDesignEngine()
{

}

// 自动设计
FString AutoDesignEngine::AIDesign(FString HouseData, FString DesignData)
{
	AutoDesignModel *DesignModel = new AutoDesignModel();

	/** 数据库路径 */
	FString FileDirDB =  FPaths::ProjectContentDir() + TEXT("AutoDesign/aidesign.db");
	std::string StrFileDirDB(TCHAR_TO_UTF8(*FileDirDB));

	/** 模板路径 */
	FString FileDirSampleData = FPaths::ProjectContentDir() + TEXT("AutoDesign/sample_data/");
	std::string StrFileDirSampleData(TCHAR_TO_UTF8(*FileDirSampleData));

	/** log路径 */
	FString FileDirLog = FPaths::ProjectConfigDir() + TEXT("External/AutoDesignLog.ini");
	std::string StrFileDirLog(TCHAR_TO_UTF8(*FileDirLog));

	/** dump路径 */
	FString FileDirDump = FPaths::ProjectContentDir() + TEXT("AutoDesign/dump");
	std::string StrFileDirDump(TCHAR_TO_UTF8(*FileDirDump));

	/** 初始化自动设计相关数据 */
	bool b_init_AI = DesignModel->InitAiDesignData((char*)StrFileDirDB.c_str(), (char*)StrFileDirSampleData.c_str(), (char*)StrFileDirLog.c_str(), (char*)StrFileDirDump.c_str());

	//bool b_init_AI = InitAiDesign();
	std::string StrHouseData(TCHAR_TO_UTF8(*HouseData));
	std::string StrDesignData(TCHAR_TO_UTF8(*DesignData));

	FString OutResult = DesignModel->AutomicalDesign((char*)StrHouseData.c_str(), (char*)StrDesignData.c_str());

	DesignModel->Clear();

	return OutResult;
}