#include "TRWaveManager.h"

#include "TRDefines.h"
#include "TREnemy.h"
#include "DebugWindows/TRLoggerImGui.h"
#include "Objects/TRWorld.h"

#include <ctime>

namespace
{
	clock_t lastSpawnTime;
	clock_t waveEndTime;
	bool bWaveEnded = false;
	uint16_t uiEnemiesSpawned = 0;

	enum class ENEMY_TYPE : int
	{
		BASIC,
		TOUGH,
		SUICIDER,
		COUNT
	};
}

TRWaveManager* TRWaveManager::instancePtr;
TRWaveManager* const TRWaveManager::QInstance()
{
	if (instancePtr == nullptr)
	{
		std::srand((unsigned)time(0));	// Seed the RNG when we first get an instance of the class. If we have enough use cases, we can make a dedicated TRRandom class
		instancePtr = new TRWaveManager();
	}
	return instancePtr;
}

void TRWaveManager::Update()
{
	const clock_t cCurrentClock = clock();
	float fTimeSinceLastSpawn = cCurrentClock - lastSpawnTime;
	float fTimeSinceWaveEnd = cCurrentClock - waveEndTime;

	if (bWaveEnded)
	{
		// Wave end timer - move to the next wave when the timer is elapsed
	}
	else
	{
		// Normal gameplay, spawn enemies in clusters on set intervals until a limit has been reached
		if (fTimeSinceLastSpawn >= CLUSTER_TIMER)
		{
			lastSpawnTime = cCurrentClock;
			for (int i = 0; i < MAX_ENEMY_CLUSTER; i++)
			{
				SpawnEnemy();
			}
		}
	}
}

void TRWaveManager::SpawnEnemy()
{
	if (uiEnemiesSpawned < MAX_ENEMY_COUNT)
	{
		TRLogger::QInstance()->Log("Spawning enemy!");

		float iSpawnY = rand() % 21 + (-10);
		Transform spawnTransform = Transform(20, iSpawnY, 0, 0);

		switch (rand() % (int)ENEMY_TYPE::COUNT)
		{
		case (int)ENEMY_TYPE::BASIC:
			TRWorld::QInstance()->InstanciateObject<TREnemy>(TRWorld::QInstance()->QObjEnemy(), spawnTransform, 1.0f, CollisionLayer::CL_ENEMY);
			break;
		case (int)ENEMY_TYPE::TOUGH:
			{
				std::shared_ptr<TREnemy> e = TRWorld::QInstance()->InstanciateObject<TREnemy>(TRWorld::QInstance()->QObjEnemyTough(), spawnTransform, 1.0f, CollisionLayer::CL_ENEMY);
				e->AddFirePoint(0, 0);
				e->AddFirePoint(0, -1);
				e->AddFirePoint(0, 1);
			}
			break;
		case (int)ENEMY_TYPE::SUICIDER:
			{
				std::shared_ptr<TREnemy> e = TRWorld::QInstance()->InstanciateObject<TREnemy>(TRWorld::QInstance()->QObjEnemySuicider(), spawnTransform, 1.0f, CollisionLayer::CL_ENEMY_SUICIDER);
				e->SetMotionType(ENEMY_MOTION_TYPE::MT_HOMING);
			}
			break;
		}
		uiEnemiesSpawned++;
	}
	else
	{
		TRLogger::QInstance()->Log("Enemy budget reached!");
	}
}

void TRWaveManager::OnEnemyDeath(bool abDespawned)
{
	uiEnemiesSpawned--;
}