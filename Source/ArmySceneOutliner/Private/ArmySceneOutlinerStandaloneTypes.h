#pragma once

#include "UObject/ObjectKey.h"
#include "Templates/MaxSizeof.h"

/** Variant type that defines an identifier for a tree item. Assumes 'trivial relocatability' as with many unreal containers. */
struct FArmyTreeItemID
{
public:
	/** Default constructed null item ID */
	FArmyTreeItemID() : Type(EType::Null), CachedHash(0) {}

	/** ID representing a UObject */
	FArmyTreeItemID(const UObject* InObject) : Type(EType::Object)
	{
		check(InObject);
		new (Data) FObjectKey(InObject);
		CachedHash = CalculateTypeHash();
	}
	FArmyTreeItemID(const FObjectKey& InKey) : Type(EType::Object)
	{
		new (Data) FObjectKey(InKey);
		CachedHash = CalculateTypeHash();
	}

	/** ID representing a folder */
	FArmyTreeItemID(const FName& InFolder) : Type(EType::Folder)
	{
		new (Data) FName(InFolder);
		CachedHash = CalculateTypeHash();
	}

	/** Copy construction / assignment */
	FArmyTreeItemID(const FArmyTreeItemID& Other)
	{
		*this = Other;
	}
	FArmyTreeItemID& operator=(const FArmyTreeItemID& Other)
	{
		Type = Other.Type;
		switch(Type)
		{
			case EType::Object:			new (Data) FObjectKey(Other.GetAsObjectKey());											break;
			case EType::Folder:			new (Data) FName(Other.GetAsFolderRef());												break;
			default:																											break;
		}

		CachedHash = CalculateTypeHash();
		return *this;
	}

	/** Move construction / assignment */
	FArmyTreeItemID(FArmyTreeItemID&& Other)
	{
		*this = MoveTemp(Other);
	}
	FArmyTreeItemID& operator=(FArmyTreeItemID&& Other)
	{
		FMemory::Memswap(this, &Other, sizeof(FArmyTreeItemID));
		return *this;
	}

	~FArmyTreeItemID()
	{
		switch(Type)
		{
			case EType::Object:			GetAsObjectKey().~FObjectKey();							break;
			case EType::Folder:			GetAsFolderRef().~FName();								break;
			default:																			break;
		}
	}

	friend bool operator==(const FArmyTreeItemID& One, const FArmyTreeItemID& Other)
	{
		return One.Type == Other.Type && One.CachedHash == Other.CachedHash && One.Compare(Other);
	}
	friend bool operator!=(const FArmyTreeItemID& One, const FArmyTreeItemID& Other)
	{
		return One.Type != Other.Type || One.CachedHash != Other.CachedHash || !One.Compare(Other);
	}

	uint32 CalculateTypeHash() const
	{
		uint32 Hash = 0;
		switch(Type)
		{
			case EType::Object:			Hash = GetTypeHash(GetAsObjectKey());				break;
			case EType::Folder:			Hash = GetTypeHash(GetAsFolderRef());				break;
			default:																		break;
		}

		return HashCombine((uint8)Type, Hash);
	}

	friend uint32 GetTypeHash(const FArmyTreeItemID& ItemID)
	{
		return ItemID.CachedHash;
	}

private:

	FObjectKey& 				GetAsObjectKey() const 			{ return *reinterpret_cast<FObjectKey*>(Data); }
	FName& 						GetAsFolderRef() const			{ return *reinterpret_cast<FName*>(Data); }

	/** Compares the specified ID with this one - assumes matching types */
	bool Compare(const FArmyTreeItemID& Other) const
	{
		switch(Type)
		{
			case EType::Object:			return GetAsObjectKey() == Other.GetAsObjectKey();
			case EType::Folder:			return GetAsFolderRef() == Other.GetAsFolderRef();
			case EType::Null:			return true;
			default: check(false);		return false;
		}
	}

	enum class EType : uint8 { Object, Folder, Null };
	EType Type;

	uint32 CachedHash;
	static const uint32 MaxSize = TMaxSizeof<FObjectKey, FName>::Value;
	mutable uint8 Data[MaxSize];
};

/** Get the parent path for the specified folder path */
FORCEINLINE FName GetParentPath(FName Path)
{
    return FName(*FPaths::GetPath(Path.ToString()));
}