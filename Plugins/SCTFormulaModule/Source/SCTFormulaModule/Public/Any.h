#pragma once
#include "UnrealTypeTraits.h"
#include "Misc/Guid.h"

enum EAnyValueClassType
{
	EAVCT_None = 0,
	EAVCT_Array,
	EAVCT_Set,
	EAVCT_Map,
	EAVCT_Pair,
	EAVCT_Tuple3Elements
};

enum EAnyValueType
{
	EAVT_None = 0,
	EAVT_Bool,
	EAVT_Int,
	EAVT_Float,
	EAVT_Double,
	EAVT_String,
	EAVT_Pointer
};

template <class ValueType>
class TValueTypeId
{
public:
	static FGuid GetTypeId() { FGuid(); }
};

template <>
class TValueTypeId<float>
{
public:
	static FGuid GetTypeId() { return FGuid(EAVT_Float, EAVCT_None, 0, 0); }
};

template <>
class TValueTypeId<FString>
{
public:
	static FGuid GetTypeId() { return FGuid(EAVT_String, EAVCT_None, 0, 0); }
};

template <>
class TValueTypeId<bool>
{
public:
	static FGuid GetTypeId() { return FGuid(EAVT_Bool, EAVCT_None, 0, 0); }
};

template <>
class TValueTypeId<UPTRINT>
{
public:
	static FGuid GetTypeId() { return FGuid(EAVT_Pointer, EAVCT_None, 0, 0); }
};

template <>
class TValueTypeId<int32>
{
public:
	static FGuid GetTypeId() { return FGuid(EAVT_Int, EAVCT_None, 0, 0); }
};

template <>
class TValueTypeId< TArray<FString> >
{
public:
	static FGuid GetTypeId() { return FGuid(EAVT_String, EAVCT_Array, 0, 0); }
};

template <>
class TValueTypeId< TArray<float> >
{
public:
	static FGuid GetTypeId() { return FGuid(EAVT_Float, EAVCT_Array, 0, 0); }
};

template <>
class TValueTypeId< TArray<double> >
{
public:
	static FGuid GetTypeId() { return FGuid(EAVT_Double, EAVCT_Array, 0, 0); }
};

class FPlaceHolder 
{
public:
	virtual ~FPlaceHolder() {}
	template <class Type>
	bool IsSameType() const { return TypeId == TValueTypeId<Type>::GetTypeId(); }

protected:
	FGuid TypeId;
};

template <class ValueType>
class FHolder : public FPlaceHolder
{
public:
	FHolder(typename TCallTraits<ValueType>::ConstReference value) : Value(value) { TypeId = TValueTypeId<ValueType>::GetTypeId(); }
	ValueType Value;
};

class FAny 
{
public:
	FAny() {}

	template <class ValueType>
	explicit FAny(const ValueType &value)
		: PlaceHolder(MakeShareable(new FHolder<ValueType>(value))) {}

	template <class ValueType>
	FAny& operator = (const ValueType &value) 
	{
		PlaceHolder = MakeShareable(new FHolder<ValueType>(value));
		return *this;
	}

	template <class OtherType>
	bool IsSameType() const { return PlaceHolder->IsSameType<OtherType>(); }

	const TSharedPtr<FPlaceHolder>& Holder() const { return PlaceHolder; }

protected:
	TSharedPtr<FPlaceHolder> PlaceHolder;
};

template <class ValueType>
inline ValueType* AnyCast(FAny *pAny)
{
	return &(static_cast<FHolder<ValueType>*>(pAny->Holder().Get())->Value);
}

template <class ValueType>
inline ValueType* AnyCastSafe(FAny *pAny)
{
	if (pAny && pAny->Holder().IsValid() && pAny->IsSameType<ValueType>())
		return AnyCast<ValueType>(pAny);
	else
		return nullptr;
}