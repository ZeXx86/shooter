#include "shooter.h"
#include "level.h"
#include "tex.h"
#include "gl.h"
#include "shader.h"
#include "camera.h"

#include "particle.h"

static GLuint vbo_particle_id;
static GLuint shader[1];

static particle_t *part_list;

#define PARTICLE_LIST_SIZE	208

bool particle_init ()
{
	shader[0] = shader_init ("data/shader_part");
	
	part_list = (particle_t *) malloc (sizeof (particle_t) * PARTICLE_LIST_SIZE);
	
	if (!part_list)
		return false;
	
/*
% Data
n = 10000;
radius = rand;
xc = randn;
yc = randn;
% Engine
theta = rand(1,n)*(2*pi);
r = sqrt(rand(1,n))*radius;
x = xc + r.*cos(theta);
y = yc + r.*sin(theta);
*/
	float radius = 6;

	for (int i = 0; i < PARTICLE_LIST_SIZE; i ++) {
		part_list[i].x = 5;
		part_list[i].y = 0;
		part_list[i].z = 5;

		float theta = ((float) ((rand () % RAND_MAX) / ((float) RAND_MAX)) + 1) * 2 * M_PI;
		float r = sqrtf ((float) ((rand () % RAND_MAX) / ((float) RAND_MAX))) * radius;

		part_list[i].u = r * cosf (theta);
		part_list[i].v = r * sinf (theta);

		part_list[i].l = 1.0f;
		
	}

	glEnable (GL_POINT_SPRITE);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE);

	return true;
}

void particle_update_ballistic ()
{
	float speed = 0.005f;

	for (int i = 0; i < PARTICLE_LIST_SIZE; i ++) {
		part_list[i].x += sinf (M_PI/180 * -part_list[i].u) * (speed);
		part_list[i].z += cosf (M_PI/180 * -part_list[i].u) * (speed);
		part_list[i].y += sinf (M_PI/180 * -part_list[i].v) * (speed);

		if (part_list[i].l > 0)
			part_list[i].l -= 0.001f;
		
	}
}

void particle_render (float size)
{
	glBindTexture (GL_TEXTURE_2D, tex_get (6));
	glEnable (GL_BLEND); 
	glDepthMask (GL_FALSE);
	
	glEnableVertexAttribArray (0);
	glEnableVertexAttribArray (1);
		
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, sizeof (particle_t), part_list);
	glVertexAttribPointer (1, 1, GL_FLOAT, GL_FALSE, sizeof (particle_t), (void *) ((void *) part_list+3*sizeof (float)));	

	glPointSize (size);
	glDrawArrays (GL_POINTS, 0, PARTICLE_LIST_SIZE);
	
	glDisableVertexAttribArray (0);
	glDisableVertexAttribArray (1);
	glDisable (GL_BLEND);
	glDepthMask (GL_TRUE);
}

void particle_system_render ()
{
	camera_t *cam = camera_get ();
	
	glm::mat4 mdl_matrix;
	mdl_matrix = glm::translate (glm::vec3 (0, 0, 0));
			
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

	GLuint tex_id  = glGetUniformLocation (shader[0], "TexSampler");
	glUniform1i (tex_id, 0);

	particle_render (16.0f);
			
	/* disable program */
	glUseProgram (0);
}
