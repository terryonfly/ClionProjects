//
// Created by Terry on 15/12/31.
//

#include "vector_shape.h"
#include "gl.h"

void draw_vector(GLfloat sx, GLfloat sy, GLfloat sz,
                 GLfloat ex, GLfloat ey, GLfloat ez,
                 GLfloat r, GLfloat g, GLfloat b) {
    GLfloat earth_mat[] = {r, g, b, 1.0f};
    GLfloat earth_mat_shininess = 128.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
    glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
    glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);

    glBegin(GL_LINES);
    glNormal3f(1, 0, 0);
    glVertex3f(sx, sy, sz);
    glVertex3f(ex, ey, ez);
    glEnd();
    glTranslatef(ex, ey, ez);
    glutSolidSphere(0.005, 10, 10);
}
