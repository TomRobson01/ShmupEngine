#pragma once
#include <algorithm>

class TRMath
{
public:
	static float FLerp(const float afA, const float afB, const float afTime)
	{
		return (1 - afTime) * afA + afTime * afB;
	}

	static float FSine(const float afAmplitude, const float afFrequency, const float afTime)
	{
		return afAmplitude * sin(2.0f * 3.14f * afFrequency * afTime);
	}

	static float FDist(const float afPosA_X, const float afPosA_Y, const float afPosB_X, const float afPosB_Y)
	{
		float fTangent = abs(afPosB_Y - afPosA_Y);
		float fAdjacent = abs(afPosB_X - afPosA_X);
		float fDist = sqrtf((fTangent * fTangent) + (fAdjacent * fAdjacent));
		return fDist;
	}

	static void VFNormalize(float& afX, float& afY)
	{
		const float fTangent = abs(afX);
		const float fAdjacent = abs(afY);
		const float fDist = sqrtf((fTangent * fTangent) + (fAdjacent * fAdjacent));

		afX /= fDist;
		afY /= fDist;
	}
};
