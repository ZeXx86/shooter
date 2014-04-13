#ifndef __particle_2d_h__
#define __particle_2d_h__

typedef struct {
	float x, y, z;	// pozice castice
	float l;	// zivotnost castice
	float t; 	// cas	[t]
} spatter_t;

typedef struct {
	spatter_t *list;
	unsigned count;
} spatter_sys_t;


extern bool spatter_init ();
extern void spatter_reset (spatter_sys_t *s, float x, float y);
extern void spatter_update (spatter_sys_t *s);
//extern void spatter_render ();
//extern void spatter_sys_render ();
extern void render_spatter ();
extern void gl_init_spatter ();
#endif
