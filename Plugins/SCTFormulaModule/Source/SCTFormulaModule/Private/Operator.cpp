#include "Operator.h"

IOperand* FBraceOperator::ValidateOperandType(IOperand* Operand1, IOperand* Operand2)
{
	return nullptr;
}
IOperand* FBraceOperator::Calculate(IOperand *op1, IOperand *op2)
{
	return nullptr;
}

IOperand* FConditionOperator::ValidateOperandType(IOperand* Operand1, IOperand* Operand2)
{
	IOperand::EOperandType OperatorType1 = Operand1->GetOperandType();
	IOperand::EOperandType OperatorType2 = Operand2->GetOperandType();
	if (Operand1 != Operand2)
	{
		return nullptr;
	}
	return CreateOperand(OperatorType1);
}
IOperand* FConditionOperator::Calculate(IOperand *op1, IOperand *op2)
{
	return nullptr;
}

IOperand* FAndOperator::ValidateOperandType(IOperand* Operand1, IOperand* Operand2)
{
	if ((Operand1->GetOperandType() != IOperand::EOpType_Boolean) || 
		(Operand2->GetOperandType() != IOperand::EOpType_Boolean))
	{
		return nullptr;
	}
	return CreateOperand(IOperand::EOpType_Boolean);
}
IOperand* FAndOperator::Calculate(IOperand *op1, IOperand *op2)
{
	IOperand *ret = CreateOperand(IOperand::EOpType_Boolean);
	ret->SetValue(op1->And(op2));
	return ret;
}

IOperand* FOrOperator::ValidateOperandType(IOperand* Operand1, IOperand* Operand2)
{
	if ((Operand1->GetOperandType() != IOperand::EOpType_Boolean) ||
		(Operand2->GetOperandType() != IOperand::EOpType_Boolean))
	{
		return nullptr;
	}
	return CreateOperand(IOperand::EOpType_Boolean);
}
IOperand* FOrOperator::Calculate(IOperand *op1, IOperand *op2)
{
	IOperand *ret = CreateOperand(IOperand::EOpType_Boolean);
	ret->SetValue(op1->Or(op2));
	return ret;
}

IOperand* FEqualOperator::ValidateOperandType(IOperand* Operand1, IOperand* Operand2)
{
	if ((Operand1->GetOperandType() != IOperand::EOpType_Number) ||
		(Operand2->GetOperandType() != IOperand::EOpType_Number))
	{
		return nullptr;
	}
	return CreateOperand(IOperand::EOpType_Boolean);
}
IOperand* FEqualOperator::Calculate(IOperand *op1, IOperand *op2)
{
	IOperand *ret = CreateOperand(IOperand::EOpType_Boolean);
	ret->SetValue(op1->Equal(op2));
	return ret;
}

IOperand* FNotEqualOperator::ValidateOperandType(IOperand* Operand1, IOperand* Operand2)
{
	if ((Operand1->GetOperandType() != IOperand::EOpType_Number) ||
		(Operand2->GetOperandType() != IOperand::EOpType_Number))
	{
		return nullptr;
	}
	return CreateOperand(IOperand::EOpType_Boolean);
}
IOperand* FNotEqualOperator::Calculate(IOperand *op1, IOperand *op2)
{
	IOperand *ret = CreateOperand(IOperand::EOpType_Boolean);
	ret->SetValue(op1->NotEqual(op2));
	return ret;
}

IOperand* FGreaterOperator::ValidateOperandType(IOperand* Operand1, IOperand* Operand2)
{
	if ((Operand1->GetOperandType() != IOperand::EOpType_Number) ||
		(Operand2->GetOperandType() != IOperand::EOpType_Number))
	{
		return nullptr;
	}
	return CreateOperand(IOperand::EOpType_Boolean);
}
IOperand* FGreaterOperator::Calculate(IOperand *op1, IOperand *op2)
{
	IOperand *ret = CreateOperand(IOperand::EOpType_Boolean);
	ret->SetValue(op1->Greater(op2));
	return ret;
}

IOperand* FLessOperator::ValidateOperandType(IOperand* Operand1, IOperand* Operand2)
{
	if ((Operand1->GetOperandType() != IOperand::EOpType_Number) ||
		(Operand2->GetOperandType() != IOperand::EOpType_Number))
	{
		return nullptr;
	}
	return CreateOperand(IOperand::EOpType_Boolean);
}
IOperand* FLessOperator::Calculate(IOperand *op1, IOperand *op2)
{
	IOperand *ret = CreateOperand(IOperand::EOpType_Boolean);
	ret->SetValue(op1->Less(op2));
	return ret;
}

IOperand* FGreaterEqualOperator::ValidateOperandType(IOperand* Operand1, IOperand* Operand2)
{
	if ((Operand1->GetOperandType() != IOperand::EOpType_Number) ||
		(Operand2->GetOperandType() != IOperand::EOpType_Number))
	{
		return nullptr;
	}
	return CreateOperand(IOperand::EOpType_Boolean);
}
IOperand* FGreaterEqualOperator::Calculate(IOperand *op1, IOperand *op2)
{
	IOperand *ret = CreateOperand(IOperand::EOpType_Boolean);
	ret->SetValue(op1->GEqual(op2));
	return ret;
}

IOperand* FLessEqualOperator::ValidateOperandType(IOperand* Operand1, IOperand* Operand2)
{
	if ((Operand1->GetOperandType() != IOperand::EOpType_Number) ||
		(Operand2->GetOperandType() != IOperand::EOpType_Number))
	{
		return nullptr;
	}
	return CreateOperand(IOperand::EOpType_Boolean);
}
IOperand* FLessEqualOperator::Calculate(IOperand *op1, IOperand *op2)
{
	IOperand *ret = CreateOperand(IOperand::EOpType_Boolean);
	ret->SetValue(op1->LEqual(op2));
	return ret;
}

IOperand* FAddOperator::ValidateOperandType(IOperand* Operand1, IOperand* Operand2)
{
	if ((Operand1->GetOperandType() != IOperand::EOpType_Number) ||
		(Operand2->GetOperandType() != IOperand::EOpType_Number))
	{
		return nullptr;
	}
	return CreateOperand(IOperand::EOpType_Number);
}
IOperand* FAddOperator::Calculate(IOperand *op1, IOperand *op2)
{
	IOperand *ret = CreateOperand(IOperand::EOpType_Number);
	ret->SetValue(op1->Add(op2));
	return ret;
}

IOperand* FSubOperator::ValidateOperandType(IOperand* Operand1, IOperand* Operand2)
{
	if ((Operand1->GetOperandType() != IOperand::EOpType_Number) ||
		(Operand2->GetOperandType() != IOperand::EOpType_Number))
	{
		return nullptr;
	}
	return CreateOperand(IOperand::EOpType_Number);
}
IOperand* FSubOperator::Calculate(IOperand *op1, IOperand *op2)
{
	IOperand *ret = CreateOperand(IOperand::EOpType_Number);
	ret->SetValue(op1->Sub(op2));
	return ret;
}

IOperand* FMulOperator::ValidateOperandType(IOperand* Operand1, IOperand* Operand2)
{
	if ((Operand1->GetOperandType() != IOperand::EOpType_Number) ||
		(Operand2->GetOperandType() != IOperand::EOpType_Number))
	{
		return nullptr;
	}
	return CreateOperand(IOperand::EOpType_Number);
}
IOperand* FMulOperator::Calculate(IOperand *op1, IOperand *op2)
{
	IOperand *ret = CreateOperand(IOperand::EOpType_Number);
	ret->SetValue(op1->Mul(op2));
	return ret;
}

IOperand* FDivOperator::ValidateOperandType(IOperand* Operand1, IOperand* Operand2)
{
	if ((Operand1->GetOperandType() != IOperand::EOpType_Number) ||
		(Operand2->GetOperandType() != IOperand::EOpType_Number))
	{
		return nullptr;
	}
	return CreateOperand(IOperand::EOpType_Number);
}
IOperand* FDivOperator::Calculate(IOperand *op1, IOperand *op2)
{
	IOperand *ret = CreateOperand(IOperand::EOpType_Number);
	ret->SetValue(op1->Div(op2));
	return ret;
}