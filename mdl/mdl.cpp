/*
 * mdl.c -- mdl model loader
 * last modification: aug. 14, 2007
 *
 * Copyright (c) 2005-2007 David HENRY
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * gcc -Wall -ansi -lGL -lGLU -lglut mdl.c -o mdl
 */

#include "mdl.h"

/* Table of precalculated normals */
vec3_t anorms_table[162] = {
#include "anorms.h"
};

/* Palette */
unsigned char colormap[256][3] = {
#include "colormap.h"
};

/**
 * Make a texture given a skin index 'n'.
 */
GLuint
MakeTextureFromSkin (int n, const struct mdl_model_t *mdl)
{
  int i;
  GLuint id;

  GLubyte *pixels = (GLubyte *)
    malloc (mdl->header.skinwidth * mdl->header.skinheight * 3);

  /* Convert indexed 8 bits texture to RGB 24 bits */
  for (i = 0; i < mdl->header.skinwidth * mdl->header.skinheight; ++i)
    {
      pixels[(i * 3) + 0] = colormap[mdl->skins[n].data[i]][0];
      pixels[(i * 3) + 1] = colormap[mdl->skins[n].data[i]][1];
      pixels[(i * 3) + 2] = colormap[mdl->skins[n].data[i]][2];
    }

  /* Generate OpenGL texture */
  glGenTextures (1, &id);
  glBindTexture (GL_TEXTURE_2D, id);

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  /*gluBuild2DMipmaps (GL_TEXTURE_2D, GL_RGB, mdl->header.skinwidth,
		     mdl->header.skinheight, GL_RGB, GL_UNSIGNED_BYTE,
		     pixels);*/
  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, mdl->header.skinwidth, mdl->header.skinheight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

  /* OpenGL has its own copy of image data */
  free (pixels);
  return id;
}

/**
 * Load an MDL model from file.
 *
 * Note: MDL format stores model's data in little-endian ordering.  On
 * big-endian machines, you'll have to perform proper conversions.
 */
int mdl_read (const char *filename, struct mdl_model_t *mdl)
{
  FILE *fp;
  int i;

  fp = fopen (filename, "rb");
  if (!fp)
    {
      fprintf (stderr, "error: couldn't open \"%s\"!\n", filename);
      return 0;
    }

  /* Read header */
  fread (&mdl->header, 1, sizeof (struct mdl_header_t), fp);

  if ((mdl->header.ident != 1330660425) ||
      (mdl->header.version != 6))
    {
      /* Error! */
      fprintf (stderr, "Error: bad version or identifier\n");
      fclose (fp);
      return 0;
    }

  /* Memory allocations */
  mdl->skins = (struct mdl_skin_t *)
    malloc (sizeof (struct mdl_skin_t) * mdl->header.num_skins);
  mdl->texcoords = (struct mdl_texcoord_t *)
    malloc (sizeof (struct mdl_texcoord_t) * mdl->header.num_verts);
  mdl->triangles = (struct mdl_triangle_t *)
    malloc (sizeof (struct mdl_triangle_t) * mdl->header.num_tris);
  mdl->frames = (struct mdl_frame_t *)
    malloc (sizeof (struct mdl_frame_t) * mdl->header.num_frames);
  mdl->tex_id = (GLuint *)
    malloc (sizeof (GLuint) * mdl->header.num_skins);

  mdl->gles_verts = (float *) malloc (sizeof (float) * mdl->header.num_tris * 9);
  mdl->gles_norms = (float *) malloc (sizeof (float) * mdl->header.num_tris * 9);
  mdl->gles_coords = (float *) malloc (sizeof (float) * mdl->header.num_tris * 6);

  mdl->iskin = 0;

  /* Read texture data */
  for (i = 0; i < mdl->header.num_skins; ++i)
    {
      mdl->skins[i].data = (GLubyte *)malloc (sizeof (GLubyte)
		* mdl->header.skinwidth * mdl->header.skinheight);

      fread (&mdl->skins[i].group, sizeof (int), 1, fp);
      fread (mdl->skins[i].data, sizeof (GLubyte),
	     mdl->header.skinwidth * mdl->header.skinheight, fp);

      mdl->tex_id[i] = MakeTextureFromSkin (i, mdl);

      free (mdl->skins[i].data);
      mdl->skins[i].data = NULL;
    }

  fread (mdl->texcoords, sizeof (struct mdl_texcoord_t),
	 mdl->header.num_verts, fp);
  fread (mdl->triangles, sizeof (struct mdl_triangle_t),
	 mdl->header.num_tris, fp);

  /* Read frames */
  for (i = 0; i < mdl->header.num_frames; ++i)
    {
      /* Memory allocation for vertices of this frame */
      mdl->frames[i].frame.verts = (struct mdl_vertex_t *)
	malloc (sizeof (struct mdl_vertex_t) * mdl->header.num_verts);

      /* Read frame data */
      fread (&mdl->frames[i].type, sizeof (int), 1, fp);
      fread (&mdl->frames[i].frame.bboxmin,
	     sizeof (struct mdl_vertex_t), 1, fp);
      fread (&mdl->frames[i].frame.bboxmax,
	     sizeof (struct mdl_vertex_t), 1, fp);
      fread (mdl->frames[i].frame.name, sizeof (char), 16, fp);
      fread (mdl->frames[i].frame.verts, sizeof (struct mdl_vertex_t),
	     mdl->header.num_verts, fp);
    }

  fclose (fp);
  return 1;
}

/**
 * Free resources allocated for the model.
 */
void
FreeModel (struct mdl_model_t *mdl)
{
  int i;

  if (mdl->skins)
    {
      free (mdl->skins);
      mdl->skins = NULL;
    }

  if (mdl->texcoords)
    {
      free (mdl->texcoords);
      free (mdl->gles_coords);
      mdl->texcoords = NULL;
    }

  if (mdl->triangles)
    {
      free (mdl->triangles);
      free (mdl->gles_verts);
      mdl->triangles = NULL;
    }

  if (mdl->tex_id)
    {
      /* Delete OpenGL textures */
      glDeleteTextures (mdl->header.num_skins, mdl->tex_id);

      free (mdl->tex_id);
      mdl->tex_id = NULL;
    }

  if (mdl->frames)
    {
      for (i = 0; i < mdl->header.num_frames; ++i)
	{
	  free (mdl->frames[i].frame.verts);
	  mdl->frames[i].frame.verts = NULL;
	}

      free (mdl->frames);
      mdl->frames = NULL;
    }
}
#include <unistd.h>
/**
 * Render the model with interpolation between frame n and n+1.
 * interp is the interpolation percent. (from 0.0 to 1.0)
 */
void mdl_renderitp (int n, float interp, const struct mdl_model_t *mdl)
{
  int i, j;
  GLfloat s, t;
  vec3_t norm, v;
  GLfloat *n_curr, *n_next;
  struct mdl_vertex_t *pvert1, *pvert2;

  /* Check if n is in a valid range */
  if ((n < 0) || (n > mdl->header.num_frames))
    return;

  /* Enable model's texture */
  glBindTexture (GL_TEXTURE_2D, mdl->tex_id[mdl->iskin]);

  float *gles_verts = mdl->gles_verts;
  float *gles_norms = mdl->gles_norms;
  float *gles_coords = mdl->gles_coords;

  /* Draw the model */
 // glBegin (GL_TRIANGLES);
    /* Draw each triangle */
    for (i = 0; i < mdl->header.num_tris; ++i)
      {
	/* Draw each vertex */
	for (j = 0; j < 3; ++j)
	  {
	    pvert1 = &mdl->frames[n].frame.verts[mdl->triangles[i].vertex[j]];
	    pvert2 = &mdl->frames[n + 1].frame.verts[mdl->triangles[i].vertex[j]];

	    /* Compute texture coordinates */
	    s = (GLfloat)mdl->texcoords[mdl->triangles[i].vertex[j]].s;
	    t = (GLfloat)mdl->texcoords[mdl->triangles[i].vertex[j]].t;

	    if (!mdl->triangles[i].facesfront &&
		mdl->texcoords[mdl->triangles[i].vertex[j]].onseam)
	      {
		s += mdl->header.skinwidth * 0.5f; /* Backface */
	      }

	    /* Scale s and t to range from 0.0 to 1.0 */
	    gles_coords[0] = s = (s + 0.5) / mdl->header.skinwidth;
	    gles_coords[1] = t = (t + 0.5) / mdl->header.skinheight;

	    /* Pass texture coordinates to OpenGL */
	//    glTexCoord2f (s, t);

	    /* Interpolate normals */
	    n_curr = anorms_table[pvert1->normalIndex];
	    n_next = anorms_table[pvert2->normalIndex];

	    gles_norms[0] = norm[0] = n_curr[0] + interp * (n_next[0] - n_curr[0]);
	    gles_norms[1] = norm[1] = n_curr[1] + interp * (n_next[1] - n_curr[1]);
	    gles_norms[2] = norm[2] = n_curr[2] + interp * (n_next[2] - n_curr[2]);

	   // glNormal3fv (norm);

	    /* Interpolate vertices */
	    v[0] = gles_verts[0] = (mdl->header.scale[0] * (pvert1->v[0] + interp
		* (pvert2->v[0] - pvert1->v[0])) + mdl->header.translate[0]);
	    v[1] = gles_verts[1] = (mdl->header.scale[1] * (pvert1->v[1] + interp
		* (pvert2->v[1] - pvert1->v[1])) + mdl->header.translate[1]);
	    v[2] = gles_verts[2] = (mdl->header.scale[2] * (pvert1->v[2] + interp
		* (pvert2->v[2] - pvert1->v[2])) + mdl->header.translate[2]);
	    
		gles_norms += 3;
		gles_verts += 3;
		gles_coords += 2;
	  }
	//    glVertex3fv (v);	  
      }
  //glEnd ();
  
	gles_verts = mdl->gles_verts;
  
	glEnableClientState (GL_VERTEX_ARRAY);
	glEnableClientState (GL_NORMAL_ARRAY);
	glEnableClientState (GL_TEXTURE_COORD_ARRAY);

	glVertexPointer (3, GL_FLOAT, 0, mdl->gles_verts);
	glNormalPointer (GL_FLOAT, 0, mdl->gles_norms);
	glTexCoordPointer (2, GL_FLOAT, 0, mdl->gles_coords);

	glDrawArrays (GL_TRIANGLES, 0, mdl->header.num_tris*3);

	glDisableClientState (GL_VERTEX_ARRAY);
	glDisableClientState (GL_NORMAL_ARRAY);
	glDisableClientState (GL_TEXTURE_COORD_ARRAY);
}

/**
 * Calculate the current frame in animation beginning at frame
 * 'start' and ending at frame 'end', given interpolation percent.
 * interp will be reseted to 0.0 if the next frame is reached.
 */
void mdl_animate (int start, int end, int *frame, float *interp)
{
  if ((*frame < start) || (*frame > end))
    *frame = start;

  if (*interp >= 1.0f)
    {
      /* Move to next frame */
      *interp = 0.0f;
      (*frame)++;

      if (*frame >= end)
	*frame = start;
    }
}
#ifdef TEST
void
init (const char *filename)
{
  GLfloat lightpos[] = { 5.0f, 10.0f, 0.0f, 1.0f };

  /* Initialize OpenGL context */
  glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
  glShadeModel (GL_SMOOTH);

  glEnable (GL_DEPTH_TEST);
  glEnable (GL_TEXTURE_2D);
  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);

  glLightfv (GL_LIGHT0, GL_POSITION, lightpos);

  /* Load MDL model file */
  if (!mdl_read (filename, &mdlfile))
      exit (EXIT_FAILURE);
}

void
cleanup ()
{
  FreeModel (&mdlfile);
}

void
reshape (int w, int h)
{
  if (h == 0)
    h = 1;

  glViewport (0, 0, (GLsizei)w, (GLsizei)h);

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (45.0, w/(GLdouble)h, 0.1, 1000.0);

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
}

void
display ()
{
  static int n = 0;
  static float interp = 0.0;
  static double curent_time = 0;
  static double last_time = 0;

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity ();

  last_time = curent_time;
  curent_time = (double)glutGet (GLUT_ELAPSED_TIME) / 1000.0;

  /* Animate model from frames 0 to num_frames-1 */
  interp += 10 * (curent_time - last_time);
  Animate (0, mdlfile.header.num_frames - 1, &n, &interp);

  glTranslatef (0.0f, 0.0f, -100.0f);
  glRotatef (-90.0f, 1.0, 0.0, 0.0);
  glRotatef (-90.0f, 0.0, 0.0, 1.0);

  /* Draw the model */
  /* RenderFrame (n, mdlfile); */
  mdl_renderitp (n, interp, &mdlfile);

  glutSwapBuffers ();
  glutPostRedisplay ();
}

void
keyboard (unsigned char key, int x, int y)
{
  switch (key)
    {
    case '+':
      mdlfile.iskin++;
      break;

    case '-':
      mdlfile.iskin--;
      break;

    case 27: /* escape */
      exit (0);
      break;
    }
}

int
main (int argc, char *argv[])
{
  if (argc < 2)
    {
      fprintf (stderr, "usage: %s <filename.mdl>\n", argv[0]);
      return 0;
    }

  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize (640, 480);
  glutCreateWindow ("MDL Model");

  atexit (cleanup);
  init (argv[1]);

  glutReshapeFunc (reshape);
  glutDisplayFunc (display);
  glutKeyboardFunc (keyboard);

  glutMainLoop ();

  return 0;
}
#endif
