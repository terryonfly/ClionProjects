//
// Created by Terry on 15/12/31.
//

#include "vector_shape.h"

void draw_vector(GLfloat sx, GLfloat sy, GLfloat sz, GLfloat ex, GLfloat ey, GLfloat ez) {
    GLfloat earth_mat[] = {0.0f, 0.5f, 0.5f, 1.0f};
    GLfloat earth_mat_shininess = 128.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat);
    glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat);
    glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);

    glBegin(GL_LINES);
    glVertex3f(sx, sy, sz);
    glVertex3f(ex, ey, ez);
    glEnd();
    glTranslatef(ex, ey, ez);
    glutWireSphere(0.01, 20, 20);
}
