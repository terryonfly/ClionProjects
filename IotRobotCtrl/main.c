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

#include "common/gl.h"
#include "tcpclient.h"
#include "shapes/two_axis_aircraft_shape.h"
#include "shapes/vector_shape.h"
#include "shapes/plot_2d_shape.h"

GLfloat look_rad_y = 0;
GLfloat look_pos_y = 0;
int mouse_move_x = 0;
int mouse_move_y = 0;

int auto_look = 1;

GLfloat window_w = 900.f;
GLfloat window_h = 1000.f;

float ctrl_x = 0.0;
float ctrl_y = 0.0;
float ctrl_z = 0.0;
float ctrl_w = 0.0;

float plot_w = 300;
float plot_h = 200;

void display(void) {
//    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2D
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D( 0.0, window_w, 0.0, window_h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPushMatrix();
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(0.f, 0.f);
    glVertex2f(0.f, plot_h);
    glVertex2f(plot_w, plot_h);
    glVertex2f(plot_w, 0.f);
    glEnd();
    draw_plot_2d(status_x_array, MAX_PLOT_LEN, status_x_current_index, plot_w, plot_h, 0.2f, 0.f, 0.f);
    draw_plot_2d(status_y_array, MAX_PLOT_LEN, status_y_current_index, plot_w, plot_h, 0.2f, 0.2f, 0.f);
    draw_plot_2d(status_z_array, MAX_PLOT_LEN, status_z_current_index, plot_w, plot_h, 0.2f, 0.f, 0.2f);
    draw_plot_2d(status_w_array, MAX_PLOT_LEN, status_w_current_index, plot_w, plot_h, 0.f, 0.2f, 0.2f);
    glPopMatrix();

    // 3D
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, window_w / window_h, 1.0, 20.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(1.5 * sin(look_rad_y + M_PI / 6), look_pos_y + 0.4, 1.5 * cos(look_rad_y + M_PI / 6), 0, 0, 0, 0, 1, 0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);

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
        glColor3f(0.15f, 0.3f, 0.5f);// 0.5f
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
        glColor3f(0.05f, 0.35f, 0.05f);// 0.5f
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
        glColor3f(0.5f, 0.3f, 0.05f);// 0.5f
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
    draw_two_axis_aircraft(left_angle, right_angle, left_power, right_power);
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

    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);

    glFlush();
    glutSwapBuffers();
}

void init() {
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0,0.0,0.0,0.0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
//    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    window_w = (GLfloat)w;
    window_h = (GLfloat)h;
}

void idle(void) {
    if (auto_look) {
        if (mouse_move_x == 0 && look_rad_y != 0.0) {
            look_rad_y -= look_rad_y / 50;
        }
        if (mouse_move_y == 0 && look_pos_y != 0.0) {
            look_pos_y -= look_pos_y / 50;
        }
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

void sync_ctrl_msg() {
    unsigned char msg[16];
    int c_i = 0;
    unsigned char *pdata;
    int i;

    printf("ctrl : ");
    {/* Ctrl */
        printf("%6.3f ", ctrl_x);
        pdata = ((unsigned char *) &ctrl_x);
        for (i = 0; i < 4; i++) {
            msg[c_i++] = *pdata++;
        }
        printf("%6.3f ", ctrl_y);
        pdata = ((unsigned char *) &ctrl_y);
        for (i = 0; i < 4; i++) {
            msg[c_i++] = *pdata++;
        }
        printf("%6.3f ", ctrl_z);
        pdata = ((unsigned char *) &ctrl_z);
        for (i = 0; i < 4; i++) {
            msg[c_i++] = *pdata++;
        }
        printf("%6.3f ", ctrl_w);
        pdata = ((unsigned char *) &ctrl_w);
        for (i = 0; i < 4; i++) {
            msg[c_i++] = *pdata++;
        }
    }
    printf("\n");

    tcpclient_send(msg, 16);
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'r':
            auto_look = !auto_look;
            break;
        case 't':// left up
            ctrl_x += 0.001;
            sync_ctrl_msg();
            break;
        case 'g':// left down
            ctrl_x -= 0.001;
            sync_ctrl_msg();
            break;
        case 'y':// right up
            ctrl_y += 0.001;
            sync_ctrl_msg();
            break;
        case 'h':// right down
            ctrl_y -= 0.001;
            sync_ctrl_msg();
            break;
    }
}

int main(int argc, char *argv[]) {
    printf("== begin ==\n");
    tcpclient_init();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(window_w, window_h);
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