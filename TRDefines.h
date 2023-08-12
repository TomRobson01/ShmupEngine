#pragma once

#define ANIMATION_FPS 12.0f

#define FIXED_DELTA_TIME 0.06		
#define FIXED_UPDATE_TICKS 16.666f // CLOCKS_PER_SEC is 1000, / 60 target fixed updates a second, we get 16.6 recurring

#define MAX_ENEMY_COUNT		4
#define MAX_ENEMY_CLUSTER	2
#define WAVE_END_TIMER		5000.0f
#define CLUSTER_TIMER		2000.0f
#define GAME_BEGIN_TIMER	7500.0f

#define NORMAL_SCROLL_SPEED 1.0f
#define TRANSITION_SCROLL_SPEED 12.0f

#define DROP_CHANCE 30
#define DROP_COOLDOWN 10
#define MAX_WAVE_DROPS 3
