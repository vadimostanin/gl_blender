/*
 * main.cpp
 *
 *  Created on: Oct 8, 2014
 *      Author: vostanin
 */
#include<stdio.h>
#include<stdlib.h>

#define GL_GLEXT_PROTOTYPES 1
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>
#include <GL/freeglut_std.h>

#include<X11/X.h>
#include<X11/Xlib.h>
#include <iostream>
using namespace std;

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

void DrawScene();
void InitMesh();

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

void SetUpLights()
{
    float pos[] = {10, 10, 10, 0};
    float ambient[] = {0.3, 0.3, 0.3, 1};
    float white[] = {1, 1, 1, 1};
    
    glEnable(GL_LIGHTING);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    
    glEnable(GL_LIGHT0);
}

int main(int argc, char *argv[])
{
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
    glEnable(GL_CULL_FACE);
    
    glutInit(&argc, argv);
    SetUpLights();
    
    srand(time(0));

    while(1)
    {
        XNextEvent(dpy, &xev);
        
        if(xev.type == Expose)
        {
            XGetWindowAttributes(dpy, win, &gwa);
            glViewport(0, 0, gwa.width, gwa.height);
            window_width = gwa.width;
            window_height = gwa.height;
            DrawScene();
            cout<<"Invalidate"<<endl;
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
            
            cout<<"rotate_x="<<rotate_x<<endl;
            cout<<"rotate_y="<<rotate_y<<endl;
            cout<<"rotate_z="<<rotate_z<<endl;
            cout<<"camera_look_from_z="<<camera_look_from_z<<endl;
                
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
    } /* this closes while(1) { */
} /* this is the } which closes int main(int argc, char *argv[]) { */
