#include "particle.h"
#include "shooter.h"
#include "player.h"
#include "camera.h"
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
	camera_t *c = camera_get ();
	
	float speed = 0;

#ifdef ANDROID
	p->rot_y += m->x / 150.0f;

	speed = m->y < 0 ? 10 : (m->y > 0 ? -10 : 0);

	if (m->state == 0) {
		p->state |= PLAYER_STATE_FIRE;
	}
#else	
	m->x_s += 0.07f * (m->x - m->x_s);

	p->rot_y += m->x_s / 2.0f;
	
	if (m->state & SDL_BUTTON(1)) {
		p->state |= PLAYER_STATE_FIRE;
	}

	if (kbd_key_pressed (SDLK_a))
		p->rot_y -= 0.5f;
	if (kbd_key_pressed (SDLK_d))
		p->rot_y += 0.5f;
	if (kbd_key_pressed (SDLK_w)) {
		speed = 1;
		p->state |= PLAYER_STATE_WALK;
	}
	if (kbd_key_pressed (SDLK_s)) {
		speed = -1;
		p->state |= PLAYER_STATE_WALK;
	}
#endif

	p->pos_x += sinf (M_PI/180 * -p->rot_y) * (speed * PLAYER_SPEED);
	p->pos_y += cosf (M_PI/180 * -p->rot_y) * (speed * PLAYER_SPEED);
	
	/* aktualizace kamery */
	c->heading = p->rot_y;
	c->position[0] = p->pos_x;
	c->position[2] = p->pos_y;	
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

		part_sys_t *part_blood = part_sys_get (0);
		
		/* PARTICLE UPDATE */
		particle_update_ballistic (part_blood);

		if (p->state & PLAYER_STATE_FIRE) {		
			//primka ax+by+c=0
			float a1, b1, a2, b2, c1, c2, x, y;	
			for (r = player_list.next; r != &player_list; r = r->next) {
				if (r != p) {
					a1=cosf(M_PI/180 * p->rot_y);
					b1=sinf(M_PI/180 * p->rot_y);
					c1=a1*p->pos_x+b1*p->pos_y;
			
					a2 = cosf(M_PI/180 * r->rot_y);
					b2 = sinf(M_PI/180 * r->rot_y);
					c2=a2*r->pos_x+b2*r->pos_y;

					y=((a2*c1)/a1-c2)/(-1*(a2*b1)/a1+b2);
					x=(-b1*y-c1)/(a1);

					float sigma = glm::distance(glm::vec2(x,y),glm::vec2(-r->pos_x,-r->pos_y));

					//HIT
					if (sigma<1.0f) {
						//float distance = glm::distance(glm::vec2(-r->pos_x,-r->pos_y),glm::vec2(-p->pos_x,-p->pos_y));
						//printf("%f %f\n",sigma,distance);
						particle_reset(part_blood, -r->pos_x, 0.0f, -r->pos_y, p->rot_y+180, 0);
						//break;
					}
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
