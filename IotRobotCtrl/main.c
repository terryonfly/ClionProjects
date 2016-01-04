#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> /* netdb is necessary for struct hostent */
#include <math.h>
#include <time.h>

#include "gl.h"
#include "tcpclient.h"
#include "two_axis_aircraft_shape.h"
#include "vector_shape.h"

GLfloat look_rad_y = 0;
GLfloat look_pos_y = 0;
int mouse_move_x = 0;
int mouse_move_y = 0;

void display(void) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(1.5 * sin(look_rad_y + M_PI / 6), look_pos_y + 0.4, 1.5 * cos(look_rad_y + M_PI / 6), 0, 0, 0, 0, 1, 0);

    glPushMatrix();
    GLfloat light_position0[] = {2.0f, 2.0f, 1.0f, 1.0f};
    GLfloat light_diffuse0[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
    glPopMatrix();

    glPushMatrix();
    GLfloat light_position1[] = {-8.f, -8.f, -4.f, 1.0f};
    GLfloat light_diffuse1[] = {0.3f, 0.3f, 0.3f, 1.0f};
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
    glPopMatrix();

    glPushMatrix();
    int i;
    {
        glNormal3f(0, 1, 0);
        GLfloat earth_mat[] = {0.15f, 0.3f, 0.5f, 0.5f};
        GLfloat earth_mat_shininess = 128.0f;
        glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
        glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
        glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
        glBegin(GL_LINES);
        // Bottom
        for (i = -10; i < 10; i++) {
            glVertex3f(1.0f, -1.0f, i / 10.0f);
            glVertex3f(-1.0f, -1.0f, i / 10.0f);
        }
        for (i = -10; i < 10; i++) {
            glVertex3f(i / 10.0f, -1.0f, 1.0f);
            glVertex3f(i / 10.0f, -1.0f, -1.0f);
        }
    }
    {
        glNormal3f(1, 0, 0);
        GLfloat earth_mat[] = {0.05f, 0.35f, 0.05f, 0.5f};
        GLfloat earth_mat_shininess = 128.0f;
        glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
        glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
        glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
        glBegin(GL_LINES);
        // Left
        for (i = -10; i < 10; i++) {
            glVertex3f(-1.0f, 1.0f, i / 10.0f);
            glVertex3f(-1.0f, -1.0f, i / 10.0f);
        }
        for (i = -10; i < 10; i++) {
            glVertex3f(-1.0f, i / 10.0f, 1.0f);
            glVertex3f(-1.0f, i / 10.0f, -1.0f);
        }
    }
    {
        glNormal3f(0, 0, 1);
        GLfloat earth_mat[] = {0.5f, 0.3f, 0.05f, 0.5f};
        GLfloat earth_mat_shininess = 128.0f;
        glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
        glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
        glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
        glBegin(GL_LINES);
        // Front
        for (i = -10; i < 10; i++) {
            glVertex3f(1.0f, i / 10.0f, -1.0f);
            glVertex3f(-1.0f, i / 10.0f, -1.0f);
        }
        for (i = -10; i < 10; i++) {
            glVertex3f(i / 10.0f, 1.0f, -1.0f);
            glVertex3f(i / 10.0f, -1.0f, -1.0f);
        }
    }
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glRotatef(rotate_a, rotate_x, rotate_y, rotate_z);
    draw_two_axis_aircraft((left_angle - 0.5f) * 110.f, (right_angle - 0.5f) * 110.f, left_power * 20.f, right_power * 20.f);
    {// Accel
        glPushMatrix();
        draw_vector(0.0f, 0.0f, 0.0f, accel_x / 4, accel_y / 4, accel_z / 4, 0.5f, 1.0f, 0.0f);
        glPopMatrix();
    }

    glPopMatrix();

    // Magnet
    glPushMatrix();
    draw_vector(0.0f, 0.0f, 0.0f, magnet_x / 200, magnet_y / 200, magnet_z / 200, 0.0f, 1.0f, 1.0f);
    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}

void init() {
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0,0.0,0.0,0.0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, (GLfloat)w/(GLfloat)h, 1.0, 20.0);
}

void idle(void) {
    if (mouse_move_x == 0 && look_rad_y != 0.0) {
        look_rad_y -= look_rad_y / 50;
    }
    if (mouse_move_y == 0 && look_pos_y != 0.0) {
        look_pos_y -= look_pos_y / 50;
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) {
        mouse_move_x = x;
        mouse_move_y = y;
    } else if (state == GLUT_UP) {
        mouse_move_x = 0;
        mouse_move_y = 0;
    }
}

void mouse_move(int x,int y)
{
    look_rad_y -= (x - mouse_move_x) / 5.f * M_PI / 180;
    mouse_move_x = x;
    look_pos_y += (y - mouse_move_y) / 100.f;
    mouse_move_y = y;
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'r':
            glutIdleFunc(idle);
            break;
        case 's':
            glutIdleFunc(NULL);
            break;
    }
}

int main(int argc, char *argv[]) {
    printf("== begin ==\n");
    tcpclient_init();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(1024, 1024);
    glutCreateWindow("T'Lab");
    init();
    glutIdleFunc(idle);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(mouse_move);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    tcpclient_release();
    printf("=== end ===\n");
    return 0;
}