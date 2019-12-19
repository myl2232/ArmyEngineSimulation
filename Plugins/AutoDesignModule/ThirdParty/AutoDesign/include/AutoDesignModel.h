
#ifndef __AUTO_DESIGN__ 
#define __AUTO_DESIGN__

#define AIExport __declspec(dllexport)   
  


#pragma once
#include<string>

using namespace std;

//#define  BASE_PATH  "D://test//sample_data/"
//#define  DB_PATH  "D://test//aidesign.db"
//#define  SAMPLE_DATA_PATH  "D://test//sample_data/"
//#define  LOG_INIT_PATH  "D://test//my_log.ini"
//#define  DUMP_PATH  "D://test//dump"

class AutoDesignModel
{
public:
	AIExport AutoDesignModel();
	AIExport ~AutoDesignModel();

public:
	// ��ʼ������
	AIExport bool InitAiDesignData(char * db_path, char * sample_path, char * log_path, char * dump_path);
	// �Զ����
	AIExport char* AutomicalDesign(char* client_house_layout,char* client_design);
	// ����
	AIExport void Clear();
private:
	char *out_result;
};

#endif