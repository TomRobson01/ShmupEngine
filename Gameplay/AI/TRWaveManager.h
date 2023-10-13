#pragma once

class TRWaveManager
{
public:
	static TRWaveManager* const QInstance();

	void Update();
	void SpawnEnemy();
	void OnEnemyDeath(bool abDespawned = false);
	void OnWaveEnd();
	void TrySpawnDrop(const float afSpawnX, const float afSpawnY);
	void KillAll();

	float QScrollSpeedMultiplier() const { return fScrollSpeedMultiplier; }
	bool QWaveEnded() const;

private:
	TRWaveManager() {};

	static TRWaveManager* instancePtr;

	float fScrollSpeedMultiplier = 1.0f;

};

