//
// Created by Terry on 15/12/31.
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include "four_axis_aircraft_shape.h"
#include "gl.h"

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

void draw_four_axis_aircraft() {
    int i, j;
    for (i = 0; i < 42; i++) {
        glBegin(GL_QUADS);
        if ((i >= 19 && i <= 22) || (i >= 37 && i <= 40)) {
            GLfloat earth_mat[] = {1.0f, 0.0f, 0.0f, 1.0f};
            GLfloat earth_mat_shininess = 128.0f;
            glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
            glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
            glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
            float x2 = vertext_list[quads_index[i][0]][0] - vertext_list[quads_index[i][1]][0];
            float y2 = vertext_list[quads_index[i][0]][1] - vertext_list[quads_index[i][1]][1];
            float z2 = vertext_list[quads_index[i][0]][2] - vertext_list[quads_index[i][1]][2];
            float x1 = vertext_list[quads_index[i][2]][0] - vertext_list[quads_index[i][1]][0];
            float y1 = vertext_list[quads_index[i][2]][1] - vertext_list[quads_index[i][1]][1];
            float z1 = vertext_list[quads_index[i][2]][2] - vertext_list[quads_index[i][1]][2];
            float x = y1 * z2 - y2 * z1;
            float y = z1 * x2 - z2 * x1;
            float z = x1 * y2 - x2 * y1;
            float norn = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
            x /= norn;
            y /= norn;
            z /= norn;
            glNormal3f(x, y, z);
        } else if (i < 9) {
            GLfloat earth_mat[] = {1.0f, 0.5f, 0.0f, 1.0f};
            GLfloat earth_mat_shininess = 128.0f;
            glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
            glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
            glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
            float x2 = vertext_list[quads_index[i][0]][0] - vertext_list[quads_index[i][1]][0];
            float y2 = vertext_list[quads_index[i][0]][1] - vertext_list[quads_index[i][1]][1];
            float z2 = vertext_list[quads_index[i][0]][2] - vertext_list[quads_index[i][1]][2];
            float x1 = vertext_list[quads_index[i][2]][0] - vertext_list[quads_index[i][1]][0];
            float y1 = vertext_list[quads_index[i][2]][1] - vertext_list[quads_index[i][1]][1];
            float z1 = vertext_list[quads_index[i][2]][2] - vertext_list[quads_index[i][1]][2];
            float x = y1 * z2 - y2 * z1;
            float y = z1 * x2 - z2 * x1;
            float z = x1 * y2 - x2 * y1;
            float norn = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
            x /= norn;
            y /= norn;
            z /= norn;
            glNormal3f(x, y, z);
        } else if (i < 18) {
            GLfloat earth_mat[] = {0.0f, 0.2f, 0.0f, 1.0f};
//            GLfloat earth_mat[] = {0.25f, 0.125f, 0.0f, 1.0f};
            GLfloat earth_mat_shininess = 128.0f;
            glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
            glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
            glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
            float x1 = vertext_list[quads_index[i][0]][0] - vertext_list[quads_index[i][1]][0];
            float y1 = vertext_list[quads_index[i][0]][1] - vertext_list[quads_index[i][1]][1];
            float z1 = vertext_list[quads_index[i][0]][2] - vertext_list[quads_index[i][1]][2];
            float x2 = vertext_list[quads_index[i][2]][0] - vertext_list[quads_index[i][1]][0];
            float y2 = vertext_list[quads_index[i][2]][1] - vertext_list[quads_index[i][1]][1];
            float z2 = vertext_list[quads_index[i][2]][2] - vertext_list[quads_index[i][1]][2];
            float x = y1 * z2 - y2 * z1;
            float y = z1 * x2 - z2 * x1;
            float z = x1 * y2 - x2 * y1;
            float norn = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
            x /= norn;
            y /= norn;
            z /= norn;
            glNormal3f(x, y, z);
        } else {
            GLfloat earth_mat[] = {0.8f, 0.4f, 0.0f, 1.0f};
//            GLfloat earth_mat[] = {0.5f, 0.25f, 0.0f, 1.0f};
            GLfloat earth_mat_shininess = 128.0f;
            glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
            glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
            glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
            float x2 = vertext_list[quads_index[i][0]][0] - vertext_list[quads_index[i][1]][0];
            float y2 = vertext_list[quads_index[i][0]][1] - vertext_list[quads_index[i][1]][1];
            float z2 = vertext_list[quads_index[i][0]][2] - vertext_list[quads_index[i][1]][2];
            float x1 = vertext_list[quads_index[i][2]][0] - vertext_list[quads_index[i][1]][0];
            float y1 = vertext_list[quads_index[i][2]][1] - vertext_list[quads_index[i][1]][1];
            float z1 = vertext_list[quads_index[i][2]][2] - vertext_list[quads_index[i][1]][2];
            float x = y1 * z2 - y2 * z1;
            float y = z1 * x2 - z2 * x1;
            float z = x1 * y2 - x2 * y1;
            float norn = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
            x /= norn;
            y /= norn;
            z /= norn;
            glNormal3f(x, y, z);
        }
        for (j = 0; j < 4; j++) {
            glVertex3f(vertext_list[quads_index[i][j]][0],
                       vertext_list[quads_index[i][j]][1],
                       vertext_list[quads_index[i][j]][2]);
        }
        glEnd();
    }
}

void draw_four_axis_aircraft_stipple() {
    int b, i, j;
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0x6666);
    glBegin(GL_LINES);
    for (i = 0; i < 42; i++) {
        GLfloat earth_mat[] = {0.8f, 0.8f, 0.8f, 1.0f};
        GLfloat earth_mat_shininess = 128.0f;
        glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
        glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
        glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
        for (j = 0; j < 4; j++) {
            b = j - 1;
            if (b < 0) b = 3;
            glVertex3f(vertext_list[quads_index[i][j]][0],
                       vertext_list[quads_index[i][j]][1],
                       vertext_list[quads_index[i][j]][2]);
            glVertex3f(vertext_list[quads_index[i][b]][0],
                       vertext_list[quads_index[i][b]][1],
                       vertext_list[quads_index[i][b]][2]);
        }
    }
    glEnd();
    glBegin(GL_QUADS);
    for (i = 0; i < 42; i++) {
        if ((i >= 19 && i <= 22) || (i >= 37 && i <= 40)) {
            GLfloat earth_mat[] = {0.0f, 0.5f, 0.0f, 1.0f};
            GLfloat earth_mat_shininess = 128.0f;
            glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
            glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
            glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
            for (j = 0; j < 4; j++) {
                glVertex3f(vertext_list[quads_index[i][j]][0],
                           vertext_list[quads_index[i][j]][1],
                           vertext_list[quads_index[i][j]][2]);
            }
        }
    }
    glEnd();
    glDisable(GL_LINE_STIPPLE);
}