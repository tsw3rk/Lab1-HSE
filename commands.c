#include "commands.h"
#include <stdio.h>
#include <string.h>

// 1 - exelent(успех, правильный ввод, все хорошо)
// 0 - error(будет сообщена ошибка, неверный ввод, все плохо)

int executeCommand(Field* f, Dino* d, char* line) {
    char cmd[20];
    sscanf(line, "%s", cmd);

    if (strcmp(cmd, "SIZE") == 0) {
        int w,h;                                  //временные ширина и высота
        if (sscanf(line, "SIZE %d %d", &w, &h) == 2) {
            return 1;  //exelent
        }

        return 0;
    }

    else if (strcmp(cmd, "START") == 0) {
        int x,y; //временные координаты
        if (sscanf(line, "START %d %d", &x, &y) == 2) {
            if (x >= 0 && y >= 0 && x < f-> width && y < f->height) {
                d->x = x;
                d->y = y;
                f->grid[y][x] = '#';
                return 1;           //exelent
            }
        }
        return 0;

    }

    else if (strcmp(cmd, "MOVE") == 0) {
        char direction[10];
        if (sscanf(line, "MOVE %s", direction) == 1) {
            int dx,dy; //временные перемещения
            if (strcmp(direction, "UP") == 0){dy = -1;}
            else if (strcmp(direction, "DOWN") == 0){dy = 1;}
            else if (strcmp(direction, "LEFT") == 0){dx = -1;}
            else if (strcmp(direction, "RIGHT") == 0){dx = 1;}
            else return 0;// неизвестное направление
            if (f->paintGrid[d->y][d->x] != 0) {
                f->grid[d->y][d->x] = f->paintGrid[d->y][d->x];
            } else {
                f->grid[d->y][d->x] = '_';
            }
            moveDino(d, dx, dy, f->width, f->height);
            f->grid[d->y][d->x] = '#';
            return 1;
        }
        return 0;
    }
    // Будут и другие комманды
    return 0;
}