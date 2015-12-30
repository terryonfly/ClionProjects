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

const static GLfloat vertext_list[][3] = {
        // 0
        0.0f, 0.05f, 0.1f,
        0.3f, 0.05f, 0.4f,
        0.3f, 0.05f, 0.5f,
        0.5f, 0.05f, 0.5f,
        0.5f, 0.05f, 0.3f,
        0.4f, 0.05f, 0.3f,
        0.1f, 0.05f, -0.0f,
        0.4f, 0.05f, -0.3f,
        0.5f, 0.05f, -0.3f,
        0.5f, 0.05f, -0.5f,
        0.3f, 0.05f, -0.5f,
        0.3f, 0.05f, -0.4f,
        0.0f, 0.05f, -0.1f,
        -0.3f, 0.05f, -0.4f,
        -0.3f, 0.05f, -0.5f,
        -0.5f, 0.05f, -0.5f,
        -0.5f, 0.05f, -0.3f,
        -0.4f, 0.05f, -0.3f,
        -0.1f, 0.05f, -0.0f,
        -0.4f, 0.05f, 0.3f,
        -0.5f, 0.05f, 0.3f,
        -0.5f, 0.05f, 0.5f,
        -0.3f, 0.05f, 0.5f,
        -0.3f, 0.05f, 0.4f,
        // 24
        0.0f, -0.05f, 0.1f,
        0.3f, -0.05f, 0.4f,
        0.3f, -0.05f, 0.5f,
        0.5f, -0.05f, 0.5f,
        0.5f, -0.05f, 0.3f,
        0.4f, -0.05f, 0.3f,
        0.1f, -0.05f, -0.0f,
        0.4f, -0.05f, -0.3f,
        0.5f, -0.05f, -0.3f,
        0.5f, -0.05f, -0.5f,
        0.3f, -0.05f, -0.5f,
        0.3f, -0.05f, -0.4f,
        0.0f, -0.05f, -0.1f,
        -0.3f, -0.05f, -0.4f,
        -0.3f, -0.05f, -0.5f,
        -0.5f, -0.05f, -0.5f,
        -0.5f, -0.05f, -0.3f,
        -0.4f, -0.05f, -0.3f,
        -0.1f, -0.05f, -0.0f,
        -0.4f, -0.05f, 0.3f,
        -0.5f, -0.05f, 0.3f,
        -0.5f, -0.05f, 0.5f,
        -0.3f, -0.05f, 0.5f,
        -0.3f, -0.05f, 0.4f,
        // 48
        0.3f, 0.05f, 0.3f,
        0.3f, 0.05f, -0.3f,
        -0.3f, 0.05f, -0.3f,
        -0.3f, 0.05f, 0.3f,
        // 52
        0.3f, -0.05f, 0.3f,
        0.3f, -0.05f, -0.3f,
        -0.3f, -0.05f, -0.3f,
        -0.3f, -0.05f, 0.3f,
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

float alpha = 0;

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    GLfloat lightposition[] = {0.0, 0.0, 3.0, 1.0 };//光源位置
//    glLightfv(GL_LIGHT0, GL_POSITION, lightposition);//........................................前

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    gluLookAt(/*3 * sin(alpha * M_PI / 180)*/0, 1.5, /*3 * cos(alpha * M_PI / 180)*/3, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
//    glLightfv(GL_LIGHT0, GL_POSITION, lightposition);//............................................后

    glBegin(GL_LINES);
    int i, j;
    for (i = -100; i < 100; i++) {
        glVertex3f(-10.0f, 0.0f, i / 10.0f);
        glVertex3f(10.0f, 0.0f, i / 10.0f);
    }
    for (i = -100; i < 100; i++) {
        glVertex3f(i / 10.0f, 0.0f, -10.0f);
        glVertex3f(i / 10.0f, 0.0f, 10.0f);
    }
    glEnd();

//    glColor3f(1.0f, 1.0f, 0.0f);
//    glRotatef(rotate_a, rotate_x, rotate_y, rotate_z);
    glRotatef(alpha, 0, 1, 0);
//    glTranslatef(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    for (i = 0; i < 42; i++) {
//        if ((i >= 19 && i <= 22) || (i >= 37 && i <= 40)) {
////            GLfloat earth_mat[] = {0.5f, 0.0f, 0.0f, 1.0f};
//            GLfloat earth_mat[] = {1.0f, 0.5f, 0.0f, 1.0f};
//            GLfloat earth_mat_shininess = 128.0f;
//            glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
//            glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
//            glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
//            glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
//        } else if (i < 9) {
//            GLfloat earth_mat[] = {1.0f, 0.5f, 0.0f, 1.0f};
//            GLfloat earth_mat_shininess = 128.0f;
//            glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
//            glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
//            glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
//            glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
//        } else if (i < 18) {
////            GLfloat earth_mat[] = {0.25f, 0.125f, 0.0f, 1.0f};
//            GLfloat earth_mat[] = {1.0f, 0.5f, 0.0f, 1.0f};
//            GLfloat earth_mat_shininess = 128.0f;
//            glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
//            glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
//            glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
//            glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
//        } else {
////            GLfloat earth_mat[] = {0.5f, 0.25f, 0.0f, 1.0f};
//            GLfloat earth_mat[] = {1.0f, 0.5f, 0.0f, 1.0f};
//            GLfloat earth_mat_shininess = 128.0f;
//            glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
//            glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
//            glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
//            glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
//        }
        for (j = 0; j < 4; j++) {
            glVertex3f(vertext_list[quads_index[i][j]][0],
                       vertext_list[quads_index[i][j]][1],
                       vertext_list[quads_index[i][j]][2]);
        }
    }
    glEnd();

    glPopMatrix();
    glFlush();
    glutSwapBuffers();











//    int i, j;
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    GLfloat ambientLight[] = {0.3f, 0.3f, 0.3f, 1.0f};
//    GLfloat diffuseLight[] = {0.7f, 0.7f, 0.7f, 1.0f};
//    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
//    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
//    glEnable(GL_LIGHT0);
//    GLfloat lightPos[] = {50.f, 50.f, 100.f, 1.f};
//    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
//    glEnable(GL_LIGHTING);
//
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(30, 1, 1, 40);
//    glEnable(GL_DEPTH_TEST);
//
////    GLfloat sun_light_position[] = {1.5f, 1.5f, 1.5f, 0.0f};
////    GLfloat sun_light_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
////    GLfloat sun_light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
////    GLfloat sun_light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
////    glLightfv(GL_LIGHT0, GL_POSITION, sun_light_position);
////    glLightfv(GL_LIGHT0, GL_AMBIENT, sun_light_ambient);
////    glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_light_diffuse);
////    glLightfv(GL_LIGHT0, GL_SPECULAR, sun_light_specular);
////    GLfloat sun_light_position_1[] = {1.5f, 1.5f, 1.5f, 0.0f};
////    glEnable(GL_LIGHTING);
////    glEnable(GL_LIGHT0);
//
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    gluLookAt(0, 1.5, 3, 0, 0, 0, 0, 1, 0);
//
//    GLfloat earth_mat[] = {0.2f, 0.2f, 0.2f, 1.0f};
//    GLfloat earth_mat_shininess = 128.0f;
//    glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
//    glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
//    glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
//    glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
//    glBegin(GL_LINES);
//    for (i = -100; i < 100; i++) {
//        glVertex3f(-10.0f, 0.0f, i / 10.0f);
//        glVertex3f(10.0f, 0.0f, i / 10.0f);
//    }
//    for (i = -100; i < 100; i++) {
//        glVertex3f(i / 10.0f, 0.0f, -10.0f);
//        glVertex3f(i / 10.0f, 0.0f, 10.0f);
//    }
//    glEnd();
//
////    glColor3f(1.0f, 1.0f, 0.0f);
//    glRotatef(rotate_a, rotate_x, rotate_y, rotate_z);
////    glTranslatef(0.0f, 0.0f, 0.0f);
//    glBegin(GL_QUADS);
//    for (i = 0; i < 42; i++) {
//        if ((i >= 19 && i <= 22) || (i >= 37 && i <= 40)) {
//            GLfloat earth_mat[] = {0.5f, 0.0f, 0.0f, 1.0f};
//            GLfloat earth_mat_shininess = 128.0f;
//            glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
//            glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
//            glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
//            glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
//        } else if (i < 9) {
//            GLfloat earth_mat[] = {1.0f, 0.5f, 0.0f, 1.0f};
//            GLfloat earth_mat_shininess = 128.0f;
//            glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
//            glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
//            glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
//            glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
//        } else if (i < 18) {
//            GLfloat earth_mat[] = {0.25f, 0.125f, 0.0f, 1.0f};
//            GLfloat earth_mat_shininess = 128.0f;
//            glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
//            glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
//            glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
//            glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
//        } else {
//            GLfloat earth_mat[] = {0.5f, 0.25f, 0.0f, 1.0f};
//            GLfloat earth_mat_shininess = 128.0f;
//            glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
//            glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
//            glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
//            glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
//        }
//        for (j = 0; j < 4; j++) {
//            glVertex3f(vertext_list[quads_index[i][j]][0],
//                       vertext_list[quads_index[i][j]][1],
//                       vertext_list[quads_index[i][j]][2]);
//        }
//    }
//    glEnd();
//
//    // Accel
//    {
//        GLfloat earth_mat[] = {0.0f, 0.5f, 0.5f, 1.0f};
//        GLfloat earth_mat_shininess = 128.0f;
//        glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
//        glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
//        glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
//        glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
//    }
//    glBegin(GL_LINES);
//    glVertex3f(0,
//               0,
//               0);
//    glVertex3f(accel_x / 2, accel_y / 2, accel_z / 2);
//    glEnd();
//    glTranslatef(accel_x / 2, accel_y / 2, accel_z / 2);
//    glutWireSphere(0.01, 20, 20);
//    glTranslatef(-accel_x / 2, -accel_y / 2, -accel_z / 2);
//
//    float rotate_a_back = rotate_a + 180;
//    while (rotate_a_back >= 360) rotate_a_back -= 360;
//    glRotatef(-rotate_a_back, rotate_x, rotate_y, rotate_z);
//
//    // Magnet
//    {
//        GLfloat earth_mat[] = {0.0f, 1.0f, 0.0f, 1.0f};
//        GLfloat earth_mat_shininess = 128.0f;
//        glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
//        glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
//        glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
//        glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
//    }
//    glBegin(GL_LINES);
//    glVertex3f(0,
//               0,
//               0);
//    glVertex3f(magnet_x / 100, magnet_y / 100, magnet_z / 100);
//    glEnd();
//    glTranslatef(magnet_x / 100, magnet_y / 100, magnet_z / 100);
//    glutWireSphere(0.01, 20, 20);
//    glTranslatef(-magnet_x / 100, -magnet_y / 100, -magnet_z / 100);
//
//    /*
//    glEnable(GL_LINE_STIPPLE);
//    glLineStipple(2, 0x6666);
////    glColor3f(1.0f, 1.0f, 0.0f);
//    double anglex_raw = angle_x;
//    double angley_raw = angle_y;
//    double anglez_raw = angle_z;
//    glRotatef(angley_raw, 0.0f, 1.0f, 0.0f);
//    glRotatef(anglex_raw, 1.0f, 0.0f, 0.0f);
//    glRotatef(anglez_raw, 0.0f, 0.0f, 1.0f);
////    glTranslatef(0.0f, 0.1f, 0.0f);
//    glBegin(GL_LINES);
//    int b;
//    for (i = 0; i < 42; i++) {
//        GLfloat earth_mat[] = {0.8f, 0.8f, 0.8f, 1.0f};
//        GLfloat earth_mat_shininess = 128.0f;
//        glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
//        glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
//        glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
//        glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
//        for (j = 0; j < 4; j++) {
//            b = j - 1;
//            if (b < 0) b = 3;
//            glVertex3f(vertext_list[quads_index[i][j]][0],
//                       vertext_list[quads_index[i][j]][1],
//                       vertext_list[quads_index[i][j]][2]);
//            glVertex3f(vertext_list[quads_index[i][b]][0],
//                       vertext_list[quads_index[i][b]][1],
//                       vertext_list[quads_index[i][b]][2]);
//        }
//    }
//    glEnd();
//    glBegin(GL_QUADS);
//    for (i = 0; i < 42; i++) {
//        if ((i >= 19 && i <= 22) || (i >= 37 && i <= 40)) {
//            GLfloat earth_mat[] = {0.0f, 0.5f, 0.0f, 1.0f};
//            GLfloat earth_mat_shininess = 128.0f;
//            glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
//            glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
//            glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
//            glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
//            for (j = 0; j < 4; j++) {
//                glVertex3f(vertext_list[quads_index[i][j]][0],
//                           vertext_list[quads_index[i][j]][1],
//                           vertext_list[quads_index[i][j]][2]);
//            }
//        }
//    }
//    glEnd();
//    glRotatef(-angley_raw, 0.0f, 1.0f, 0.0f);
//    glRotatef(-anglex_raw, 1.0f, 0.0f, 0.0f);
//    glRotatef(-anglez_raw, 0.0f, 0.0f, 1.0f);
////    glTranslatef(0.0f, -0.1f, 0.0f);
//    glDisable(GL_LINE_STIPPLE);
//     */
//
//    glFlush();
//    glutSwapBuffers();
}

void init() {
    glClearColor(1.0,1.0,1.0,0.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat lightambient[] = {1.0,1.0,1.0,1.0};//环境光
    GLfloat lightdiffuse[] = {1.0,1.0,1.0,1.0};//漫反射
    GLfloat lightspecular[] = {1.0,1.0,1.0,1.0 };//镜面反射光
    GLfloat lightposition[] = {0.0, 0.0, 3.0, 1.0 };//光源位置

    glLightfv(GL_LIGHT0,GL_AMBIENT,lightambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,lightdiffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR,lightspecular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightposition);

    glLightModelf(GL_LIGHT_MODEL_AMBIENT, (0.0, 0.0, 0.0));

    GLfloat emission[] = {0.0, 0.0, 0.0, 1.0};//发射光
    GLfloat ambient[] ={0.2,0.2,0.2,0.0};//环境光
    GLfloat diffuse[] ={1.0,0.5,0.5,0.5};//漫反射特性
    GLfloat specular[] ={0.2,1.0,0.5,0.0 };//镜面反射光色
    GLfloat shininess[] ={100.0}; //镜面反射的光亮度

    glMaterialfv(GL_FRONT,GL_AMBIENT,ambient);
    glMaterialfv(GL_FRONT,GL_DIFFUSE,diffuse);
    glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
    glMaterialfv(GL_FRONT,GL_SHININESS,shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, (GLfloat)w/(GLfloat)h, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
}

void idle(void) {
    alpha = alpha + 0.1;
    if(alpha >= 360) alpha -= 360;
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        alpha = alpha + 45;
        if(alpha >= 360) alpha -= 360;
        glutPostRedisplay();
    }
}

int main(int argc, char *argv[]) {
    printf("== begin ==\n");
    tcpclient_init();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(500, 500);
    glutCreateWindow("T'Lab");
    glutIdleFunc(&idle);
    init();
    glutDisplayFunc(&display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMainLoop();
    tcpclient_release();
    printf("=== end ===\n");
    return 0;
}