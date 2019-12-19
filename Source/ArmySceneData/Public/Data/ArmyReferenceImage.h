#pragma once

#include "ArmyObject.h"

class UMaterialInstanceDynamic;

/**
 * base map
 */
class ARMYSCENEDATA_API FArmyReferenceImage : public FArmyObject
{
public:
    FArmyReferenceImage();
    virtual ~FArmyReferenceImage() {}

    virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
    virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
    virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;

    /** @欧石楠 设置底图路径和透明度 */
    bool LoadFacsimile(const FString& InFacsimilePath);

	void SetTexture(UTexture2D* InTexture);

    /** 设置透明度 */
    const int32 GetTransparency() const { return Transparency; }
    void SetTransparency(const int32 InTransparency);

	void SetScale(float InScale);

    TArray<uint8>& GetData() { return FacsimileData; }

    FString GetFilename();

private:
    TSharedPtr<class FArmyRect> ImagePanel;

	UMaterial* ReferenceImageMt;

	UMaterialInstanceDynamic* ReferenceImageDynamicMt;

    TArray<uint8> FacsimileData;

    /** @欧石楠 底图文件后缀 */
    FString Suffix;

    /** @欧石楠 不透明度 */
    int32 Transparency = 60;

    /** @欧石楠 底图缩放值 */
    float Scale = 1.f;
};

REGISTERCLASS(FArmyReferenceImage)