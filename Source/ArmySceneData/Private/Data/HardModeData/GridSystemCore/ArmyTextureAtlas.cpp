#include "ArmyTextureAtlas.h"
#define  MaxTextureSize 4096
uint32 NextPowerOfTwo(uint32 v)
{
	v -= 1;
	v |= v >> 16;
	v |= v >> 8;
	v |= v >> 4;
	v |= v >> 2;
	v |= v >> 1;
	return v + 1;
}
uint32 ClosestPowerOfTwo(uint32 v)
{
	uint32 nextPower = NextPowerOfTwo(v);
	uint32 prevPower = nextPower >> 1;

	if (v - prevPower < nextPower - v)
		return prevPower;
	else
		return nextPower;
}
Node* Node::Insert(float width, float height, float padding, bool use4PixelBoundaries)
{
	if (child[0])
	{
		Node * newNode = child[0]->Insert(width, height, padding, use4PixelBoundaries);
		if (newNode)
			return newNode;

		return child[1]->Insert(width, height, padding, use4PixelBoundaries);
	}

	if (taken)
		return NULL;

	if (width > rect.Width() - padding + 0.5f || height > rect.Height() - padding + 0.5f)
		return NULL;

	float dw = rect.Width() - width;

	float dh = rect.Height() - height;

	if (dw <= padding * 2 && dh <= padding * 2)
	{
		taken = true;
		return this;
	}
	if (use4PixelBoundaries && dw < 4 && dh < 4)
	{
		taken = true;
		return this;
	}

	child[0] = new Node();
	child[1] = new Node();

	if (dw > dh)
	{
		int split = int(width + padding);
		if (use4PixelBoundaries)
			split = (split + 3)&(~3);
		child[0]->rect = MinMaxRect(rect.x, rect.y, rect.x + width + padding, rect.GetBottom());
		child[1]->rect = MinMaxRect(rect.x + split, rect.y, rect.GetRight(), rect.GetBottom());

	}
	else
	{
		int split = int(height + padding);
		if (use4PixelBoundaries)
			split = (split + 3)&(~3);

		child[0]->rect = MinMaxRect(rect.x, rect.y, rect.GetRight(), rect.y + height + padding);
		child[1]->rect = MinMaxRect(rect.x, rect.y + split, rect.GetRight(), rect.GetBottom());
	}

	return child[0]->Insert(width, height, padding, use4PixelBoundaries);

}
typedef TPair<int, int> IntPair;
typedef TArray<IntPair> TextureSizes;
struct CompareTextureSizes
{
	int M_TextureIndex;

	IntPair textureSize;
	CompareTextureSizes(int INindex, IntPair& tempInPair)
	{
		textureSize = tempInPair;
		M_TextureIndex = INindex;
	}
	bool operator<(const CompareTextureSizes& b)const
	{
		return textureSize.Key*textureSize.Value > b.textureSize.Key * b.textureSize.Value;
	}

};
void ClipTexure2D(UTexture2D* atlas, int destCoordX, int destCoordY, int destWidth, int destHeight, UTexture2D* InTexture);
UTexture2D* PackTextureAtlas(int atlasMaximumSize, int textureCount, TArray<UTexture2D*> textures, Rectf* outRects, int padding, int textureMode)
{
	const int kMinTextureSize = 4;
	const int kMinAtlasSize = 8;

	atlasMaximumSize = FMath::Max(atlasMaximumSize, kMinAtlasSize);

	TextureSizes textureSizes;
	textureSizes.AddUninitialized(textureCount);

	for (int i = 0; i < textureCount; i++)
	{
		IntPair& size = textureSizes[i];

		size.Key = textures[i]->GetSizeX();
		size.Value = textures[i]->GetSizeY();

		while (size.Key > atlasMaximumSize && size.Key > kMinTextureSize)
		{
			size.Key /= 2;
		}
		while (size.Value > atlasMaximumSize && size.Value > kMinTextureSize)
		{
			size.Value /= 2;
		}

	}
	int number = textureSizes.Num();
	TArray<CompareTextureSizes> tempCompareSizes;
	tempCompareSizes.AddUninitialized(number);
	for (int i = 0; i < number; i++)
	{
		tempCompareSizes[i] = CompareTextureSizes(i, textureSizes[i]);
	}
	tempCompareSizes.Sort();


	TArray<int> sortedIndices;
	sortedIndices.AddUninitialized(number);
	for (int i = 0; i < number; i++)
	{
		sortedIndices[i] = tempCompareSizes[i].M_TextureIndex;
	}

	int totalPixels = 0;
	for (int i = 0; i < textureCount; ++i)
	{
		IntPair& size = textureSizes[i];
		totalPixels += size.Key * size.Value;
	}

	int atlasWidth = FMath::Min(NextPowerOfTwo(FMath::Sqrt(totalPixels)), (uint32)atlasMaximumSize);
	int atlasHeight = FMath::Min(NextPowerOfTwo((totalPixels / atlasWidth)), (uint32)atlasMaximumSize);
	bool packOk = true;
	const int kMaxPackIterations = 100;
	int packIterations = 0;
	TArray<Node*> textureNodes;
	textureNodes.AddUninitialized(textureCount);

	Node tree;
	do
	{
		packOk = true;
		tree.Reset();

		tree.rect = MinMaxRect<float>(0, 0, atlasWidth, atlasHeight);
		bool use4PixelBoundaries = false;
		for (int i = 0; i < textureCount; ++i)
		{
			int texIndex = sortedIndices[i];
			int texWidth = textureSizes[texIndex].Key;
			int texHeight = textureSizes[texIndex].Value;
			Node* node = tree.Insert(texWidth, texHeight, padding, use4PixelBoundaries);
			textureNodes[texIndex] = node;
			if (!node)
			{
				packOk = false;
				break;
			}
		}
		if (!packOk)
		{
			if (atlasWidth != atlasMaximumSize || atlasHeight != atlasMaximumSize)
			{
				if (atlasWidth == atlasMaximumSize)
					atlasHeight *= 2;
				else if (atlasHeight == atlasMaximumSize)
					atlasWidth *= 2;
				else if (atlasWidth < atlasHeight)
					atlasWidth *= 2;
				else
					atlasHeight *= 2;
			}
			else
			{
				for (int i = 0; i < textureCount; ++i)
				{
					IntPair& size = textureSizes[i];
					if (size.Key > kMinTextureSize && size.Value > kMinTextureSize)
					{
						size.Key = size.Key * 3 / 4;
						size.Value = size.Value * 3 / 4;
					}
				}
				++packIterations;
			}
		}

	} while (!packOk && packIterations < kMaxPackIterations);
	if (!packOk)
		return NULL;


	// fill out uv rectangles for the input textures

	for (int i = 0; i < textureCount; ++i)
	{
		int texIndex = sortedIndices[i];

		int texWidth = textureSizes[texIndex].Key;
		int texHeight = textureSizes[texIndex].Value;

		const Node* node = textureNodes[texIndex];

		outRects[texIndex] = MinMaxRect(
			node->rect.x / atlasWidth,
			node->rect.y / atlasHeight,
			(node->rect.x + texWidth) / atlasWidth,
			(node->rect.y + texHeight) / atlasHeight);
	}
	UTexture2D*	atlas = UTexture2D::CreateTransient(atlasWidth, atlasHeight);
	const int numatlaMips = atlas->GetNumMips();
	for (int i = 0; i < textureCount; ++i)
	{
		int texIndex = sortedIndices[i];

		int texWidth = textureSizes[texIndex].Key;
		int texHeight = textureSizes[texIndex].Value;

		const Node* node = textureNodes[texIndex];

		int destCoordX = node->rect.x;
		int destCoordY = node->rect.y;

		int destWidth = FMath::Min(texWidth, FMath::Max(1, (int)node->rect.width - padding));
		int destHeight = FMath::Min(texHeight, FMath::Max(1, (int)node->rect.height - padding));

		int atlasMipWidth = atlasWidth;
		int atlasMipHeight = atlasHeight;
		// copy  all mips to atlas
		ClipTexure2D(atlas, destCoordX, destCoordY, destWidth, destHeight, textures[texIndex]);
	}
	if (atlas->PlatformData->Mips[0].BulkData.IsLocked())
	{
		atlas->PlatformData->Mips[0].BulkData.Unlock();
	}

	atlas->SRGB = true;
#if WITH_EDITORONLY_DATA
	atlas->CompressionNone = true;
	atlas->MipGenSettings = TMGS_NoMipmaps;
#endif
	atlas->CompressionSettings = TC_EditorIcon;

	atlas->UpdateResource();
	return atlas;

}
void ClipTexure2D(UTexture2D* atlas, int destCoordX, int destCoordY, int destWidth, int destHeight, UTexture2D* InTexture)
{
	if (atlas->PlatformData->Mips[0].BulkData.IsLocked())
	{
		atlas->PlatformData->Mips[0].BulkData.Unlock();
	}
	if (InTexture->PlatformData->Mips[0].BulkData.IsLocked())
	{
		InTexture->PlatformData->Mips[0].BulkData.Unlock();
	}
	uint8* MipData = (uint8*)atlas->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	if (InTexture->GetSizeX() > destWidth || InTexture->GetSizeY() > destHeight)
		return;
	int atlasWidth = atlas->PlatformData->SizeX;
	int atlasHeight = atlas->PlatformData->SizeY;

	uint8* MipData1 = (uint8*)InTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	for (int height = 0; height < destHeight; ++height)
	{
		memcpy(MipData + ((height + destCoordY) * atlasWidth + destCoordX) * sizeof(FColor), MipData1 + (height * destWidth) * sizeof(FColor), destWidth * sizeof(FColor));
	}

	atlas->PlatformData->Mips[0].BulkData.Unlock();
	InTexture->PlatformData->Mips[0].BulkData.Unlock();
}
UTexture2D* PackTextureAtlasSimple(int atlasMaximumSize, int textureCount, TArray<UTexture2D*> textures, Rectf* outRects, int padding, bool upload, bool markNoLongerReadable)
{
	atlasMaximumSize = FMath::Min(MaxTextureSize, atlasMaximumSize);
	TArray<int> remap;
	remap.Init(0, textureCount);
	TArray<UTexture2D*> uniqueTextures;
	for (int i = 0; i < textureCount; i++)
	{
		if (!textures[i]->IsValidLowLevel())
		{
			*outRects = Rectf(0, 0, 0, 0);
			remap[i] = -1;
			continue;
		}
		int32 index = uniqueTextures.Find(textures[i]);
		if (index != INDEX_NONE)
		{
			remap[i] = index;
		}
		else
		{
			remap[i] = uniqueTextures.Num();
			uniqueTextures.Push(textures[i]);
		}
	}
	UTexture2D* atlas = NULL;
	if (uniqueTextures.Num() > 0)
	{
		TArray<Rectf> uniqueRects;
		uniqueRects.AddUninitialized(uniqueTextures.Num());
		atlas = PackTextureAtlas(atlasMaximumSize, uniqueTextures.Num(), uniqueTextures, &uniqueRects[0], padding, 0);
		for (int i = 0; i < textureCount; ++i)
		{
			if (remap[i] != -1)
				outRects[i] = uniqueRects[remap[i]];
		}
	}
	return atlas;
}

void FArmyTextureAtlas::AddTexture(uint32 id, UTexture2D* InTexture)
{
	Tile tile;
	tile.image = InTexture;
	AllTiles.Add(id, tile);
}

bool FArmyTextureAtlas::GetRectPosById(uint32 id, Rectf& OutResult)
{
	Tile* tile = AllTiles.Find(id);
	if (tile)
	{
		OutResult = tile->posInfo;
		return true;
	}
	return false;
}

UTexture2D* FArmyTextureAtlas::Create(uint32 max_size)
{
	UTexture2D* altlas = NULL;
	int number = AllTiles.Num();
	TArray<UTexture2D*> allTextures;
	for (auto& iter : AllTiles)
	{
		allTextures.Push(iter.Value.image);
	}
	TArray<Rectf> allRectfs;
	allRectfs.AddUninitialized(number);
	altlas = Pack(max_size, number, allTextures, &allRectfs[0], M_Padding);
	int index = 0;
	for (auto& iter : AllTiles)
	{
		iter.Value.posInfo = allRectfs[index] ;
		index++;
	}
	return altlas;
}

uint32 FArmyTextureAtlas::GetNumImages() const
{
	return AllTiles.Num();
}

void FArmyTextureAtlas::Reset()
{

	AllTiles.Empty();
}

UTexture2D* FArmyTextureAtlas::Pack(int atlasMaximumSize, int textureCount, TArray<UTexture2D*> textures, Rectf* outRects, int padding)
{
	return PackTextureAtlasSimple(atlasMaximumSize, textureCount, textures, outRects, padding, false, false);
}
