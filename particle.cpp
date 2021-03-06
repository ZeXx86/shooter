#include "shooter.h"
#include "player.h"
#include "level.h"
#include "tex.h"
#include "gl.h"
#include "shader.h"
#include "camera.h"

#include "particle.h"

static GLuint vbo_particle_id;
static GLuint shader[1];

static part_sys_t part_sys[2];

#define PARTICLE_LIST_SIZE	1024

part_sys_t *part_sys_get (unsigned id)
{
	return &part_sys[id];
}

static bool particle_list_alloc (part_sys_t *s, unsigned count, unsigned div_factor, float l_delta, float gravity)
{
	s->list = (particle_t *) malloc (sizeof (particle_t) * count);
	
	if (!s->list)
		return false;
	
	s->count = count;
	s->div_factor = div_factor;
	s->l_delta = l_delta;
	s->gravity = gravity;
	
	return true;
}

int particle_thread (void *unused)
{
	part_sys_t *part_blood = part_sys_get (0);
	level_t *l = level_get ();
	
	for (;;) {
		bool col = false;
		for (unsigned x = 0; x < l->dim_x; x ++) {
			for (unsigned y = 0; y < l->dim_y; y ++) {
				unsigned char b = l->data[y*l->dim_x + x];

				if (b == ' ')
					continue;

				if (b != '0') {
					float fx = (int) x;
					float fy = (int) y;

					particle_collision_level (part_blood, fx*WALL_DIM, fy*WALL_DIM);
				}
			}
		}
	}
}

bool particle_init ()
{
	shader[0] = shader_init ("data/shader_part");
	
	/* Casticovy system [0] - rozstrik krve */
	particle_list_alloc (&part_sys[0], PARTICLE_LIST_SIZE, 4, 0.001f, 9.8f);
	/* Casticovy system [1] - rozprsk krve */
	particle_list_alloc (&part_sys[1], PARTICLE_LIST_SIZE, 1, 0.0005f, 0.2f);
	
	// prvotni umisteni generatoru castic na pozici 5,0,5
	//particle_reset (&part_sys[0], 5, 0, 5, 0, 0);
	
	glEnable (GL_POINT_SPRITE);
	glEnable (GL_PROGRAM_POINT_SIZE);
	
	glBlendFunc (GL_SRC_ALPHA, GL_ONE);
	
	if (!SDL_CreateThread (particle_thread, NULL, NULL)) {
		fprintf (stderr, "Unable to create thread: %s\n", SDL_GetError ());
		return false;
	}

	return true;
}

void particle_reset (part_sys_t *s, float x, float y, float z)
{
	for (int i = 0; i < s->count; i ++) {
		if (s->list[i].l > 0)
			continue;
		
		s->list[i].x = x;
		s->list[i].y = y;
		s->list[i].z = z;

		s->list[i].s = 0;

		s->list[i].u = 0;
		s->list[i].v = 0;

		s->list[i].t = 0.0f;
		s->list[i].l = 1;//(float) (rand () % s->div_factor) / s->div_factor;
		
		break;
	}
}

void particle_reset_ballistic (part_sys_t *s, float x, float y, float z, float u, float v, float nm)
{
	for (int i = 0; i < s->count; i ++) {
		if (s->list[i].l > 0)
			continue;
		
		s->list[i].x = x;
		s->list[i].y = y;
		s->list[i].z = z;
		
		//s->list[i].sz = s->list[i].sy = s->list[i].sx = s->list[i].s = 0.005f * (((rand () % RAND_MAX) / (float) RAND_MAX) + 0.1f);

		float alfa = (((rand () % RAND_MAX) / (float) RAND_MAX) + 0.1f);
		
		s->list[i].s = alfa / nm;
		
		float theta = ((float) ((rand () % RAND_MAX) / ((float) RAND_MAX)) + 1) * 2 * M_PI;
		float r = sqrtf ((float) ((rand () % RAND_MAX) / ((float) RAND_MAX))) * (nm * alfa);

		s->list[i].u = r * cosf (theta) + u;
		s->list[i].v = r * sinf (theta) + v;

		
		s->list[i].t = 0.0f;
		s->list[i].l = (float) (rand () % s->div_factor) / s->div_factor;
	}
}

void particle_collision_level (part_sys_t *s, float x, float y)
{
	for (int i = 0; i < s->count; i ++) {
		if (s->list[i].y > 1.0f || s->list[i].y < -1.0f)
			continue;
		
		float dim = WALL_DIM - 0.95f;
		float dim_col = 2*WALL_DIM - 2.0f;

		float a = s->list[i].x - (x-dim);
		float b = s->list[i].x - (x+dim);
		float c = s->list[i].z - (y-dim);
		float d = s->list[i].z - (y+dim);

		float angle = atan2f (y, x) - atan2f (s->list[i].z, s->list[i].x);// atan2(v1.y,v1.x)

		bool collide = false;
		float newangle = s->list[i].u;
		
		if (a > 0 && b < 0 && c > 0 && d < 0) {
			if (d <= -dim_col) {
				//newangle = angle;
				s->list[i].u -= 90;
				collide = true;
			} else if (c >= dim_col) {
				//newangle = angle;
				s->list[i].u -= 90;
				collide = true;
			} else if (b <= -dim_col) {
				//newangle = angle;
				s->list[i].u -= 90;
				collide = true;
			} else if (a >= dim_col) {
				//newangle = angle;
				s->list[i].u -= 90;
				collide = true;
			}
		}
		
		//if (collide == false && s->list[i].y < -0.90f)
		//	collide = true;
		
		if (collide)
			particle_reset (&part_sys[1], s->list[i].x, s->list[i].y, s->list[i].z);
		
		//s->list[i].u = angle;
	}
}

void particle_update_ballistic (part_sys_t *s)
{
	for (int i = 0; i < s->count; i ++) {	
		s->list[i].x += sinf (M_PI/180 * -s->list[i].u) * s->list[i].s;
		s->list[i].z += cosf (M_PI/180 * -s->list[i].u) * s->list[i].s;
		s->list[i].y += sinf (M_PI/180 * -s->list[i].v) * s->list[i].s - (s->gravity * pow (s->list[i].t, 2.0f) / 2.0f);
		s->list[i].t += 0.0001f;		// 1/s

		if (s->list[i].l > 0)
			s->list[i].l -= s->l_delta;
	}
}

void particle_render (part_sys_t *s)
{
	glBindTexture (GL_TEXTURE_2D, tex_get (6));
	glEnable (GL_BLEND); 
	glDepthMask (GL_FALSE);
	
	glEnableVertexAttribArray (0);
	glEnableVertexAttribArray (1);
		
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, sizeof (particle_t), s->list);
	glVertexAttribPointer (1, 1, GL_FLOAT, GL_FALSE, sizeof (particle_t), (void *) (s->list+3*sizeof (float)));	

	glDrawArrays (GL_POINTS, 0, PARTICLE_LIST_SIZE);
	
	glDisableVertexAttribArray (0);
	glDisableVertexAttribArray (1);
	glDisable (GL_BLEND);
	glDepthMask (GL_TRUE);
}

void particle_system_render (part_sys_t *s)
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

	player_t *p = player_get ();
	
	glm::vec3 campos = glm::vec3 (-p->pos_x, 0, -p->pos_y);
	uniform = glGetUniformLocation (shader[0], "CameraPos");
	glUniform3fv (uniform, 1, (float*)&campos[0]);
	
	GLuint tex_id  = glGetUniformLocation (shader[0], "TexSampler");
	glUniform1i (tex_id, 0);

	particle_render (s);
			
	/* disable program */
	glUseProgram (0);
}


