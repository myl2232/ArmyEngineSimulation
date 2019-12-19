#pragma once
#include<iostream>
 
 
/**
*@垂直对齐
*/
enum VAlign {
	VAlignTop,  
	VAlignMiddle, 
	VAlignBase, 
	VAlignBottom
};

/**
*@多边形类型
*/
enum PolygonType
{
	/** 开多边 */
	P_OPEN = 1,

	/**封闭多边形 */
	P_CLOSE=3,
};

/**
*@水平对齐
*/
enum HAlign {
	HAlignLeft,    
	HAlignCenter,    
	HAlignRight,   
	HAlignAlign,   
	HAlignMid, 
	HAlignFit   
};
/**
* 文字渲染方向
*/
enum TextDrawingDirection {
	LeftToRight = 1,    
	TopToBottom = 3,
	/** 和文字风格textStyle保持一致 */
	ByStyle = 5    
};

/**
* @线性空间风格
*/
enum TextLineSpacingStyle {
	AtLeast = 1,        /**< Taller characters will override */
	Exact = 2           /**< Taller characters will not override */
};

/**
* @线宽
*/
enum LineWeight {
	Weight000 = 0,
	Weight005 = 5,
	Weight009 = 9,
	Weight013 = 13,
	Weight015 = 15,
	Weight018 = 18,
	Weight020 = 20,
	Weight025 = 25,
	Weight030 = 30,
	Weight035 = 35,
	Weight040 = 40,
	Weight050 = 50,
	Weight053 = 53,
	Weight060 = 60,
	Weight070 = 70,
	Weight080 = 80,
	Weight090 = 90,
	Weight100 = 100,
	Weight106 = 106,
	Weight120 = 120,
	Weight140 = 140,
	Weight158 = 158,
	Weight200 = 200,
	Weight211 = 211,
	WeightByLayer = -1,
	WeightByBlock = -2,
	WeightByLwDefault = -3,
	WeightInvalid = -4
};

/**
* @色彩类型
*/
enum ColorType
{
	/** 和图层保持一致 */
	ByLayer,
	/** 和块保持一致 */
	ByBlock,
	/** 固定颜色 */
	Fixed
};


/**
* @图层属性
*/
enum LayerFlag
{
	/** 默认打开 */
	LayerDefault = 0,
	/** 冻结 */
	LayerFrozen =0x1,
	/** 上锁 */
	LayerLock     = 0x4,
	/** 关闭，不显示*/
	LayerOff		 = 0x8
};

struct AttributeDesc 
{
	/** 图层名 */
	std::string layerName;
	/** 线型名 */
	std::string lineType;
	/** 颜色 */
	int	  red, green, blue;
	/** 线宽 */
	LineWeight  weight;
	/** 色彩类型  */
	ColorType	colorType;
	/** original */
	bool	  blockProperty;

	int		color;

	int		color24;
	double  lineScale;
};

struct Double4
{
	Double4(double px, double py, double pz=0, double pw=0):
		x(px), y(py),z(pz),w(pw)
	{

	}
	Double4():x(0.0), y(0.0), z(0.0), w(0.0)
	{

	}
 

	double		x, y, z, w;
};

struct  Color 
{
	Color(int px, int py, int pz, int pw=0) :
		x(px), y(py), z(pz), w(pw)
	{

	}
	Color()
	{

	}
	int x, y, z, w;
};
 
struct TextDesc
{
	/** 文本插入点 */
	Double4 insertPoint;
	/** 对齐点 */
	Double4 alignPoint;
	/** 高度 */
	double	height;
	double xScaleFactor;
	int textGenerationFlags;
	int hJustification;
	int vJustification;
	/**文本 */
	 char* text;
	 /** 风格 */
	 char* style;
	 /** 旋转角度 */
	double angle;
};

struct MTextDesc
{
	Double4  alignmentPoint;
	
	/*! Text height */
	double height;
	/*! Width of the text box. */
	double width;
	/**
	* Attachment point.
	*
	* 1 = Top left, 2 = Top center, 3 = Top right,
	* 4 = Middle left, 5 = Middle center, 6 = Middle right,
	* 7 = Bottom left, 8 = Bottom center, 9 = Bottom right
	*/
	HAlign hAlign;
	VAlign	vAlign;
	TextDrawingDirection	drawDirection;
	TextLineSpacingStyle	lineSpaceStyle;
	/**
	* Line spacing factor. 0.25 .. 4.0
	*/
	double lineSpacingFactor;
	/*! Text string. */
	 std::string text;
	/*! Style string. */
	 char* style;
	/*! Rotation angle. */
	double angle;
};

struct TextStyleDesc
{
	/** Style name */
	const char* name;
	/** Style flags */
	int flags;
	/** Fixed text height or 0 for not fixed. */
	double fixedTextHeight;
	/** Width factor */
	double widthFactor;
	/** Oblique angle */
	double obliqueAngle;
	/** Text generation flags */
	int textGenerationFlags;
	/** Last height used */
	double lastHeightUsed;
	/** Primary font file name */
	const char* primaryFontFile;
	/** Big font file name */
	const char* bigFontFile;
	bool bold;
	bool italic;
};

struct  DimensionDesc {
 
	/*!   Coordinate of definition point. */
	Double4 definitionPoint;
	/*!  Coordinate of middle point of the text. */
	Double4 midTextPoint;
	/**
	* Dimension type.
	*
	* 0   Rotated, horizontal, or vertical
	* 1   Aligned
	* 2   Angular
	* 3   Diametric
	* 4   Radius
	* 5   Angular 3-point
	* 6   Ordinate
	* 64  Ordinate type. This is a bit value (bit 7)
	*     used only with integer value 6. If set,
	*     ordinate is X-type; if not set, ordinate is
	*     Y-type
	* 128 This is a bit value (bit 8) added to the
	*     other group 70 values if the dimension text
	*     has been positioned at a user-defined
	*    location rather than at the default location
	*/
	int type;
	/**
	* Attachment point.
	*
	* 1 = Top left, 2 = Top center, 3 = Top right,
	* 4 = Middle left, 5 = Middle center, 6 = Middle right,
	* 7 = Bottom left, 8 = Bottom center, 9 = Bottom right,
	*/
	int attachmentPoint;
	/**
	* Line spacing style.
	*
	* 1 = at least, 2 = exact
	*/
	int lineSpacingStyle;
	/**
	* Line spacing factor. 0.25 .. 4.0
	*/
	double lineSpacingFactor;
	/**
	* Text string.
	*
	* Text string entered explicitly by user or null
	* or "<>" for the actual measurement or " " (one blank space).
	* for supressing the text.
	*/
	const char* text;
	/*! Dimension style (font name). */
	const char* style;
	/**
	* Rotation angle of dimension text away from
	* default orientation.
	*/
	double angle;
	/**
	* Linear factor style override.
	*/
	double linearFactor;
	/**
	* Dimension scale (dimscale) style override.
	*/
	double dimScale;
	bool arrow1Flipped;
	bool arrow2Flipped;
};