#include "shooter.h"
#include "player.h"
#include "level.h"
#include "logic.h"
#include "mouse.h"
#include "kbd.h"
#include "bot.h"
#include "gl.h"


bool logic_collision (player_t *p, float x, float y)
{
	float dim = WALL_DIM - 0.5f;
	float dim_col = 2*WALL_DIM - 1.2f;

	float a = p->pos_x - (x-dim);
	float b = p->pos_x - (x+dim);
	float c = p->pos_y - (y-dim);
	float d = p->pos_y - (y+dim);

	float angle = atan2f (y, x) - atan2f (p->pos_y, p->pos_x);// atan2(v1.y,v1.x)

	if (a > 0 && b < 0 && c > 0 && d < 0) {
		if (d <= -dim_col) {
			p->pos_x -= sinf (M_PI/180 * -angle) * (PLAYER_SPEED);
			p->pos_y -= cosf (M_PI/180 * -angle) * (PLAYER_SPEED);
		} else if (c >= dim_col) {
			p->pos_x -= sinf (M_PI/180 * angle) * (-PLAYER_SPEED);
			p->pos_y -= cosf (M_PI/180 * angle) * (-PLAYER_SPEED);
		} else if (b <= -dim_col) {
			p->pos_x -= cosf (M_PI/180 * -angle) * (PLAYER_SPEED);
			p->pos_y -= sinf (M_PI/180 * -angle) * (PLAYER_SPEED);
		} else if (a >= dim_col) {
			p->pos_x -= cosf (M_PI/180 * angle) * (-PLAYER_SPEED);
			p->pos_y -= sinf (M_PI/180 * angle) * (-PLAYER_SPEED);
		}

		return true;
	}

	return false;
}

void logic_motion (player_t *p)
{
	/* MOUSE MOTION */
	mouse_t *m = mouse_get ();

	float speed = 0;

#ifdef ANDROID
	p->rot_y += m->x / 150.0f;

	speed = m->y < 0 ? 10 : (m->y > 0 ? -10 : 0);

	if (m->state == 0) {
		p->state |= PLAYER_STATE_FIRE;
	}
#else	
	p->rot_y += m->x / 2;

	if (m->state & SDL_BUTTON(1)) {
		p->state |= PLAYER_STATE_FIRE;
	}

	if (kbd_key_pressed (SDLK_LEFT))
		p->rot_y -= 0.5f;
	if (kbd_key_pressed (SDLK_RIGHT))
		p->rot_y += 0.5f;
	if (kbd_key_pressed (SDLK_UP)) {
		speed = 1;
		p->state |= PLAYER_STATE_WALK;
	}
	if (kbd_key_pressed (SDLK_DOWN)) {
		speed = -1;
		p->state |= PLAYER_STATE_WALK;
	}
#endif

	p->pos_x += sinf (M_PI/180 * -p->rot_y) * (speed * PLAYER_SPEED);
	p->pos_y += cosf (M_PI/180 * -p->rot_y) * (speed * PLAYER_SPEED);
}

int logic_thread (void *unused)
{
	for (;;) {
		player_t *p = player_get ();

		logic_motion (p);

		player_t *r;
		for (r = player_list.next; r != &player_list; r = r->next)
			if (r != p)
				bot_motion (p, r);

		/* LEVEL RENDERING */
		level_t *l = level_get ();

		bool col = false;
		for (unsigned x = 0; x < l->dim_x; x ++) {
			for (unsigned y = 0; y < l->dim_y; y ++) {
				unsigned char b = l->data[y*l->dim_x + x];

				if (b == ' ')
					continue;

				if (b != '0') {
					float fx = (int) -x;
					float fy = (int) -y;

					logic_collision (p, fx*WALL_DIM, fy*WALL_DIM);

					for (r = player_list.next; r != &player_list; r = r->next)
						if (r != p)
							logic_collision (r, fx*WALL_DIM, fy*WALL_DIM);
				}
			}
		}	

		SDL_Delay (1);
	}

	return 0;
}

bool logic_init ()
{
	if (!SDL_CreateThread (logic_thread, NULL, NULL)) {
		fprintf (stderr, "Unable to create thread: %s\n", SDL_GetError ());
		return false;
	}

	return true;
}

void logic_deinit ()
{
	
}