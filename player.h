#ifndef __player_h__
#define __player_h__

#define PLAYER_STATE_WALK	0x1
#define PLAYER_STATE_FIRE	0x2
#define PLAYER_STATE_DEAD	0x4

#define PLAYER_SPEED	1.0f / 150

typedef struct player_ctx {
	player_ctx *next, *prev;

	unsigned char state;

	unsigned hp;
	
	float pos_x;
	float pos_y;

	float rot_y;
	
	unsigned char ui_cmd;

	int mdl_frame;
	float mdl_itp;
} player_t;

extern player_t player_list;

extern player_t *player_get ();
extern bool player_init ();

#endif

