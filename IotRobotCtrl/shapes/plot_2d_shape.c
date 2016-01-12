//
// Created by Terry on 16/1/11.
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include "plot_2d_shape.h"
#include "../common/gl.h"

void draw_plot_2d(float *values, int value_len, int begin_index, float w, float h, float r, float g, float b) {
    int i;
    int j = begin_index;
    float pre_value = NAN;
    glPushMatrix();
    glColor3f(r, g, b);
    for (i = 0; i < value_len; i ++) {
        j --;
        if (j < 0) j = value_len - 1;

        if (pre_value != NAN) {
            glBegin(GL_LINES);
            glVertex2f(w - i, h / 2 + pre_value * 10);// old
            glVertex2f(w - i - 1, h / 2 + values[j] * 10);// new
            glEnd();
        }

        pre_value = values[j];
    }
    glPopMatrix();
}
