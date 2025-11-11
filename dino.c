#include "dino.h"
#include <stdlib.h>

// --- Создание динозавра ---
Dino* createDino(int x, int y) {
    Dino* d = malloc(sizeof(Dino));
    if (d) {
        d->x = x;
        d->y = y;
    }
    return d;
}

// --- Освобождение памяти динозавра ---
void freeDino(Dino* d) {
    if (d) {
        free(d);
    }
}