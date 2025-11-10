#include "dino.h"
#include <stdlib.h>
#include <stdio.h>

Dino* createDino(int x, int y) {
    Dino* d = malloc(sizeof(Dino));
    d->x = x;
    d->y = y;
    return d;
}

int moveDino(Dino* d, int dx, int dy, int width, int height) {
    d->x = (d->x + dx + width) % width;
    d->y = (d->y + dy + height) % height;
    return 1;
}