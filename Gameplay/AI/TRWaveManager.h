#pragma once

class TRWaveManager
{
public:
	static TRWaveManager* const QInstance();

	void Update();
	void SpawnEnemy();
	void OnEnemyDeath(bool abDespawned = false);
	void OnWaveEnd();

private:
	TRWaveManager() {};

	static TRWaveManager* instancePtr;

};

