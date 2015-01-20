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

#include "untitled.h"


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

template<typename T>inline T ABS(T x)
{
	return (x<0)?(-1)*x:x;
}

GLuint *vbo;
GLuint *vinx;
#define BUFFER_OFFSET(x)((char *)NULL+(x))

void InitMesh()
{
  unsigned int i;

  vbo = (GLuint *)malloc(OBJECTS_COUNT * sizeof(GLuint));
  vinx = (GLuint *)malloc(OBJECTS_COUNT * sizeof(GLuint));

  glGenBuffers(OBJECTS_COUNT, vbo);

  for (i=0; i<OBJECTS_COUNT; i++) { 
      glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
      glBufferData(GL_ARRAY_BUFFER, sizeof (struct vertex_struct) * vertex_count[i], &vertices[vertex_offset_table[i]], GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  glGenBuffers(OBJECTS_COUNT, vinx);
  for (i=0; i<OBJECTS_COUNT; i++) { 
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vinx[i]);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (indexes[0]) * faces_count[i] * 3, &indexes[indices_offset_table[i]], GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }
}

void DrawMesh(unsigned int index, int apply_transformations)
{
////    cout<<"-------DRAW START-------"<<endl<<endl<<endl;
    srand(time(0));
//    cout<<__LINE__<<endl;
#define COLOR_RANDOM 1.0//(0.1*(rand()%10) + 0.01*(rand()%10) + 0.001*(rand()%10))
    cout<<__LINE__<<endl;
    if (apply_transformations)
    {cout<<__LINE__<<endl;
        glPushMatrix();
        glMultMatrixf(transformations[index]);
    }
    cout<<__LINE__<<endl;
    glBindBuffer(GL_ARRAY_BUFFER, vbo[index]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vinx[index]);
    cout<<__LINE__<<endl;
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof (struct vertex_struct), BUFFER_OFFSET(0));
    cout<<__LINE__<<endl;
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, sizeof (struct vertex_struct), BUFFER_OFFSET(3 * sizeof (float)));
    cout<<__LINE__<<endl;
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof (struct vertex_struct), BUFFER_OFFSET(6 * sizeof (float)));
    cout<<__LINE__<<endl;
    glDrawElements(GL_TRIANGLES, faces_count[index] * 3, INX_TYPE, BUFFER_OFFSET(0));
    cout<<__LINE__<<endl;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    cout<<__LINE__<<endl;
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    cout<<__LINE__<<endl;
    if (apply_transformations)
    {
        glPopMatrix();
    }
    cout<<__LINE__<<endl;
}

void DrawAllMeshes()
{
    unsigned int i;

    for (i=0; i<OBJECTS_COUNT; i++) {
        DrawMesh(i, 1);
    }
}

void DrawScene(float dT)
{
    cout<<__LINE__<<endl;
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-5., 5., -5., 5., -10, 10.);
    
    cout<<__LINE__<<endl;
//    glClearColor(0.51, 0, 0, 0);
//    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 10, 10,
          0, 0, 0,
          0, 0, 1); /* Up direction is Z-axis... like in Blende ;-) */
    cout<<__LINE__<<endl;
    
    
    gluLookAt(mouse_pointer_x, mouse_pointer_y, camera_look_from_z, -0.0, -0.0, 0., 0,0,1);
    glTranslatef(-mouse_pointer_x, mouse_pointer_y, 0.0);
    
    glRotatef(rotate_x, 0, 1, 0);//x like y
    glRotatef(rotate_y, 1, 0, 0);//y like x
    glRotatef(rotate_z, 0, 0, 1);
    
    glScalef(scale_z, scale_z, scale_z);
    
    SetUpLights();
    cout<<__LINE__<<endl;
    DrawAllMeshes();
    cout<<__LINE__<<endl;
}

//TRIANGLE_STRIP = {before last vertex, last vertex and new vertex}
//TRIANGLE_FAN = first vertex, last vertex and new vertex}

