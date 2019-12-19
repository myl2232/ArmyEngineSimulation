#pragma once
class  ILineType;
class  AttributeEntity;
class  ITextStyle;
class  IBlock
{
public:
	IBlock();
	virtual ~IBlock();
public:
	/** 
	*@输出文件
	* param fileName = 文件名，如果文件存在直接返回false
	*/
	virtual bool asFile(const char*fileName) = 0;
 
	/**
	 * @ 保存实体组件
	 * @param entity - AttributeEntity * - 
	 *  
	 */
	virtual void drawEntity(AttributeEntity*entity) = 0;

	/**
	 * @添加直线类型 ：TODO
	 * @param lineType - ILineType * - 线段类型
	 * @return void - 
	 */
	virtual void addLineType(ILineType*lineType)=0;


	/**
	 *  @添加图层
	 * @param entity - AttributeEntity * - 图层实体
	 * @return void - 
	 */
	virtual void addLayer(AttributeEntity*entity)=0;

	/**
	 * @添加字体风格
	 * @param style - ITextStyle * - 字体风格类
	 * @return void - 
	 */
	virtual void addTextStyle(ITextStyle*style)=0;

	/**
	 * @添加块
	 * @param block - IBlock * - 
	 * @return void - 
	 */
	virtual void addBlock(IBlock*block)=0;

	/**
	* @ 删除对象
	*/
	virtual void release()=0;

	/**
	*@ 清理
	*/
	virtual void clear() = 0;
};

