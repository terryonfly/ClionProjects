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
#include "four_axis_aircraft.h"

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    GLfloat light_position[] = {5.0f, 5.0f, 2.0f, 1.0f};
    GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0,GL_POSITION,light_position);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,light_diffuse);
    glPopMatrix();

    glPushMatrix();
    GLfloat earth_mat[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat earth_mat_shininess = 128.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
    glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
    glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
    glBegin(GL_LINES);
    int i;
    for (i = -100; i < 100; i++) {
        glVertex3f(-10.0f, 0.0f, i / 10.0f);
        glVertex3f(10.0f, 0.0f, i / 10.0f);
    }
    for (i = -100; i < 100; i++) {
        glVertex3f(i / 10.0f, 0.0f, -10.0f);
        glVertex3f(i / 10.0f, 0.0f, 10.0f);
    }
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glRotatef(rotate_a, rotate_x, rotate_y, rotate_z);
    draw_four_axis_aircraft();
    // Accel
    glPushMatrix();
    {
        GLfloat earth_mat[] = {0.0f, 0.5f, 0.5f, 1.0f};
        GLfloat earth_mat_shininess = 128.0f;
        glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
        glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
        glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
    }
    glBegin(GL_LINES);
    glVertex3f(0,
               0,
               0);
    glVertex3f(accel_x / 2, accel_y / 2, accel_z / 2);
    glEnd();
    glTranslatef(accel_x / 2, accel_y / 2, accel_z / 2);
    glutWireSphere(0.01, 20, 20);
    glPopMatrix();

    glPopMatrix();

    // Magnet
    glPushMatrix();
    {
        GLfloat earth_mat[] = {0.0f, 1.0f, 0.0f, 1.0f};
        GLfloat earth_mat_shininess = 128.0f;
        glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
        glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
        glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
    }
    glBegin(GL_LINES);
    glVertex3f(0,
               0,
               0);
    glVertex3f(magnet_x / 100, magnet_y / 100, magnet_z / 100);
    glEnd();
    glTranslatef(magnet_x / 100, magnet_y / 100, magnet_z / 100);
    glutWireSphere(0.01, 20, 20);
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
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, (GLfloat)w/(GLfloat)h, 1.0, 20.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 1.5, 3, 0, 0, 0, 0, 1, 0);
}

void idle(void) {
    glutPostRedisplay();
}

int main(int argc, char *argv[]) {
    printf("== begin ==\n");
    tcpclient_init();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(500, 500);
    glutCreateWindow("T'Lab");
    init();
    glutIdleFunc(idle);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMainLoop();
    tcpclient_release();
    printf("=== end ===\n");
    return 0;
}