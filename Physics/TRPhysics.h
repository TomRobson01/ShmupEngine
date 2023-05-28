#pragma once

struct Transform
{
public:
	Transform() = default;
	Transform(float afX, float afY, float afZ, float afRotation) 
	{
		x = afX;
		y = afY;
		z = afZ;

		rotation = afRotation;
	};

	void Translate(float afXDelta, float afYDelta, float afZDelta, float afRotationDelta)
	{
		x += afXDelta;
		y += afYDelta;
		z += afZDelta;
		rotation += afRotationDelta;
		ResolveClamps();
	}

	void SetClamp(float afXMin, float afXMax, float afYMin, float afYMax)
	{
		xClampMin = afXMin;
		xClampMax = afXMax;
		yClampMin = afYMin;
		yClampMax = afYMax;

		bClampsActive = true;
	}

	void ReleaseClamps()
	{
		bClampsActive = false;
		xClampMin = 0;
		xClampMax = 0;
		yClampMin = 0;
		yClampMax = 0;
	}

	void SetPosition(float afX, float afY, float afZ)
	{
		x = afX;
		y = afY;
		z = afZ;
		ResolveClamps();
	}

	float SetPositionX(float afNewVal)
	{
		x = afNewVal;
		ResolveClamps();
		QPositionX();
	}
	float SetPositionY(float afNewVal)
	{
		y = afNewVal;
		ResolveClamps();
		QPositionY();
	}
	float SetPositionZ(float afNewVal)
	{
		z = afNewVal;
		ResolveClamps();
		QPositionZ();
	}

	float Rotate(float afDelta) { rotation += afDelta; return rotation; };

	float QPositionX() { return x; }
	float QPositionY() { return y; }
	float QPositionZ() { return z; }
	float QRotation() { return rotation; };

protected:
	float x, y, z;
	float rotation;

	float xClampMin, xClampMax;
	float yClampMin, yClampMax;

private:
	void ResolveClamps()
	{
		if (!bClampsActive)
			return;

		x = x < xClampMin ? xClampMin : x;
		x = x > xClampMax ? xClampMax : x;
		y = y < yClampMin ? yClampMin : y;
		y = y > yClampMax ? yClampMax : y;
	}

	bool bClampsActive;

};

class TRPhysics
{

};

