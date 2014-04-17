#include "shooter.h"
#include "player.h"
#include "level.h"
#include "tex.h"
#include "gl.h"
#include "shader.h"
#include "camera.h"
#include "sky.h"

static GLuint vbo_sky_id;
static GLuint shader[1];

bool sky_init ()
{
	shader[0] = shader_init ("data/shader_sky");	
	
	const GLfloat buf[] = { 
		-1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0, 0.0f,
		 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, -1.0, 0.0f,
		 1.0f, 0.0f,  1.0f, 1.0f, 1.0f, 0.0f, -1.0, 0.0f,

		 1.0f, 0.0f,  1.0f, 1.0f, 1.0f, 0.0f, -1.0, 0.0f,
		-1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f, -1.0, 0.0f,
		-1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0, 0.0f
	};
	
	glGenBuffers (1, &vbo_sky_id);
	glBindBuffer (GL_ARRAY_BUFFER, vbo_sky_id);
	glBufferData (GL_ARRAY_BUFFER, sizeof (buf), buf, GL_STATIC_DRAW);
	
	return true;
}


void sky_render ()
{
	glBindBuffer (GL_ARRAY_BUFFER, vbo_sky_id);

	glEnableVertexAttribArray (0);
	glEnableVertexAttribArray (1);
	glEnableVertexAttribArray (2);
	
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, (8 * sizeof(GLfloat)), 0);
	glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, (8 * sizeof(GLfloat)), (GLvoid *) (3 * sizeof(GLfloat)));
	glVertexAttribPointer (2, 3, GL_FLOAT, GL_FALSE, (8 * sizeof(GLfloat)), (GLvoid *) (5 * sizeof(GLfloat)));
	
	glDrawArrays (GL_TRIANGLES, 0, 6);
	
	glDisableVertexAttribArray (0);
	glDisableVertexAttribArray (1);
	glDisableVertexAttribArray (2);
	
	glBindBuffer (GL_ARRAY_BUFFER, 0);
}

void sky_system_render ()
{
	camera_t *cam = camera_get ();
	level_t *l = level_get ();
	
	glm::mat4 mdl_matrix;
	mdl_matrix = glm::translate (glm::vec3 (l->dim_x-WALL_DIM/2, 1, l->dim_y-WALL_DIM/2)) * glm::scale (glm::vec3 (l->dim_x, 1, l->dim_x));
			
	/* enable program and set uniform variables */
	glUseProgram (shader[0]);
	
	glm::mat4 tmp = cam->view * mdl_matrix;

	int uniform = glGetUniformLocation (shader[0], "PMatrix");
	glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*)&cam->projection[0]);
	uniform = glGetUniformLocation (shader[0], "VMatrix");
	glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*)&cam->view[0]);
	uniform = glGetUniformLocation (shader[0], "MVMatrix");
	glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*)&tmp[0]);
	uniform = glGetUniformLocation (shader[0], "NormalMatrix");
	glUniformMatrix3fv (uniform, 1, GL_FALSE, (float*)&(glm::inverseTranspose(glm::mat3(tmp)))[0]);

	float time = (float) SDL_GetTicks () / 1000.f;
	float noiseIteration = 30;
	glm::vec3 noisePosition = glm::vec3 (0, 0, (float) SDL_GetTicks () / 3000.f);
	float disp = 0.0f;
	
	uniform = glGetUniformLocation (shader[0], "time");
	glUniform1f (uniform, time);
	uniform = glGetUniformLocation (shader[0], "noiseIteration");
	glUniform1f (uniform, noiseIteration);
	uniform = glGetUniformLocation (shader[0], "disp");
	glUniform1f (uniform, disp);
	uniform = glGetUniformLocation (shader[0], "noisePosition");
	glUniform3fv (uniform, 1, (float*)&noisePosition[0]);
	
	GLuint tex_id  = glGetUniformLocation (shader[0], "MainTex");
	glUniform1i (tex_id, 0);

	sky_render ();
			
	/* disable program */
	glUseProgram (0);
}


