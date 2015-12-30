#include "platform.h"
#ifdef APPLE
#include <GLUT/GLUT.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> /* netdb is necessary for struct hostent */
#include <math.h>
#include <time.h>

#include "tcpclient.h"

GLfloat r1 = 0.0f;
GLfloat r2 = 0.0f;
GLfloat r3 = 0.0f;
GLfloat rlight1 = 0.0f;

GLfloat mat_specular[] = {1.0f,1.0f,1.0f,1.0f};
GLfloat low_shiniess[] = {5.0};      // 镜面反射

GLfloat material1[] = {1.0f,0.0f,0.0f,1.0f};
GLfloat material2[] = {0.0f,1.0f,0.0f,1.0f};
GLfloat material3[] = {0.0f,0.0f,1.0f,1.0f};


GLfloat light_position1[] = {0.0f,0.0f,4.0f,1.0f};
GLfloat light_diffuse1[] = {1.0f,1.0f,1.0f,1.0f};   //白光

GLfloat light_position2[] = {1.0f,-2.0f,1.0f,1.0f}; //红光
GLfloat light_diffuse2[] = {1.0f,0.0f,0.0f,1.0f};

GLfloat light_position3[] = {0.0f,0.0f,2.0f,1.0f};
GLfloat light_diffuse3[] = {0.0f,0.0f,1.0f,1.0f};// 蓝光
GLfloat light_specular3[] = {1.0f,1.0f,1.0f,1.0f};
//漫射光
void  init ()
{
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0,0.0,0.0,0.0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
    glEnable(GL_LIGHTING);
    //glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
    //glMaterialfv(GL_FRONT,GL_SHININESS,low_shiniess);
}

void reshape(int w,int h)
{
    glViewport(0,0,(GLsizei)w,(GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0,(GLfloat)w/(GLfloat)h,0.1,100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f,0.0f,10.0f,
              0.0f,0.0f,0.0f,
              0.0f,1.0f,0.0f);


}
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glRotated(rlight1,0.0f,1.0f,0.0f);
    glLightfv(GL_LIGHT0,GL_POSITION,light_position1);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,light_diffuse1);  //世界坐标系光源
    glPopMatrix();

    glPushMatrix();
    glDisable(GL_LIGHTING);
    glRotated(rlight1,0.0f,1.0f,0.0f);
    glTranslatef(0.0f,0.0f,4.0f);
    glColor3f(1.0f,1.0f,1.0f);         //光源1 白光 的小球
    glutSolidSphere(0.05,10,10);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    glLightfv(GL_LIGHT1,GL_POSITION,light_position2);
    glLightfv(GL_LIGHT1,GL_DIFFUSE,light_diffuse2);    //观察坐标系光源

    glPushMatrix();
    glDisable(GL_LIGHTING);
    glTranslatef(1.0f,-2.0f,1.0f);   //光源2  红光的小球
    glColor3f(1.0f,0.0f,0.0f);
    glutSolidSphere(0.05,10,10);
    glEnable(GL_LIGHTING);
    glPopMatrix();



    //teapot1
    glPushMatrix();
    glTranslatef(0.0f,2.0f,0.0f);
    glRotatef(r1,0.0f,1.0f,0.0f);
    //glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,material1);
    glLightfv(GL_LIGHT2,GL_POSITION,light_position3);
    glLightfv(GL_LIGHT2,GL_DIFFUSE,light_diffuse3);
    glLightfv(GL_LIGHT2,GL_SPECULAR,light_specular3);  //
    glLightf(GL_LIGHT2,GL_SPOT_CUTOFF,45.0f);
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glTranslated(0.0f,0.0f,2.0f);
    glRotatef(r1,0.0f,1.0f,0.0f);
    glColor3f(0.0f,0.0f,1.0f);     //蓝色光源的小球
    glutSolidSphere(0.05,10,10);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glutSolidTeapot(1.0f);
    glPopMatrix();



    //teapot2
    glPushMatrix();
    glTranslatef(-2.5f,-1.0f,0.0f);
    glRotatef(r2,1.0f,0.0f,0.0f);
    //glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,material2);
    //glColor3f(1.0f,0.0f,0.0f);
    glutSolidTeapot(1.0f);
    glPopMatrix();

    //teapot3
    glPushMatrix();
    glTranslatef(2.5f,-1.0f,0.0f);
    glRotatef(r3,0.0f,0.0f,1.0f);
    //glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,material3);
    //glColor3f(1.0f,1.0f,0.0f);
    glutSolidTeapot(1.0f);
    glPopMatrix();

    glFlush();
}
void mouse_move1(int x,int y)
{
    r1+=10.0f;
    glutPostRedisplay();
}
void mouse_move2(int x,int y)
{
    r2+=10.0f;
    glutPostRedisplay();
}
void mouse_move3(int x,int y)
{
    r3+=10.0f;
    glutPostRedisplay();
}
void rotate1()
{
    rlight1+=1.0f;
    if(rlight1>360.0)
        rlight1 = rlight1-360;
    glutPostRedisplay();
}
void keyboard(unsigned char key,int x,int y)
{
    switch(key)
    {
        case '1':
            glutMotionFunc(mouse_move1);
            break;
        case '2':
            glutMotionFunc(mouse_move2);
            break;
        case '3':
            glutMotionFunc(mouse_move3);
            break;
        case 'r':
            glutIdleFunc(rotate1);
            break;
        case 's':
            glutIdleFunc(NULL);
            break;
        case 'a':        // light1
            glDisable(GL_LIGHT1);
            glDisable(GL_LIGHT2);
            glEnable(GL_LIGHT0);
            glutPostRedisplay();
            break;
        case 'b':       //light2
            glDisable(GL_LIGHT0);
            glDisable(GL_LIGHT2);
            glEnable(GL_LIGHT1);
            glutPostRedisplay();
            break;
        case 'c':      //light2
            glDisable(GL_LIGHT1);
            glDisable(GL_LIGHT0);
            glEnable(GL_LIGHT2);
            glutPostRedisplay();
            break;
    }
}

/*void mouse(int button,int state,int x,int y)
{
    switch(button){
    case GLUT_LEFT_BUTTON:
        if(state == GLUT_DOWN)
        break;
    case GLUT_RIGHT_BUTTON:
        if(state == GLUT_DOWN)
        break;
    }
}*/
int main(int argc,char **argv)
{
    printf("keyboard 1,2,3 choose the teapot   mousemove control\n");
    printf("keyboard r light1 rotated, s stop");
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(900,600);
    glutInitWindowPosition(400,100);
    glutCreateWindow("hello world");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    //glutMouseFunc(mouse);
    glutMainLoop();
    return 0;
}