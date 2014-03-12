#ifndef __camera_h__
#define __camera_h__

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define NEAR_PLANE 	0.15
#define FAR_PLANE 	100.0

typedef struct {
	int viewport_x;
	int viewport_y;

	int window_width;
	int window_height;

	double aspect;
	float fov;
	double near_clip;
	double far_clip;
	
	float heading;
	glm::vec3 position;
	
	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 model;
	glm::mat4 MVP;
} camera_t;

extern camera_t *camera_get ();
extern void camera_position (glm::vec3 pos);
extern void camera_update ();
extern bool camera_init (int viewport_x, int viewport_y, int window_width, int window_height);
extern void camera_deinit ();

#endif

