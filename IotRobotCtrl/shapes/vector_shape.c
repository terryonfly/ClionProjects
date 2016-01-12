//
// Created by Terry on 15/12/31.
//

#include "vector_shape.h"
#include "../common/gl.h"

void draw_vector(GLfloat sx, GLfloat sy, GLfloat sz,
                 GLfloat ex, GLfloat ey, GLfloat ez,
                 GLfloat r, GLfloat g, GLfloat b) {
    glColor3f(r, g, b);// 1.0f

    glBegin(GL_LINES);
    glNormal3f(1, 0, 0);
    glVertex3f(sx, sy, sz);
    glVertex3f(ex, ey, ez);
    glEnd();
    glTranslatef(ex, ey, ez);
    glutSolidSphere(0.005, 10, 10);
}
