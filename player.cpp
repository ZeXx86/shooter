#include "shooter.h"
#include "player.h"

player_t player_list;

static int pos_boot_x;

player_t *player_add ()
{
	/* alloc and init context */
	player_t *p = (player_t *) malloc (sizeof (player_t));

	if (!p)
		return 0;
	
	p->state = 0;
	p->hp = 100;

	p->pos_x = (pos_boot_x -= 8) / 2;
	p->pos_y = pos_boot_x / 2;

	p->rot_y = 0;

	/* add into list */
	p->next = &player_list;
	p->prev = player_list.prev;
	p->prev->next = p;
	p->next->prev = p;

	return p;
}

player_t *player_get ()
{
	return player_list.next;
}

bool player_init ()
{
	player_list.next = &player_list;
	player_list.prev = &player_list;

	/* our player */
	player_add ();

	player_add ();
	player_add ();
	player_add ();
	
	return true;
}
