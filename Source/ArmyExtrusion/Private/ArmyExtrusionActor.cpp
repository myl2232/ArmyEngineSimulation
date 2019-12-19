#include "XRExtrusionActor.h"
#include "XRMath.h"
//#include "XRGenMeshComponent.h"
#include "XRProceduralMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "XREngineModule.h"
#include <string>
#include "dxfFile.h"
#include "XRFurniture.h"
#include "XRDxfModel.h"
#include "XRPolyline.h"
#include "XRHomeData/Private/Data/HardModeData/Mathematics/EarcutTesselator.h"
#include "XRHardware.h"
#include "XRRoomSpaceArea.h"
#include "XRConstructionManager.h"
#include "XRBaseArea.h"
#include "XRRoom.h"

//#include "DTOOLGenMeshComponent.h"

const float TextureRepeatDistance = 300.f;

void GenerateSegment_Split(TArray<FXRProcMeshVertex> &Verts,
	int32 &nIndex,
	const TArray<FVector> &Outline,
	int32 nStart, int32 nEnd,
	const TArray<FVector2D> &ProfileOutline,
	const TArray<float> &LengthBetweenOutlinePoints,
	bool bSwapUV)
{
	FVector v01, v02, v12, Right;
	bool bStartClockwise = true;

	float LocalTextureRepeatDistance = TextureRepeatDistance / 10.0f;

	if (nStart == 0) {
		v01 = (Outline[nEnd] - Outline[nStart]).GetSafeNormal();
		Right = (FVector::UpVector ^ v01).GetSafeNormal();
		for (int32 i = 0; i < ProfileOutline.Num(); ++i, ++nIndex) {
			FVector ProfileVert = Outline[nStart] + Right * ProfileOutline[i].X;
			ProfileVert.Z += ProfileOutline[i].Y;
			Verts[nIndex].Position = ProfileVert;
			Verts[nIndex].UV = FVector2D(0.0f, LengthBetweenOutlinePoints[i] / LocalTextureRepeatDistance);
			Verts[nIndex].Color = FColor::White;
			if (bSwapUV) {
				float Temp = Verts[nIndex].UV.X;
				Verts[nIndex].UV.X = Verts[nIndex].UV.Y;
				Verts[nIndex].UV.Y = Temp;
			}
			if (i > 0 && i < ProfileOutline.Num() - 1) {
				Verts[nIndex + 1] = Verts[nIndex];
				++nIndex;
			}
		}
	}
	else {
		v01 = (Outline[nStart - 1] - Outline[nStart]).GetSafeNormal();
		v12 = (Outline[nEnd] - Outline[nStart]).GetSafeNormal();
		v02 = (v12 - v01).GetSafeNormal();
		Right = (FVector::UpVector ^ v02).GetSafeNormal();

		float Angle = PI - FMath::Acos(FVector::DotProduct(v01, v12));

		FQuat Rot(FVector::UpVector, Angle);
		FVector RotV = Rot.RotateVector(v01);
		bStartClockwise = (RotV - v12).IsNearlyZero();

		float sinhAngle, coshAngle;
		FMath::SinCos(&sinhAngle, &coshAngle, Angle * 0.5f);

		for (int32 i = 0; i < ProfileOutline.Num(); ++i, ++nIndex) {
			float SlopeDis = ProfileOutline[i].X / coshAngle;
			FVector ProfileVert = Outline[nStart] + Right * SlopeDis;
			ProfileVert.Z += ProfileOutline[i].Y;
			Verts[nIndex].Position = Verts[nIndex - (ProfileOutline.Num() * 2 - 2)].Position;
			Verts[nIndex].UV = FVector2D(SlopeDis * sinhAngle / LocalTextureRepeatDistance * (bStartClockwise ? -1.f : 1.f), LengthBetweenOutlinePoints[i] / LocalTextureRepeatDistance);
			Verts[nIndex].Color = FColor::White;
			if (bSwapUV) {
				float Temp = Verts[nIndex].UV.X;
				Verts[nIndex].UV.X = Verts[nIndex].UV.Y;
				Verts[nIndex].UV.Y = Temp;
			}
			if (i > 0 && i < ProfileOutline.Num() - 1) {
				Verts[nIndex + 1] = Verts[nIndex];
				++nIndex;
			}
		}
	}

	if (nEnd == Outline.Num() - 1) {
		v01 = (Outline[Outline.Num() - 1] - Outline[Outline.Num() - 2]).GetSafeNormal();
		Right = (FVector::UpVector ^ v01).GetSafeNormal();
		for (int32 i = 0; i < ProfileOutline.Num(); ++i, ++nIndex) {
			FVector ProfileVert = Outline[Outline.Num() - 1] + Right * ProfileOutline[i].X;
			ProfileVert.Z += ProfileOutline[i].Y;
			Verts[nIndex].Position = ProfileVert;
			Verts[nIndex].UV = FVector2D(
				(ProfileVert - Verts[nIndex - (ProfileOutline.Num() * 2 - 2)].Position).Size() / LocalTextureRepeatDistance + Verts[nIndex - (ProfileOutline.Num() * 2 - 2)].UV.X,
				LengthBetweenOutlinePoints[i] / LocalTextureRepeatDistance);
			Verts[nIndex].Color = FColor::White;
			if (bSwapUV) {
				float Temp = Verts[nIndex].UV.X;
				Verts[nIndex].UV.X = Verts[nIndex].UV.Y;
				Verts[nIndex].UV.Y = Temp;
			}
			if (i > 0 && i < ProfileOutline.Num() - 1) {
				Verts[nIndex + 1] = Verts[nIndex];
				++nIndex;
			}
		}
	}
	else {
		v01 = (Outline[nStart] - Outline[nEnd]).GetSafeNormal();
		v12 = (Outline[nEnd + 1] - Outline[nEnd]).GetSafeNormal();
		v02 = (v12 - v01).GetSafeNormal();
		Right = (FVector::UpVector ^ v02).GetSafeNormal();

		float hAngle = (PI - FMath::Acos(FVector::DotProduct(v01, v12))) * 0.5f;
		float coshAngle = FMath::Acos(hAngle);

		for (int32 i = 0; i < ProfileOutline.Num(); ++i, ++nIndex) {
			FVector ProfileVert = Outline[nEnd] + Right * ProfileOutline[i].X / coshAngle;
			ProfileVert.Z += ProfileOutline[i].Y;
			Verts[nIndex].Position = ProfileVert;
			Verts[nIndex].UV = FVector2D(
				(ProfileVert - Verts[nIndex - (ProfileOutline.Num() * 2 - 2)].Position).Size() / LocalTextureRepeatDistance + Verts[nIndex - (ProfileOutline.Num() * 2 - 2)].UV.X,
				LengthBetweenOutlinePoints[i] / LocalTextureRepeatDistance);
			Verts[nIndex].Color = FColor::White;
			/*if (bSwapUV) {
				float Temp = Verts[nIndex].UV.X;
				Verts[nIndex].UV.X = Verts[nIndex].UV.Y;
				Verts[nIndex].UV.Y = Temp;
			}*/
			if (i > 0 && i < ProfileOutline.Num() - 1) {
				Verts[nIndex + 1] = Verts[nIndex];
				++nIndex;
			}
		}
	}
}

// @罗雕 V1.7版本临时使用
void GenerateSkitLineSegment_Split(TArray<FXRProcMeshVertex> &Verts,
    int32 &nIndex,
    const TArray<FVector> &Outline,
    int32 nStart, int32 nEnd,
    const TArray<FVector2D> &ProfileOutline,
    const TArray<float> &LengthBetweenOutlinePoints,
    bool bSwapUV)
{
    FVector v01, v02, v12, Right;
    bool bStartClockwise = true;

    float LocalTextureRepeatDistance = TextureRepeatDistance / 10.0f;

    if (Outline[0] == Outline[1])
    {
        v01 = (Outline[nEnd] - Outline[nStart]).GetSafeNormal();
        Right = (FVector::UpVector ^ v01).GetSafeNormal();
        for (int32 i = 0; i < ProfileOutline.Num(); ++i, ++nIndex) {
            FVector ProfileVert = Outline[nStart] + Right * ProfileOutline[i].X;
            ProfileVert.Z += ProfileOutline[i].Y;
            Verts[nIndex].Position = ProfileVert;
            Verts[nIndex].UV = FVector2D(0.0f, LengthBetweenOutlinePoints[i] / LocalTextureRepeatDistance);
            Verts[nIndex].Color = FColor::White;
            if (bSwapUV) {
                float Temp = Verts[nIndex].UV.X;
                Verts[nIndex].UV.X = Verts[nIndex].UV.Y;
                Verts[nIndex].UV.Y = Temp;
            }
            if (i > 0 && i < ProfileOutline.Num() - 1) {
                Verts[nIndex + 1] = Verts[nIndex];
                ++nIndex;
            }
        }
    }
    else
    {
        v01 = (Outline[nStart - 1] - Outline[nStart]).GetSafeNormal();
        v12 = (Outline[nEnd] - Outline[nStart]).GetSafeNormal();
        v02 = (v12 - v01).GetSafeNormal();
        Right = (FVector::UpVector ^ v02).GetSafeNormal();

        float Angle = PI - FMath::Acos(FVector::DotProduct(v01, v12));

        FQuat Rot(FVector::UpVector, Angle);
        FVector RotV = Rot.RotateVector(v01);
        bStartClockwise = (RotV - v12).IsNearlyZero();

        float sinhAngle, coshAngle;
        FMath::SinCos(&sinhAngle, &coshAngle, Angle * 0.5f);

        for (int32 i = 0; i < ProfileOutline.Num(); ++i, ++nIndex) {
            float SlopeDis = ProfileOutline[i].X / coshAngle;
            FVector ProfileVert = Outline[nStart] + Right * SlopeDis;
            ProfileVert.Z += ProfileOutline[i].Y;
            Verts[nIndex].Position = nStart == 1 ? ProfileVert : Verts[nIndex - (ProfileOutline.Num() * 2 - 2)].Position;
            Verts[nIndex].UV = FVector2D(SlopeDis * sinhAngle / LocalTextureRepeatDistance * (bStartClockwise ? -1.f : 1.f), LengthBetweenOutlinePoints[i] / LocalTextureRepeatDistance);
            Verts[nIndex].Color = FColor::White;
            if (bSwapUV)
            {
                float Temp = Verts[nIndex].UV.X;
                Verts[nIndex].UV.X = Verts[nIndex].UV.Y;
                Verts[nIndex].UV.Y = Temp;
            }
            if (i > 0 && i < ProfileOutline.Num() - 1)
            {
                Verts[nIndex + 1] = Verts[nIndex];
                ++nIndex;
            }
        }
    }

    if (Outline[2] == Outline[3])
    {
        v01 = (Outline[2] - Outline[1]).GetSafeNormal();
        Right = (FVector::UpVector ^ v01).GetSafeNormal();
        for (int32 i = 0; i < ProfileOutline.Num(); ++i, ++nIndex) {
            FVector ProfileVert = Outline[2] + Right * ProfileOutline[i].X;
            ProfileVert.Z += ProfileOutline[i].Y;
            Verts[nIndex].Position = ProfileVert;
            Verts[nIndex].UV = FVector2D(
                (ProfileVert - Verts[nIndex - (ProfileOutline.Num() * 2 - 2)].Position).Size() / LocalTextureRepeatDistance + Verts[nIndex - (ProfileOutline.Num() * 2 - 2)].UV.X,
                LengthBetweenOutlinePoints[i] / LocalTextureRepeatDistance);
            Verts[nIndex].Color = FColor::White;
            if (bSwapUV)
            {
                float Temp = Verts[nIndex].UV.X;
                Verts[nIndex].UV.X = Verts[nIndex].UV.Y;
                Verts[nIndex].UV.Y = Temp;
            }
            if (i > 0 && i < ProfileOutline.Num() - 1)
            {
                Verts[nIndex + 1] = Verts[nIndex];
                ++nIndex;
            }
        }
    }
    else
    {
        v01 = (Outline[nStart] - Outline[nEnd]).GetSafeNormal();
        v12 = (Outline[nEnd + 1] - Outline[nEnd]).GetSafeNormal();
        v02 = (v12 - v01).GetSafeNormal();
        Right = (FVector::UpVector ^ v02).GetSafeNormal();

        float Angle = PI - FMath::Acos(FVector::DotProduct(v01, v12));
        float sinhAngle, coshAngle;
        FMath::SinCos(&sinhAngle, &coshAngle, Angle * 0.5f);

        for (int32 i = 0; i < ProfileOutline.Num(); ++i, ++nIndex)
        {
            FVector ProfileVert = Outline[nEnd] + Right * ProfileOutline[i].X / coshAngle;
            ProfileVert.Z += ProfileOutline[i].Y;
            Verts[nIndex].Position = ProfileVert;
            Verts[nIndex].UV = FVector2D(
                (ProfileVert - Verts[nIndex - (ProfileOutline.Num() * 2 - 2)].Position).Size() / LocalTextureRepeatDistance + Verts[nIndex - (ProfileOutline.Num() * 2 - 2)].UV.X,
                LengthBetweenOutlinePoints[i] / LocalTextureRepeatDistance);
            Verts[nIndex].Color = FColor::White;
            /*if (bSwapUV) {
            float Temp = Verts[nIndex].UV.X;
            Verts[nIndex].UV.X = Verts[nIndex].UV.Y;
            Verts[nIndex].UV.Y = Temp;
            }*/
            if (i > 0 && i < ProfileOutline.Num() - 1)
            {
                Verts[nIndex + 1] = Verts[nIndex];
                ++nIndex;
            }
        }
    }
}

void GenerateProfileVerts_Split(TArray<FXRProcMeshVertex> &Verts,
	const TArray<FVector> &Outline,
	const TArray<FVector2D> &ProfileOutline,
	const TArray<float> &LengthBetweenOutlinePoints, bool bSwapUV)
{
	Verts.SetNum((Outline.Num() - 1) * (ProfileOutline.Num() * 2 - 2) * 2);

	int32 nIndex = 0;

	for (int32 i = 0; i < Outline.Num() - 1; ++i)
		GenerateSegment_Split(Verts, nIndex, Outline, i, i + 1, ProfileOutline, LengthBetweenOutlinePoints, bSwapUV);
}

void GenerateProfileIndicesTop_Split(TArray<FXRProcMeshTriangle> &Triangles, int32 nNumOutline, int32 nNumProfileOutline)
{
	int32 Stride = (nNumProfileOutline - 1) * 2;
	Triangles.SetNum((nNumProfileOutline - 1) * 2 * (nNumOutline - 1));
	for (int32 i = 0; i < nNumOutline - 1; ++i) {
		for (int32 j = 0; j < nNumProfileOutline - 1; ++j) {
			FXRProcMeshTriangle &Tri = Triangles[i * Stride + j * 2];
			/*Tri.Vertex0 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 2;
			Tri.Vertex1 = (i * 2 + 1) * (nNumProfileOutline * 2 - 2) + j * 2;
			Tri.Vertex2 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 2 + 1;*/
			Tri.Vertex0 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 4;
			Tri.Vertex1 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 4 + 1;
			Tri.Vertex2 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 4 + 2;
			FXRProcMeshTriangle &Tri1 = Triangles[i * Stride + j * 2 + 1];
			/*Tri1.Vertex0 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 2 + 1;
			Tri1.Vertex1 = (i * 2 + 1) * (nNumProfileOutline * 2 - 2) + j * 2;
			Tri1.Vertex2 = (i * 2 + 1) * (nNumProfileOutline * 2 - 2) + j * 2 + 1;*/
			Tri1.Vertex0 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 4;
			Tri1.Vertex1 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 4 + 2;
			Tri1.Vertex2 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 4 + 3;
		}
	}
}

void GenerateProfileIndicesBottom_Split(TArray<FXRProcMeshTriangle> &Triangles, int32 nNumOutline, int32 nNumProfileOutline)
{
	int32 Stride = (nNumProfileOutline - 1) * 2;
	Triangles.SetNum((nNumProfileOutline - 1) * 2 * (nNumOutline - 1));
	for (int32 i = 0; i < nNumOutline - 1; ++i) {
		for (int32 j = 0; j < nNumProfileOutline - 1; ++j) {
			FXRProcMeshTriangle &Tri = Triangles[i * Stride + j * 2];
			/*Tri.Vertex0 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 2;
			Tri.Vertex1 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 2 + 1;
			Tri.Vertex2 = (i * 2 + 1) * (nNumProfileOutline * 2 - 2) + j * 2;*/
			Tri.Vertex0 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 4;
			Tri.Vertex1 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 4 + 2;
			Tri.Vertex2 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 4 + 1;
			FXRProcMeshTriangle &Tri1 = Triangles[i * Stride + j * 2 + 1];
			/*Tri1.Vertex0 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 2 + 1;
			Tri1.Vertex1 = (i * 2 + 1) * (nNumProfileOutline * 2 - 2) + j * 2 + 1;
			Tri1.Vertex2 = (i * 2 + 1) * (nNumProfileOutline * 2 - 2) + j * 2;*/
			Tri1.Vertex0 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 4;
			Tri1.Vertex1 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 4 + 3;
			Tri1.Vertex2 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 4 + 2;
		}
	}
}

void GeneratedSegmentClosed_Splitted(
	TArray<FXRProcMeshVertex> &Verts,
	int32 &nIndex,
	const TArray<FVector> &Outline,
	int32 nStart, int32 nEnd,
	const TArray<FVector2D> &ProfileOutline,
	const TArray<float> &LengthBetweenOutlinePoints)
{
	float LocalTextureRepeatDistance = TextureRepeatDistance / 10.0f;

	FVector v01, v02, v12, Right;
	bool bStartClockwise = true;

	int32 nLastBeforeStart = nStart == 0 ? Outline.Num() - 1 : nStart - 1;
	int32 nNextAfterEnd = nEnd == Outline.Num() - 1 ? 0 : nEnd + 1;

	v01 = (Outline[nLastBeforeStart] - Outline[nStart]).GetSafeNormal();
	v12 = (Outline[nEnd] - Outline[nStart]).GetSafeNormal();
	v02 = (v12 - v01).GetSafeNormal();
	Right = (FVector::UpVector ^ v02).GetSafeNormal();

	float Angle = PI - FMath::Acos(FVector::DotProduct(v01, v12));

	FQuat Rot(FVector::UpVector, Angle);
	FVector RotV = Rot.RotateVector(v01);
	bStartClockwise = (RotV - v12).IsNearlyZero();

	float sinhAngle, coshAngle;
	FMath::SinCos(&sinhAngle, &coshAngle, Angle * 0.5f);
	for (int32 i = 0; i < ProfileOutline.Num(); ++i, ++nIndex) {
		float SlopeDis = ProfileOutline[i].X / coshAngle;
		FVector ProfileVert = Outline[nStart] + Right * SlopeDis;
		ProfileVert.Z += ProfileOutline[i].Y;
		Verts[nIndex].Position = nStart == 0 ? ProfileVert : Verts[nIndex - (ProfileOutline.Num() * 2 - 2)].Position;
		Verts[nIndex].UV = FVector2D(SlopeDis * sinhAngle / LocalTextureRepeatDistance * (bStartClockwise ? -1.f : 1.f), LengthBetweenOutlinePoints[i] / LocalTextureRepeatDistance);
		Verts[nIndex].Color = FColor::White;
		if (i > 0 && i < ProfileOutline.Num() - 1) {
			Verts[nIndex + 1] = Verts[nIndex];
			++nIndex;
		}
	}



	v01 = (Outline[nStart] - Outline[nEnd]).GetSafeNormal();
	v12 = (Outline[nNextAfterEnd] - Outline[nEnd]).GetSafeNormal();
	v02 = (v12 - v01).GetSafeNormal();
	Right = (FVector::UpVector ^ v02).GetSafeNormal();

	Angle = (PI - FMath::Acos(FVector::DotProduct(v01, v12))) * 0.5f;
	coshAngle = FMath::Cos(Angle);

	for (int32 i = 0; i < ProfileOutline.Num(); ++i, ++nIndex) {
		FVector ProfileVert = Outline[nEnd] + Right * ProfileOutline[i].X / coshAngle;
		ProfileVert.Z += ProfileOutline[i].Y;
		Verts[nIndex].Position = ProfileVert;
		Verts[nIndex].UV = FVector2D(
			(ProfileVert - Verts[nIndex - (ProfileOutline.Num() * 2 - 2)].Position).Size() / LocalTextureRepeatDistance + Verts[nIndex - (ProfileOutline.Num() * 2 - 2)].UV.X,
			LengthBetweenOutlinePoints[i] / LocalTextureRepeatDistance);
		Verts[nIndex].Color = FColor::White;

		if (i > 0 && i < ProfileOutline.Num() - 1) {
			Verts[nIndex + 1] = Verts[nIndex];
			++nIndex;
		}
	}
}

// @罗雕 V1.7版本临时使用
void GeneratedSkitLineSegmentClosed_Splitted(
    TArray<FXRProcMeshVertex> &Verts,
    int32 &nIndex,
    const TArray<FVector> &Outline,
    int32 nStart, int32 nEnd,
    const TArray<FVector2D> &ProfileOutline,
    const TArray<float> &LengthBetweenOutlinePoints)
{
    float LocalTextureRepeatDistance = TextureRepeatDistance / 10.0f;

    FVector v01, v02, v12, Right;
    bool bStartClockwise = true;

    int32 nLastBeforeStart = 0;
    int32 nNextAfterEnd = 3;

    v01 = (Outline[nLastBeforeStart] - Outline[nStart]).GetSafeNormal();
    v12 = (Outline[nEnd] - Outline[nStart]).GetSafeNormal();
    v02 = (v12 - v01).GetSafeNormal();
    Right = (FVector::UpVector ^ v02).GetSafeNormal();

    float Angle = PI - FMath::Acos(FVector::DotProduct(v01, v12));

    FQuat Rot(FVector::UpVector, Angle);
    FVector RotV = Rot.RotateVector(v01);
    bStartClockwise = (RotV - v12).IsNearlyZero();

    float sinhAngle, coshAngle;
    FMath::SinCos(&sinhAngle, &coshAngle, Angle * 0.5f);
    for (int32 i = 0; i < ProfileOutline.Num(); ++i, ++nIndex) {
        float SlopeDis = ProfileOutline[i].X / coshAngle;
        FVector ProfileVert = Outline[nStart] + Right * SlopeDis;
        ProfileVert.Z += ProfileOutline[i].Y;
        Verts[nIndex].Position = nStart == 1 ? ProfileVert : Verts[nIndex - (ProfileOutline.Num() * 2 - 2)].Position;
        Verts[nIndex].UV = FVector2D(SlopeDis * sinhAngle / LocalTextureRepeatDistance * (bStartClockwise ? -1.f : 1.f), LengthBetweenOutlinePoints[i] / LocalTextureRepeatDistance);
        Verts[nIndex].Color = FColor::White;
        if (i > 0 && i < ProfileOutline.Num() - 1) {
            Verts[nIndex + 1] = Verts[nIndex];
            ++nIndex;
        }
    }



    v01 = (Outline[nStart] - Outline[nEnd]).GetSafeNormal();
    v12 = (Outline[nNextAfterEnd] - Outline[nEnd]).GetSafeNormal();
    v02 = (v12 - v01).GetSafeNormal();
    Right = (FVector::UpVector ^ v02).GetSafeNormal();

    Angle = (PI - FMath::Acos(FVector::DotProduct(v01, v12))) * 0.5f;
    coshAngle = FMath::Cos(Angle);

    for (int32 i = 0; i < ProfileOutline.Num(); ++i, ++nIndex) {
        FVector ProfileVert = Outline[nEnd] + Right * ProfileOutline[i].X / coshAngle;
        ProfileVert.Z += ProfileOutline[i].Y;
        Verts[nIndex].Position = ProfileVert;
        Verts[nIndex].UV = FVector2D(
            (ProfileVert - Verts[nIndex - (ProfileOutline.Num() * 2 - 2)].Position).Size() / LocalTextureRepeatDistance + Verts[nIndex - (ProfileOutline.Num() * 2 - 2)].UV.X,
            LengthBetweenOutlinePoints[i] / LocalTextureRepeatDistance);
        Verts[nIndex].Color = FColor::White;

        if (i > 0 && i < ProfileOutline.Num() - 1) {
            Verts[nIndex + 1] = Verts[nIndex];
            ++nIndex;
        }
    }
}

void GenerateProfileVertsClosed_Split(TArray<FXRProcMeshVertex> &Verts,
	const TArray<FVector> &Outline,
	const TArray<FVector2D> &ProfileOutline,
	const TArray<float> &LengthBetweenOutlinePoints)
{
	Verts.SetNum(Outline.Num() * (ProfileOutline.Num() * 2 - 2) * 2);

	int32 nIndex = 0;

	for (int32 i = 0; i < Outline.Num() - 1; ++i)
		GeneratedSegmentClosed_Splitted(Verts, nIndex, Outline, i, i + 1, ProfileOutline, LengthBetweenOutlinePoints);
	GeneratedSegmentClosed_Splitted(Verts, nIndex, Outline, Outline.Num() - 1, 0, ProfileOutline, LengthBetweenOutlinePoints);
}

void GenerateProfileIndicesTopClosed_Split(TArray<FXRProcMeshTriangle> &Triangles, int32 nNumOutline, int32 nNumProfileOutline)
{
	int32 Stride = (nNumProfileOutline - 1) * 2;
	Triangles.SetNum((nNumProfileOutline - 1) * 2 * nNumOutline);
	for (int32 i = 0; i < nNumOutline; ++i) {
		for (int32 j = 0; j < nNumProfileOutline - 1; ++j) {
			FXRProcMeshTriangle &Tri = Triangles[i * Stride + j * 2];
			Tri.Vertex0 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 2;
			Tri.Vertex1 = (i * 2 + 1) * (nNumProfileOutline * 2 - 2) + j * 2;
			Tri.Vertex2 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 2 + 1;
			FXRProcMeshTriangle &Tri1 = Triangles[i * Stride + j * 2 + 1];
			Tri1.Vertex0 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 2 + 1;
			Tri1.Vertex1 = (i * 2 + 1) * (nNumProfileOutline * 2 - 2) + j * 2;
			Tri1.Vertex2 = (i * 2 + 1) * (nNumProfileOutline * 2 - 2) + j * 2 + 1;
		}
	}
}

void GenerateProfileIndicesBottomClosed_Split(TArray<FXRProcMeshTriangle> &Triangles, int32 nNumOutline, int32 nNumProfileOutline)
{
	int32 Stride = (nNumProfileOutline - 1) * 2;
	Triangles.AddUninitialized((nNumProfileOutline - 1) * 2 * nNumOutline);
	for (int32 i = 0; i < nNumOutline; ++i) {
		for (int32 j = 0; j < nNumProfileOutline - 1; ++j) {
			FXRProcMeshTriangle &Tri = Triangles[i * Stride + j * 2];
			Tri.Vertex0 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 2;
			Tri.Vertex1 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 2 + 1;
			Tri.Vertex2 = (i * 2 + 1) * (nNumProfileOutline * 2 - 2) + j * 2;
			FXRProcMeshTriangle &Tri1 = Triangles[i * Stride + j * 2 + 1];
			Tri1.Vertex0 = i * 2 * (nNumProfileOutline * 2 - 2) + j * 2 + 1;
			Tri1.Vertex1 = (i * 2 + 1) * (nNumProfileOutline * 2 - 2) + j * 2 + 1;
			Tri1.Vertex2 = (i * 2 + 1) * (nNumProfileOutline * 2 - 2) + j * 2;
		}
	}
}


AArmyExtrusionActor::AArmyExtrusionActor() {
	PrimaryActorTick.bCanEverTick = false;

	//static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("Material'/Game/Common/Material/Board_Standard_01.Board_Standard_01'"));

	UMaterialInstance* WallMaterialInstance = FXREngineModule::Get().GetEngineResource()->GetBaseboardMaterial();
	MeshComponent = CreateDefaultSubobject<UXRProceduralMeshComponent>(TEXT("ExtrusionComponent"));
	MeshComponent->SetMaterial(0, WallMaterialInstance);
	MeshComponent->BSkitLine = true;
	RootComponent = MeshComponent;

	Circumference = 0;
	Length = 0;
}

void AArmyExtrusionActor::BeginPlay() {
	Super::BeginPlay();
}

void AArmyExtrusionActor::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
}

void AArmyExtrusionActor::SetMaterial(UMaterialInterface* InMaterial)
{
	MeshComponent->SetMaterial(0, InMaterial);
}

FGuid AArmyExtrusionActor::GetLightMapGUIDFromSMC()
{
	UStaticMeshComponent* SMC = MeshComponent->TempConvertedMeshComponentForBuildingLight;
	if (SMC && SMC->LODData.IsValidIndex(0))
	{
		const FStaticMeshComponentLODInfo& ComponentLODInfo = SMC->LODData[0];
		return ComponentLODInfo.MapBuildDataId;
	}
	return GetLightMapGUIDFromAttachSurface();
}

FGuid AArmyExtrusionActor::GetLightMapGUIDFromAttachSurface()
{
	if (AttachRoomSpaceArea.IsValid())
	{
		return AttachRoomSpaceArea.Pin()->GetExtrusionLightMapID(this);
	}
	else if (AttachSurfaceArea.IsValid())
	{
		return AttachSurfaceArea.Pin()->GetExtrusionLightMapID();
	}
	else if (AttachHardware.IsValid())
	{
		return AttachHardware.Pin()->LightMapID;
	}
	return FGuid();
}

FVector2D AArmyExtrusionActor::CalculateMissingVertex(FVector2D InA, FVector2D InB, bool bReverse)
{
    FVector2D Result = FVector2D::ZeroVector;
    Result.X = FMath::Min<float>(InA.X, InB.X);
    Result.Y = bReverse ? FMath::Max<float>(InA.Y, InB.Y) : FMath::Min<float>(InA.Y, InB.Y);

    return Result;
}

void AArmyExtrusionActor::GenerateMeshFromFilePath(
    const FString& FilePath,
    const TArray<FVector>& TrackPoints,
    bool bReverse/* = false*/,
    bool bIsClose/* = true*/,
    bool bNeedFillMissingVertext/* = false*/,
    FVector planeXDir/* = FVector(1, 0, 0)*/,
    FVector planeYDir/* = FVector(0, 1, 0)*/,
    FVector planeNormal/* = FVector(0, 0, 1)*/,
    FVector planeCenter/* = FVector::ZeroVector*/)
{
	//@梁晓菲 求周长和施工长度
    if (bIsClose)
    {
        Circumference = FXRMath::CircumferenceOfPointList(TrackPoints, true);
    }
    else
    {
        Circumference = FXRMath::CircumferenceOfPointList(TrackPoints, false);
    }

	TArray<FVector2D> InternalList;
	using namespace std;
	std::string FilePaths(TCHAR_TO_UTF8(*FilePath));
	dxfFile DxfFile(FilePaths);
	TSharedPtr<FXRDxfModel> DxfModel = NULL;
	if (DxfFile.parseFile())
	{
		DxfModel = MakeShareable(DxfFile.DxfToHomeEditorData());
		TArray<TSharedPtr<FXRFurniture>> TempObjs = DxfModel->GetAllLayers();
		if (TempObjs.Num() > 0)
		{
			TSharedPtr<FXRFurniture> SelectedComponent = TempObjs[0];
			TArray<TSharedPtr<FXRPolyline>> Polys = SelectedComponent->GetPolys();
			TArray<FVector> points = Polys[0]->GetAllVertices();
			float height = 0.0f;
			if (bReverse)
			{
				FBox box(points);
				height = box.GetSize().Y;
			}
			for (auto & It : points)
			{
				InternalList.Push(FVector2D(It.X, It.Y - height));
			}
			if (FXRMath::IsClockWise(InternalList))
				FXRMath::ReverPointList(InternalList);
			if (SelectedComponent->GetBaseEditPoint().IsValid())
			{
				FVector2D centerPos = FVector2D(SelectedComponent->GetPivortPoint());
				for (FVector2D& iter : InternalList)
				{
					iter -= centerPos;
				}
			}
		}
	}

    if (InternalList.Num() > 0)
    {
        GenerateMeshWithOutlineData(InternalList, TrackPoints, bReverse, bIsClose, bNeedFillMissingVertext, planeXDir, planeYDir, planeNormal, planeCenter);
    }
}

void AArmyExtrusionActor::GenerateSkitLineMeshFromFilePath(const FString& FilePath, const TArray<FVector>& TrackPoints, bool bReverse /*= false*/, bool bIsClose /*= true*/, bool bNeedFillMissingVertext /*= false*/, FVector planeXDir /*= FVector(1, 0, 0)*/, FVector planeYDir /*= FVector(0, 1, 0)*/, FVector planeNormal /*= FVector(0, 0, 1)*/, FVector planeCenter /*= FVector::ZeroVector*/)
{
    //@梁晓菲 求周长和施工长度
    Circumference = (TrackPoints[2] - TrackPoints[1]).Size();

    TArray<FVector2D> InternalList;
    using namespace std;
    std::string FilePaths(TCHAR_TO_UTF8(*FilePath));
    dxfFile DxfFile(FilePaths);
    TSharedPtr<FXRDxfModel> DxfModel = NULL;
    if (DxfFile.parseFile())
    {
        DxfModel = MakeShareable(DxfFile.DxfToHomeEditorData());
        TArray<TSharedPtr<FXRFurniture>> TempObjs = DxfModel->GetAllLayers();
        if (TempObjs.Num() > 0)
        {
            TSharedPtr<FXRFurniture> SelectedComponent = TempObjs[0];
            TArray<TSharedPtr<FXRPolyline>> Polys = SelectedComponent->GetPolys();
            TArray<FVector> points = Polys[0]->GetAllVertices();
            float height = 0.0f;
            if (bReverse)
            {
                FBox box(points);
                height = box.GetSize().Y;
            }
            for (auto & It : points)
            {
                InternalList.Push(FVector2D(It.X, It.Y - height));
            }
            if (FXRMath::IsClockWise(InternalList))
                FXRMath::ReverPointList(InternalList);
            if (SelectedComponent->GetBaseEditPoint().IsValid())
            {
                FVector2D centerPos = FVector2D(SelectedComponent->GetPivortPoint());
                for (FVector2D& iter : InternalList)
                {
                    iter -= centerPos;
                }
            }
        }
    }

    if (InternalList.Num() > 0)
    {
        GenerateSkitLineMeshWithOutlineData(InternalList, TrackPoints, bReverse, bIsClose, bNeedFillMissingVertext, planeXDir, planeYDir, planeNormal, planeCenter);
    }
}

void AArmyExtrusionActor::GeneratePassMeshFromFilePath(const FString& FilePath, const TArray<FVector>& TrackPoints, float passWidth, bool bClose /*= false*/, FVector planeXDir /*= FVector(1, 0, 0)*/, FVector planeYDir /*= FVector(0, 1, 0)*/, FVector planeNormal /*= FVector(0, 0, 1)*/, FVector planeCenter /*= FVector::ZeroVector*/)
{
	TArray<FVector2D> InternalList;
	using namespace std;
	std::string FilePaths(TCHAR_TO_UTF8(*FilePath));
	dxfFile DxfFile(FilePaths);
	TSharedPtr<FXRDxfModel> DxfModel = NULL;
	if (DxfFile.parseFile())
	{
		DxfModel = MakeShareable(DxfFile.DxfToHomeEditorData());
		TArray<TSharedPtr<FXRFurniture>> TempObjs = DxfModel->GetAllLayers();
		if (TempObjs.Num() > 0)
		{
			TSharedPtr<FXRFurniture> SelectedComponent = TempObjs[0];
			TArray<TSharedPtr<FXRPolyline>> Polys = SelectedComponent->GetPolys();
			TArray<FVector> points = Polys[0]->GetAllVertices();
			float height = 0.0f;
			if (passWidth / 2 - 6.0f > 0)
			{
				height = passWidth / 2 - 6.0f;
			}
			FVector minPos = DxfModel->boundingBox.Min;
			for (auto & It : points)
			{
				if (It.Y > 0)
				{
					InternalList.Push(FVector2D(It.X - 7, It.Y + height));

				}
				else
				{
					InternalList.Push(FVector2D(It.X - 7, It.Y));
				}

			}

			int number = points.Num();
			InternalList.RemoveAt(number - 1);
			for (int i = number - 3; i >= 1; i--)
			{
				InternalList.Push(FVector2D(points[i].X - 7, -points[i].Y - height));
			}
		}
	}
	if (InternalList.Num() == 0)
		return;
	GenerateMeshWithOutlineData(InternalList, TrackPoints, false, bClose, false, planeXDir, planeYDir, planeNormal, planeCenter);
}



void AArmyExtrusionActor::GenerateMeshWithOutlineData(
    const TArray<FVector2D> &ProfileOutline,
    const TArray<FVector> &TrackPoints,
    bool bReverse,
    bool bIsClosed,
    bool bNeedFillMissingVertext,
    FVector planeXDir,
    FVector planeYDir,
    FVector planeNormal,
    FVector planeCenter)
{
    ProfileVertexes.Empty();
    ProfileVertexes = ProfileOutline;

	bool bOnTop = false;
	bool bSwapUV = false;

	if (TrackPoints.Num() < 2)
		return;
	if (bIsClosed && TrackPoints.Num() < 3)
		return;

	//@马云龙 临时
	/*TArray<FVector> TempTrackPoints = TrackPoints;
	FVector Start = TempTrackPoints[0];
	FVector End = TempTrackPoints[TempTrackPoints.Num()-1];
	float Length = (End - Start).Size();
	int32 SegmentNum = FMath::RoundToInt(Length / 200);
	FVector Dir = End - Start;
	Dir.Normalize();
	TempTrackPoints.Empty();
	TempTrackPoints.Push(Start);
	for (int32 i =0;i<SegmentNum-1;i++)
	{
		TempTrackPoints.Push(Start + Dir*200*(i+1));
	}
	TempTrackPoints.Push(End);*/


	TArray<float> LengthBetweenOutlinePoints;
	LengthBetweenOutlinePoints.AddUninitialized(ProfileOutline.Num());
	LengthBetweenOutlinePoints[0] = 0.0f;
	for (int32 i = 1; i < ProfileOutline.Num(); ++i)
		LengthBetweenOutlinePoints[i] = (ProfileOutline[i] - ProfileOutline[i - 1]).Size() + LengthBetweenOutlinePoints[i - 1];

	TArray<FXRProcMeshVertex> Verts;
	TArray<FXRProcMeshTriangle> Triangles;

	int32 Stride = (ProfileOutline.Num() - 1) * 2;
	if (bIsClosed) {

		GenerateProfileVertsClosed_Split(Verts, TrackPoints, ProfileOutline, LengthBetweenOutlinePoints);

		if (bOnTop)
			GenerateProfileIndicesTopClosed_Split(Triangles, TrackPoints.Num(), ProfileOutline.Num());
		else
			GenerateProfileIndicesBottomClosed_Split(Triangles, TrackPoints.Num(), ProfileOutline.Num());
		for (auto& iter : Verts)
		{
			iter.Position = iter.Position.X * planeXDir + iter.Position.Y * planeYDir + iter.Position.Z * planeNormal + planeCenter;
		}
	}
	else {
		GenerateProfileVerts_Split(Verts, TrackPoints, ProfileOutline, LengthBetweenOutlinePoints, bSwapUV);

		TArray<FXRProcMeshVertex> RegroupedVerts;
		RegroupedVerts.SetNum((TrackPoints.Num() - 1) * (ProfileOutline.Num() * 2 - 2) * 2);
		int32 RegroupIndex = 0;
		for (int32 Seg = 0; Seg < TrackPoints.Num() - 1; ++Seg) {
			for (int32 ProfileSeg = 0; ProfileSeg < ProfileOutline.Num() - 1; ++ProfileSeg) {
				RegroupedVerts[RegroupIndex++] = Verts[Stride * 2 * Seg + ProfileSeg * 2];
				RegroupedVerts[RegroupIndex++] = Verts[Stride * 2 * Seg + Stride + ProfileSeg * 2];
				RegroupedVerts[RegroupIndex++] = Verts[Stride * 2 * Seg + Stride + ProfileSeg * 2 + 1];
				RegroupedVerts[RegroupIndex++] = Verts[Stride * 2 * Seg + ProfileSeg * 2 + 1];
			}
		}

		if (bOnTop)
			GenerateProfileIndicesTop_Split(Triangles, TrackPoints.Num(), ProfileOutline.Num());
		else
			GenerateProfileIndicesBottom_Split(Triangles, TrackPoints.Num(), ProfileOutline.Num());


		for (auto& iter : RegroupedVerts)
		{
			iter.Position = iter.Position.X * planeXDir + iter.Position.Y * planeYDir + iter.Position.Z * planeNormal + planeCenter;
		}


		TArray<FVector2D> TempProfileOutline = ProfileOutline;

		if (bNeedFillMissingVertext && TempProfileOutline.IsValidIndex(0) && TempProfileOutline.IsValidIndex(TempProfileOutline.Num() - 1))
		{
			FVector2D MissingOne = CalculateMissingVertex(TempProfileOutline[0], TempProfileOutline[TempProfileOutline.Num() - 1], bReverse);
			TempProfileOutline.Push(MissingOne);
		}

		int32 VertOffset = Verts.Num();

		TArray<int32> IndexList;

		TArray<FVector> SidePoints;
		//// 左侧面
		FVector v01 = (TrackPoints[1] - TrackPoints[0]).GetSafeNormal();
		FVector Right = (FVector::UpVector ^ v01).GetSafeNormal();

		Translate::Triangle(TempProfileOutline, Right, TrackPoints[0], IndexList, SidePoints);

		FXRProcMeshTriangle Triangle;
		for (int32 i = 0; i < SidePoints.Num(); ++i)
		{
			FXRProcMeshVertex Vertex;
			Vertex.Position = SidePoints[i];

			Vertex.Position = Vertex.Position.X * planeXDir + Vertex.Position.Y * planeYDir + Vertex.Position.Z * planeNormal + planeCenter;

			float UCoord = FVector::DotProduct(Vertex.Position - TrackPoints[0], Right);
			float VCoord = FVector::DotProduct(Vertex.Position - TrackPoints[0], FVector(0, 0, 1));
			Vertex.UV = FVector2D(UCoord, VCoord);
			Vertex.Color = FColor::White;
			RegroupedVerts.Push(Vertex);
			if (i % 3 == 0)
			{
				int index = i / 3;
				Triangle.Vertex0 = index * 3 + VertOffset;
				Triangle.Vertex1 = index * 3 + 1 + VertOffset;
				Triangle.Vertex2 = index * 3 + 2 + VertOffset;
				Triangles.Push(Triangle);
			}

		}

		VertOffset = RegroupedVerts.Num();

		// 右侧面
		v01 = (TrackPoints[TrackPoints.Num() - 1] - TrackPoints[TrackPoints.Num() - 2]).GetSafeNormal();
		Right = (FVector::UpVector ^ v01).GetSafeNormal();
		{
			IndexList.Empty();
			SidePoints.Empty();
			Translate::Triangle(TempProfileOutline, Right, TrackPoints[TrackPoints.Num() - 1], IndexList, SidePoints);
			FXRProcMeshTriangle Triangle;
			for (int32 i = 0; i < SidePoints.Num(); ++i) {
				FXRProcMeshVertex Vertex;
				Vertex.Position = SidePoints[i];
				Vertex.Position = Vertex.Position.X * planeXDir + Vertex.Position.Y * planeYDir + Vertex.Position.Z * planeNormal + planeCenter;
				Vertex.Color = FColor::White;
				float UCoord = FVector::DotProduct(Vertex.Position - TrackPoints[TrackPoints.Num() - 1], Right);
				float VCoord = FVector::DotProduct(Vertex.Position - TrackPoints[TrackPoints.Num() - 1], FVector(0, 0, 1));
				Vertex.UV = FVector2D(UCoord, VCoord);
				RegroupedVerts.Push(Vertex);
				if (i % 3 == 0)
				{
					int index = i / 3;
					Triangle.Vertex0 = index * 3 + VertOffset;
					Triangle.Vertex1 = index * 3 + 2 + VertOffset;
					Triangle.Vertex2 = index * 3 + 1 + VertOffset;
					Triangles.Push(Triangle);
				}
			}
		}

		Verts = RegroupedVerts;
	}
	/* @梁晓菲 解决崩溃*/
	if (!RootComponent->IsValidLowLevel())
	{
		return;
	}

	MeshComponent->CreateMeshSection(0, Verts, Triangles, true, true);
}

void AArmyExtrusionActor::GenerateSkitLineMeshWithOutlineData(
    const TArray<FVector2D> &ProfileOutline,
    const TArray<FVector> &TrackPoints,
    bool bReverse,
    bool bIsClosed /*= false*/,
    bool bNeedFillMissingVertext /*= false*/,
    FVector planeXDir /*= FVector(1, 0, 0)*/,
    FVector planeYDir /*= FVector(0, 1, 0)*/,
    FVector planeNormal /*= FVector(0, 0, 1)*/,
    FVector planeCenter /*= FVector::ZeroVector*/)
{
    ProfileVertexes.Empty();
    ProfileVertexes = ProfileOutline;

    bool bOnTop = false;
	bool bSwapUV = false;

    if ((TrackPoints.Num() < 2) || (bIsClosed && TrackPoints.Num() < 3))
    {
		return;
    }

	TArray<float> LengthBetweenOutlinePoints;
	LengthBetweenOutlinePoints.AddUninitialized(ProfileOutline.Num());
	LengthBetweenOutlinePoints[0] = 0.0f;
    for (int32 i = 1; i < ProfileOutline.Num(); ++i)
    {
		LengthBetweenOutlinePoints[i] = (ProfileOutline[i] - ProfileOutline[i - 1]).Size() + LengthBetweenOutlinePoints[i - 1];
    }

	TArray<FXRProcMeshVertex> Verts;
	TArray<FXRProcMeshTriangle> Triangles;

	int32 Stride = (ProfileOutline.Num() - 1) * 2;
	if (bIsClosed)
    {
        Verts.SetNum((ProfileOutline.Num() * 2 - 2) * 2);
        int32 nIndex = 0;
        GeneratedSkitLineSegmentClosed_Splitted(Verts, nIndex, TrackPoints, 1, 2, ProfileOutline, LengthBetweenOutlinePoints);

        int32 nNumProfileOutline = ProfileOutline.Num();
        int32 Stride = (nNumProfileOutline - 1) * 2;
        Triangles.AddUninitialized((nNumProfileOutline - 1) * 2);
        for (int32 j = 0; j < nNumProfileOutline - 1; ++j)
        {
            FXRProcMeshTriangle &Tri = Triangles[j * 2];
            Tri.Vertex0 = j * 2;
            Tri.Vertex1 = j * 2 + 1;
            Tri.Vertex2 = (nNumProfileOutline * 2 - 2) + j * 2;
            FXRProcMeshTriangle &Tri1 = Triangles[j * 2 + 1];
            Tri1.Vertex0 = j * 2 + 1;
            Tri1.Vertex1 = (nNumProfileOutline * 2 - 2) + j * 2 + 1;
            Tri1.Vertex2 = (nNumProfileOutline * 2 - 2) + j * 2;
        }

		for (auto& iter : Verts)
		{
			iter.Position = iter.Position.X * planeXDir + iter.Position.Y * planeYDir + iter.Position.Z * planeNormal + planeCenter;
		}
	}
	else {
        Verts.SetNum((ProfileOutline.Num() * 2 - 2) * 2);
        int32 nIndex = 0;
        int32 TempIndex = TrackPoints.Num() == 3 ? 0 : 1;
        GenerateSkitLineSegment_Split(Verts, nIndex, TrackPoints, TempIndex, TempIndex + 1, ProfileOutline, LengthBetweenOutlinePoints, bSwapUV);

		TArray<FXRProcMeshVertex> RegroupedVerts;
		RegroupedVerts.SetNum((ProfileOutline.Num() * 2 - 2) * 2);
		int32 RegroupIndex = 0;
        for (int32 ProfileSeg = 0; ProfileSeg < ProfileOutline.Num() - 1; ++ProfileSeg)
        {
            RegroupedVerts[RegroupIndex++] = Verts[ProfileSeg * 2];
            RegroupedVerts[RegroupIndex++] = Verts[Stride + ProfileSeg * 2];
            RegroupedVerts[RegroupIndex++] = Verts[Stride + ProfileSeg * 2 + 1];
            RegroupedVerts[RegroupIndex++] = Verts[ProfileSeg * 2 + 1];
        }

        int32 nNumProfileOutline = ProfileOutline.Num();
        int32 Stride = (nNumProfileOutline - 1) * 2;
        Triangles.SetNum((nNumProfileOutline - 1) * 2);
        for (int32 j = 0; j < nNumProfileOutline - 1; ++j)
        {
            FXRProcMeshTriangle &Tri = Triangles[j * 2];
            Tri.Vertex0 = j * 4;
            Tri.Vertex1 = j * 4 + 2;
            Tri.Vertex2 = j * 4 + 1;
            FXRProcMeshTriangle &Tri1 = Triangles[j * 2 + 1];
            Tri1.Vertex0 = j * 4;
            Tri1.Vertex1 = j * 4 + 3;
            Tri1.Vertex2 = j * 4 + 2;
        }

		for (auto& iter : RegroupedVerts)
		{
			iter.Position = iter.Position.X * planeXDir + iter.Position.Y * planeYDir + iter.Position.Z * planeNormal + planeCenter;
		}

		TArray<FVector2D> TempProfileOutline = ProfileOutline;

		if (bNeedFillMissingVertext && TempProfileOutline.IsValidIndex(0) && TempProfileOutline.IsValidIndex(TempProfileOutline.Num() - 1))
		{
			FVector2D MissingOne = CalculateMissingVertex(TempProfileOutline[0], TempProfileOutline[TempProfileOutline.Num() - 1], bReverse);
			TempProfileOutline.Push(MissingOne);
		}

		int32 VertOffset = Verts.Num();

		TArray<int32> IndexList;

		TArray<FVector> SidePoints;

        if (TrackPoints[0] == TrackPoints[1])
        {
            //// 左侧面
            FVector v01 = (TrackPoints[2] - TrackPoints[1]).GetSafeNormal();
            FVector Right = (FVector::UpVector ^ v01).GetSafeNormal();

            Translate::Triangle(TempProfileOutline, Right, TrackPoints[1], IndexList, SidePoints);

            FXRProcMeshTriangle Triangle;
            for (int32 i = 0; i < SidePoints.Num(); ++i)
            {
                FXRProcMeshVertex Vertex;
                Vertex.Position = SidePoints[i];

                Vertex.Position = Vertex.Position.X * planeXDir + Vertex.Position.Y * planeYDir + Vertex.Position.Z * planeNormal + planeCenter;

                float UCoord = FVector::DotProduct(Vertex.Position - TrackPoints[1], Right);
                float VCoord = FVector::DotProduct(Vertex.Position - TrackPoints[1], FVector(0, 0, 1));
                Vertex.UV = FVector2D(UCoord, VCoord);
                Vertex.Color = FColor::White;
                RegroupedVerts.Push(Vertex);
                if (i % 3 == 0)
                {
                    int index = i / 3;
                    Triangle.Vertex0 = index * 3 + VertOffset;
                    Triangle.Vertex1 = index * 3 + 1 + VertOffset;
                    Triangle.Vertex2 = index * 3 + 2 + VertOffset;
                    Triangles.Push(Triangle);
                }
            }
        }
		
        FVector v01, Right;
        if (TrackPoints[2] == TrackPoints[3])
        {
            VertOffset = RegroupedVerts.Num();

            // 右侧面
            v01 = (TrackPoints[TrackPoints.Num() - 2] - TrackPoints[TrackPoints.Num() - 3]).GetSafeNormal();
            Right = (FVector::UpVector ^ v01).GetSafeNormal();
            {
                IndexList.Empty();
                SidePoints.Empty();
                Translate::Triangle(TempProfileOutline, Right, TrackPoints[TrackPoints.Num() - 2], IndexList, SidePoints);
                FXRProcMeshTriangle Triangle;
                for (int32 i = 0; i < SidePoints.Num(); ++i) {
                    FXRProcMeshVertex Vertex;
                    Vertex.Position = SidePoints[i];
                    Vertex.Position = Vertex.Position.X * planeXDir + Vertex.Position.Y * planeYDir + Vertex.Position.Z * planeNormal + planeCenter;
                    Vertex.Color = FColor::White;
                    float UCoord = FVector::DotProduct(Vertex.Position - TrackPoints[TrackPoints.Num() - 2], Right);
                    float VCoord = FVector::DotProduct(Vertex.Position - TrackPoints[TrackPoints.Num() - 2], FVector(0, 0, 1));
                    Vertex.UV = FVector2D(UCoord, VCoord);
                    RegroupedVerts.Push(Vertex);
                    if (i % 3 == 0)
                    {
                        int index = i / 3;
                        Triangle.Vertex0 = index * 3 + VertOffset;
                        Triangle.Vertex1 = index * 3 + 2 + VertOffset;
                        Triangle.Vertex2 = index * 3 + 1 + VertOffset;
                        Triangles.Push(Triangle);
                    }
                }
            }
        }

        Verts = RegroupedVerts;
	}
	/* @梁晓菲 解决崩溃*/
	if (!RootComponent->IsValidLowLevel())
	{
		return;
	}

	MeshComponent->CreateMeshSection(0, Verts, Triangles, true, true);
}

void AArmyExtrusionActor::ReplaceTexture(UTexture *ColorMap, UTexture *NormalMap) {
	UMaterialInterface *MtlInterface = MeshComponent->GetMaterial(0);
	UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(MtlInterface);
	if (MID == nullptr)
		MID = UMaterialInstanceDynamic::Create(MtlInterface, GetWorld()->GetCurrentLevel());
	MID->SetScalarParameterValue(TEXT("Color_UseTexture"), 1.f);
	MID->SetTextureParameterValue(TEXT("Color_Map"), ColorMap);
	MID->SetScalarParameterValue(TEXT("Normal_UseTexture"), 1.f);
	MID->SetTextureParameterValue(TEXT("Normal_Map"), NormalMap);

	MeshComponent->SetMaterial(0, MID);
}

ConstructionPatameters AArmyExtrusionActor::GetConstructionParameter()
{
	ConstructionPatameters P;

	int32 SpaceID = -1;
	auto Room = GetRoom();
	if (Room.IsValid())
	{
		SpaceID = Room->GetSpaceId();
	}

	P.SetNormalGoodsInfo(GetSynID(), SpaceID);

	return P;

}

TSharedPtr<FXRRoom> AArmyExtrusionActor::GetRoom()
{
	
	if (UniqueCodeExtrusion.StartsWith(TEXT("Buckle")))
	{

		if (!AttachRoomSpaceArea.IsValid())
		{
			//发生错误，放样类线必须依赖于一个面
			return nullptr;
		}

		TArray<FObjectWeakPtr> RoomObjs;
		FXRHomeData::Get()->GetObjects(EModelType::E_LayoutModel, EObjectType::OT_InternalRoom, RoomObjs);

		for (auto & RoomObj : RoomObjs)
		{

			auto Room = StaticCastSharedPtr<FXRRoom>(RoomObj.Pin());

			//获得房间的地面
			TArray<TSharedPtr<FXRRoomSpaceArea>> totalRoomAreas = FXRHomeData::Get()->GetRoomAttachedSurfacesWidthRoomID(Room->GetUniqueID().ToString());
			for (auto & Area : totalRoomAreas)
			{
				if (Area->SurfaceType == 0)
				{
					if (AttachRoomSpaceArea.Pin()->IsBuckleConnected(this, Area))
					{
						return Room;
					}
				}
			}
		}

	}
	else
	{
		if (!AttachSurfaceArea.IsValid())
		{
			//发生错误，放样类线必须依赖于一个面
			return nullptr;
		}
		else
		{
			auto Room = AttachSurfaceArea.Pin()->GetRoom();
			if (Room.IsValid())
			{
				return  Room;
			}
		}
	}

	//不应当运行到此处，因为所有的放样类都依赖于面
	return nullptr;

}

void AArmyExtrusionActor::OnRoomSpaceIDChanged(int32 NewSpaceID)
{

	FGuid ID;
	if (UniqueCodeExtrusion.StartsWith(TEXT("Buckle")))
	{

		if (!AttachRoomSpaceArea.IsValid())
		{
			//发生错误，放样类线必须依赖于一个面
			return ;
		}
		ID = AttachRoomSpaceArea.Pin()->GetUniqueID();
	}
	else
	{
		if (!AttachSurfaceArea.IsValid())
		{
			//发生错误，放样类线必须依赖于一个面
			return;
		}
		ID = AttachSurfaceArea.Pin()->GetUniqueID();
	}
	XRConstructionManager::Get()->TryToFindConstructionData(ID, GetConstructionParameter(), nullptr);
}
