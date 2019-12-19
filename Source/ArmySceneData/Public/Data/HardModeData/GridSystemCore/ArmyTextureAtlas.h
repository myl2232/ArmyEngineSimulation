#pragma  once
template<typename T>
class RectT
{
public:
	typedef RectT<T> RectType;

	typedef float BaseType;

	T x;  // rectangle x coordinate
	T y; // rectangle y coordinate
	T width; // rectangle width;
	T height; // rectangle height;
	RectT() { Reset(); }
	RectT(T inX, T inY, T iWidth, T iHeight)
	{
		x = inX; width = iWidth;
		y = inY; height = iHeight;
	}

	T GetRight()const { return x + width; }
	T GetBottom()const { return y + height; }
	T GetXMax()const { return x + width; }
	T GetYMax()const { return y + height; }
	inline bool IsEmpty()const { return width <= 0 || height <= 0; }
	inline void SetPosition(const FVector2D& position) { x = position.X; y = position.Y; }
	inline FVector2D GetPosition()const { return FVector2D(x, y); }

	void SetCenterPos(T cx, T cy) { x = cx - width / 2; y = cy - height / 2; }
	FVector2D GetCenterPos()const { return FVector2D(x + (BaseType)width / 2, y + (BaseType)height / 2); }
	inline void Set(T inX, T inY, T iWidth, T iHeight)
	{
		x = inX; width = iWidth;
		y = inY; height = iHeight;
	}

	void Clamp(const RectType &r)
	{
		T x2 = x + width;
		T y2 = y + height;

		T rx2 = r.x + r.width;
		T ry2 = r.y + r.height;

		if (x < r.x) x = r.x;
		if (x2 > rx2) x2 = rx2;
		if (y < r.y) y = r.y;
		if (y2 > ry2) y2 = ry2;

		width = x2 - x;
		if (width < 0)width = 0;
		height = y2 - y;
		if (height < 0) height = 0;
	}
	inline T Width()const { return width; }
	inline T Height()const { return height; }
	inline void Reset() { x = y = width = height = 0; }
	bool operator==(const RectType& r)const { return x == r.x && y == r.y && width == r.width && height = r.height; }
	bool operator!=(const RectType& r)const { return x != r.x || y != r.y || width != r.width || height != r.height; }
};
typedef RectT<float> Rectf;
typedef RectT<int> RectInt;

// make a rect with width & heihgt;

template<typename T>
inline RectT<T> MinMaxRect(T minx, T minY, T maxX, T maxY) { return RectT<T>(minx, minY, maxX - minx, maxY - minY); }

template<>
inline bool Rectf::IsEmpty()const { return width <= 0.00001f || height <= 0.00001f; }

struct Node
{
	Node() :taken(false) { child[0] = NULL; child[1] = NULL; }
	~Node() { delete child[0]; delete child[1]; }

	void Reset()
	{
		delete child[0]; delete child[1];
		child[0] = NULL; child[1] = NULL;

		taken = false;
	}

	Node* Insert(float width, float height, float padding, bool use4PixelBoundaries);
	Node* child[2];

	Rectf rect;
	bool taken;
};
ARMYSCENEDATA_API UTexture2D*  PackTextureAtlasSimple(int atlasMaximumSize, int textureCount, TArray<UTexture2D*>  textures, Rectf* outRects, int padding, bool upload, bool markNoLongerReadable);

class FArmyTextureAtlas
{
public:
	FArmyTextureAtlas(int padding) { M_Padding = padding; }

	void AddTexture(uint32 id, UTexture2D* InTexture);

	bool GetRectPosById(uint32 id,Rectf& OutRectUV);

	UTexture2D* Create(uint32 max_size);

	uint32 GetNumImages()const;

	void Reset();
protected:

	struct Tile
	{
		UTexture2D* image;
		Rectf posInfo;
	};
	UTexture2D*  Pack(int atlasMaximumSize, int textureCount, TArray<UTexture2D*>  textures, Rectf* outRects, int padding);
	TMap<uint32, Tile> AllTiles;
	uint32 M_Padding;
};