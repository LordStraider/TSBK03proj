// Lab 1-1, multi-pass rendering with FBOs, HDR bloom effects
// OpenGL 3 revision
// Updated 120910-14 with ZPR support and a revised model loading scheme.
// Updated 120920, fixing the FBO creation for the zillionth time. Can we stop overwriting the fix?
// Updated 130908 for MicroGlut, new zpr and VectorUtils3.

// gcc lab1-1.c ../common/*.c -lGL -o lab1-1 -I../common

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include "lib/MicroGlut.h"
	#include "lib/VectorUtils3.h"
	#include "lib/GL_utilities.h"
	#include "lib/loadobj.h"
	#include "lib/zpr.h"
	#include "PVRShell.h"
//	#include "OGLES3Tools.h"
	// uses framework Cocoa
#else
	#include <GL/gl.h>
	#include "MicroGlut.h" // <GL/glut.h>
	#include "VectorUtils3.h"
	#include "GL_utilities.h"
	#include "loadobj.h"
	#include "zpr.h"
#endif

// initial width and heights
const int initWidth=1000,initHeight=600;
#define W 1200
#define H 600

#define NEAR 1.0
#define FAR 150.0
#define RIGHT 0.5
#define LEFT -0.5
#define TOP 0.5
#define BOTTOM -0.5

#define NUM_LIGHTS 4

//----------------------Globals-------------------------------------------------
mat4 projectionMatrix;
mat4 viewMatrix;
vec3 cam = {0,0,15};
vec3 point = {0,2,0};
Model * model1;
Model * model2;
FBOstruct * fbo1;
FBOstruct * fbo2;
FBOstruct * fbo3;
GLuint shader1 = 0;
GLuint shader2 = 0;
GLuint sub_one = 0;
GLuint lowpass_hor = 0;
GLuint lowpass_ver = 0;
GLuint add = 0;
//------------------------------------------------------------------------------

void init(void) {
	dumpInfo();  // shader info

	// GL inits
	glClearColor(0.1, 0.1, 0.3, 0);
	glEnable(GL_DEPTH_TEST);
	glDepthRange(0.0f, 1.0f); 
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_TEXTURE);

	// Load and compile shaders
	shader1 = loadShaders("shaders/lab1-1a.vert", "shaders/lab1-1a.frag");
	sub_one = loadShaders("shaders/rect.vert", "shaders/sub-one.frag");
	lowpass_hor = loadShaders("shaders/rect.vert", "shaders/lowpass-horizontal.frag");
	lowpass_ver = loadShaders("shaders/rect.vert", "shaders/lowpass-vertical.frag");
  add = loadShaders("shaders/rect.vert", "shaders/add.frag");

	fbo1 = initFBO(W, H, 0);
	fbo2 = initFBO(W, H, 0);
	fbo3 = initFBO(W, H, 0);

	// load the model
	model1 = LoadModelPlus("objects/stanford-bunny.obj");
	model2 = LoadModelPlus("objects/rectangle.obj");

	glUseProgram(sub_one);
	glUniform1i(glGetUniformLocation(sub_one, "texUnit"), 0);
	glUseProgram(lowpass_hor);
	glUniform1i(glGetUniformLocation(lowpass_hor, "texUnit"), 0);
	glUseProgram(lowpass_ver);
	glUniform1i(glGetUniformLocation(lowpass_ver, "texUnit"), 0);
	glUseProgram(add);
	glUniform1i(glGetUniformLocation(add, "texUnit1"), 0);
	glUniform1i(glGetUniformLocation(add, "texUnit2"), 1);

	glUseProgram(shader1);
}

//-------------------------------callback functions------------------------------------------
void display(void) {
  glEnable(GL_DEPTH_TEST);
	// Pass
	useFBO(fbo3, 0L, 0L);
	glClearColor(0.1, 0.1, 0.3, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shader1);

	glUniformMatrix4fv(glGetUniformLocation(shader1, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	mat4 modelView = Mult(viewMatrix, S(30,30,30));
	glUniformMatrix4fv(glGetUniformLocation(shader1, "modelviewMatrix"), 1, GL_TRUE, modelView.m);

	DrawModel(model1, shader1, "in_Position", "in_Normal", NULL);

  glDisable(GL_DEPTH_TEST);

  // Pass
  useFBO(fbo1, fbo3, 0L);
	glUseProgram(sub_one);
	DrawModel(model2, sub_one, "in_Position", NULL, "in_TexCoord");

	// Pass
	useFBO(fbo2, fbo1, 0L);
	glUseProgram(lowpass_hor);
	DrawModel(model2, lowpass_hor, "in_Position", NULL, "in_TexCoord");

	for (int i = 0; i < 2; ++i) {
		// Pass
		useFBO(fbo1, fbo2, 0L);
		glUseProgram(lowpass_ver);
		DrawModel(model2, lowpass_ver, "in_Position", NULL, "in_TexCoord");

		// Pass
		useFBO(fbo2, fbo1, 0L);
		glUseProgram(lowpass_hor);
		DrawModel(model2, lowpass_hor, "in_Position", NULL, "in_TexCoord");
	}

	// Pass
	useFBO(fbo1, fbo2, 0L);
	glUseProgram(lowpass_ver);
	DrawModel(model2, lowpass_ver, "in_Position", NULL, "in_TexCoord");
	
	// Pass
	useFBO(0L, fbo1, fbo3);
	glClearColor(1.0, 0.0, 0.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(add);
	DrawModel(model2, add, "in_Position", NULL, "in_TexCoord");

	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h) {
	GLfloat ratio = (GLfloat) w / (GLfloat) h;
	projectionMatrix = perspective(70, ratio, 0.2, 1000.0);
	//updateScreenSizeForFBOHandler(w, h); // Workaround, should be removed when problem fixed
}

void idle(void) {
	glutPostRedisplay();
}

//-----------------------------main-----------------------------------------------
int main(int argc, char *argv[]) {
	glutInit(&argc, argv);

	// Configure GLUT:
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(initWidth, initHeight);
	glutInitContextVersion(3, 2);
	glutCreateWindow ("Lab 1_1 - FBOs and HDR");
	zprInit(&viewMatrix, cam, point);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);

	init();
	glutMainLoop();
	exit(0);
}
