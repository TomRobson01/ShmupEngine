#include "TRWaveManager.h"

#include "TRDefines.h"
#include "TREnemy.h"
#include "TRMath.h"
#include "DebugWindows/TRLoggerImGui.h"
#include "Gameplay/TRDrop.h"
#include "Objects/TRWorld.h"

#include <algorithm>
#include <ctime>

namespace
{
	clock_t lastSpawnTime;
	clock_t waveEndTime;
	bool bWaveEnded = false;
	bool bKillAll = true;

	uint16_t uiWave = 1;

	uint16_t uiEnemiesSpawned			= 0;
	uint16_t uiEnemiesKilled			= 0;
	uint16_t uiWaveTarget				= 12;

	uint16_t uiSpawnedToughs			= 0;
	uint16_t uiSpawnedSuiciders			= 0;
	uint16_t uiToughsClusterBudget		= 2;
	uint16_t uiSuiciderClusterBudget	= 4;

	uint16_t uiAllowedEnemyTypes		= 0;
	uint16_t uiWavesUntilNewType		= 3;
	uint16_t uiNextEnemyIntroducedWave	= 3;
	uint16_t uiWaveTargetIncrement		= 6;

	uint16_t uiDropsThisWave			= 0;
	clock_t lastDrop;

	enum class ENEMY_TYPE : uint16_t
	{
		BASIC,
		EMITTER,
		DEBRIS,
		SUICIDER,
		TOUGH,
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

bool TRWaveManager::QWaveEnded() const { return bWaveEnded; }

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
		uiSpawnedToughs = 0;
		uiSpawnedSuiciders = 0;
		uiDropsThisWave = 0;
		return;
	}

	const clock_t cCurrentClock = clock();
	float fTimeSinceLastSpawn = cCurrentClock - lastSpawnTime;
	float fTimeSinceWaveEnd = cCurrentClock - waveEndTime;

	if (cCurrentClock > GAME_BEGIN_TIMER)
	{
		if (bWaveEnded)
		{
			fScrollSpeedMultiplier = TRMath::FLerp(fScrollSpeedMultiplier, TRANSITION_SCROLL_SPEED, std::min(fTimeSinceWaveEnd * 0.0001f, 1.0f));
			// Wave end timer - move to the next wave when the timer is elapsedif (fTimeSinceLastSpawn >= CLUSTER_TIMER)
			if (fTimeSinceWaveEnd >= WAVE_END_TIMER)
			{
				bWaveEnded = false;
				uiEnemiesKilled = 0;
				uiEnemiesSpawned = 0;
				uiDropsThisWave = 0;
				uiWave++;
				TRLogger::QInstance()->Log("Wave started!");
			}
		}
		else
		{
			fScrollSpeedMultiplier = TRMath::FLerp(fScrollSpeedMultiplier, NORMAL_SCROLL_SPEED, std::min(fTimeSinceWaveEnd * 0.0001f, 1.0f));
			// Normal gameplay, spawn enemies in clusters on set intervals until a limit has been reached
			if (fTimeSinceLastSpawn >= CLUSTER_TIMER)
			{
				lastSpawnTime = cCurrentClock;
				for (int i = 0; i < MAX_ENEMY_CLUSTER && uiEnemiesKilled + uiEnemiesSpawned < uiWaveTarget; i++)
				{
					uiSpawnedToughs = 0;
					uiSpawnedSuiciders = 0;
					SpawnEnemy();
				}
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

		uint16_t uiPickedType = rand() % (uint16_t)ENEMY_TYPE::COUNT;
		uiPickedType = std::max(uiPickedType, uiAllowedEnemyTypes);

		switch (uiPickedType)
		{
		case (uint16_t)ENEMY_TYPE::EMITTER:
			if (uiAllowedEnemyTypes >= (int)ENEMY_TYPE::EMITTER)
			{
				std::shared_ptr<TREnemy> e = TRWorld::QInstance()->InstanciateObject<TREnemy>(TRWorld::QInstance()->QObjEnemyEmitter(), spawnTransform, 1.0f, CollisionLayer::CL_ENEMY);

				e->AddFirePoint(0, 0,  -1,	0);
				e->AddFirePoint(0, 0,	1,	0);
				e->AddFirePoint(0, 0,	0, -1);
				e->AddFirePoint(0, 0,	0,	1);

				e->SetMoveSpeed(1);
				e->SetHealth(10);
				e->SetMotionType(ENEMY_MOTION_TYPE::MT_SINE);
				e->SetSineAmplitude(0.5f);
				break;
			}
		case (uint16_t)ENEMY_TYPE::DEBRIS:
			if (uiAllowedEnemyTypes >= (int)ENEMY_TYPE::DEBRIS)
			{
				std::shared_ptr<TREnemy> e = TRWorld::QInstance()->InstanciateObject<TREnemy>(TRWorld::QInstance()->QObjEnemyDebris(), spawnTransform, 1.0f, CollisionLayer::CL_ENEMY_SUICIDER);
				e->SetMoveSpeed(3);
				e->SetHealth(5);
				e->SetMotionType(ENEMY_MOTION_TYPE::MT_SINE);
				e->SetSineAmplitude(0.5f);
				break;
			}
		case (uint16_t)ENEMY_TYPE::SUICIDER:
			if (uiAllowedEnemyTypes >= (int)ENEMY_TYPE::SUICIDER && uiSpawnedSuiciders < uiSuiciderClusterBudget)
			{
				std::shared_ptr<TREnemy> e = TRWorld::QInstance()->InstanciateObject<TREnemy>(TRWorld::QInstance()->QObjEnemySuicider(), spawnTransform, 1.0f, CollisionLayer::CL_ENEMY_SUICIDER);
				e->SetMotionType(ENEMY_MOTION_TYPE::MT_HOMING);
				e->SetMoveSpeed(3);

				++uiSpawnedSuiciders;
				break;
			}
		case (uint16_t)ENEMY_TYPE::TOUGH:
			if (uiAllowedEnemyTypes >= (int)ENEMY_TYPE::TOUGH && uiSpawnedToughs < uiToughsClusterBudget)
			{
				std::shared_ptr<TREnemy> e = TRWorld::QInstance()->InstanciateObject<TREnemy>(TRWorld::QInstance()->QObjEnemyTough(), spawnTransform, 1.0f, CollisionLayer::CL_ENEMY);
				e->AddFirePoint(0, 0);
				e->AddFirePoint(0, -1);
				e->AddFirePoint(0, 1);
				e->SetHealth(12);
				e->SetMoveSpeed(0.5f);
				e->SetMotionType(ENEMY_MOTION_TYPE::MT_SINE);

				++uiSpawnedToughs;
				break;
			}
		case (uint16_t)ENEMY_TYPE::BASIC:
		{
			std::shared_ptr<TREnemy> e = TRWorld::QInstance()->InstanciateObject<TREnemy>(TRWorld::QInstance()->QObjEnemy(), spawnTransform, 1.0f, CollisionLayer::CL_ENEMY);
			e->AddFirePoint(0, 0);
			e->SetMoveSpeed(3.f);
			e->SetProjectileSpeed(8.f);
			e->SetHealth(1);
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

/// <summary> Checks if a drop is available, and if so, picks a drop to spawn. </summary>
/// <param name="afSpawnX">Spawn position X value</param>
/// <param name="afSpawnY">Spawn position Y value</param>
void TRWaveManager::TrySpawnDrop(const float afSpawnX, const float afSpawnY)
{
	if (uiDropsThisWave < MAX_WAVE_DROPS)
	{
		const clock_t cCurrentClock = clock();
		float fTimeSinceDrop = cCurrentClock - lastDrop;
		if (fTimeSinceDrop > DROP_COOLDOWN)
		{
			if (rand() % 100 < DROP_CHANCE)
			{
				Transform spawnTransform = Transform(afSpawnX, afSpawnY, 0, 0);
				std::shared_ptr<TRDrop> e = TRWorld::QInstance()->InstanciateObject<TRDrop>(TRWorld::QInstance()->QObjPickup(), spawnTransform, 1.0f, CollisionLayer::CL_DEFAULT);
				TRLogger::QInstance()->Log("Spawn drop!");
				lastDrop = cCurrentClock;
				++uiDropsThisWave;
			}
		}
	}
}

/// <summary>
/// Automatically ends the wave, killing all enemies
/// </summary>
void TRWaveManager::KillAll()
{
	uiEnemiesKilled = uiWaveTarget;
	OnEnemyDeath();
	uiEnemiesSpawned = 0;
}


