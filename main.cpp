#include "particle.h"
#include "shooter.h"
#include "player.h"
#include "event.h"
#include "level.h"
#include "logic.h"
#include "tex.h"
#include "gl.h"

SDL_Window *g_window;

bool init ()
{
	LOGI ("Shooter initialization\n");

	if (level_init () == false) {
		LOGI ("Unable to initialize Level subsystem\n");
		return false;
	}

	if (player_init () == false) {
		LOGI ("Unable to initialize Player subsystem\n");
		return false;
	}

	if (SDL_Init (SDL_SUBSYSTEMS) == -1) {
		LOGI ("Unable to initialize SDL: %s\n", SDL_GetError ());
		return false;
	}
	
#ifdef FSAA
	SDL_GL_SetAttribute (SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute (SDL_GL_MULTISAMPLESAMPLES, FSAA);
#endif

  	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 1);

	g_window = SDL_CreateWindow ("Shooter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
						WIN_WIDTH, WIN_HEIGHT, WIN_FLAGS);
  
  	// Create an OpenGL context associated with the window.
  	SDL_GLContext glcontext = SDL_GL_CreateContext (g_window);

	glewInit ();

	if (!GLEW_VERSION_3_3) {
		printf ("WARNING -> your graphic card does not support OpenGL 3.3+\n");
		printf ("-> switching to GLSL 3.0 ES\n");
	}
	
	if (!gl_init ())	// Inicializace OpenGL
		return false;
	
	if (!tex_init ()) {
		LOGI ("Unable to initialize Texture subsystem\n");
		return false;
	}

	if (!particle_init ())
		return false;
	
	gl_resize (WIN_WIDTH, WIN_HEIGHT);// Nastavi perspektivu
#ifndef ANDROID
	SDL_ShowCursor (SDL_DISABLE);
	SDL_SetWindowGrab (g_window, SDL_TRUE);
	SDL_SetRelativeMouseMode (SDL_TRUE);
#endif
	if (!logic_init ()) {
		LOGI ("Unable to initialize Logic subsystem\n");
		return false;
	}

	return true;
}

void deinit ()
{
	logic_deinit ();
	tex_deinit ();
	level_deinit ();

	SDL_Quit ();
}

int SDL_main (int argc, char *argv[])
{
	if (!init ())
		return -1;

	bool f = false;
	while (!f) {
		f = !event_handler (); 	// Osetri udalosti

		gl_render ();		// Rendering
	}
	
	deinit ();

	return 0;
}
