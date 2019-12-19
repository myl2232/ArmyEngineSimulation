/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SCTInsideShape.h
* @Description 空间：用于对单元柜中进行空间分割
*
* @Author 赵志强
* @Date 2018年5月15日
* @Version 1.0
*/
#pragma once

class FSCTShape;

//空白空间
class FMarginSpace
{
public:
	FMarginSpace();
	FMarginSpace(int32 W, int32 D, int32 H);
	FMarginSpace(FMarginSpace* InParent);
	~FMarginSpace();

public:
	/** 判断空间是否为空 */
	bool IsEmptySpace()  const;
	/** 设置空间尺寸 */
	void SetDimension(int32 W, int32 D, int32 H);
	/** 设置空间位置 */
	void SetPosition(int32 X, int32 Y, int32 Z);

public:
	//空间尺寸信息
	int32 Width;
	int32 Depth;
	int32 Height;

	//空间位置信息
	int32 PosX;
	int32 PosY;
	int32 PosZ;
};


//包含型录的空间
class FShapeSpace : public FMarginSpace
{
public:
	FShapeSpace();
	FShapeSpace(TSharedPtr<FSCTShape> InShape);
	~FShapeSpace();

public:
	void SetWithinShape(TSharedPtr<FSCTShape> InShape);

public:
	//空间中的型录
	TWeakPtr<FSCTShape> ShapeWithin;
};

//垂直方向空间集合
class FVerticalSpaceSet : public FMarginSpace
{
public:
	FVerticalSpaceSet();
	~FVerticalSpaceSet();

public:
	bool AddShape(int32 InIndex, TSharedPtr<FSCTShape> InShape);
	bool DeleteShape(int32 InIndex);
	bool SetSpaceSize(int32 InIndex);
	bool SetSpaceLocation(int32 InIndex);

public:
	//垂直方向空间集合
	TArray<TSharedPtr<FMarginSpace>> SpaceSet;
};