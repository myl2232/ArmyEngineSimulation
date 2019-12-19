#pragma once
#include "ArmyBaseEditStyle.h"

class ARMYSCENEDATA_API FArmyIntervalStyle :public FArmyBaseEditStyle
{
public:
	FArmyIntervalStyle();

	virtual ~FArmyIntervalStyle();

	void SetFirstAndSecondImage(UTexture2D* InTexture0, UTexture2D* InTexture1)override;
	UTexture2D * GetSecondTexture2D()const;

	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;

	void SetSecondTexturePath(const FString& InPath) {
		SecondTextureUrl = InPath;
	}

	void CalculateClipperBricks(const TArray<FVector>& OutAreaVertices, const TArray<TArray<FVector>>& InneraHoles);
protected:


	UTexture2D* SecondTexture;

	FString SecondTextureUrl;


};