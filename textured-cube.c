/*
 * textured-cube version 0.1
 * 
 * Part of the Mesa Memory Testing tool (https://github.com/dimitar-kunchev/mesa-memtester)
 *
 * This creates a spinning textured cube and displays it on the framebuffer
 * Code is based on bits and pieces from various sources, including:
 *	https://github.com/ssvb/lima-memtester (including the textured-cube demo from the driver)
 * 	http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/
 * 
 * Copyright (C) 2018 Dimitar Kunchev <d.kunchev@gmail.com>
 * Licensed under the terms of the GNU General Public License version 2 (only).
 * See the file COPYING for details.
 *
 */

#include "textured-cube.h"

#include "texture.c"

static GLint u_matrix = -1;
static GLint attr_in_position = 0, attr_in_texture_coord = 1, attr_in_texture = 2;

static int view_rotx = 0, view_rotz = 0, view_roty = 0;

#define CUBE_VERTEX_COUNT 36

static const GLfloat g_vertex_buffer_data[] = {
	-1, -1, 1,
	-1, -1, -1,
	-1, 1, -1,	
	-1, 1, -1,
	-1, 1, 1,
	-1, -1, 1,

	-1, 1, 1,
	-1, 1, -1,
	1, 1, -1,
	1, 1, -1,
	1, 1, 1,
	-1, 1, 1,

	1, 1, 1,
	1, 1, -1,
	1, -1, -1,
	1, -1, -1,
	1, -1, 1,
	1, 1, 1,
	
	1, -1, 1,
	1, -1, -1,
	-1, -1, -1,
	-1, -1, -1,
	-1, -1, 1,
	1, -1, 1,

	-1, -1, 1,
	-1, 1, 1,
	1, 1, 1,
	1, 1, 1,
	1, -1, 1,
	-1, -1, 1,
	
	-1, 1, -1,
	1, 1, -1,
	1, -1, -1,
	1, -1, -1,
	-1, -1, -1,
	-1 ,1 ,-1
};

static const GLfloat g_uv_buffer_data[] = {
	0.0f, 		0.0f,
	0.0f, 		1.0f,
	1.0f, 		1.0f,
	1.0f, 		1.0f,
	1.0f, 		0,
	0,		0,

	0.0f, 		0.0f,
	0.0f, 		1.0f,
	1.0f, 		1.0f,
	1.0f, 		1.0f,
	1.0f, 		0,
	0,		0,

	0.0f, 		0.0f,
	0.0f, 		1.0f,
	1.0f, 		1.0f,
	1.0f, 		1.0f,
	1.0f, 		0,
	0,		0,

	0.0f, 		0.0f,
	0.0f, 		1.0f,
	1.0f, 		1.0f,
	1.0f, 		1.0f,
	1.0f, 		0,
	0,		0,

	0.0f, 		0.0f,
	0.0f, 		1.0f,
	1.0f, 		1.0f,
	1.0f, 		1.0f,
	1.0f, 		0,
	0,		0,

	0.0f, 		0.0f,
	0.0f, 		1.0f,
	1.0f, 		1.0f,
	1.0f, 		1.0f,
	1.0f, 		0,
	0,		0,
};

static EGLint const attribute_list[] = {
	EGL_BUFFER_SIZE, 16,
	EGL_RENDERABLE_TYPE,
	EGL_OPENGL_ES2_BIT,
        EGL_NONE
};

static void make_x_rot_matrix(GLfloat angle, GLfloat *m) {
	float c = cos(angle * M_PI / 180.0);
	float s = sin(angle * M_PI / 180.0);
	int i;
	for (i = 0; i < 16; i++)
		m[i] = 0.0;
	m[0] = m[5] = m[10] = m[15] = 1.0;

	m[5] = c;
	m[6] = s;
	m[9] = -s;
	m[10] = c;
}

static void make_y_rot_matrix(GLfloat angle, GLfloat *m) {
	float c = cos(angle * M_PI / 180.0);
	float s = sin(angle * M_PI / 180.0);
	int i;
	for (i = 0; i < 16; i++)
		m[i] = 0.0;
	m[0] = m[5] = m[10] = m[15] = 1.0;

	m[0] = c;
	m[2] = -s;
	m[8] = s;
	m[10] = c;
}

static void make_z_rot_matrix(GLfloat angle, GLfloat *m) {
	float c = cos(angle * M_PI / 180.0);
	float s = sin(angle * M_PI / 180.0);
	int i;
	for (i = 0; i < 16; i++)
		m[i] = 0.0;
	m[0] = m[5] = m[10] = m[15] = 1.0;

	m[0] = c;
	m[1] = s;
	m[4] = -s;
	m[5] = c;
}

static void make_scale_matrix(GLfloat xs, GLfloat ys, GLfloat zs, GLfloat *m) {
	int i;
	for (i = 0; i < 16; i++)
		m[i] = 0.0;
	m[0] = xs;
	m[5] = ys;
	m[10] = zs;
	m[15] = 1.0;
}

static void mul_matrix(GLfloat *prod, const GLfloat *a, const GLfloat *b) {
#define A(row,col)  a[(col<<2)+row]
#define B(row,col)  b[(col<<2)+row]
#define P(row,col)  p[(col<<2)+row]
	GLfloat p[16];
	GLint i;
	for (i = 0; i < 4; i++) {
		const GLfloat ai0=A(i,0),  ai1=A(i,1),  ai2=A(i,2),  ai3=A(i,3);
		P(i,0) = ai0 * B(0,0) + ai1 * B(1,0) + ai2 * B(2,0) + ai3 * B(3,0);
		P(i,1) = ai0 * B(0,1) + ai1 * B(1,1) + ai2 * B(2,1) + ai3 * B(3,1);
		P(i,2) = ai0 * B(0,2) + ai1 * B(1,2) + ai2 * B(2,2) + ai3 * B(3,2);
		P(i,3) = ai0 * B(0,3) + ai1 * B(1,3) + ai2 * B(2,3) + ai3 * B(3,3);
	}
	memcpy(prod, p, sizeof(p));
#undef A
#undef B
#undef PROD
}

static GLuint loadCubeTexture () {
	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cube_texture.width, cube_texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, cube_texture.pixel_data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	return textureID;
}

static void create_shaders(void) {
	static const char *fragShaderText =
		"precision mediump float;\n"
		"varying vec2 coord;\n"
		"uniform sampler2D in_texture;\n"
		"void main() {\n"
		"   gl_FragColor = texture2D(in_texture, coord);\n"
		"}\n";
	static const char *vertShaderText =
		"uniform mat4 modelViewProjection;\n"
		"attribute vec4 in_position;\n"
		"attribute vec2 in_texture_coord;\n"
		"varying vec2 coord;\n"
		"void main() {\n"
		"   gl_Position = modelViewProjection * in_position;\n"
		"   coord = in_texture_coord;\n"
	"}\n";

	GLuint fragShader, vertShader, program;
	GLint stat;

	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (fragShader < 1) {
		printf("Error fragment sharer create %d\n", fragShader);
		exit(1);
	}
	glShaderSource(fragShader, 1, (const char **) &fragShaderText, NULL);
	glCompileShader(fragShader);
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &stat);
	if (!stat) {
		printf("Error: fragment shader did not compile!\n");
		exit(1);
	}

	vertShader = glCreateShader(GL_VERTEX_SHADER);
	if (vertShader < 1) {
		printf("Err vertex shared create %d\n", vertShader);
		exit(1);
	}
	glShaderSource(vertShader, 1, (const char **) &vertShaderText, NULL);
	glCompileShader(vertShader);
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &stat);
	if (!stat) {
		printf("Error: vertex shader did not compile!\n");
		exit(1);
	}
/*
	{
   		char log[1000];
		GLsizei len;
		glGetShaderInfoLog(vertShader, 1000,  &len, log);
		printf("Shared log: %s", log);
	}; */

	program = glCreateProgram();
	glAttachShader(program, fragShader);
	glAttachShader(program, vertShader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &stat);
	if (!stat) {
		char log[1000];
		GLsizei len;
		glGetProgramInfoLog(program, 1000, &len, log);
		printf("Error: linking:\n%s\n", log);
		exit(1);
	}

	glUseProgram(program);

	glBindAttribLocation(program, attr_in_position, "in_position");
	glBindAttribLocation(program, attr_in_texture_coord, "in_texture_coord");
	glBindAttribLocation(program, attr_in_texture, "in_texture");\
	glLinkProgram(program);  /* needed to put attribs into effect */

	u_matrix = glGetUniformLocation(program, "modelViewProjection");
   	printf("Uniform modelViewProjection at %d\n", u_matrix);
	printf("Attrib in_position at %d\n", attr_in_position);
	printf("Attrib in_texture_coord at %d\n", attr_in_texture_coord);
	printf("Attrib in_texture at %d\n", attr_in_texture);

	// Prepare the texture
	if (loadCubeTexture() == 0) {
		printf("Error loading texture\n");
		exit(1);
	}
}

static void draw(void) {
	GLfloat mat[16], rot_x[16], rot_y[16], rot_z[16], scale[16];

	/* Set modelview/projection matrix */
	make_x_rot_matrix(view_rotx, rot_x);
	make_y_rot_matrix(view_roty, rot_y);
	make_z_rot_matrix(view_rotz, rot_z);
	make_scale_matrix(0.5, 0.5, 0.5, scale);
	
	mul_matrix(mat, rot_x, rot_y);
	mul_matrix(mat, mat, rot_z);
	mul_matrix(mat, mat, scale);
	glUniformMatrix4fv(u_matrix, 1, GL_FALSE, mat);

	glClearColor(0, 0, 0, 1.0);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glVertexAttribPointer(attr_in_position, 3, GL_FLOAT, GL_FALSE, 0, g_vertex_buffer_data);
	glVertexAttribPointer(attr_in_texture_coord, 2, GL_FLOAT, GL_FALSE,0 , g_uv_buffer_data);
	glEnableVertexAttribArray(attr_in_position);
	glEnableVertexAttribArray(attr_in_texture_coord);

	glDrawArrays(GL_TRIANGLES, 0, CUBE_VERTEX_COUNT);

	glDisableVertexAttribArray(attr_in_position);
	glDisableVertexAttribArray(attr_in_texture_coord);

	view_rotx = (view_rotx + 3) % 360;
	view_roty = (view_roty + 1) % 360;
	view_rotz = (view_rotz + 2) % 360;
}

int textured_cube_main () {
	int width = 480, height = 272;
        EGLDisplay display;
        EGLConfig config;
        EGLContext context;
        EGLSurface surface;
        NativeWindowType native_window;
        EGLint num_config;
        /* get an EGL display connection */
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (display == EGL_NO_DISPLAY) {
		printf("No EGL Display\n");
		exit(1);
	}
        /* initialize the EGL display connection */
        if (!eglInitialize(display, NULL, NULL)) {
		printf("Failed to initialize GL\n");
		exit(1);
	}
        /* get an appropriate EGL frame buffer configuration */
        if (!eglChooseConfig(display, attribute_list, &config, 1, &num_config)) {
		printf("Failed to choose config %d\n", eglGetError());
		exit(1);
	}

	//eglBindAPI(EGL_OPENGL_API);

        /* create an EGL rendering context */
	EGLint ctxattr[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
        context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctxattr);
	if (context == EGL_NO_CONTEXT) {
		printf("Failed to create context\n");
		exit(1);
	}
        
	
        /* create an EGL window surface */
        fbdev_window fbdev_window = {width, height};
	native_window = &fbdev_window;
	surface = eglCreateWindowSurface(display, config, native_window, NULL);

        /* connect the context to the surface */
        eglMakeCurrent(display, surface, surface, context);
		
	{
		EGLint val;
		eglQuerySurface(display, surface, EGL_WIDTH, &val);
		assert(val == width);
		eglQuerySurface(display, surface, EGL_HEIGHT, &val);
		assert(val == height);
		assert(eglGetConfigAttrib(display, config, EGL_SURFACE_TYPE, &val));
		assert(val & EGL_WINDOW_BIT);
	}
	/*
	{
		const char *s;
		s = eglQueryString(display, EGL_VERSION);
		printf("EGL_VERSION = %s\n", s);
		s = eglQueryString(display, EGL_VENDOR);
		printf("EGL_VENDOR = %s\n", s);
		s = eglQueryString(display, EGL_EXTENSIONS);
		printf("EGL_EXTENSIONS = %s\n", s);
		s = eglQueryString(display, EGL_CLIENT_APIS);
		printf("EGL_CLIENT_APIS = %s\n", s);
		printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
		printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
		printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
		printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
	}*/

	glViewport(0, 0, width, height);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
	glShadeModel(GL_SMOOTH);   // Enable smooth shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
	
	create_shaders();
	
        /* clear the color buffer */
        glClearColor(0, .6, .3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glFlush();
        eglSwapBuffers(display, surface);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat aspect = 480.0/272.0; // (GLfloat)width / (GLfloat)height;
	float m_w = 4;
	float m_h = m_w / aspect;
	glFrustumf(-m_w/2.0, m_w/2.0, -m_h/2.0, m_h/2.0, -2, 20);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	printf("Starting loop\n");
        while (1) {
		draw();
		eglSwapBuffers(display, surface);
		usleep(25000);
	};

	eglDestroyContext(display, context);
	eglDestroySurface(display, surface);
	eglTerminate(display);
        return EXIT_SUCCESS;
}

