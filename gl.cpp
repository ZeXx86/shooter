#include "shooter.h"
#include "player.h"
#include "level.h"
#include "tex.h"
#include "mdl/mdl.h"
#include "gl.h"
#include "shader.h"
#include "camera.h"
#include "particle.h"
#include "spatter.h"
#include "sky.h"


static unsigned fps_stick, fps_dtick;

static GLuint vbo_wall_id;
static GLuint vbo_floor_id;
static GLuint fbo_screen_quad_id, fbo_screen_quad_id2;
static GLuint vbo_screen_quad_id;
static GLuint renderTexture, depthTexture, renderTexture2;

static GLuint shader[10];

static GLuint tex_depth;
static GLuint fbo_shadows;

static light_t light1;
static material_t mat1;
static material_t mat2;

void gl_init_wall ();
void gl_init_floor ();
void gl_init_screen_quad ();
void gl_init_shadows ();

glm::mat4 depthMVP;


/*** An MDL model ***/
struct mdl_model_t mdlfile[3];

bool gl_init ()
{
	glClearColor (0.0, 0.0, 0.0, 0.0);
#ifdef ANDROID
	glClearDepthf (1.0);
#else
	glClearDepth (1.0);
#endif
	glDepthFunc (GL_LEQUAL);
	glEnable (GL_DEPTH_TEST);
	glShadeModel (GL_SMOOTH);
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable (GL_TEXTURE_2D);

	/* Load MDL model file */
	if (!mdl_read (PATH_DATA "v_shot.mdl", &mdlfile[0]))
		return false;

	if (!mdl_read (PATH_DATA "player1.mdl", &mdlfile[1]))
		return false;
	
	gl_init_shadows ();
	gl_init_wall ();
	gl_init_floor ();
	gl_init_screen_quad ();
	
	shader[0] = shader_init ("data/shader_bot");
	shader[1] = shader_init ("data/shader_gun");
	shader[2] = shader_init ("data/shader_level");
	shader[3] = shader_init ("data/shader_spatter");
	shader[4] = shader_init ("data/shader_blur");
	shader[5] = shader_init ("data/shader_level_s");
	shader[6] = shader_init ("data/shader_bot_s");
	shader[7] = shader_init ("data/shader_tess", true);

	light1.ambient[0] = light1.ambient[1] = light1.ambient[2] = 0.2f;
	light1.ambient[3] = 1.0f;
	light1.diffuse[0] = light1.diffuse[1] = light1.diffuse[2] = 0.5f;
	light1.diffuse[3] = 1.0f;
	light1.specular[0] = light1.specular[1] = light1.specular[2] = 0.8f;
	light1.specular[3] = 1.0f;
	light1.position[0] = light1.position[2] = 3.0f;
	light1.position[1] = 0.0f;
	light1.position[3] = 1.0f; 	
	light1.name = strdup ("light");

	/* hraci */
	mat1.ambient[0] = mat1.ambient[1] = mat1.ambient[2] = 2.0f;
	mat1.ambient[3] = 1.0f;
	mat1.diffuse[0] = mat1.diffuse[1] = mat1.diffuse[2] = 0.8f;
	mat1.diffuse[3] = 1.0f;
	mat1.specular[0] = mat1.specular[1] = mat1.specular[2] = 0.3f;
	mat1.specular[3] = 1.0f;
	mat1.name = strdup ("material");
	
	/* level */
	mat2.ambient[0] = mat2.ambient[1] = mat2.ambient[2] = 0.9f;
	mat2.ambient[3] = 1.0f;
	mat2.diffuse[0] = mat2.diffuse[1] = mat2.diffuse[2] = 0.6f;
	mat2.diffuse[3] = 1.0f;
	mat2.specular[0] = mat2.specular[1] = mat2.specular[2] = 1.0f;
	mat2.specular[3] = 1.0f;
	mat2.name = strdup ("material");
	
	
	//glm::mat4 tmp = glm::rotate (180.0f, glm::vec3 (0, 1, 0));
	
	/*glm::vec3 lightInvDir = glm::vec3(-2,0,-2);

	// Compute the MVP matrix from the light's point of view
	glm::mat4 depthProjectionMatrix = glm::perspective (70.0f, (float) 4.0f/3.0f, 0.1f, 100.0f);//glm::ortho<float>(-10,10,-10,10,-10,20);
	glm::mat4 depthViewMatrix = glm::rotate (180.0f, glm::vec3 (0, 1, 0)) * glm::translate (glm::vec3(light1.position[0], light1.position[1], light1.position[2]));//mglm::lookAt(lightInvDir, glm::vec3(0,0,0), glm::vec3(0,1,0));
	glm::mat4 depthModelMatrix = glm::mat4(1.0);
	depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;*/
	
	return true;
}

void gl_init_shadows ()
{
	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	glGenTextures (1, &tex_depth);
	glBindTexture (GL_TEXTURE_2D, tex_depth);

        glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, 1024, 1024, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE); 

	//glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,1024,1024,0,GL_RGBA,GL_FLOAT, NULL);
	
	glBindTexture (GL_TEXTURE_2D, 0);
	
	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers (1, &fbo_shadows);
	glBindFramebuffer (GL_FRAMEBUFFER, fbo_shadows);

	//glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_depth, 0);
	//glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex_depth, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tex_depth, 0);
	
	glDrawBuffer (GL_NONE); // No color buffer is drawn to.
	//glReadBuffer (GL_NONE);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;
	
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
}




void gl_init_wall ()
{
	const GLfloat buf[] = { 
		//x, y, z, u ,v	, nx, ny, nz
		// front
		-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

		// back
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,

		1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
		-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,

		// right
		1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

		// left
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,

		-1.0f, 1.0f, 1.0f, 1.0f, 1.0f,-1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f
	};

	glGenBuffers (1, &vbo_wall_id);
	glBindBuffer (GL_ARRAY_BUFFER, vbo_wall_id);
	glBufferData (GL_ARRAY_BUFFER, sizeof (buf), buf, GL_STATIC_DRAW);
}

void gl_init_screen_quad()
{
	const GLfloat buf[] = {
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f
	};
	
	glGenBuffers (1, &vbo_screen_quad_id);
	glBindBuffer (GL_ARRAY_BUFFER, vbo_screen_quad_id);
	glBufferData (GL_ARRAY_BUFFER, sizeof (buf), buf, GL_STATIC_DRAW);
	
	int textureWidth; 
	int textureHeight;
	SDL_GetWindowSize(g_window,&textureWidth,&textureHeight);
	
	//create depth texture
	glGenTextures(1,&depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,textureWidth,textureHeight,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	
	int i = glGetError();
	
	if(i!=0)
		printf("%s\n",gluErrorString(i));
	
	glBindTexture(GL_TEXTURE_2D,0);


	//Create framebuffer texture
	glGenTextures(1,&renderTexture);
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,textureWidth,textureHeight,0,GL_RGBA,GL_FLOAT,NULL);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	i = glGetError();
	
	if(i!=0)
		printf("%s\n",gluErrorString(i));
	
	glBindTexture(GL_TEXTURE_2D,0);

	//Create framebuffer texture2
	glGenTextures(1,&renderTexture2);
	glBindTexture(GL_TEXTURE_2D, renderTexture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,textureWidth,textureHeight,0,GL_RGBA,GL_FLOAT,NULL);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	
	i = glGetError();
	
	if (i!=0)
		printf("%s\n",gluErrorString(i));
	
	glBindTexture(GL_TEXTURE_2D,0);
	
	//Create FBO
	glGenFramebuffers(1,&fbo_screen_quad_id);
	glBindFramebuffer(GL_FRAMEBUFFER,fbo_screen_quad_id);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,renderTexture,0);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,depthTexture,0);
	
	i=glCheckFramebufferStatus(GL_FRAMEBUFFER);
	
	if (i!=GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer failed: %d\n",i);

	glEnable (GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	//Create FBO
	glGenFramebuffers(1,&fbo_screen_quad_id2);
	glBindFramebuffer(GL_FRAMEBUFFER,fbo_screen_quad_id2);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,renderTexture2,0);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,depthTexture,0);
	
	i=glCheckFramebufferStatus(GL_FRAMEBUFFER);
	
	if (i!=GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer failed: %d\n",i);

	glEnable (GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void gl_render_screen_quad ()
{
	glBindBuffer (GL_ARRAY_BUFFER, vbo_screen_quad_id);
	
	glEnableVertexAttribArray (0);
	glEnableVertexAttribArray (1);
	
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, (5 * sizeof(GLfloat)), 0);
	glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, (5 * sizeof(GLfloat)), (GLvoid *) (3 * sizeof(GLfloat)));
	
	glDrawArrays (GL_TRIANGLES, 0, 6);
	
	glDisableVertexAttribArray (0);
	glDisableVertexAttribArray (1);
	
	glBindBuffer (GL_ARRAY_BUFFER, 0);
}

void render_screen_quad (GLuint srcTexture, GLuint blur, GLuint s)
{
	glUseProgram (s);
	glm::mat4 ortho = glm::ortho (0.0f,1.0f,0.0f,1.0f,-1.0f,1.0f);
	int textureWidth, textureHeight;
	SDL_GetWindowSize (g_window, &textureWidth, &textureHeight);
	int uniform = glGetUniformLocation (s, "PMatrix");
	glUniformMatrix4fv (uniform, 1, GL_FALSE, (float *) &ortho);
	
	uniform = glGetUniformLocation (s, "pixelSize");
	glUniform2f (uniform, 1 / (float) textureWidth, 1 / (float) textureHeight);

	uniform = glGetUniformLocation (s, "blur");
	glUniform1ui (uniform,blur);
	
	GLuint tex_id = glGetUniformLocation (s, "TexSampler");
	glUniform1i (tex_id, 0);
	
	glActiveTexture (GL_TEXTURE0); 
	glBindTexture (GL_TEXTURE_2D, srcTexture);
	
	gl_render_screen_quad ();
	
	glUseProgram (0);		
}

void gl_init_floor ()
{
	const GLfloat buf[] = { 
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0, 0.0f,
		 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, -1.0, 0.0f,
		 1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 0.0f, -1.0, 0.0f,

		 1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 0.0f, -1.0, 0.0f,
		-1.0f, -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, -1.0, 0.0f,
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0, 0.0f
	};

	glGenBuffers (1, &vbo_floor_id);
	glBindBuffer (GL_ARRAY_BUFFER, vbo_floor_id);
	glBufferData (GL_ARRAY_BUFFER, sizeof (buf), buf, GL_STATIC_DRAW);
}

void gl_resize (int width, int height)
{
	camera_init (0, 0, width, height);
}

float gl_fps_get ()
{
	if (!fps_dtick)
		return 1000.0f;

	return 1000.0f / fps_dtick;
}

void gl_render_wall ()
{
	glBindBuffer (GL_ARRAY_BUFFER, vbo_wall_id);

	glEnableVertexAttribArray (0);
	glEnableVertexAttribArray (1);
	glEnableVertexAttribArray (2);
	
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, (8 * sizeof(GLfloat)), 0);
	glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, (8 * sizeof(GLfloat)), (GLvoid *) (3 * sizeof(GLfloat)));
	glVertexAttribPointer (2, 3, GL_FLOAT, GL_FALSE, (8 * sizeof(GLfloat)), (GLvoid *) (5 * sizeof(GLfloat)));
	
	glDrawArrays (GL_TRIANGLES, 0, 24);
	
	glDisableVertexAttribArray (0);
	glDisableVertexAttribArray (1);
	glDisableVertexAttribArray (2);
	
	glBindBuffer (GL_ARRAY_BUFFER, 0);
}

void gl_render_floor ()
{	
	glBindBuffer (GL_ARRAY_BUFFER, vbo_floor_id);

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



bool gl_frustum (player_t *p, float x, float y)
{
	float a = p->rot_y-90;

	float xn = p->pos_x * cosf (M_PI/180 * a) - p->pos_y * sinf (M_PI/180 * a);

	if (xn < x-(WALL_DIM*2))
		return true;

	return false;
}

void gl_render_weapon (player_t *p)
{
	camera_t *cam = camera_get ();
	
	if (p->state & PLAYER_STATE_FIRE) {
		p->state &= ~PLAYER_STATE_FIRE;

		/* new shot */
		if (p->mdl_frame == 5) {
			p->mdl_frame = 0;
			p->action = 1;
		}
	}

	if (p->mdl_frame < 5)
		p->mdl_itp += 0.08f;

 	mdl_animate (0, mdlfile[0].header.num_frames - 1, &p->mdl_frame, &p->mdl_itp);

	glm::mat4 mdl_matrix;
	mdl_matrix = 	glm::translate (glm::vec3 (0.08f, -0.03, -0.18f)) *
			glm::rotate (90.0f, glm::vec3 (0, 1, 0)) *
			glm::rotate (-90.0f, glm::vec3 (1, 0, 0)) *
			glm::scale (glm::vec3 (0.01f, 0.01f, 0.01f));
	
	/* enable program and set uniform variables */
	glUseProgram (shader[1]);
	
	glm::mat4 tmp = /*cam->view * */mdl_matrix;

	int uniform = glGetUniformLocation (shader[1], "PMatrix");
	glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*)&cam->projection[0]);
	uniform = glGetUniformLocation (shader[1], "VMatrix");
	glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*)&cam->view[0]);
	uniform = glGetUniformLocation (shader[1], "MVMatrix");
	glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*)&tmp[0]);
	uniform = glGetUniformLocation (shader[1], "NormalMatrix");
	glUniformMatrix3fv (uniform, 1, GL_FALSE, (float*)&(glm::inverseTranspose(glm::mat3(tmp)))[0]);

	GLuint tex_id  = glGetUniformLocation (shader[1], "TexSampler");
	glUniform1i (tex_id, 0);
	
	shader_getuniform_light (shader[1], &light1);

	mdl_renderitp (p->mdl_frame, p->mdl_itp, &mdlfile[0]);
	
	glUseProgram (0);
}

void gl_render_weapon_tess (player_t *p)
{
	camera_t *cam = camera_get ();
	
	if (p->state & PLAYER_STATE_FIRE) {
		p->state &= ~PLAYER_STATE_FIRE;

		/* new shot */
		if (p->mdl_frame == 5) {
			p->mdl_frame = 0;
			p->action = 1;
		}
	}

	if (p->mdl_frame < 5)
		p->mdl_itp += 0.08f;

 	mdl_animate (0, mdlfile[0].header.num_frames - 1, &p->mdl_frame, &p->mdl_itp);

	glm::mat4 mdl_matrix;
	mdl_matrix = 	glm::translate (glm::vec3 (0.08f, -0.03, -0.18f)) *
			glm::rotate (90.0f, glm::vec3 (0, 1, 0)) *
			glm::rotate (-90.0f, glm::vec3 (1, 0, 0)) *
			glm::scale (glm::vec3 (0.01f, 0.01f, 0.01f));
	
	/* enable program and set uniform variables */
	glUseProgram (shader[7]);
	
	glm::mat4 tmp = /*cam->view * */mdl_matrix;

	int uniform = glGetUniformLocation (shader[7], "PMatrix");
	glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*)&cam->projection[0]);
	uniform = glGetUniformLocation (shader[7], "VMatrix");
	glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*)&cam->view[0]);
	uniform = glGetUniformLocation (shader[7], "MVMatrix");
	glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*)&tmp[0]);
	uniform = glGetUniformLocation (shader[7], "NormalMatrix");
	glUniformMatrix3fv (uniform, 1, GL_FALSE, (float*)&(glm::inverseTranspose(glm::mat3(tmp)))[0]);

	GLuint tex_id  = glGetUniformLocation (shader[1], "TexSampler");
	glUniform1i (tex_id, 0);
	
	shader_getuniform_light (shader[7], &light1);

	mdl_renderitp (p->mdl_frame, p->mdl_itp, &mdlfile[0]);
	
	glUseProgram (0);
}

void gl_render_players (player_t *p)
{
	camera_t *cam = camera_get ();
	
	player_t *l;
	for (l = player_list.next; l != &player_list; l = l->next) {
		if (p == l)
			continue;

		l->mdl_itp += 0.08f;

	 	mdl_animate (0, mdlfile[1].header.num_frames - 1, &l->mdl_frame, &l->mdl_itp);

		glm::mat4 mdl_matrix;
		mdl_matrix = 	glm::translate (glm::vec3 (-l->pos_x, -0.4, -l->pos_y)) *
				glm::rotate (l->rot_y-90, glm::vec3 (0, 1, 0)) *
				glm::rotate (-90.0f, glm::vec3 (1, 0, 0)) *
				glm::scale (glm::vec3 (0.023f, 0.023f, 0.023f));
			
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
		
		shader_getuniform_light (shader[0], &light1);
		shader_getuniform_material (shader[0], &mat1);
		
		GLuint tex_id  = glGetUniformLocation (shader[0], "MainTex");
		glUniform1i (tex_id, 0);
		
		mdl_renderitp (l->mdl_frame, l->mdl_itp, &mdlfile[1]);
			
		/* disable program */
		glUseProgram (0);
		
		switch (l->state) {
			case PLAYER_STATE_WALK:
				if (l->mdl_frame > 4)
					l->mdl_frame = 0;
				break;
			case PLAYER_STATE_FIRE:
				if (l->mdl_frame > 116)
					l->mdl_frame = 110;
				if (l->mdl_frame < 110)
					l->mdl_frame = 110;
				break;
			case 0:
				if (l->mdl_frame > 27)
					l->mdl_frame = 17;
				else if (l->mdl_frame < 17)
					l->mdl_frame = 17;
				break;
		}
	}
}

void gl_render_level ()
{
	camera_t *cam = camera_get ();
	
	glm::mat4 mdl_matrix;
	
	/* LEVEL RENDERING */
	level_t *l = level_get ();

	bool col = false;
	for (unsigned x = 0; x < l->dim_x; x ++) {
		for (unsigned y = 0; y < l->dim_y; y ++) {
			unsigned char b = l->data[y*l->dim_x + x];

			if (b == ' ')
				continue;

			mdl_matrix = glm::translate (glm::vec3 (x*WALL_DIM, 0.0f, y*WALL_DIM));

			/* enable program and set uniform variables */
			glUseProgram (shader[2]);
			
			glm::vec3 lightInvDir = glm::vec3(1,0.3f,1);
			
			glm::mat4 biasMatrix(
				0.5, 0.0, 0.0, 0.0, 
				0.0, 0.5, 0.0, 0.0,
				0.0, 0.0, 0.5, 0.0,
				0.5, 0.5, 0.5, 1.0
			);

			// Compute the MVP matrix from the light's point of view
			glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10,10,-10,10,-10,20);
			glm::mat4 depthViewMatrix = glm::rotate (180.0f, glm::vec3 (0, 1, 0)) * glm::translate (glm::vec3 (-4, 0, -4));// glm::lookAt(lightInvDir, glm::vec3(4,0,4), glm::vec3(0,1,0));
			glm::mat4 depthModelMatrix = mdl_matrix;
			depthMVP = depthProjectionMatrix * depthViewMatrix;
			
			glm::mat4 depthBiasMVP = biasMatrix * depthMVP * mdl_matrix;
			
			
			glm::mat4 tmp = cam->view * mdl_matrix;
		
			int uniform = glGetUniformLocation (shader[2], "PMatrix");
			glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*)&cam->projection[0]);
			uniform = glGetUniformLocation (shader[2], "VMatrix");
			glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*)&cam->view[0]);
			uniform = glGetUniformLocation (shader[2], "MVMatrix");
			glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*)&tmp[0]);
			uniform = glGetUniformLocation (shader[2], "NormalMatrix");
			glUniformMatrix3fv (uniform, 1, GL_FALSE, (float*)&(glm::inverseTranspose(glm::mat3(tmp)))[0]);
			
			uniform = glGetUniformLocation (shader[2], "DepthBiasMVP");
			glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*)&depthBiasMVP[0]);
			
			shader_getuniform_light (shader[2], &light1);
			shader_getuniform_material (shader[2], &mat2);
				
			GLuint tex_id  = glGetUniformLocation (shader[2], "TexSampler");
			GLuint bump_id  = glGetUniformLocation (shader[2], "TexBump");
			GLuint depth_id  = glGetUniformLocation (shader[2], "shadowMap");
			glUniform1i (tex_id, 0);
			glUniform1i (bump_id, 1);
			glUniform1i (depth_id, 2);

			/* depth texure */
			glActiveTexture (GL_TEXTURE2); 
			glBindTexture (GL_TEXTURE_2D, tex_depth);
			
			/* bump map */
			glActiveTexture (GL_TEXTURE1); 
			switch (b) {
				case '0':
					glBindTexture (GL_TEXTURE_2D, tex_get (1));
					break;
				case '1':
					glBindTexture (GL_TEXTURE_2D, tex_get (3));
					break;
				case '2':
					glBindTexture (GL_TEXTURE_2D, tex_get (5));
					break;
			}
			
			/* texure */
			glActiveTexture (GL_TEXTURE0); 
			switch (b) {
				case '0':
					glBindTexture (GL_TEXTURE_2D, tex_get (0));
					break;
				case '1':
					glBindTexture (GL_TEXTURE_2D, tex_get (2));
					break;
				case '2':
					glBindTexture (GL_TEXTURE_2D, tex_get (4));
					break;
			}
			
			if (b == '0')
				gl_render_floor ();
			else
				gl_render_wall ();
	
			/* disable program */
			glUseProgram (0);
		}
	}
}

void blur_screen (player_t *p)
{
	int blur = (100 - p->hp) / 10;
	
	//PING PONG - MULTIPASS BLUR - MORE ITERATIONS MORE BLUR
	for (int i = 0; i < blur; i ++) {
		if (i % 2 == 0) {
			glBindFramebuffer (GL_FRAMEBUFFER,fbo_screen_quad_id2);
			render_screen_quad (renderTexture, 1, shader[4]);
		} else {
			glBindFramebuffer (GL_FRAMEBUFFER,fbo_screen_quad_id);
			render_screen_quad (renderTexture2, 1, shader[4]);	
		}
	}
}

void gl_render_shadows (player_t *p)
{
#ifdef SHADOW_LEVEL_ENABLE
	camera_t *cam = camera_get ();
	
	/* LEVEL RENDERING */
	level_t *l = level_get ();


	bool col = false;
	for (unsigned x = 0; x < l->dim_x; x ++) {
		for (unsigned y = 0; y < l->dim_y; y ++) {
			unsigned char b = l->data[y*l->dim_x + x];

			if (b == ' ')
				continue;

			/* enable program and set uniform variables */
			glUseProgram (shader[5]);
						
			glm::mat4 tmp = depthMVP * glm::translate (glm::vec3 (x*WALL_DIM, 0.0f, y*WALL_DIM));
			
			int uniform = glGetUniformLocation (shader[5], "depthMVP");
			glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*)&tmp[0]);
			
			if (b == '0')
				gl_render_floor ();
			else
				gl_render_wall ();
	
			/* disable program */
			glUseProgram (0);
		}
	}
#endif
	
	camera_t *cam = camera_get ();
	
	player_t *l;
	for (l = player_list.next; l != &player_list; l = l->next) {
		if (p == l)
			continue;

	 	mdl_animate (0, mdlfile[1].header.num_frames - 1, &l->mdl_frame, &l->mdl_itp);
	

		/*glTranslatef (-l->pos_x, -0.4, -l->pos_y);
		glRotatef (l->rot_y-90, 0, 1, 0);
		glRotatef (-90, 1, 0, 0);
		glScalef (0.023f, 0.023f, 0.023f);*/
		glm::mat4 mdl_matrix;
		mdl_matrix = 	glm::translate (glm::vec3 (-l->pos_x, -0.4, -l->pos_y)) *
				glm::rotate (l->rot_y-90, glm::vec3 (0, 1, 0)) *
				glm::rotate (-90.0f, glm::vec3 (1, 0, 0)) *
				glm::scale (glm::vec3 (0.023f, 0.023f, 0.023f));
			
		/* enable program and set uniform variables */
		glUseProgram (shader[6]);
	
		glm::mat4 tmp = depthMVP * mdl_matrix;
			
		int uniform = glGetUniformLocation (shader[6], "depthMVP");
		glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*)&tmp[0]);
		
		mdl_renderitp (l->mdl_frame, l->mdl_itp, &mdlfile[1]);
			
		/* disable program */
		glUseProgram (0);
		
		switch (l->state) {
			case PLAYER_STATE_WALK:
				if (l->mdl_frame > 4)
					l->mdl_frame = 0;
				break;
			case PLAYER_STATE_FIRE:
				if (l->mdl_frame > 116)
					l->mdl_frame = 110;
				if (l->mdl_frame < 110)
					l->mdl_frame = 110;
				break;
			case 0:
				if (l->mdl_frame > 27)
					l->mdl_frame = 17;
				else if (l->mdl_frame < 17)
					l->mdl_frame = 17;
				break;
		}
	}
}


void gl_render ()
{
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity ();

	player_t *p = player_get ();
	
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_shadows);
	glViewport (0, 0, 1024, 1024);
	
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable (GL_DEPTH_TEST);
	gl_render_shadows (p);
	//gl_render_level ();
	//gl_render_players (p);

	glBindFramebuffer (GL_FRAMEBUFFER, 0);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera_update ();

       // glCullFace (GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
	
	//RENDER TO TEXTURE
	glBindFramebuffer (GL_FRAMEBUFFER, fbo_screen_quad_id);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable (GL_DEPTH_TEST);

	/* Weapon */
	gl_render_weapon (p);

	/* Players */
	gl_render_players (p);

	/* Level */
	gl_render_level ();
	
	/* Sky */
	sky_system_render ();
	
	/*Particle systems */
	particle_system_render (part_sys_get (0));
	particle_system_render (part_sys_get (1));

	/* Onscreen blood */
	render_spatters ();
	
	/* screen blurr based on player's hp */
	blur_screen (p);

	//RENDER TO SCREEN
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
	render_screen_quad (renderTexture, 0, shader[4]);
	//render_screen_quad (tex_depth, 0, shader[6]);

	glFlush ();

	//SDL_GL_SwapBuffers ();// Prohodi predni a zadni buffer
	SDL_GL_SwapWindow (g_window);
	//SDL_Delay (2);
	fps_dtick = SDL_GetTicks () - fps_stick;
	
	if (fps_dtick < 1000.0f/FPS_MAX) {
		float dly = 1000.0f/FPS_MAX - fps_dtick;
		
		if (dly > 0.0f)
			SDL_Delay (dly);
	}
	
	fps_stick = SDL_GetTicks ();
}

