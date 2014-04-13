#ifndef __particle_2d_h__
#define __particle_2d_h__

typedef struct {
	float x, y, z;	// pozice castice
	float l;	// zivotnost castice
} spatter_t;

typedef struct {
	spatter_t *list;
	unsigned count;
} spatter_sys_t;


extern bool spatter_init ();
extern void spatter_reset ();
extern void spatter_update ();
//extern void spatter_render ();
//extern void spatter_sys_render ();
extern void render_spatters ();
extern void gl_init_spatter ();
extern void spatter_update();
extern void spatter_apply();
#endif
