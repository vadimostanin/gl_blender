 #include <stdarg.h>
    #include <math.h>
    #include <iostream>
    #include <fstream>
    #include <sstream>
    #include <string>
    #include <vector>
    #include <GL/glut.h>


    using namespace std;

    void display();
    void specialKeys();
    float stringToDouble(string& s);
    vector<string> explode(const string& str,const char& ch);


    double rotate_y=0; 
    double rotate_x=0;
    double rotate_z=0;


    void display(){
        cout<<__LINE__<<endl;
      float x,y,z;
      string v;
      string a,b,c;
      cout<<__LINE__<<endl;
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(-2, 2, -2, 2, 2, -2);
            
      
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      cout<<__LINE__<<endl;
    /**perform object rotation using keyboard arrows in specialKeys function**/
      glRotatef( rotate_x, 1.0, 0.0, 0.0 );
      glRotatef( rotate_y, 0.0, 1.0, 0.0 );
      glRotatef( rotate_z, 0.0, 0.0, 1.0 );

      cout<<__LINE__<<endl;
      ifstream myfile("./src/test.obj"); // open file
       if (myfile.is_open())
      {
           glColor3f(1.0, 1.0, 1.0);
           glBegin(GL_TRIANGLE_STRIP);
           cout<<__LINE__<<endl;
        while ( myfile.good())
        {
            cout<<__LINE__<<endl;
          string line;
          getline (myfile,line); // get current line
          vector<string> subs;
          subs=explode(line,' '); //explode line by space
          if(subs.empty() == false)
          {
              cout<<__LINE__<<endl;
              v=subs[0];
              cout<<__LINE__<<endl;
              if(v == "v")
              { //if the line begins with v
                  cout<<line.c_str()<<endl;
                  cout<<__LINE__<<endl;
                  /***draw polygon***/
                  
                  cout<<__LINE__<<endl;
                  a=subs[1];
                  cout<<__LINE__<<endl;
                  b=subs[2];
                  cout<<__LINE__<<endl;
                  c=subs[3];
                  cout<<__LINE__<<endl;
                  cout<<a<< " "<<b<<" "<<c<<endl;
                   x=stringToDouble(a);
                   y=stringToDouble(b);
                   z=stringToDouble(c);
                   /****draw 3D vertex*****/
                   glColor3f(1.0, 1.0, 1.0);
                   glVertex3f(x,y,z); 
                   
                   cout<<__LINE__<<endl;
               }
          }

        }
        glEnd();
        myfile.close(); //close file
      }
      

    glFlush();
    glutSwapBuffers();

cout<<__LINE__<<endl;
    }



    /**** special key***/
    void specialKeys( int key, int x, int y ) {


      if (key == GLUT_KEY_RIGHT)
        rotate_y += 5;


      else if (key == GLUT_KEY_LEFT)
        rotate_y -= 5;

      else if (key == GLUT_KEY_UP)
        rotate_x += 5;

      else if (key == GLUT_KEY_DOWN)
        rotate_x -= 5;
      
      else if (key == GLUT_KEY_PAGE_DOWN)
        rotate_z -= 5;

      else if (key == GLUT_KEY_PAGE_UP)
        rotate_z += 5;

      glutPostRedisplay();

    }



/*** main***/
    int main(int argc, char* argv[]){

      glutInit(&argc,argv);
      glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
      glutCreateWindow("Hello world");
      glEnable(GL_DEPTH_TEST);
      glutDisplayFunc(display);
      glutSpecialFunc(specialKeys);

      glutMainLoop();

      return 0;

    }



/**converts string to double***/
    float stringToDouble(string& s )
     {
       std::istringstream i(s);
       float x;
       if (!(i >> x))
         return 0;
       return x;
     } 




/***Explode : exploding strings***/
      vector<string> explode(const string& str, const char& ch) {
        string next;
        vector<string> result;

        // For each character in the string
        for (string::const_iterator it = str.begin(); it != str.end(); it++) {
            // If we've hit the terminal character
            if (*it == ch) {
                // If we have some characters accumulated
                if (!next.empty()) {
                    // Add them to the result vector
                    result.push_back(next);
                    next.clear();
                }
            } else {

                next += *it;
            }
        }
        if (!next.empty())
             result.push_back(next);
        return result;
    }