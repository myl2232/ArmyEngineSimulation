#include "Operand.h"
#include "Operator.h"
#include "Formula.h"

IOperand::IOperand() 
{
}

IOperand::~IOperand()
{
}

EFormulaElementType IOperand::GetType() const
{
	return EFormulaElementType::EFEType_Operand;
}

FAny* IOperand::GetValue() const
{
	return const_cast<FAny*>(&Value);
}

bool IOperand::IsSameType(IOperand *Other)
{
	return GetType() == Other->GetType();
}

FNumberOperand::FNumberOperand() : IOperand()
{
}

IOperand::EOperandType FNumberOperand::GetOperandType() const
{
	return IOperand::EOpType_Number;
}

bool FNumberOperand::Equal(IOperand *Other)
{
	if (IsSameType(Other))
	{
		FAny *pMyValue = GetValue(), *pOtherValue = Other->GetValue();
		return (*AnyCast<float>(pMyValue)) == (*AnyCast<float>(pOtherValue));
	}
	return false;
}

bool FNumberOperand::NotEqual(IOperand *Other)
{
	if (IsSameType(Other))
	{
		FAny *pMyValue = GetValue(), *pOtherValue = Other->GetValue();
		return (*AnyCast<float>(pMyValue)) != (*AnyCast<float>(pOtherValue));
	}
	return false;
}

bool FNumberOperand::Greater(IOperand *Other)
{
	if (IsSameType(Other))
	{
		FAny *pMyValue = GetValue(), *pOtherValue = Other->GetValue();
		return (*AnyCast<float>(pMyValue)) > (*AnyCast<float>(pOtherValue));
	}
	return false;
}

bool FNumberOperand::Less(IOperand *Other)
{
	if (IsSameType(Other))
	{
		FAny *pMyValue = GetValue(), *pOtherValue = Other->GetValue();
		return (*AnyCast<float>(pMyValue)) < (*AnyCast<float>(pOtherValue));
	}
	return false;
}

bool FNumberOperand::GEqual(IOperand *Other)
{
	if (IsSameType(Other))
	{
		FAny *pMyValue = GetValue(), *pOtherValue = Other->GetValue();
		return (*AnyCast<float>(pMyValue)) >= (*AnyCast<float>(pOtherValue));
	}
	return false;
}

bool FNumberOperand::LEqual(IOperand *Other)
{
	if (IsSameType(Other))
	{
		FAny *pMyValue = GetValue(), *pOtherValue = Other->GetValue();
		return (*AnyCast<float>(pMyValue)) <= (*AnyCast<float>(pOtherValue));
	}
	return false;
}

float FNumberOperand::Add(IOperand *Other)
{
	if (IsSameType(Other))
	{
		FAny *pMyValue = GetValue(), *pOtherValue = Other->GetValue();
		return (*AnyCast<float>(pMyValue)) + (*AnyCast<float>(pOtherValue));
	}
	return 0.0;
}

float FNumberOperand::Sub(IOperand *Other)
{
	if (IsSameType(Other))
	{
		FAny *pMyValue = GetValue(), *pOtherValue = Other->GetValue();
		return (*AnyCast<float>(pMyValue)) - (*AnyCast<float>(pOtherValue));
	}
	return 0.0;
}

float FNumberOperand::Mul(IOperand *Other)
{
	if (IsSameType(Other))
	{
		FAny *pMyValue = GetValue(), *pOtherValue = Other->GetValue();
		return (*AnyCast<float>(pMyValue)) * (*AnyCast<float>(pOtherValue));
	}
	return 0.0;
}

float FNumberOperand::Div(IOperand *Other)
{
	if (IsSameType(Other))
	{
		FAny *pMyValue = GetValue(), *pOtherValue = Other->GetValue();
		return (*AnyCast<float>(pMyValue)) / (*AnyCast<float>(pOtherValue));
	}
	return 0.0;
}


FStringOperand::FStringOperand() : IOperand()
{
}

IOperand::EOperandType FStringOperand::GetOperandType() const
{
	return IOperand::EOpType_String;
}

bool FStringOperand::Equal(IOperand *Other)
{
	if (IsSameType(Other))
	{
		FAny *pMyValue = GetValue(), *pOtherValue = Other->GetValue();
		return (*AnyCast<FString>(pMyValue)) == (*AnyCast<FString>(pOtherValue));
	}
	return false;
}

bool FStringOperand::NotEqual(IOperand *Other)
{
	if (IsSameType(Other))
	{
		FAny *pMyValue = GetValue(), *pOtherValue = Other->GetValue();
		return (*AnyCast<FString>(pMyValue)) != (*AnyCast<FString>(pOtherValue));
	}
	return false;
}

FBooleanOperand::FBooleanOperand() : IOperand()
{
}

IOperand::EOperandType FBooleanOperand::GetOperandType() const
{
	return IOperand::EOpType_Boolean;
}

bool FBooleanOperand::And(IOperand *Other)
{
	if (IsSameType(Other))
	{
		FAny *pMyValue = GetValue(), *pOtherValue = Other->GetValue();
		return (*AnyCast<bool>(pMyValue)) && (*AnyCast<bool>(pOtherValue));
	}
	return false;
}

bool FBooleanOperand::Or(IOperand *Other)
{
	if (IsSameType(Other))
	{
		FAny *pMyValue = GetValue(), *pOtherValue = Other->GetValue();
		return (*AnyCast<bool>(pMyValue)) || (*AnyCast<bool>(pOtherValue));
	}
	return false;
}

IOperand* CreateOperand(IOperand::EOperandType nType)
{
	IOperand *pOperand;
	switch (nType)
	{
	case IOperand::EOpType_Number:
		pOperand = new FNumberOperand();
		break;
	case IOperand::EOpType_String:
		pOperand = new FStringOperand();
		break;
	case IOperand::EOpType_Boolean:
		pOperand = new FBooleanOperand();
		break;
	default:
		return nullptr;
	}
	return pOperand;
}

void DestroyOperand(IOperand *pOperand)
{
	delete pOperand;
}