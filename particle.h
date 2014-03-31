#ifndef __particle_h__
#define __particle_h__

typedef struct {
	float x, y, z;	// pozice castice
	float l;	// lifetime castice

	float u, v;	// smer letu castice [uhel]
	float s; // rychlost
	float t; //cas
} particle_t;

typedef struct {
	particle_t *list;
	
	unsigned count;
	unsigned div_factor;
		
	float l_delta;
	
	float param[5];
} part_sys_t;

extern part_sys_t *part_sys_get (unsigned id);
extern void particle_reset (part_sys_t *s, float x, float y, float z, float u, float v);
extern void particle_update_ballistic (part_sys_t *s);
extern void particle_system_render ();
extern bool particle_init ();


#endif
