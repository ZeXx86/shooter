#include "shooter.h"
#include "player.h"
#include "level.h"
#include "tex.h"
#include "gl.h"
#include "shader.h"
#include "camera.h"

#include "spatter.h"

static GLuint vbo_spatter_id;

static spatter_sys_t spatter_sys[1];

static GLuint shader[1];

#define SPATTER_LIST_SIZE	10


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
	spatter_sys_alloc (&spatter_sys[0], SPATTER_LIST_SIZE);
	spatter_reset();
	gl_init_spatter();
	
	return true;
}

void spatter_reset ()
{
	spatter_sys_t *s = &spatter_sys[0];
	for (int i = 0; i < s->count; i ++) {
		if (s->list[i].l > 0)
			continue;
		
		s->list[i].x = (float) ((rand () ) / ((float) RAND_MAX));
		s->list[i].y = (float) ((rand () ) / ((float) RAND_MAX));
		s->list[i].l = 0.0f;		
	}
	
	
}

void spatter_apply()
{
	spatter_sys_t *s = &spatter_sys[0];
	for (int i = 0; i < s->count; i ++) {
		//resurection
		if (s->list[i].l <= 0.0)
		{
			s->list[i].x = (float) ((rand () ) / ((float) RAND_MAX));
			s->list[i].y = (float) ((rand () ) / ((float) RAND_MAX));
			s->list[i].l = 1.0f;
			break;
		}
	}
}

void spatter_update ()
{
	spatter_sys_t *s = &spatter_sys[0];
	float l_delta = 0.001;
	for (int i = 0; i < s->count; i ++) {	
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


void render_spatter(int i)
{
	glUseProgram (shader[0]);
	glm::mat4 ortho = glm::ortho (0.0f,2.0f,0.0f,2.0f,-1.0f,1.0f) *
			  glm::translate(glm::vec3(spatter_sys[0].list[i].x,spatter_sys[0].list[i].y,0.0f));
	
	int uniform = glGetUniformLocation (shader[0], "PMatrix");
	glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*) &ortho);

	uniform = glGetUniformLocation (shader[0], "life");
	glUniform1f (uniform,spatter_sys[0].list[i].l);

	
	GLuint tex_id  = glGetUniformLocation (shader[0], "TexSampler");
	glUniform1i (tex_id, 0);
	glBindTexture (GL_TEXTURE_2D, tex_get (7));
	
	gl_render_spatter();
	
	glUseProgram (0);
}

void render_spatters()
{
	for(int i = 0; i < SPATTER_LIST_SIZE; i++)
	{
		if(spatter_sys[0].list[i].l>0.0f)
		{
			render_spatter(i);
		}
	}
}
