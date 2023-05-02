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

	void SetPosition(float aiX, float aiY, float aiZ)
	{
		x = aiX;
		y = aiY;
		z = aiZ;
	}

	float SetPositionX(float aiNewVal)
	{
		x = aiNewVal;
		QPositionX();
	}
	float SetPositionY(float aiNewVal)
	{
		y = aiNewVal;
		QPositionY();
	}
	float SetPositionZ(float aiNewVal)
	{
		z = aiNewVal;
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
};

class TRPhysics
{

};

