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

static double angle_x = 0.f;
static double angle_y = 0.f;
static double angle_z = 0.f;

const static GLfloat vertext_list[][3] = {
        // 0
        0.0f, -0.1f, 0.05f,
        0.3f, -0.4f, 0.05f,
        0.3f, -0.5f, 0.05f,
        0.5f, -0.5f, 0.05f,
        0.5f, -0.3f, 0.05f,
        0.4f, -0.3f, 0.05f,
        0.1f, 0.0f, 0.05f,
        0.4f, 0.3f, 0.05f,
        0.5f, 0.3f, 0.05f,
        0.5f, 0.5f, 0.05f,
        0.3f, 0.5f, 0.05f,
        0.3f, 0.4f, 0.05f,
        0.0f, 0.1f, 0.05f,
        -0.3f, 0.4f, 0.05f,
        -0.3f, 0.5f, 0.05f,
        -0.5f, 0.5f, 0.05f,
        -0.5f, 0.3f, 0.05f,
        -0.4f, 0.3f, 0.05f,
        -0.1f, 0.0f, 0.05f,
        -0.4f, -0.3f, 0.05f,
        -0.5f, -0.3f, 0.05f,
        -0.5f, -0.5f, 0.05f,
        -0.3f, -0.5f, 0.05f,
        -0.3f, -0.4f, 0.05f,
        // 24
        0.0f, -0.1f, -0.05f,
        0.3f, -0.4f, -0.05f,
        0.3f, -0.5f, -0.05f,
        0.5f, -0.5f, -0.05f,
        0.5f, -0.3f, -0.05f,
        0.4f, -0.3f, -0.05f,
        0.1f, 0.0f, -0.05f,
        0.4f, 0.3f, -0.05f,
        0.5f, 0.3f, -0.05f,
        0.5f, 0.5f, -0.05f,
        0.3f, 0.5f, -0.05f,
        0.3f, 0.4f, -0.05f,
        0.0f, 0.1f, -0.05f,
        -0.3f, 0.4f, -0.05f,
        -0.3f, 0.5f, -0.05f,
        -0.5f, 0.5f, -0.05f,
        -0.5f, 0.3f, -0.05f,
        -0.4f, 0.3f, -0.05f,
        -0.1f, 0.0f, -0.05f,
        -0.4f, -0.3f, -0.05f,
        -0.5f, -0.3f, -0.05f,
        -0.5f, -0.5f, -0.05f,
        -0.3f, -0.5f, -0.05f,
        -0.3f, -0.4f, -0.05f,
        // 48
        0.3f, -0.3f, 0.05f,
        0.3f, 0.3f, 0.05f,
        -0.3f, 0.3f, 0.05f,
        -0.3f, -0.3f, 0.05f,
        // 52
        0.3f, -0.3f, -0.05f,
        0.3f, 0.3f, -0.05f,
        -0.3f, 0.3f, -0.05f,
        -0.3f, -0.3f, -0.05f,
};

const static GLint quads_index[][4] = {
        // 0
        0, 6, 12, 18,
        0, 1, 5, 6,// 1
        6, 7, 11, 12,
        12, 13, 17, 18,
        18, 19, 23, 0,// 4
        48, 2, 3, 4,// 5
        49, 8, 9, 10,
        50, 14, 15, 16,
        51, 20, 21, 22,// 8
        // 9
        24, 30, 36, 42,
        24, 25, 29, 30,// 10
        30, 31, 35, 36,
        36, 37, 41, 42,
        42, 43, 47, 24,// 13
        52, 26, 27, 28,// 14
        53, 32, 33, 34,
        54, 38, 39, 40,
        55, 44, 45, 46,// 17
        // 18
        0, 24, 25, 1,
        1, 25, 26, 2,// 19
        2, 26, 27, 3,
        3, 27, 28, 4,
        4, 28, 29, 5,// 22
        5, 29, 30, 6,
        6, 30, 31, 7,
        7, 31, 32, 8,
        8, 32, 33, 9,
        9, 33, 34, 10,
        10, 34, 35, 11,
        11, 35, 36, 12,
        12, 36, 37, 13,
        13, 37, 38, 14,
        14, 38, 39, 15,
        15, 39, 40, 16,
        16, 40, 41, 17,
        17, 41, 42, 18,
        18, 42, 43, 19,
        19, 43, 44, 20,// 37
        20, 44, 45, 21,
        21, 45, 46, 22,
        22, 46, 47, 23,// 40
        23, 47, 24, 0,
};

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30, 1, 1, 40);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 1.5, 3, 0, 0, 0, 0, 1, 0);

    GLfloat earth_mat[] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat earth_mat_shininess = 128.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
    glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
    glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
    glBegin(GL_LINES);
    for (int i = -100; i < 100; i++) {
        glVertex3f(-10.0f, 0.0f, i / 10.0f);
        glVertex3f(10.0f, 0.0f, i / 10.0f);
    }
    for (int i = -100; i < 100; i++) {
        glVertex3f(i / 10.0f, 0.0f, -10.0f);
        glVertex3f(i / 10.0f, 0.0f, 10.0f);
    }
    glEnd();

    GLfloat sun_light_position[] = {0.5f, 0.5f, 0.5f, 0.5f};
    GLfloat sun_light_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat sun_light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat sun_light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, sun_light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, sun_light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, sun_light_specular);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

//    glColor3f(1.0f, 1.0f, 0.0f);
    glRotatef(90.0, -1.0f, 0.0f, 0.0f);
    glRotatef(angle_y, 0.0f, 0.0f, -1.0f);
//    glTranslatef(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    for (int i = 0; i < 42; i++) {
        if ((i >= 19 && i <= 22) || (i >= 37 && i <= 40)) {
            GLfloat earth_mat[] = {0.5f, 0.0f, 0.0f, 1.0f};
            GLfloat earth_mat_shininess = 128.0f;
            glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
            glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
            glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
        } else if (i < 9) {
            GLfloat earth_mat[] = {1.0f, 0.5f, 0.0f, 1.0f};
            GLfloat earth_mat_shininess = 128.0f;
            glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
            glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
            glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
        } else if (i < 18) {
            GLfloat earth_mat[] = {0.25f, 0.125f, 0.0f, 1.0f};
            GLfloat earth_mat_shininess = 128.0f;
            glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
            glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
            glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
        } else {
            GLfloat earth_mat[] = {0.5f, 0.25f, 0.0f, 1.0f};
            GLfloat earth_mat_shininess = 128.0f;
            glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
            glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
            glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
        }
        for (int j = 0; j < 4; j++) {
            glVertex3f(vertext_list[quads_index[i][j]][0],
                       vertext_list[quads_index[i][j]][1],
                       vertext_list[quads_index[i][j]][2]);
        }
    }
    glEnd();

    glFlush();
    glutSwapBuffers();
}

void idle(void)
{
    angle_x = get_rotate_angle_x();
    angle_y = get_rotate_angle_y();
    angle_z = get_rotate_angle_z();
    display();
}

int main(int argc, char *argv[]) {
    printf("== begin ==\n");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(400, 400);
    glutCreateWindow("T'Lab");
    glutDisplayFunc(&display);
    glutIdleFunc(&idle);

    tcpclient_init();
    glutMainLoop();
    tcpclient_release();
    printf("=== end ===\n");
    return 0;
}