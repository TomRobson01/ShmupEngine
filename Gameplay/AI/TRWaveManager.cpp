#include "TRWaveManager.h"

#include "TRDefines.h"
#include "TREnemy.h"
#include "DebugWindows/TRLoggerImGui.h"
#include "Objects/TRWorld.h"

#include <algorithm>
#include <ctime>

namespace
{
	clock_t lastSpawnTime;
	clock_t waveEndTime;
	bool bWaveEnded = false;

	uint16_t uiWave = 1;

	uint16_t uiEnemiesSpawned			= 0;
	uint16_t uiEnemiesKilled			= 0;
	uint16_t uiWaveTarget				= 6;

	uint16_t uiAllowedEnemyTypes		= 0;
	uint16_t uiWavesUntilNewType		= 3;
	uint16_t uiNextEnemyIntroducedWave	= 3;
	uint16_t uiWaveTargetIncrement		= 3;

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
		instancePtr = new TRWaveManager();
	}
	return instancePtr;
}

/// <summary>Ticks the wave manager along. Handles timers for spawning and wave intermissions.</summary>
void TRWaveManager::Update()
{
	if (TRWorld::QInstance()->QGameEnded())
	{
		lastSpawnTime = 0;
		waveEndTime = 0;
		bWaveEnded = false;
		uiEnemiesKilled = 0;
		uiEnemiesSpawned = 0;
		uiAllowedEnemyTypes = 0;
		return;
	}

	const clock_t cCurrentClock = clock();
	float fTimeSinceLastSpawn = cCurrentClock - lastSpawnTime;
	float fTimeSinceWaveEnd = cCurrentClock - waveEndTime;

	if (bWaveEnded)
	{
		// Wave end timer - move to the next wave when the timer is elapsedif (fTimeSinceLastSpawn >= CLUSTER_TIMER)
		if (fTimeSinceWaveEnd >= WAVE_END_TIMER)
		{
			bWaveEnded = false;
			uiEnemiesKilled = 0;
			uiEnemiesSpawned = 0;
			uiWave++;
			TRLogger::QInstance()->Log("Wave started!");
		}
	}
	else
	{
		// Normal gameplay, spawn enemies in clusters on set intervals until a limit has been reached
		if (fTimeSinceLastSpawn >= CLUSTER_TIMER)
		{
			lastSpawnTime = cCurrentClock;
			for (int i = 0; i < MAX_ENEMY_CLUSTER && uiEnemiesKilled + uiEnemiesSpawned < uiWaveTarget; i++)
			{
				SpawnEnemy();
			}
		}
	}
}

/// <summary>Spawns a random eligible enemy at a random possition off the right side of the screen.</summary>
/// <remarks>This function handles all setup for each enemy type. If you want to set the values for an enemy to have unique behavior, do it here.</remarks>
void TRWaveManager::SpawnEnemy()
{
	if (uiEnemiesSpawned < MAX_ENEMY_COUNT)
	{
		TRLogger::QInstance()->Log("Spawning enemy!");

		float iSpawnXOffset = (rand() % 70 + (30)) * 0.1f;
		float iSpawnY = (rand() % 210 + (-100)) * 0.1f;
		Transform spawnTransform = Transform(20 + iSpawnXOffset, iSpawnY, 0, 0);

		switch (rand() % (int)ENEMY_TYPE::COUNT)
		{
		case (int)ENEMY_TYPE::SUICIDER:
			if (uiAllowedEnemyTypes >= (int)ENEMY_TYPE::SUICIDER)
			{
				std::shared_ptr<TREnemy> e = TRWorld::QInstance()->InstanciateObject<TREnemy>(TRWorld::QInstance()->QObjEnemySuicider(), spawnTransform, 1.0f, CollisionLayer::CL_ENEMY_SUICIDER);
				e->SetMotionType(ENEMY_MOTION_TYPE::MT_HOMING);
				e->SetMoveSpeed(2);
				break;
			}
		case (int)ENEMY_TYPE::TOUGH:
			if (uiAllowedEnemyTypes >= (int)ENEMY_TYPE::TOUGH)
			{
				std::shared_ptr<TREnemy> e = TRWorld::QInstance()->InstanciateObject<TREnemy>(TRWorld::QInstance()->QObjEnemyTough(), spawnTransform, 1.0f, CollisionLayer::CL_ENEMY);
				e->AddFirePoint(0, 0);
				e->AddFirePoint(0, -1);
				e->AddFirePoint(0, 1);
				e->SetHealth(17);
				e->SetMoveSpeed(0.5f);
				e->SetMotionType(ENEMY_MOTION_TYPE::MT_SINE);
				break;
			}
		case (int)ENEMY_TYPE::BASIC:
		{
			std::shared_ptr<TREnemy> e = TRWorld::QInstance()->InstanciateObject<TREnemy>(TRWorld::QInstance()->QObjEnemy(), spawnTransform, 1.0f, CollisionLayer::CL_ENEMY);
			e->AddFirePoint(0, 0);
			e->SetHealth(6);
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

/// <summary>Called when an enemy dies. Ticks along the enemy wave count, and free up space in the budget. If we've reached our kill quota, start the wave intermission.</summary>
/// <param name="abDespawned">Whether or not the enemy "died" by leaving the screen on the left-hand side.</param>
void TRWaveManager::OnEnemyDeath(bool abDespawned)
{
	if (!abDespawned)
	{
		uiEnemiesKilled++;
		if (uiEnemiesKilled >= uiWaveTarget && !bWaveEnded)
		{
			OnWaveEnd();
		}
	}
	uiEnemiesSpawned--;
}

/// <summary>Calculates the settings for the next wave, and begins the intermission timer.</summary>
void TRWaveManager::OnWaveEnd()
{
	// End the current wave
	bWaveEnded = true;
	const clock_t cCurrentClock = clock();
	waveEndTime = cCurrentClock;
	TRLogger::QInstance()->Log("Wave ended!");

	// Calculate the stats for the next wave
	if (uiWave >= uiNextEnemyIntroducedWave)
	{
		uiAllowedEnemyTypes++;
		std::min(uiAllowedEnemyTypes, (uint16_t)ENEMY_TYPE::COUNT);
		uiNextEnemyIntroducedWave += uiWavesUntilNewType;
	}

	uiWaveTarget += uiWaveTargetIncrement;
}