#include "field.h"
#include <stdlib.h>
#include <stdio.h>

Field* createField(int width, int height) {
    if (width < 10 || height < 10 || width > 100 || height > 100) {
        return NULL;
    }

    Field* f = malloc(sizeof(Field));
    f->width = width;
    f->height = height;

    for (int i = 0; i < height; i++) {       //OY
        for (int j = 0; j < width; j++) {    //OX
            f->grid[i][j] = '_';
            f->paintGrid[i][j] = 0;          //ne okrashena
        }
    }

    return f;

}

void printField(Field* f) {
    for (int i = 0; i < f->height; i++) {
        for (int j = 0; j < f->width; j++) {
            putchar(f->grid[i][j]);
        }
        putchar('\n');
    }
}

void freeField(Field* f) {
    free(f);
}
