#include "shooter.h"
#include "mouse.h"

static mouse_t mouse;
/*
onNativeTouch(int touchDevId, int pointerFingerId,
                                            int action, float x, 
                                            float y, float p);*/
#ifdef ANDROID
int mouse_last_pos[2] = { 0, 0 };
int mouse_abs_pos[2] = { 0, 0 };
int mouse_ang_pos[2] = { 0, 0 };
unsigned char mouse_switch = 0;

extern "C" {
	JNIEXPORT void JNICALL Java_org_libsdl_app_SDLActivity_onNativeTouch (JNIEnv *env, jobject obj, jint devid, jint pfid, jint act, jfloat x, jfloat y, jfloat p);
};

JNIEXPORT void JNICALL Java_org_libsdl_app_SDLActivity_onNativeTouch (JNIEnv *env, jobject obj, jint devid, jint pfid, jint act, jfloat x, jfloat y, jfloat p)
{
	mouse.state = act;
	int m_x = (int) (x*32768);
	int m_y = (int) (y*32768);

	if (act == 0) {
		mouse_last_pos[0] = m_x;
		mouse_last_pos[1] = m_y;
	}

	mouse_abs_pos[0] = m_x;
	mouse_abs_pos[1] = m_y;
}
#endif

mouse_t *mouse_get ()
{
#ifndef ANDROID
	mouse.state = SDL_GetRelativeMouseState (&mouse.x, &mouse.y);
#else
	mouse.x = mouse_abs_pos[0] - mouse_last_pos[0];
	mouse.y = mouse_abs_pos[1] - mouse_last_pos[1];

	mouse.abs_x = mouse_abs_pos[0];
	mouse.abs_y = mouse_abs_pos[1];

	mouse_last_pos[0] = mouse_abs_pos[0];
	mouse_last_pos[1] = mouse_abs_pos[1];
#endif
	return &mouse;
}

