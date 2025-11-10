#include "commands.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// Определяем коды возврата для execute_command
#define EXEC_SUCCESS 1      // Команда выполнена успешно
#define EXEC_ERROR_FORMAT 0 // Некорректный формат команды или аргументы
#define EXEC_ERROR_FATAL -1 // Критическая ошибка (например, наступление в яму)


// Возвращает EXEC_SUCCESS (1), EXEC_ERROR_FORMAT (0), EXEC_ERROR_FATAL (-1)

int execute_command(Field* f, Dino* d, char* line) {
    char cmd[20];
    if (sscanf(line, "%19s", cmd) != 1) {
        return EXEC_ERROR_FORMAT;
    }


    // Проверка, что SIZE - первая команда, делается в main.c *** дальше все команды будут проверяться

    if (strcmp(cmd, "SIZE") == 0) {
        // Временные переменные для будут постоянно идти
        int w, h;

        if (sscanf(line, "SIZE %d %d", &w, &h) == 2) {
            if (w >= 10 && w <= 100 && h >= 10 && h <= 100) {
                return EXEC_SUCCESS;
            }
        }

        return EXEC_ERROR_FORMAT;
    }


    else if (strcmp(cmd, "START") == 0) {
        int x, y;
        if (sscanf(line, "START %d %d", &x, &y) == 2) {
            if (x >= 0 && x < f->width && y >= 0 && y < f->height) {
                d->x = x;
                d->y = y;
                // Отмечаем новое положение динозавра на поле символом '#'
                f->grid[y][x] = '#';
                return EXEC_SUCCESS;
            } else {
                return EXEC_ERROR_FORMAT;
            }
        }
        return EXEC_ERROR_FORMAT;
    }


    else if (strcmp(cmd, "MOVE") == 0) {
        char direction[10];
        if (sscanf(line, "MOVE %9s", direction) == 1) { // %9s чтобы не переполнить
            int dx = 0, dy = 0;

            // Сопоставляем строку направления с числовым смещением
            if (strcmp(direction, "UP") == 0) dy = -1;
            else if (strcmp(direction, "DOWN") == 0) dy = 1;
            else if (strcmp(direction, "LEFT") == 0) dx = -1;
            else if (strcmp(direction, "RIGHT") == 0) dx = 1;
            else return EXEC_ERROR_FORMAT;

            int newX = d->x + dx;
            int newY = d->y + dy;

            newX = (newX + f->width) % f->width;
            newY = (newY + f->height) % f->height;

            char targetCell = f->grid[newY][newX];

            if (targetCell == '%') { // Яма
                // Правило 1: Попытка наступить на яму -> ошибка и завершение
                printf("Error: Dino stepped into a pit at (%d, %d). Terminating.\n", newX, newY);
                return EXEC_ERROR_FATAL;
            } else if (targetCell == '^') { // Гора
                // Правило 2: Наступление на гору -> предупреждение, команда игнорируется
                printf("Warning: Cannot move onto a mound at (%d, %d). Command ignored.\n", newX, newY);
                return EXEC_SUCCESS;
            }

            // Стираем старое положение динозавра на поле
            if (f->paintGrid[d->y][d->x] != 0) {
                // Если клетка была покрашена, возвращаем цвет
                f->grid[d->y][d->x] = f->paintGrid[d->y][d->x];
            } else {
                // Иначе - ставим пустую клетку '_'
                f->grid[d->y][d->x] = '_';
            }

            d->x = newX;
            d->y = newY;

            f->grid[d->y][d->x] = '#';

            return EXEC_SUCCESS;
        }
        return EXEC_ERROR_FORMAT;
    }

    else if (strcmp(cmd, "PAINT") == 0) {
        char color;
        if (sscanf(line, "PAINT %c", &color) == 1) {
            if (color >= 'a' && color <= 'z') {
                f->paintGrid[d->y][d->x] = color;

                if (f->grid[d->y][d->x] != '#') {
                    f->grid[d->y][d->x] = color;
                }

                return EXEC_SUCCESS;
            }
        }
        return EXEC_ERROR_FORMAT;
    }

    else if (strcmp(cmd, "DIG") == 0) {
        char direction[10];
        if (sscanf(line, "DIG %9s", direction) == 1) {
            int targetX = d->x;
            int targetY = d->y;

            if (strcmp(direction, "UP") == 0) targetY--;
            else if (strcmp(direction, "DOWN") == 0) targetY++;
            else if (strcmp(direction, "LEFT") == 0) targetX--;
            else if (strcmp(direction, "RIGHT") == 0) targetX++;
            else return EXEC_ERROR_FORMAT;

            targetX = (targetX + f->width) % f->width;
            targetY = (targetY + f->height) % f->height;

            f->grid[targetY][targetX] = '%';

            return EXEC_SUCCESS;
        }
        return EXEC_ERROR_FORMAT;
    }

    else if (strcmp(cmd, "MOUND") == 0) {
        char direction[10]; // <-- Теперь используем 'direction'
        if (sscanf(line, "MOUND %9s", direction) == 1) {
            int targetX = d->x;
            int targetY = d->y;

            if (strcmp(direction, "UP") == 0) targetY--;
            else if (strcmp(direction, "DOWN") == 0) targetY++;
            else if (strcmp(direction, "LEFT") == 0) targetX--;
            else if (strcmp(direction, "RIGHT") == 0) targetX++;
            else return EXEC_ERROR_FORMAT;

            targetX = (targetX + f->width) % f->width;
            targetY = (targetY + f->height) % f->height;

            char targetCell = f->grid[targetY][targetX];

            // Правило 3: Насыпание горы в яму -> яма засыпается
            if (targetCell == '%') {
                f->grid[targetY][targetX] = '_';
            } else {
                f->grid[targetY][targetX] = '^';
            }
            return EXEC_SUCCESS;
        }
        return EXEC_ERROR_FORMAT;
    }


    else if (strcmp(cmd, "JUMP") == 0) {
        char direction[10];
        int n;

        if (sscanf(line, "JUMP %9s %d", direction, &n) == 2) {
            if (n <= 0) {
                return EXEC_ERROR_FORMAT;
            }


            int dx = 0, dy = 0;
            if (strcmp(direction, "UP") == 0) {
                dy = -1;
            } else if (strcmp(direction, "DOWN") == 0) {
                dy = 1;
            } else if (strcmp(direction, "LEFT") == 0) {
                dx = -1;
            } else if (strcmp(direction, "RIGHT") == 0) {
                dx = 1;
            } else {
                return EXEC_ERROR_FORMAT;
            }

            // Вычисляем начальные координаты (где стоит динозавр)
            int currentX = d->x;
            int currentY = d->y;

            int finalX = currentX;
            int finalY = currentY;

            for (int step = 0; step < n; step++) {
                int nextX = (currentX + dx + f->width) % f->width;
                int nextY = (currentY + dy + f->height) % f->height;

                if (f->grid[nextY][nextX] == '^') {
                    // Правило: Если по пути прыжка встречается гора, динозавр останавливается перед ней
                    printf("Warning: Jump stopped before mound at (%d, %d). Dino stays at (%d, %d).\n", nextX, nextY, currentX, currentY); // <-- camelCase: nextX, nextY, currentX, currentY
                    break;
                }



                currentX = nextX;
                currentY = nextY;
                // Обновляем finalX и finalY на каждом шагу, чтобы они указывали на итоговую позицию
                finalX = currentX;
                finalY = currentY;
            }

            // После цикла, finalX, finalY - это координаты, куда динозавр "приземлился" (или остановился)

            // Проверяем, не является ли итоговая клетка ямой (%)
            if (f->grid[finalY][finalX] == '%') {
                // Правило: Если динозавр приземляется на яму, ошибка и завершение
                printf("Error: Dino jumped into a pit at (%d, %d). Terminating.\n", finalX, finalY); // <-- camelCase: finalX, finalY
                return EXEC_ERROR_FATAL;
            }

            // Если дошли сюда, прыжок (или остановка) прошёл без ошибки
            // Обновляем позицию динозавра на итоговую

            if (f->paintGrid[d->y][d->x] != 0) {
                f->grid[d->y][d->x] = f->paintGrid[d->y][d->x];
            } else {
                f->grid[d->y][d->x] = '_';
            }

            // Обновляем координаты динозавра
            d->x = finalX;
            d->y = finalY;

            f->grid[d->y][d->x] = '#';
            return EXEC_SUCCESS;
        }
        return EXEC_ERROR_FORMAT;
    }

    // Если команда не подошла ни под одну из известных, возвращаем ошибку
    return EXEC_ERROR_FORMAT;
}