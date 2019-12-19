/**
* Copyright 2019 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRWHCabinetComponent.h
* @Description 櫃子組建
*
* @Author 欧石楠
* @Date 2019年6月19日
* @Version 1.0
*/
#pragma once

#include "CoreMinimal.h"
#include "ArmyWHCRect.h"
#include "Data/XRObject.h"
#include "Data/WHCModeData/XRWHCModeData.h"
#include "SCTShapeData.h"
#include "ArmyGameInstance.h"

#include "ArmyWHCModeController.h"
#include "ArmyWHCabinetComponent.h"
#include "UnrealString.h"




class XRWHCabinetIComponent;
class CabComponentsPool;
class XRWHTransform;

template <class _TComponent = XRWHCabinetIComponent>
class XRCabinetActorT;

class XRCabinetActor {
public:
	XRCabinetActor();
	XRCabinetActor(TSharedPtr<XRWHCabinetIComponent> tpComponent);
	XRCabinetActor& operator=(TSharedPtr<XRWHCabinetIComponent> tpComponent);
	virtual ~XRCabinetActor() {};

	friend class CabComponentsPool;

	enum class eCabinetComponentType {
		CCT_TRANSFORM = 0,
		CCT_ACCESSORY,
		CCT_LEGS,
		CCT_PLATFORM,
		CCT_RULER,
		CCT_FArmyFURNITURE,
		CCT_FRONTSECTIONS,
		CCT_BACKSECTIONS,
		CCT_MAX = CCT_BACKSECTIONS
	};

public:
	void Create(TSharedPtr<XRWHCabinetIComponent> tpComponent);
	void CreateTransform();
	void Remove();
	void RemoveAt(int64 hashCodes);
	void RepetBinding(TSharedPtr<XRWHCabinetIComponent> tpComponent);

	bool GenCabinetComponents(eCabinetComponentType eccType, FShapeInRoom* sharePtrRoom);

	void InitCabinetComponents(TSharedPtr<FShapeInRoom> sharePtrRoom);
	inline XRWHTransform* Transform();
	bool Have(int64 hashName);

public:
	template <class _TComponent> inline
		TSharedPtr<XRWHCabinetIComponent> Component();
	template <class _TComponent> inline
	bool CheckEnableCreateComponent();
	template <class _TComponent> inline
	bool IsComponent();
	template <class _TComponent>
	_TComponent* AsComponent();
	template <class _TComponent>
	_TComponent* AsComponent(int index);
	template <class _TComponent>
	int64 HashClassType();

	int64 HashCode() const;
protected:
	TSharedPtr<XRWHCabinetIComponent> Find(uint64 hashValue);
	TSharedPtr<XRWHCabinetIComponent> Value() const;
	TSharedPtr<XRWHCabinetIComponent> GetValue(CabComponentsPool& pool) const;

	int32 LocationID() const { return location; }
	int32 Signature() const { return signature; }
private:
	int32 location;
	int32 signature;
	bool  BeDone = false;
};

#define nameof(argument) #argument
#define CreateComponent(ComponentName)                   \
static const FString& typeName() {\
    static FString name = nameof(ComponentName);\
     return name;\
}\
virtual ComponentName* Clone() const override {\
        return nullptr;\
    }
class XRWHCabinetIComponent
{
public:
	//XRWHCabinetIComponent() :ComponentClassName("") {}
	//XRWHCabinetIComponent(FString const & name) :ComponentClassName(name) {}
	virtual ~XRWHCabinetIComponent() ;
	virtual void Update() abstract;
	virtual XRWHCabinetIComponent* Clone() const {
		GGI->Window->ShowMessage(MT_Warning, TEXT("组建克隆失败！"));
		return nullptr;
	};

	void* Get() const {
		return nullptr;
	};


	inline XRCabinetActor Self() const {
		return self;
	}
	/* <summary>
	///  添加了shape 对象
	/// </summary>
	*/
	void AddFromActor(FShapeInRoom* shapeInRoom) {
		TspShapeInRoom = shapeInRoom;
	}

	/* <summary>
	/// 从组件中移除了shape对象
	/// </summary>
	*/
	void RemovFromActor() {
		TspShapeInRoom = nullptr;
	}

	int64 ClassHashName() { return GetTypeHash(ComponentClassName); }
	FShapeInRoom* CabRoom() { return TspShapeInRoom; }

	XRCabinetActor self;
	XRCabinetActor cabTransform;
	bool IsValidComponent = false;
	FString ComponentClassName = "";
	FShapeInRoom* TspShapeInRoom = nullptr;
};



//////////////////////////////////////////////////////////////////////////
//	unigue-actor only match one only tramsform
//////////////////////////////////////////////////////////////////////////
class XRWHTransform :public XRWHCabinetIComponent {
public:
	CreateComponent(XRWHTransform);

	explicit XRWHTransform() { ComponentClassName = typeName(); };
	
	virtual ~XRWHTransform() {};
	virtual void Update() override {};

	friend class XRCabinetActor;

	virtual void* Get() const {
		return NULL;
	};
public:
	int64 HashCodeTrans = -1;
	TArray<XRCabinetActor> ArrComponentList;
};

struct FShapeAccessory;
class XRWHCabinetAccessoryComponent : public  XRWHCabinetIComponent
{
public:
	CreateComponent(XRWHCabinetAccessoryComponent);

	XRWHCabinetAccessoryComponent();
	XRWHCabinetAccessoryComponent(XRWHCabinetAccessoryComponent const& ref) {}
	~XRWHCabinetAccessoryComponent();
	virtual void Update() override {};

	
public:
	/* <summary>
	/// 从actor 添加了该组件
	/// </summary>
	*/
	void InitializeObject(FCabinetWholeAccInfo *InCabAccInfo,
		const TArray<TSharedPtr<FCabinetComponent>>& InAccessories);

	void SpawnAccessories();

	void Destory();
	/* <summary>
	/// 从actor 添加了该组件
	/// </summary>
	*/
	FShapeAccessory* AddSubAccessory(FCabinetAccInfo *InAccInfo,
		EMetalsType InMetalType, bool bBoundToSocket);
	/* <summary>
	/// 从actor 添加了该组件
	/// </summary>
	*/
	FShapeAccessory* ReplaceSubAccessory(FCabinetAccInfo *InOldAccInfo,
		FCabinetAccInfo *InNewAccInfo, EMetalsType InMetalType, bool bBoundToSocket);
	/* <summary>
	/// 从actor 添加了该组件
	/// </summary>
	*/
	void SpawnSubAccessory(FShapeAccessory *InAccessory, const FVector &InRelativeLocation);
public:

	/* <summary>
	/// 从actor 添加了该组件
	/// </summary>
	*/
	void UpdateAccessoryLocation();

	virtual TSharedPtr<FShapeAccessory> Get() const {
		return Accessory;
	};
private:
	TSharedPtr<FShapeAccessory> Accessory = nullptr;
};

class XRWHCabinetRulerComponent : public  XRWHCabinetIComponent
{
public:
	CreateComponent(XRWHCabinetRulerComponent);

	XRWHCabinetRulerComponent();
	XRWHCabinetRulerComponent(XRWHCabinetRulerComponent const& ref) {}

	virtual void Update() override {};

public:
	void SpawnShapeWidthRuler();
	void Destory();
	void RefreshShapeWidthRuler();
	auto Get() const {
		return ShapeWidthRuler;
	};

	TSharedPtr<class FShapeRuler> ShapeWidthRuler; 
};

class XRWHCabinetLegsComponent : public  XRWHCabinetIComponent
{
public:
	CreateComponent(XRWHCabinetLegsComponent);

	XRWHCabinetLegsComponent();
	XRWHCabinetLegsComponent(XRWHCabinetLegsComponent const& ref) {}

	virtual void Update() override {};

public:
	void SpawnObject();
	void SpawnShapeLegs();
	void Destory();
	using TArrayShapeLegs = TArray<class AStaticMeshActor*>;
	TArrayShapeLegs& Get() {
		return ShapeLegs;
	};

	TArrayShapeLegs ShapeLegs;
};



/* <summary>
/// PlatformInfo组件类
/// </summary>
*/
class XRWHCabinetPlatformInfoComponent : public  XRWHCabinetIComponent
{
public:
	CreateComponent(XRWHCabinetPlatformInfoComponent);

	XRWHCabinetPlatformInfoComponent();
	XRWHCabinetPlatformInfoComponent(XRWHCabinetPlatformInfoComponent const& ref) {}

	virtual void Update() override {};
public:
	void SpawnObject(const TSharedPtr<FPlatformInfo> &InPlatformInfo);
	void SpawnPlatform();
	void Destory();
	TSharedPtr<FPlatformInfo> Get() const {
		return PlatformInfo;
	};

	TSharedPtr<FPlatformInfo> PlatformInfo;    // remove
};


/* <summary>
/// PlatformInfo组件类
/// </summary>
*/
class XRWHCabinetFurnitureComponent : public  XRWHCabinetIComponent
{
public:
	CreateComponent(XRWHCabinetFurnitureComponent);

	XRWHCabinetFurnitureComponent();
	XRWHCabinetFurnitureComponent(XRWHCabinetFurnitureComponent const& ref) {}

	virtual void Update() override {};
public:
	void InitializeObject(FCabinetComponent *InCabComp);
	void Destory();
	void SpawnObject();
	void UpdateComponents();
	auto Get() const {
		return ShapeComponent;
	};

	TSharedPtr<class FArmyFurniture> ShapeComponent;
};


/* <summary>
/// PlatformInfo组件类
/// </summary>
*/
class XRWHCabinetFrontSectionsComponent : public  XRWHCabinetIComponent
{
public:
	CreateComponent(XRWHCabinetFrontSectionsComponent);

	XRWHCabinetFrontSectionsComponent();
	XRWHCabinetFrontSectionsComponent(XRWHCabinetFrontSectionsComponent const& ref) {}

	virtual void Update() override {};
public:
	void InitializeObject();
	void SpawnPlatform();
	//TArray<AXRShapeTableActor*>& Get() const {
	//	return PlatformFrontSections;
	//};

	//TArray<AXRShapeTableActor*> PlatformFrontSections;    // remove
};


/* <summary>
/// PlatformInfo组件类
/// </summary>
*/
class XRWHCabinetBackSectionsComponent : public  XRWHCabinetIComponent
{
public:
	CreateComponent(XRWHCabinetBackSectionsComponent);

	XRWHCabinetBackSectionsComponent();
	XRWHCabinetBackSectionsComponent(XRWHCabinetBackSectionsComponent const& ref) {}

	virtual void Update() override {};
public:
	void InitializeObject();
	void SpawnPlatform();
	//TArray<AXRShapeTableActor*>& Get() const {
	//	return PlatformBackSections;
	//};

	//TArray<AXRShapeTableActor*> PlatformBackSections;  
};



//////////////////////////////////////////////////////////////////////////
// XRCabinetActor be agented by XRCabinetActorT
// 非数据操作代理类
//////////////////////////////////////////////////////////////////////////
template <class _TComponent>
class XRCabinetActorT : public XRCabinetActor {
public:
	using XRCabinetActor::XRCabinetActor;
	XRCabinetActorT();
	XRCabinetActorT(TSharedPtr<XRCabinetActor> cabActor);
	using XRCabinetActor::operator=;

public:
	 _TComponent* GetComponent();
	 _TComponent* operator->();
};

