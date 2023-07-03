#pragma once

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


	std::mutex objLock;

	TRObject QObjPlayer()			{ return ObjPlayer;		}
	TRObject QObjPlayerProjectile() { return ObjPlayerProjectile; }
	TRObject QObjEnemy()			{ return ObjEnemy; }
	TRObject QObjEnemyTough()		{ return ObjEnemy_Tough; }
	TRObject QObjEnemySuicider()	{ return ObjEnemy_Suicider; }
	TRObject QObjEnemyProjectile()	{ return ObjEnemyProjectile;		}

	TRObject QObjExplosionPlayer()  { return ObjExplosionPlayer; }
	TRObject QObjExplosionEnemy()	{ return ObjExplosionEnemy; }

private:
	TRWorld() 
	{
		std::vector<const char*> cPlayerTextureAtlas				= { "Assets/Textures/T_Player_01.png", "Assets/Textures/T_Player_02.png", "Assets/Textures/T_Player_03.png" };
		std::vector<const char*> cPlayerTextureAtlasProjectile		= { "Assets/Textures/T_Projectile_Player_01.png", "Assets/Textures/T_Projectile_Player_02.png" };
		std::vector<const char*> cEnemyTextureAtlas					= { "Assets/Textures/T_Enemy_01.png", "Assets/Textures/T_Enemy_02.png", "Assets/Textures/T_Enemy_03.png" };
		std::vector<const char*> cEnemyToughTextureAtlas			= { "Assets/Textures/T_Enemy_Tough_01.png", "Assets/Textures/T_Enemy_Tough_02.png", "Assets/Textures/T_Enemy_Tough_03.png" };
		std::vector<const char*> cEnemySuiciderTextureAtlas			= { "Assets/Textures/T_Enemy_Suicide_01.png", "Assets/Textures/T_Enemy_Suicide_02.png" };
		std::vector<const char*> cEnemyTextureAtlasProjectile		= { "Assets/Textures/T_Projectile_Enemy_01.png", "Assets/Textures/T_Projectile_Enemy_02.png" };
		std::vector<const char*> cExplosionPlayerTextureAtlas		= { "Assets/Textures/T_Explosion_Player_01.png", "Assets/Textures/T_Explosion_Player_02.png", "Assets/Textures/T_Explosion_Player_03.png", "Assets/Textures/T_Explosion_Player_04.png", "Assets/Textures/T_Explosion_Player_05.png" };
		std::vector<const char*> cExplosionEnemyTextureAtlas		= { "Assets/Textures/T_Explosion_Enemy_01.png", "Assets/Textures/T_Explosion_Enemy_02.png", "Assets/Textures/T_Explosion_Enemy_03.png", "Assets/Textures/T_Explosion_Enemy_04.png", "Assets/Textures/T_Explosion_Enemy_05.png" };

		ObjPlayer			= TRObject("Player",				cPlayerTextureAtlas);
		ObjPlayerProjectile = TRObject("PlayerProjectile",		cPlayerTextureAtlasProjectile);
		ObjEnemy			= TRObject("Enemy",					cEnemyTextureAtlas);
		ObjEnemy_Tough		= TRObject("EnemyTough",			cEnemyToughTextureAtlas);
		ObjEnemy_Suicider	= TRObject("EnemySuicider",			cEnemySuiciderTextureAtlas);
		ObjEnemyProjectile	= TRObject("PlayerProjectile",		cEnemyTextureAtlasProjectile);
		ObjExplosionPlayer	= TRObject("ExplosionPlayer",		cExplosionPlayerTextureAtlas);
		ObjExplosionEnemy	= TRObject("ExplosionEnemy",		cExplosionEnemyTextureAtlas);
	}

	static TRWorld* instancePtr;

	std::map<int, std::shared_ptr<TRWorldObject>> worldObjects;
	int IDGenerator = 0;

	// Usable TRObjects are defined here. You will need to define them in the TRWorld constructor, and add an accessory for them in the public scope
	TRObject ObjPlayer;
	TRObject ObjPlayerProjectile;
	TRObject ObjEnemy;
	TRObject ObjEnemy_Tough;
	TRObject ObjEnemy_Suicider;
	TRObject ObjEnemyProjectile;

	TRObject ObjExplosionPlayer;
	TRObject ObjExplosionEnemy;
	// End of TRObject defs ------------------
};


