#include "shooter.h"
#include "player.h"
#include "bot.h"
#include "gl.h"

float bot_contact (player_t *a, player_t *b)
{
	return sqrtf ((a->pos_x-b->pos_x)*(a->pos_x-b->pos_x) + (a->pos_y-b->pos_y)*(a->pos_y-b->pos_y));
}

void bot_motion (player_t *p, player_t *l)
{
	l->state = 0;

	float speed = 1;

	float angle = atan2 (l->pos_x-p->pos_x, l->pos_y-p->pos_y);

	float dist = bot_contact (p, l);

	if (dist < 20) {
		l->rot_y = angle * 180 / M_PI;

		player_t *b;
		for (b = player_list.next; b != &player_list; b = b->next) {
			if (b == l)
				continue;
			
			if (bot_contact (l, b) < 0.8f) {
				dist = 0;
				float b_angle = atan2 (l->pos_x-b->pos_x, l->pos_y-b->pos_y);
				float b_rot = b_angle * 180 / M_PI;
				/* M_PI/180 a 180 / M_PI se da "vykratit", ale hezky to vypada.. */
				l->pos_x -= sinf (M_PI/180 * -b_rot) * (PLAYER_SPEED);
				l->pos_y -= cosf (M_PI/180 * -b_rot) * (PLAYER_SPEED);
				
				l->state = PLAYER_STATE_WALK;
				break;
			}
		}
		
		if (dist > 3.0f) {
			l->pos_x -= sinf (M_PI/180 * l->rot_y) * (PLAYER_SPEED);
			l->pos_y -= cosf (M_PI/180 * l->rot_y) * (PLAYER_SPEED);
			l->state = PLAYER_STATE_WALK;
		} else if (dist < 1.1f) {
			if (!(l->state & PLAYER_STATE_WALK))
				l->state = PLAYER_STATE_FIRE;
				
			if (p->hp)
				p->hp --;
		}
	}
}

