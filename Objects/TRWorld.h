#pragma once

#include <atomic>
#include <map>
#include <memory>
#include <mutex>

#include "TRDefines.h"
#include "TRWorldObject.h"
#include "Rendering/TRRenderer.h"

enum class WORLDSTATE
{
	DEFAULT,
	PLAY,
	GAMEOVER,
	MAINMENU,
	COUNT
};

class TRWorld
{
public:
	static TRWorld* const QInstance();

	void UpdateWorld();
	void UnloadWorld();

	/// <summary>
	/// Handles creation and initialization for TRWorldObjects, created from a TRObject base.
	/// </summary>
	/// <typeparam name="T">Ensure the type we use inherrits from TRWorldObject.</typeparam>
	/// <param name="aBase">The TRObject we wish to use as a baseline, which the WorldObject will collect data from.</param>
	/// <param name="atInitialTransform">The initial transformation for this new object.</param>
	/// <returns>A stared_ptr of the newly instanciated object.</returns>
	template<typename T>
	std::shared_ptr<T> InstanciateObject(TRObject aBase, Transform atInitialTransform = Transform(), float afColliderRadius = 0.5f, CollisionLayer aeLayer = CollisionLayer::CL_DEFAULT)
	{
		IDGenerator++;
		std::shared_ptr<T> pOut = std::make_shared<T>(aBase, atInitialTransform, afColliderRadius, aeLayer, IDGenerator);
		worldObjects.insert(std::make_pair(IDGenerator, pOut));
		pOut->CallStart();
		TRPhysics::QInstance()->RegisterCollider(aeLayer, pOut.get());
		return pOut;
	}

	std::shared_ptr<TRWorldObject> GetWorldObjectByID(int aiID);
	void RemoveWorldObject(int aiTargetID);

	void SetCamShakeDuration(float afDuration);

	void RequestQuit() { bWantsQuit = true; }
	bool QShouldQuit() { return bWantsQuit; }

	void StartGame();
	void EndGame();
	bool QGameEnded()	{ return bGameEnded; }

	std::mutex objLock;

	TRObject QObjPlayer()			{ return ObjPlayer;		}
	TRObject QObjPlayerProjectile() { return ObjPlayerProjectile; }
	TRObject QObjEnemy()			{ return ObjEnemy; }
	TRObject QObjEnemyTough()		{ return ObjEnemy_Tough; }
	TRObject QObjEnemySuicider()	{ return ObjEnemy_Suicider; }
	TRObject QObjEnemyEmitter()		{ return ObjEnemy_Emitter; }

	TRObject QObjEnemyDebris()		// Returns a random ObjEnemy_Debris
	{ 
		switch (rand() % 3)
		{
		case 1:
			return ObjEnemy_DebrisB;
		case 2:
			return ObjEnemy_DebrisC;
		default:
			return ObjEnemy_DebrisA;
		}
	}
	TRObject QObjEnemyProjectile()	{ return ObjEnemyProjectile;		}

	TRObject QObjExplosion()  { return ObjExplosion; }

	TRObject QObjPickup()			{ return ObjPickup; }
	TRObject QObjPickupHeart()		{ return ObjPickupHeart; }
	TRObject QObjPickupExplosion()	{ return ObjPickupExplosion; }

private:
	TRWorld() 
	{
		std::vector<const char*> cPlayerTextureAtlas				= { "Assets/Textures/T_Player_01.png", "Assets/Textures/T_Player_02.png", "Assets/Textures/T_Player_03.png" };
		std::vector<const char*> cPlayerTextureAtlasProjectile		= { "Assets/Textures/T_Projectile_Player.png" };
		std::vector<const char*> cEnemyTextureAtlas					= { "Assets/Textures/T_Enemy_01.png", "Assets/Textures/T_Enemy_02.png", "Assets/Textures/T_Enemy_03.png" };
		std::vector<const char*> cEnemyToughTextureAtlas			= { "Assets/Textures/T_Enemy_Tough_01.png", "Assets/Textures/T_Enemy_Tough_02.png", "Assets/Textures/T_Enemy_Tough_03.png" };
		std::vector<const char*> cEnemySuiciderTextureAtlas			= { "Assets/Textures/T_Enemy_Suicide_01.png", "Assets/Textures/T_Enemy_Suicide_02.png" };
		std::vector<const char*> cEnemyDebrisTextureAtlasA			= { "Assets/Textures/T_Asteroid_01.png" };
		std::vector<const char*> cEnemyDebrisTextureAtlasB			= { "Assets/Textures/T_Asteroid_02.png" };
		std::vector<const char*> cEnemyDebrisTextureAtlasC			= { "Assets/Textures/T_Asteroid_03.png" };
		std::vector<const char*> cEnemyEmitterTextureAtlas			= { "Assets/Textures/T_Enemy_Emitter.png" };
		std::vector<const char*> cEnemyTextureAtlasProjectile		= { "Assets/Textures/T_Projectile_Enemy.png" };
		std::vector<const char*> cExplosionTextureAtlas				= { "Assets/Textures/T_Explosion_01.png", "Assets/Textures/T_Explosion_02.png", "Assets/Textures/T_Explosion_03.png", "Assets/Textures/T_Explosion_04.png", "Assets/Textures/T_Explosion_05.png", "Assets/Textures/T_Explosion_06.png" };
		std::vector<const char*> cHeartPickupTextureAtlas			= { "Assets/Textures/T_Heart.png" };
		std::vector<const char*> cPickupTextureAtlas				= { "Assets/Textures/T_Powerup_01.png", "Assets/Textures/T_Powerup_02.png", "Assets/Textures/T_Powerup_03.png", "Assets/Textures/T_Powerup_04.png", "Assets/Textures/T_Powerup_05.png","Assets/Textures/T_Powerup_06.png" };
		std::vector<const char*> cPickupExplosionTextureAtlas		= { "Assets/Textures/T_Explosion_Powerup_01.png", "Assets/Textures/T_Explosion_Powerup_02.png", "Assets/Textures/T_Explosion_Powerup_03.png", "Assets/Textures/T_Explosion_Powerup_04.png" };

		ObjPlayer			= TRObject("Player",				cPlayerTextureAtlas);
		ObjPlayerProjectile = TRObject("PlayerProjectile",		cPlayerTextureAtlasProjectile);
		ObjEnemy			= TRObject("Enemy",					cEnemyTextureAtlas);
		ObjEnemy_Tough		= TRObject("EnemyTough",			cEnemyToughTextureAtlas);
		ObjEnemy_Suicider	= TRObject("EnemySuicider",			cEnemySuiciderTextureAtlas);
		ObjEnemy_Emitter	= TRObject("EnemyEmitter",			cEnemyEmitterTextureAtlas);
		ObjEnemy_DebrisA	= TRObject("EnemyDebrisA",			cEnemyDebrisTextureAtlasA);
		ObjEnemy_DebrisB	= TRObject("EnemyDebrisB",			cEnemyDebrisTextureAtlasB);
		ObjEnemy_DebrisC	= TRObject("EnemyDebrisC",			cEnemyDebrisTextureAtlasC);
		ObjEnemyProjectile	= TRObject("PlayerProjectile",		cEnemyTextureAtlasProjectile);
		ObjExplosion		= TRObject("Explosion",				cExplosionTextureAtlas);
		ObjPickup			= TRObject("Pickup",				cPickupTextureAtlas);
		ObjPickupHeart		= TRObject("PickupHeart",			cHeartPickupTextureAtlas);
		ObjPickupExplosion	= TRObject("PickupExplosion",		cPickupExplosionTextureAtlas);
	}

	static TRWorld* instancePtr;

	bool bGameStarted = false;
	bool bGameEnded = true;
	bool bWantsQuit;

	float fCamShakeDuration;

	std::map<int, std::shared_ptr<TRWorldObject>> worldObjects;
	int IDGenerator = 0;

	// Usable TRObjects are defined here. You will need to define them in the TRWorld constructor, and add an accessory for them in the public scope
	TRObject ObjPlayer;
	TRObject ObjPlayerProjectile;
	TRObject ObjEnemy;
	TRObject ObjEnemy_Tough;
	TRObject ObjEnemy_Suicider;
	TRObject ObjEnemy_Emitter;
	TRObject ObjEnemy_DebrisA;
	TRObject ObjEnemy_DebrisB;
	TRObject ObjEnemy_DebrisC;
	TRObject ObjEnemyProjectile;

	TRObject ObjExplosion;

	TRObject ObjPickup;
	TRObject ObjPickupHeart;
	TRObject ObjPickupExplosion;
	// End of TRObject defs ------------------
};


