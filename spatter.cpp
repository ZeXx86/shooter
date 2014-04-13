#include "shooter.h"
#include "player.h"
#include "level.h"
#include "tex.h"
#include "gl.h"
#include "shader.h"
#include "camera.h"

#include "spatter.h"

static GLuint vbo_spatter_id;

static spatter_sys_t part_sys[1];

static GLuint shader[1];

#define PARTICLE_LIST_SIZE	1


static bool spatter_sys_alloc (spatter_sys_t *s, unsigned count)
{
	s->list = (spatter_t *) malloc (sizeof (spatter_t) * count);
	
	if (!s->list)
		return false;
	
	s->count = count;
	
	return true;
}


bool spatter_init ()
{
	spatter_sys_alloc (&part_sys[0], PARTICLE_LIST_SIZE);
	return true;
}

void spatter_reset (spatter_sys_t *s, float x, float y)
{
	for (int i = 0; i < s->count; i ++) {
		if (s->list[i].l > 0)
			continue;
		
		s->list[i].x = x;
		s->list[i].y = y;
		s->list[i].t = 0.0f;
		s->list[i].l = 1000000000000000.0f;		
	}
}

void spatter_update (spatter_sys_t *s)
{

	float l_delta = 0.001;
	for (int i = 0; i < s->count; i ++) {	
		
		s->list[i].t += 0.0001f;		// 1/s

		if (s->list[i].l > 0)
			s->list[i].l -= l_delta;
	}
}



void gl_init_spatter()
{
	shader[0] = shader_init ("data/shader_spatter");
	//x y z u v
	const GLfloat buf[] = {
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f
	};

	glGenBuffers (1, &vbo_spatter_id);
	glBindBuffer (GL_ARRAY_BUFFER, vbo_spatter_id);
	glBufferData (GL_ARRAY_BUFFER, sizeof (buf), buf, GL_STATIC_DRAW);

}

void gl_render_spatter ()
{
	glEnable (GL_BLEND); 

	glBindBuffer (GL_ARRAY_BUFFER, vbo_spatter_id);
	
	glEnableVertexAttribArray (0);
	glEnableVertexAttribArray (1);

	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, (5 * sizeof(GLfloat)), 0);
	glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, (5 * sizeof(GLfloat)), (GLvoid *) (3 * sizeof(GLfloat)));
	
	glDrawArrays (GL_TRIANGLES, 0, 6);
	
	glDisableVertexAttribArray (0);
	glDisableVertexAttribArray (1);
	
	glBindBuffer (GL_ARRAY_BUFFER, 0);
	glDisable (GL_BLEND);
}


void render_spatter()
{
	float x,y = 0.0f;
	glUseProgram (shader[0]);
	glm::mat4 ortho = glm::ortho (0.0f,2.f,0.f,2.f,-1.f,1.f)*glm::translate(glm::vec3(x,y,0.0f));
	int uniform = glGetUniformLocation (shader[0], "PMatrix");
	glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*) &ortho);
	GLuint tex_id  = glGetUniformLocation (shader[0], "TexSampler");
	glUniform1i (tex_id, 0);
	glBindTexture (GL_TEXTURE_2D, tex_get (7));
	gl_render_spatter();
	glUseProgram (0);
}
