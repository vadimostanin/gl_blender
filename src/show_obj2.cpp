// -- Written in C -- //

#include<stdio.h>
#include<stdlib.h>
#define GL_GLEXT_PROTOTYPES 1
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

#include <math.h>
#include <time.h>
#include <iostream>
#include <string.h>
#include <GL/freeglut_std.h>
using namespace std;

#include "product/cup.h"


extern float rotate_x;
extern float rotate_y;
extern float rotate_z;
extern float scale_z;

extern bool mouse_button_pressed;
extern float mouse_pointer_x;
extern float mouse_pointer_y;
extern int start_mouse_point_x;
extern int start_mouse_point_y;

extern float camera_look_from_z;

#define COLOR_RANDOM (0.1*(rand()%10) + 0.01*(rand()%10) + 0.001*(rand()%10))

template<typename T>inline T ABS(T x)
{
	return (x<0)?(-1)*x:x;
}

GLuint vbo_vertices, vbo_colors;
GLuint ibo_elements;

void DrawScene()
{
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-10, 10, -10, 10, -10, 10);
    gluLookAt(mouse_pointer_x, mouse_pointer_y, camera_look_from_z,
              0.0, 0.0, 0.0,
              0.0, 0.5, 0.0); /* Up direction is Z-axis... like in Blende ;-) */
    
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glRotatef(rotate_x, 0, 1, 0);//x like y
    glRotatef(rotate_y, 1, 0, 0);//y like x
    glRotatef(rotate_z, 0, 0, 1);
    
    glScalef(scale_z, scale_z, scale_z);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableVertexAttribArray(GL_VERTEX_ARRAY);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, cubePositions);
    glDrawArrays(GL_QUADS, 0, cubeVertices);
}

//TRIANGLE_STRIP = {before last vertex, last vertex and new vertex}
//TRIANGLE_FAN = first vertex, last vertex and new vertex}

