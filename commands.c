#include "commands.h"
#include <stdio.h>
#include <string.h>

// 1 - exelent(успех, правильный ввод, все хорошо)
// 0 - error(будет сообщена ошибка, неверный ввод)
// -1 -Error(критическое состание, завершение)

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
            else {return 0;}                // неизвестное направление

            int newX = d->x + dx;
            int newY = d->y + dy;

            newX = (newX + f->width) % f->width;
            newY = (newY + f->height) % f->height;

            char targetCell = f->grid[newY][newX];

            if (targetCell == '%') {                                         //Яма правило 1
                printf("Ошибка: Дино наступил на яму (%d, %d). Завершение.\n", newX, newY);
                return -1;
            }else if (targetCell == '^') {
                printf("Предупреждение: Невозможно движение на гору (%d, %d). Комманда игнорируется.\n", newX, newY);
                return 1;
            }

            //Продолжаем движение, стираем клетку, если покрашенна, то возвращаем цвет, иначе пустая клетка.

            if (f->paintGrid[d->y][d->x] != 0) {
                f->grid[d->y][d->x] = f->paintGrid[d->y][d->x];
            } else {
                f->grid[d->y][d->x] = '_';
            }

            d->x = newX;
            d->y = newY;

            f->grid[d->y][d->x] = '#';

            return 1;
        }
        return 0;

    }

    else if (strcmp(cmd, "PAINT")==0) {
        char color;  //временный цвет (a-z)
        if (sscanf(line, "PAINT %c", &color) == 1) {
            if (color >= 'a' && color <= 'z') {
                f->paintGrid[d->y][d->x] = color;
                if (f->grid[d->y][d->x] == '#') {
                    //esli dino стоял он закрывает цвет не нужно менять символ, т к должно быть видно дино
                } else {
                    f->grid[d->y][d->x] = color;
                }
                return 1;
            }
        }
        return 0;
    }

    else if (strcmp(cmd, "DIG")==0) {
        char direction[10];
        if (sscanf(line, "DIG %s", direction) == 1) {
            int startPosition_x = d->x;
            int startPosition_y = d->y; // Начальное положение нашего дInO

            //Определяем соседнюю клетку

            if (strcmp(direction, "UP") == 0){startPosition_y--;}
            else if (strcmp(direction, "DOWN") == 0){startPosition_y++;}
            else if (strcmp(direction, "LEFT") == 0){startPosition_x--;}
            else if (strcmp(direction, "RIGHT") == 0){startPosition_x++;}
            else {return 0;}

            //Логика перемещения по тору
            startPosition_x = (startPosition_x  + f->width) % f->width;
            startPosition_y = (startPosition_y + f->height) % f->height;

            f->grid[startPosition_y][startPosition_x] = '%';
            return 1;
        }

        return 0;

    } else if (strcmp(cmd, "MOUND")==0) {
        char direction[10];
        if (sscanf(line, "MOUNT %s", direction) == 1) {
            int startPosition_x = d->x;
            int startPosition_y = d->y;
            if (strcmp(direction, "UP") == 0){startPosition_y--;}
            else if (strcmp(direction, "DOWN") == 0){startPosition_y++;}
            else if (strcmp(direction, "LEFT") == 0){startPosition_x--;}
            else if (strcmp(direction, "RIGHT") == 0){startPosition_x++;}
            else {return 0;}

            startPosition_x = (startPosition_x + f->width) % f->width;
            startPosition_y = (startPosition_y + f->height) % f->height;

            if (f->grid[startPosition_y][startPosition_x] == '%') {  //Насыпание горы в яму ---> клетка становиться пустой
                f->grid[startPosition_y][startPosition_x] = '_';
            } else {
                f->grid[startPosition_y][startPosition_x] = '^';
            }
            return 1;
        }
        return 0;
    }
}