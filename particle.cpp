#include "shooter.h"
#include "shader.h"
#include "tex.h"
#include "gl.h"
#include "particle.h"

static GLuint vbo_particle_id;
static GLuint shader[1];

bool particle_init ()
{
	shader[0] = shader_init ("data/shader_part");
	
	const GLfloat buf[] = { 
		//x, y, z, u ,v, nx, ny, nz
		-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	};
	
	glGenBuffers (1, &vbo_particle_id);
	glBindBuffer (GL_ARRAY_BUFFER, vbo_particle_id);
	glBufferData (GL_ARRAY_BUFFER, sizeof (buf), buf, GL_STATIC_DRAW);
	
	return true;
}

void particle_render ()
{
	glBindBuffer (GL_ARRAY_BUFFER, vbo_particle_id);

	glEnableVertexAttribArray (0);
	glEnableVertexAttribArray (1);
	glEnableVertexAttribArray (2);
	
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, (8 * sizeof(GLfloat)), 0);
	glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, (8 * sizeof(GLfloat)), (GLvoid *) (3 * sizeof(GLfloat)));
	glVertexAttribPointer (2, 3, GL_FLOAT, GL_FALSE, (8 * sizeof(GLfloat)), (GLvoid *) (5 * sizeof(GLfloat)));
	
	glDrawArrays (GL_TRIANGLES, 0, 2);
	
	glDisableVertexAttribArray (0);
	glDisableVertexAttribArray (1);
	glDisableVertexAttribArray (2);
	
	glBindBuffer (GL_ARRAY_BUFFER, 0);
}

void particle_system_render ()
{
	
}
