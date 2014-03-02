#include "shooter.h"
#include "kbd.h"
#include <stdio.h>
static kbd_t kbd;

static bool kbmap[323] = { false };

void kbd_handler (SDL_Event event)
{
	if (event.key.keysym.sym >= sizeof (kbmap))
		return;
	
	kbd.event = event;

	/* znacime si stisk a pusteni klaves do mapy,
	 * abychom mohli pracovat teoreticky se vsemi klavesy naraz */
	if (event.type == SDL_KEYDOWN)
		kbmap[event.key.keysym.sym] = true;
	if (event.type == SDL_KEYUP)
		kbmap[event.key.keysym.sym] = false;
}

bool kbd_key_pressed (unsigned short key)
{
	return kbmap[key];
}

