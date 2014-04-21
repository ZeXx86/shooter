#ifndef __shooter_h__
#define __shooter_h__

#include <vector>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO

#ifndef ANDROID
# define WIN_FLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
# define WIN_BPP 0
# define FSAA 4

#define SDL_main main

#define  LOGI(...) printf (__VA_ARGS__ + '\n')
#else
# define WIN_FLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
# define WIN_WIDTH 480
# define WIN_HEIGHT 800
# define WIN_BPP 0
# define FSAA 0

#include <jni.h>
#include <android/log.h>
#define  LOG_TAG    "Shooter"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

#endif

#define WIN_TITLE "OpenGL Shooter"

#ifdef ANDROID
# define PATH_DATA	"/sdcard/shooter/data/"
#else
# define PATH_DATA	"./data/"
#endif

#ifndef __WIN32__
# define CALL_SETVIDEOMODE_WHEN_RESIZING
#endif

using namespace std;

extern SDL_Window *g_window;

#endif

