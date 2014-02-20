#include "shooter.h"
#include "player.h"
#include "level.h"
#include "tex.h"
#include "mdl/mdl.h"
#include "gl.h"


static float fps_stick, fps_dtick;

/*** An MDL model ***/
struct mdl_model_t mdlfile[3];

bool gl_init ()
{
	glClearColor (0.0, 0.0, 0.0, 0.0);
#ifdef ANDROID
	glClearDepthf (1.0);
#else
	glClearDepth (1.0);
#endif
	glDepthFunc (GL_LEQUAL);
	glEnable (GL_DEPTH_TEST);
	glShadeModel (GL_SMOOTH);
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable (GL_TEXTURE_2D);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_BLEND);

	/* Load MDL model file */
	if (!mdl_read (PATH_DATA "v_shot.mdl", &mdlfile[0]))
		return false;

	if (!mdl_read (PATH_DATA "player1.mdl", &mdlfile[1]))
		return false;

	return true;
}


static void gluPerspective (GLfloat fovy, GLfloat aspect,
               GLfloat zNear, GLfloat zFar)//android ndk lacks glu tool kit (unbelievable)
{
    #define PI 3.1415926535897932f
    GLfloat xmin, xmax, ymin, ymax;

    ymax = zNear * (GLfloat)tan(fovy * PI / 360);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;

#ifndef ANDROID
glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
#else
    glFrustumx((GLfixed)(xmin * 65536), (GLfixed)(xmax * 65536),
               (GLfixed)(ymin * 65536), (GLfixed)(ymax * 65536),
               (GLfixed)(zNear * 65536), (GLfixed)(zFar * 65536));
#endif
    #undef PI
}

void gl_resize (int width, int height)
{
	if (height == 0)
		height = 1;

	glViewport (0, 0, width, height);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	gluPerspective (60.0, (GLfloat) width / (GLfloat) height, NEAR_PLANE, FAR_PLANE);

	glMatrixMode (GL_MODELVIEW);
}

float gl_fps_get ()
{
	if (!fps_dtick)
		return 1000.0f;

	return 1000.0f / fps_dtick;
}

void gl_render_cube ()
{
	const GLfloat vert[] = { 
		/*// front
		-1.0f, -1.0f, 1.0f,
		 1.0f, -1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 1.0f,

		// back
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		// right
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f,  1.0f,

		// left
		-1.0f, -1.0f, -1.0f, 
		-1.0f, -1.0f, 1.0f, 
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,*/

		// front
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		// back
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,

		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,

		// right
		1.0f, -1.0f, -1.0f, 
		1.0f, -1.0f, 1.0f, 
		1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 1.0f, 
		1.0f, 1.0f, -1.0f, 
		1.0f, -1.0f, -1.0f,

		// left
		-1.0f, -1.0f, -1.0f, 
		-1.0f, -1.0f, 1.0f, 
		-1.0f, 1.0f, 1.0f,

		-1.0f, 1.0f, 1.0f, 
		-1.0f, 1.0f, -1.0f, 
		-1.0f, -1.0f, -1.0f
	};

	static const GLfloat tex[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f
    	};

	//glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState (GL_VERTEX_ARRAY);
	glEnableClientState (GL_TEXTURE_COORD_ARRAY);

	glVertexPointer (3, GL_FLOAT, 0, vert);
	glTexCoordPointer (2, GL_FLOAT, 0, tex);

	glDrawArrays (GL_TRIANGLES, 0, 24);

   	glDisableClientState (GL_VERTEX_ARRAY);
	glDisableClientState (GL_TEXTURE_COORD_ARRAY);
}

void gl_render_floor ()
{
	static const GLfloat vert[] = { 
		-1.0f, -1.0f, -1.0f, 
		 1.0f, -1.0f, -1.0f, 
		 1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, 1.0f, 
		-1.0f, -1.0f, 1.0f, 
		-1.0f, -1.0f, -1.0f,
	};

	static const GLfloat tex[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f
    	};

	
	glEnableClientState (GL_VERTEX_ARRAY);
	glEnableClientState (GL_TEXTURE_COORD_ARRAY);

	glVertexPointer (3, GL_FLOAT, 0, vert);
	glTexCoordPointer (2, GL_FLOAT, 0, tex);

	glDrawArrays (GL_TRIANGLES, 0, 6);

   	glDisableClientState (GL_VERTEX_ARRAY);
	glDisableClientState (GL_TEXTURE_COORD_ARRAY);
}

bool gl_frustum (player_t *p, float x, float y)
{
	float a = p->rot_y-90;

	float xn = p->pos_x * cosf (M_PI/180 * a) - p->pos_y * sinf (M_PI/180 * a);

	if (xn < x-(WALL_DIM*2))
		return true;

	return false;
}

void gl_render_weapon (player_t *p)
{
	if (p->state & PLAYER_STATE_FIRE) {
		p->state &= ~PLAYER_STATE_FIRE;

		/* new shot */
		if (p->mdl_frame == 5)
			p->mdl_frame = 0;
	}

	if (p->mdl_frame < 5)
		p->mdl_itp += fps_dtick / 5;

 	mdl_animate (0, mdlfile[0].header.num_frames - 1, &p->mdl_frame, &p->mdl_itp);

	glPushMatrix ();
		glTranslatef (0.08f, -0.03, -0.18f);
		glRotatef (90, 0, 1, 0);
		glRotatef (-90, 1, 0, 0);
	
		glScalef (0.01f, 0.01f, 0.01f);
		mdl_renderitp (p->mdl_frame, p->mdl_itp, &mdlfile[0]);
	glPopMatrix ();
}

void gl_render_players (player_t *p)
{
	player_t *l;
	for (l = player_list.next; l != &player_list; l = l->next) {
		if (p == l)
			continue;
#ifndef ANDROID
	  	l->mdl_itp += fps_dtick / 5;
#else
		l->mdl_itp += fps_dtick / 100;
#endif
	 	mdl_animate (0, mdlfile[1].header.num_frames - 1, &l->mdl_frame, &l->mdl_itp);
	
		glPushMatrix ();
			glTranslatef (-l->pos_x, -0.4, -l->pos_y);
			glRotatef (l->rot_y-90, 0, 1, 0);
			glRotatef (-90, 1, 0, 0);
			glScalef (0.023f, 0.023f, 0.023f);
			mdl_renderitp (l->mdl_frame, l->mdl_itp, &mdlfile[1]);
		glPopMatrix ();
		
		switch (l->state) {
			case PLAYER_STATE_WALK:
				if (l->mdl_frame > 4)
					l->mdl_frame = 0;
				break;
			default:
				if (l->mdl_frame > 27)
					l->mdl_frame = 17;
				else if (l->mdl_frame < 17)
					l->mdl_frame = 17;
				break;
		}
	}
}


void gl_render_level ()
{
	/* LEVEL RENDERING */
	level_t *l = level_get ();

	bool col = false;
	for (unsigned x = 0; x < l->dim_x; x ++) {
		for (unsigned y = 0; y < l->dim_y; y ++) {
			unsigned char b = l->data[y*l->dim_x + x];

			if (b == ' ')
				continue;

			glPushMatrix ();
				glTranslatef (x*WALL_DIM, 0, y*WALL_DIM);


				switch (b) {
					case '0':
						glBindTexture (GL_TEXTURE_2D, tex_get (0));
						break;
					case '1':
						glBindTexture (GL_TEXTURE_2D, tex_get (1));
						break;
					case '2':
						glBindTexture (GL_TEXTURE_2D, tex_get (2));
						break;
				}

				if (b == '0')
					gl_render_floor ();
				else
					gl_render_cube ();
	
			glPopMatrix ();
		}
	}
}

void gl_render ()
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity ();

	player_t *p = player_get ();

	/* Weapon */
	gl_render_weapon (p);

	/* scene motion */
	glRotatef (p->rot_y, 0, 1, 0);
	glTranslatef (p->pos_x, 0, p->pos_y);

	gl_render_players (p);

	gl_render_level ();

	glFlush ();
	//SDL_GL_SwapBuffers ();// Prohodi predni a zadni buffer
	SDL_GL_SwapWindow (g_window);

	fps_dtick = SDL_GetTicks () - fps_stick;

	if (1000/FPS_MAX > fps_dtick)
		SDL_Delay (1000/FPS_MAX - fps_dtick);
	
	fps_stick = SDL_GetTicks ();
}

