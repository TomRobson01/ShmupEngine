#pragma once

// Forward definitions
class TRWorldObject;

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

	float const QPositionX() { return x; }
	float const QPositionY() { return y; }
	float const QPositionZ() { return z; }
	float const QRotation()		{ return rotation; };

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

enum class CollisionLayer
{
	// WARNING!
	// Each time you add to this, please increment the size of the CollisionLayers array in TRPhysics.cpp
	CL_DEFAULT				= 0,
	CL_PLAYER				= 1,
	CL_ENEMY				= 2,
	CL_PLAYER_PROJECTILE	= 3,
	CL_ENEMY_PROJECTILE		= 4,
	CL_COUNT				= 5	// END

};

/// <summary>
/// Class containing data for a simple, radial collider
/// </summary>
class CircleCollider
{
public:
	CircleCollider(float afRadius, CollisionLayer aeLayer = CollisionLayer::CL_DEFAULT);
	CircleCollider(float ax, float ay, float afRadius, CollisionLayer aeLayer = CollisionLayer::CL_DEFAULT);

	void SetPosition(float afX, float afY)
	{
		x = afX;
		y = afY;
	}

	float const QPosX()		{ return x; }
	float const QPosY()		{ return y; }
	float const QRadius()	{ return fRadius; }

	void SetID(int aID) { ID = aID; }
	int  const QID()	{ return ID; }

	CollisionLayer QCollisionLayer() { return colLayer; }

protected:
	float x, y;
	float fRadius;
	CollisionLayer colLayer = CollisionLayer::CL_DEFAULT;

private:
	int ID;
};


class TRPhysics
{
public:
	static TRPhysics* const QInstance();

	void InitCollisionThread();
	void ShutdownCollisionThread();

	void RegisterCollider(CollisionLayer aeTargetLayer, TRWorldObject* apCollider);
	void UnRegisterCollider(TRWorldObject* apCollider);

private:
	TRPhysics() {}

	static TRPhysics* instancePtr;
};

