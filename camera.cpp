#include "shooter.h"
#include "camera.h"

static camera_t cam;

camera_t *camera_get ()
{
	return &cam;
}

void camera_update ()
{
	//need to set the matrix state. this is only important because lighting doesn't work if this isn't done
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity ();
	glViewport (cam.viewport_x, cam.viewport_y, cam.window_width, cam.window_height);
	
	/** TransformedVector = TranslationMatrix * RotationMatrix * ScaleMatrix * OriginalVector; **/
	
	cam.view = glm::rotate (cam.heading, glm::vec3 (0, 1, 0)) * glm::translate (cam.position);

	cam.MVP = cam.projection * cam.view * cam.model; // Remember, matrix multiplication is the other way around

	glLoadMatrixf (glm::value_ptr (cam.MVP));
}

bool camera_init (int viewport_x, int viewport_y, int window_width, int window_height)
{
	cam.viewport_x = viewport_x;
	cam.viewport_y = viewport_y;
	cam.window_width = window_width;
	cam.window_height = window_height;
	
	cam.aspect = double (window_width) / double (window_height);

	cam.near_clip = NEAR_PLANE;
	cam.far_clip = FAR_PLANE;
	
	cam.fov = 70;
	
	// Projection matrix : 70° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	cam.projection = glm::perspective (cam.fov, (float) cam.aspect, 0.1f, 100.0f);
	// Camera matrix
	/*cam.view       = glm::lookAt(
		glm::vec3(0,0,0.1f), // Camera is at (4,3,3), in World Space
		glm::vec3(0,0,0), // and looks at the origin
		glm::vec3(0,-1,0)  // Head is up (set to 0,-1,0 to look upside-down)
	);*/
	// Model matrix : an identity matrix (model will be at the origin)
	cam.model      = glm::mat4 (1.0f);  // Changes for each model !
	// Our ModelViewProjection : multiplication of our 3 matrices
	
	//cam.view *= glm::translate (glm::vec3 (1, 0, 0));
	
	
	return true;
}

void camera_deinit ()
{
	
}
