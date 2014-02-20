#include "shooter.h"
#include "kbd.h"
#include <stdio.h>
static kbd_t kbd;

static bool kbmap[323] = { false };

void kbd_handler (SDL_Event event)
{
	kbd.event = event;
	//printf ("%d : %s\n", event.key.keysym.scancode, SDL_GetKeyName( event.key.keysym.sym ));

	if (event.type == SDL_KEYDOWN)
		kbmap[event.key.keysym.scancode] = true;
	if (event.type == SDL_KEYUP)
		kbmap[event.key.keysym.scancode] = false;
}

bool kbd_key_pressed (unsigned short key)
{
	return kbmap[key];
}

