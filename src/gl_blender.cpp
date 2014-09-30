// -- Written in C -- //

#include<stdio.h>
#include<stdlib.h>
#include<X11/X.h>
#include<X11/Xlib.h>
#define GL_GLEXT_PROTOTYPES 1
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

#include <math.h>
#include <time.h>
#include <iostream>
#include <string.h>
#include <GL/freeglut_std.h>
#include "biocycle_3.h"
#include "func.h"
using namespace std;

Display                 *dpy;
Window                  root;
GLint                   att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
XVisualInfo             *vi;
Colormap                cmap;
XSetWindowAttributes    swa;
Window                  win;
GLXContext              glc;
XWindowAttributes       gwa;
XEvent                  xev;

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600

int window_width = WINDOW_WIDTH;
int window_height = WINDOW_HEIGHT;

float rotate_x = 0.0;
float rotate_y = 0.0;
float rotate_z = 0.0;
float scale_z = 1.0;

bool mouse_button_pressed = false;
float mouse_pointer_x = 0;
float mouse_pointer_y = 0;
int start_mouse_point_x = 0;
int start_mouse_point_y = 0;

float camera_look_from_z = 4.0;

void InvalidateWindow()
{
    XExposeEvent event = {0};
//    memset(&event, 0x00, sizeof(event));

    event.type = Expose;
    event.send_event = true;
    event.display = dpy;
    event.window = win;
    event.width = WINDOW_WIDTH;
    event.height = WINDOW_HEIGHT;

    XSendEvent(dpy, win, False, ExposureMask, (XEvent *) &event);
}

template<typename T>inline T ABS(T x)
{
    return (x<0)?(-1)*x:x;
}

GLuint *vbo;
GLuint *vinx;

void InitMesh() {
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

#define BUFFER_OFFSET(x)((char *)NULL+(x))

void DrawMesh(unsigned int index, int apply_transformations)
{
    if (apply_transformations)
    {
        glPushMatrix();
        glMultMatrixf(transformations[index]);
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo[index]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vinx[index]);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof (struct vertex_struct), BUFFER_OFFSET(0));

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, sizeof (struct vertex_struct), BUFFER_OFFSET(3 * sizeof (float)));


    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof (struct vertex_struct), BUFFER_OFFSET(6 * sizeof (float)));

    glDrawElements(GL_TRIANGLES, faces_count[index] * 3, INX_TYPE, BUFFER_OFFSET(0));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    if (apply_transformations)
    {
        glPopMatrix();
    }
}

void SetUpLights()
{ 
    float pos[] = {10, 10, 10, 0};
    float ambient[] = {0.3, 0.3, 0.3, 1};
    float white[] = {1.0, 1, 1.0, 1};
    
    glEnable(GL_LIGHTING);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    
    glEnable(GL_LIGHT0);
}

void DrawAllMeshes()
{
    unsigned int i;
    const int whill_front = 66;
    const int whill_back = 69;
    
    float bike_move_forward = (-1)*rotate_z*0.1;
    
    glTranslatef(bike_move_forward, 0.0, 0.0);
    
    for (i=0; i<OBJECTS_COUNT; i++)
    {
        if( i != whill_front && i != whill_back )
        {
            glPushMatrix();
            glMultMatrixf(transformations[i]);
            
            DrawMesh(i, false);
            glPopMatrix();
        }
    }
//    float matrix[16] = { 0.0 };
//    glGetFloatv(GL_MATRIX_MODE, matrix);
//    
//    glTranslatef((-1)*matrix[3], (-1)*matrix[4*1+3], (-1)*matrix[4*2+3]);
//    
    
    for (i=0; i<OBJECTS_COUNT; i++)
    {
        if( i == whill_front || i == whill_back )
        {
            glPushMatrix();
            glMultMatrixf(transformations[i]);
//            glRotatef(rotate_x, 0, 1, 0);//x like y
//            glRotatef(rotate_y, 1, 0, 0);//y like x
            glRotatef(rotate_z, 0, 0, 1);
            DrawMesh(i, false);
            glPopMatrix();
        }
    }
    
//    glTranslatef(matrix[3], matrix[4*1+3], matrix[4*2+3]);
}

void DrawScene(float dT)
{
//    cout<<"-------DRAW START-------"<<endl<<endl<<endl;
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-50., 50., -50., 50., -50, 50.);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(mouse_pointer_x, mouse_pointer_y, camera_look_from_z, -0.0, -0.0, 0., 0,1,0);
//    glTranslatef(-mouse_pointer_x, mouse_pointer_y, 0.0);
    
    
    
    glScalef(scale_z, scale_z, scale_z);
    
    
    srand(time(0));
    
    SetUpLights();
    
#define COLOR_RANDOM 1.0//(0.1*(rand()%10) + 0.01*(rand()%10) + 0.001*(rand()%10))

    DrawAllMeshes();
}

//TRIANGLE_STRIP = {before last vertex, last vertex and new vertex}
//TRIANGLE_FAN = first vertex, last vertex and new vertex}

int main(int argc, char *argv[])
{
    float t = 0.0, T = 0.0;
    cout<<OBJECTS_COUNT<<endl;
    dpy = XOpenDisplay(NULL);
    
    if(dpy == NULL)
    {
        printf("\n\tcannot connect to X server\n\n");
        exit(0);
    }
    
    root = DefaultRootWindow(dpy);
    
    vi = glXChooseVisual(dpy, 0, att);
    
    if(vi == NULL)
    {
        printf("\n\tno appropriate visual found\n\n");
        exit(0);
    }
    else
    {
        printf("\n\tvisual %p selected\n", (void *)vi->visualid); /* %p creates hexadecimal output like in glxinfo */
    }
    
    
    cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
    
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
    
    win = XCreateWindow(dpy, root, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
    
    XMapWindow(dpy, win);
    XStoreName(dpy, win, "VERY SIMPLE APPLICATION");
    
    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);
    
    glEnable(GL_DEPTH_TEST);
    
    glutInit(&argc, argv);
    InitMesh();
    while(1)
    {
        XNextEvent(dpy, &xev);
        
        if(xev.type == Expose)
        {
            XGetWindowAttributes(dpy, win, &gwa);
            glViewport(0, 0, gwa.width, gwa.height);
            window_width = gwa.width;
            window_height = gwa.height;
            DrawScene(t);
            glXSwapBuffers(dpy, win);
        }
        else if(xev.type == KeyPress)
        {
//            cout<<xev.xbutton.button<<endl;
            switch(xev.xbutton.button)
            {
                case 113://LEFT
                        rotate_x += 1.1;
                    break;
                case 114://RIGHT
                        rotate_x -= 1.1;
                    break;
                case 111://UP
                        rotate_y += 1.1;
                    break;
                case 116://DOWN
                        rotate_y -= 1.1;
                    break;
                case 52://Z
                        rotate_z += 1.1;
                    break;
                case 53://X
                        rotate_z -= 1.1;
                    break;
                case 54://C
                    scale_z -= 0.1;
                break;
                case 55://V
                    scale_z += 0.1;
                break;
                case 56://B
                    camera_look_from_z -= 0.1;
                break;
                case 57://N
                    camera_look_from_z += 0.1;
                break;
            }
            InvalidateWindow();
            
//            cout<<"rotate_x="<<rotate_x<<endl;
//            cout<<"rotate_y="<<rotate_y<<endl;
//            cout<<"rotate_z="<<rotate_z<<endl;
//            cout<<"camera_look_from_z="<<camera_look_from_z<<endl;
                
//            glXMakeCurrent(dpy, None, NULL);
//            glXDestroyContext(dpy, glc);
//            XDestroyWindow(dpy, win);
//            XCloseDisplay(dpy);
//            exit(0);
        }
        else if(xev.type == ButtonPress)
        {
            start_mouse_point_x = xev.xbutton.x;
            start_mouse_point_y = xev.xbutton.y;
            mouse_button_pressed = true;
        }
        else if(xev.type == ButtonRelease)
        {
            mouse_button_pressed = false;
        }
        else if(xev.type == MotionNotify && mouse_button_pressed)
        {
            mouse_pointer_x = (start_mouse_point_x - xev.xmotion.x)*0.1;
            mouse_pointer_y = (start_mouse_point_y - xev.xmotion.y)*0.1;
            
            cout<<"mouse_pointer_x="<<mouse_pointer_x<<", mouse_pointer_y="<<mouse_pointer_y<<endl;
            InvalidateWindow();
        }


//        T += (t = GetTimeInterval());
//        DrawScene1(0.0);
    } /* this closes while(1) { */
} /* this is the } which closes int main(int argc, char *argv[]) { */