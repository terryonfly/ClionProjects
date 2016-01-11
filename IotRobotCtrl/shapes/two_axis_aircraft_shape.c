//
// Created by Terry on 15/12/31.
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include "four_axis_aircraft_shape.h"
#include "gl.h"
#include "vector_shape.h"

void set_normal(float p0x, float p0y, float p0z,
                float p1x, float p1y, float p1z,
                float p2x, float p2y, float p2z) {
    float x1 = p0x - p1x;
    float y1 = p0y - p1y;
    float z1 = p0z - p1z;
    float x2 = p1x - p2x;
    float y2 = p1y - p2y;
    float z2 = p1z - p2z;
    float x = y1 * z2 - y2 * z1;
    float y = z1 * x2 - z2 * x1;
    float z = x1 * y2 - x2 * y1;
    float norn = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    x /= norn;
    y /= norn;
    z /= norn;
    glNormal3f(x, y, z);
}

void draw_jet_engine(float radius, float thickness, float center_offset) {
    int i;
    int slices = 100;
    for (i = 0; i < slices; i ++) {
        GLfloat triangle_points[][3] = {
                // p1-1
                0.f,
                thickness / 2.f - center_offset,
                0.f,
                // p1-2
                radius * sin(2 * M_PI * i / slices),
                thickness / 2.f,
                radius * cos(2 * M_PI * i / slices),
                // p1-3
                radius * sin(2 * M_PI * (i + 1) / slices),
                thickness / 2.f,
                radius * cos(2 * M_PI * (i + 1) / slices),
                // p2-1
                0.f,
                - thickness / 2.f  - center_offset * 4.f,
                0.f,
                // p2-2
                radius * sin(2 * M_PI * (i + 1) / slices),
                - thickness / 2.f,
                radius * cos(2 * M_PI * (i + 1) / slices),
                // p2-3
                radius * sin(2 * M_PI * i / slices),
                - thickness / 2.f,
                radius * cos(2 * M_PI * i / slices),
        };

        glBegin(GL_QUADS);

        set_normal(triangle_points[2][0],
                   triangle_points[2][1],
                   triangle_points[2][2],
                   triangle_points[1][0],
                   triangle_points[1][1],
                   triangle_points[1][2],
                   triangle_points[4][0],
                   triangle_points[4][1],
                   triangle_points[4][2]);

        glVertex3f(triangle_points[1][0],
                   triangle_points[1][1],
                   triangle_points[1][2]);
        glVertex3f(triangle_points[2][0],
                   triangle_points[2][1],
                   triangle_points[2][2]);
        glVertex3f(triangle_points[4][0],
                   triangle_points[4][1],
                   triangle_points[4][2]);
        glVertex3f(triangle_points[5][0],
                   triangle_points[5][1],
                   triangle_points[5][2]);

        glEnd();

        glBegin(GL_TRIANGLES);

        set_normal(triangle_points[0][0],
                   triangle_points[0][1],
                   triangle_points[0][2],
                   triangle_points[1][0],
                   triangle_points[1][1],
                   triangle_points[1][2],
                   triangle_points[2][0],
                   triangle_points[2][1],
                   triangle_points[2][2]);

        glVertex3f(triangle_points[0][0],
                   triangle_points[0][1],
                   triangle_points[0][2]);
        glVertex3f(triangle_points[1][0],
                   triangle_points[1][1],
                   triangle_points[1][2]);
        glVertex3f(triangle_points[2][0],
                   triangle_points[2][1],
                   triangle_points[2][2]);

        set_normal(triangle_points[3][0],
                   triangle_points[3][1],
                   triangle_points[3][2],
                   triangle_points[4][0],
                   triangle_points[4][1],
                   triangle_points[4][2],
                   triangle_points[5][0],
                   triangle_points[5][1],
                   triangle_points[5][2]);

        glVertex3f(triangle_points[3][0],
                   triangle_points[3][1],
                   triangle_points[3][2]);
        glVertex3f(triangle_points[4][0],
                   triangle_points[4][1],
                   triangle_points[4][2]);
        glVertex3f(triangle_points[5][0],
                   triangle_points[5][1],
                   triangle_points[5][2]);

        glEnd();
    }
}

void draw_cylindrical(float radius, float thickness) {
    draw_jet_engine(radius, thickness, 0.f);
}

void draw_object(float lx, float ly, float lz) {
    GLfloat quad_points[][3] = {
            -lx / 2.f, ly / 2.f, lz / 2.f,
            lx / 2.f, ly / 2.f, lz / 2.f,
            lx / 2.f, ly / 2.f, -lz / 2.f,
            -lx / 2.f, ly / 2.f, -lz / 2.f,
            -lx / 2.f, -ly / 2.f, lz / 2.f,
            lx / 2.f, -ly / 2.f, lz / 2.f,
            lx / 2.f, -ly / 2.f, -lz / 2.f,
            -lx / 2.f, -ly / 2.f, -lz / 2.f,
    };
    GLint quad_indexs[][4] = {
            0, 1, 2, 3,
            7, 6, 5, 4,
            0, 4, 5, 1,
            1, 5, 6, 2,
            2, 6, 7, 3,
            3, 7, 4, 0
    };
    int i, j;
    for (i = 0; i < 6; i++) {
        glBegin(GL_QUADS);
        for (j = 0; j < 4; j++) {
            set_normal(quad_points[quad_indexs[i][0]][0],
                       quad_points[quad_indexs[i][0]][1],
                       quad_points[quad_indexs[i][0]][2],
                       quad_points[quad_indexs[i][1]][0],
                       quad_points[quad_indexs[i][1]][1],
                       quad_points[quad_indexs[i][1]][2],
                       quad_points[quad_indexs[i][2]][0],
                       quad_points[quad_indexs[i][2]][1],
                       quad_points[quad_indexs[i][2]][2]);
            glVertex3f(quad_points[quad_indexs[i][j]][0],
                       quad_points[quad_indexs[i][j]][1],
                       quad_points[quad_indexs[i][j]][2]);
        }
        glEnd();
    }
}

void draw_servo(float lx, float ly, float lz, float iny) {
    GLfloat quad_points[][3] = {
            -lx / 2.f, ly / 2.f - iny, lz / 2.f,
            lx / 2.f, ly / 2.f, lz / 2.f,
            lx / 2.f, ly / 2.f, -lz / 2.f,
            -lx / 2.f, ly / 2.f - iny, -lz / 2.f,
            -lx / 2.f, -ly / 2.f, lz / 2.f,
            lx / 2.f, -ly / 2.f, lz / 2.f,
            lx / 2.f, -ly / 2.f, -lz / 2.f,
            -lx / 2.f, -ly / 2.f, -lz / 2.f,
    };
    GLint quad_indexs[][4] = {
            0, 1, 2, 3,
            7, 6, 5, 4,
            0, 4, 5, 1,
            1, 5, 6, 2,
            2, 6, 7, 3,
            3, 7, 4, 0
    };
    int i, j;
    for (i = 0; i < 6; i++) {
        glBegin(GL_QUADS);
        for (j = 0; j < 4; j++) {
            set_normal(quad_points[quad_indexs[i][0]][0],
                       quad_points[quad_indexs[i][0]][1],
                       quad_points[quad_indexs[i][0]][2],
                       quad_points[quad_indexs[i][1]][0],
                       quad_points[quad_indexs[i][1]][1],
                       quad_points[quad_indexs[i][1]][2],
                       quad_points[quad_indexs[i][2]][0],
                       quad_points[quad_indexs[i][2]][1],
                       quad_points[quad_indexs[i][2]][2]);
            glVertex3f(quad_points[quad_indexs[i][j]][0],
                       quad_points[quad_indexs[i][j]][1],
                       quad_points[quad_indexs[i][j]][2]);
        }
        glEnd();
    }
}

void draw_two_axis_aircraft(float left_engine_angle, float right_engine_angle,
                            float left_engine_power, float right_engine_power) {
    GLfloat earth_mat[] = {1.0f, 0.5f, 0.0f, 1.0f};
    GLfloat earth_mat_shininess = 128.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
    glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
    glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);

    glPushMatrix();
    glTranslatef(0.0f, -0.026f, 0.f);
    draw_object(0.165f, 0.01f, 0.065f);
    glPopMatrix();

    glPushMatrix();// Left Servo
    glRotatef(0, 0, 1, 0);
    glTranslatef(0.063f, 0.007f, 0.0f);
    draw_servo(0.027f, 0.050f, 0.020f, 0.006f);
    {
        glPushMatrix();//
        glTranslatef(0.028f, -0.007f, 0.000f);
        glRotatef(left_engine_angle, 1, 0, 0);
        draw_object(0.013f, 0.025f, 0.100f);
        {
            glPushMatrix();
            glTranslatef(0.053f, 0.f, 0.f);
            draw_object(0.003f, 0.025f, 0.090f);
            glPopMatrix();
            {
                glPushMatrix();
                glTranslatef(-0.011f, 0.000f, 0.000f);
                glRotatef(90, 0, 0, 1);
                draw_cylindrical(0.010f, 0.010f);
                glPopMatrix();
            }
            {
                glPushMatrix();
                glTranslatef(0.029f, 0.006f, 0.041f);
                glRotatef(90, 0, 0, 1);
                draw_cylindrical(0.002f, 0.044f);
                glPopMatrix();
            }
            {
                glPushMatrix();
                glTranslatef(0.029f, 0.006f, -0.041f);
                glRotatef(90, 0, 0, 1);
                draw_cylindrical(0.002f, 0.044f);
                glPopMatrix();
            }
            {
                glPushMatrix();
                glTranslatef(0.029f, -0.006f, 0.041f);
                glRotatef(90, 0, 0, 1);
                draw_cylindrical(0.002f, 0.044f);
                glPopMatrix();
            }
            {
                glPushMatrix();
                glTranslatef(0.029f, -0.006f, -0.041f);
                glRotatef(90, 0, 0, 1);
                draw_cylindrical(0.002f, 0.044f);
                glPopMatrix();
            }
        }
        {
            glPushMatrix();// Left Jet Engine
            glTranslatef(0.053f, 0.f, 0.f);
            draw_jet_engine(0.035f, 0.056f, 0.008f);
            draw_vector(0.f, 0.f, 0.f,
                        0.f, left_engine_power / 100.f, 0.f,
                        1.f, left_engine_power / 100.f, 0.f);
            glPopMatrix();
        }
        glPopMatrix();
    }
    glPopMatrix();

    glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
    glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
    glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);

    glPushMatrix();// Right Servo
    glRotatef(180, 0, 1, 0);
    glTranslatef(0.063f, 0.007f, 0.0f);
    draw_servo(0.027f, 0.050f, 0.020f, 0.006f);
    {
        glPushMatrix();//
        glTranslatef(0.028f, -0.007f, 0.000f);
        glRotatef(-right_engine_angle, 1, 0, 0);
        draw_object(0.013f, 0.025f, 0.100f);
        {
            glPushMatrix();
            glTranslatef(0.053f, 0.f, 0.f);
            draw_object(0.003f, 0.025f, 0.090f);
            glPopMatrix();
            {
                glPushMatrix();
                glTranslatef(-0.011f, 0.000f, 0.000f);
                glRotatef(90, 0, 0, 1);
                draw_cylindrical(0.010f, 0.010f);
                glPopMatrix();
            }
            {
                glPushMatrix();
                glTranslatef(0.029f, 0.006f, 0.041f);
                glRotatef(90, 0, 0, 1);
                draw_cylindrical(0.002f, 0.044f);
                glPopMatrix();
            }
            {
                glPushMatrix();
                glTranslatef(0.029f, 0.006f, -0.041f);
                glRotatef(90, 0, 0, 1);
                draw_cylindrical(0.002f, 0.044f);
                glPopMatrix();
            }
            {
                glPushMatrix();
                glTranslatef(0.029f, -0.006f, 0.041f);
                glRotatef(90, 0, 0, 1);
                draw_cylindrical(0.002f, 0.044f);
                glPopMatrix();
            }
            {
                glPushMatrix();
                glTranslatef(0.029f, -0.006f, -0.041f);
                glRotatef(90, 0, 0, 1);
                draw_cylindrical(0.002f, 0.044f);
                glPopMatrix();
            }
        }
        {
            glPushMatrix();// Right Jet Engine
            glTranslatef(0.053f, 0.f, 0.f);
            draw_jet_engine(0.035f, 0.056f, 0.008f);
            draw_vector(0.f, 0.f, 0.f,
                        0.f, right_engine_power / 100.f, 0.f,
                        1.f, right_engine_power / 100.f, 0.f);
            glPopMatrix();
        }
        glPopMatrix();
    }
    glPopMatrix();
}