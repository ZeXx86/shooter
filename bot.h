#ifndef __bot_h__
#define __bot_h__

#include <math.h>

extern bool bot_collision (player_t *p, float x, float y);
extern void bot_motion (player_t *p, player_t *l);

#endif

