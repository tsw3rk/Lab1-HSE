#include "commands.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define MAX_EXEC_DEPTH 10 // Определяем максимальную глубину вложенности EXEC

// Определяем коды возврата для execute_command
#define EXEC_SUCCESS 1      // Команда выполнена успешно
#define EXEC_ERROR_FORMAT 0 // Некорректный формат команды или аргументы
#define EXEC_ERROR_FATAL -1 // Критическая ошибка (например, наступление в яму)


// Возвращает EXEC_SUCCESS (1), EXEC_ERROR_FORMAT (0), EXEC_ERROR_FATAL (-1)

int execute_command_internal(Field* f, Dino* d, char* line, ExecutionState* state) {
    return executeCommand(f, d, line, state);
} // Вызываем основную функцию

int executeСommand(Field* f, Dino* d, char* line,  ExecutionState* state) {
    char cmd[20];
    if (sscanf(line, "%19s", cmd) != 1) {
        return EXEC_ERROR_FORMAT;
    }


    // Проверка, что SIZE - первая команда, делается в main.c дальше все команды будут проверяться

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
        if (sscanf(line, "MOVE %9s", direction) == 1) {
            int dx = 0, dy = 0;

            if (strcmp(direction, "UP") == 0) dy = -1;
            else if (strcmp(direction, "DOWN") == 0) dy = 1;
            else if (strcmp(direction, "LEFT") == 0) dx = -1;
            else if (strcmp(direction, "RIGHT") == 0) dx = 1;
            else return EXEC_ERROR_FORMAT;

            int newX = (d->x + dx + f->width) % f->width;
            int newY = (d->y + dy + f->height) % f->height;

            char targetCell = f->grid[newY][newX];

            if (targetCell == '%') { // Яма
                printf("Error: Dino stepped into a pit at (%d, %d). Terminating.\n", newX, newY);
                return EXEC_ERROR_FATAL;
            } else if (targetCell == '^') { // Гора
                printf("Warning: Cannot move onto a mound at (%d, %d). Command ignored.\n", newX, newY);
                return EXEC_SUCCESS; // Команда "выполнена", но без эффекта
            } else if (targetCell == '&' || targetCell == '@') { // Дерево или Камень
                // Правило: Нельзя наступить на дерево или камень
                printf("Warning: Cannot move onto an obstacle (%c) at (%d, %d). Command ignored.\n", targetCell, newX, newY);
                return EXEC_SUCCESS; // Команда "выполнена", но без эффекта
            }
            // Если целевая клетка _, #, a-z - можно идти

            // Стираем старую позицию
            if (f->paintGrid[d->y][d->x] != 0) {
                f->grid[d->y][d->x] = f->paintGrid[d->y][d->x];
            } else {
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
            if (strcmp(direction, "UP") == 0) dy = -1;
            else if (strcmp(direction, "DOWN") == 0) dy = 1;
            else if (strcmp(direction, "LEFT") == 0) dx = -1;
            else if (strcmp(direction, "RIGHT") == 0) dx = 1;
            else return EXEC_ERROR_FORMAT;

            int currentX = d->x;
            int currentY = d->y;

            int finalX = currentX;
            int finalY = currentY;

            for (int step = 0; step < n; step++) {
                int nextX = (currentX + dx + f->width) % f->width;
                int nextY = (currentY + dy + f->height) % f->height;

                char targetCell = f->grid[nextY][nextX];

                if (targetCell == '^') { // Гора
                    // Правило: Если по пути прыжка встречается гора, динозавр останавливается перед ней
                    printf("Warning: Jump stopped before mound at (%d, %d). Dino stays at (%d, %d).\n", nextX, nextY, currentX, currentY);
                    break; // Останавливаем прыжок
                } else if (targetCell == '&' || targetCell == '@') { // Дерево или Камень
                    // Правило: Если по пути прыжка встречается дерево или камень, динозавр останавливается перед ним
                    printf("Warning: Jump stopped before obstacle (%c) at (%d, %d). Dino stays at (%d, %d).\n", targetCell, nextX, nextY, currentX, currentY);
                    break; // Останавливаем прыжок
                }


                currentX = nextX;
                currentY = nextY;
                finalX = currentX;
                finalY = currentY;
            }


            if (f->grid[finalY][finalX] == '%') {
                printf("Error: Dino jumped into a pit at (%d, %d). Terminating.\n", finalX, finalY);
                return EXEC_ERROR_FATAL;
            }


            if (f->paintGrid[d->y][d->x] != 0) {
                f->grid[d->y][d->x] = f->paintGrid[d->y][d->x];
            } else {
                f->grid[d->y][d->x] = '_';
            }

            d->x = finalX;
            d->y = finalY;

            f->grid[d->y][d->x] = '#';

            return EXEC_SUCCESS;
        }
        return EXEC_ERROR_FORMAT;
    }

    else if (strcmp(cmd, "GROW") == 0) {
        char direction[10];
        if (sscanf(line, "GROW %9s", direction) == 1) {
            int targetX = d->x;
            int targetY = d->y;

            if (strcmp(direction, "UP") == 0) targetY--;
            else if (strcmp(direction, "DOWN") == 0) targetY++;
            else if (strcmp(direction, "LEFT") == 0) targetX--;
            else if (strcmp(direction, "RIGHT") == 0) targetX++;
            else return EXEC_ERROR_FORMAT;


            targetX = (targetX + f->width) % f->width;
            targetY = (targetY + f->height) % f->height;

            // Проверяем: можно создать дерево только в пустой клетке
            if (f->grid[targetY][targetX] == '_') {
                f->grid[targetY][targetX] = '&'; // Создаём дерево
                return EXEC_SUCCESS;
            } else {
                // Нельзя создать дерево не в пустой клетке
                return EXEC_ERROR_FORMAT; // Ошибка формата или логики (зависит от требований)
                // Можно возвращать EXEC_ERROR_FORMAT или выводить предупреждение и игнорировать.
                // Пусть будет ошибка формата.
            }
        }
        return EXEC_ERROR_FORMAT;
    }


    else if (strcmp(cmd, "CUT") == 0) {
        char direction[10];
        if (sscanf(line, "CUT %9s", direction) == 1) {
            int targetX = d->x;
            int targetY = d->y;

            if (strcmp(direction, "UP") == 0) targetY--;
            else if (strcmp(direction, "DOWN") == 0) targetY++;
            else if (strcmp(direction, "LEFT") == 0) targetX--;
            else if (strcmp(direction, "RIGHT") == 0) targetX++;
            else return EXEC_ERROR_FORMAT;


            targetX = (targetX + f->width) % f->width;
            targetY = (targetY + f->height) % f->height;

            // Проверяем: можно срубить только дерево
            if (f->grid[targetY][targetX] == '&') {
                if (f->paintGrid[targetY][targetX] != 0) {
                    f->grid[targetY][targetX] = f->paintGrid[targetY][targetX];
                } else {
                    f->grid[targetY][targetX] = '_';
                }
                return EXEC_SUCCESS;
            } else {
                return EXEC_ERROR_FORMAT;
            }
        }
        return EXEC_ERROR_FORMAT;
    }

    else if (strcmp(cmd, "MAKE") == 0) {
        char direction[10];
        if (sscanf(line, "MAKE %9s", direction) == 1) {
            int targetX = d->x;
            int targetY = d->y;

            if (strcmp(direction, "UP") == 0) targetY--;
            else if (strcmp(direction, "DOWN") == 0) targetY++;
            else if (strcmp(direction, "LEFT") == 0) targetX--;
            else if (strcmp(direction, "RIGHT") == 0) targetX++;
            else return EXEC_ERROR_FORMAT;


            targetX = (targetX + f->width) % f->width;
            targetY = (targetY + f->height) % f->height;


            if (f->grid[targetY][targetX] == '_') {
                f->grid[targetY][targetX] = '@'; // Создаём камень
                return EXEC_SUCCESS;
            } else {
                // Нельзя создать камень не в пустой клетке
                return EXEC_ERROR_FORMAT;
            }
        }
        return EXEC_ERROR_FORMAT;
    }


    else if (strcmp(cmd, "PUSH") == 0) {
        char direction[10];
        if (sscanf(line, "PUSH %9s", direction) == 1) {
            // Определяем координаты "соседней" клетки, откуда будем толкать камень
            int pushFromX = d->x;
            int pushFromY = d->y;

            if (strcmp(direction, "UP") == 0) pushFromY--;
            else if (strcmp(direction, "DOWN") == 0) pushFromY++;
            else if (strcmp(direction, "LEFT") == 0) pushFromX--;
            else if (strcmp(direction, "RIGHT") == 0) pushFromX++;
            else return EXEC_ERROR_FORMAT;


            pushFromX = (pushFromX + f->width) % f->width;
            pushFromY = (pushFromY + f->height) % f->height;

            // Проверяем: там ли камень?
            if (f->grid[pushFromY][pushFromX] != '@') {
                // Нельзя толкнуть, если там нет камня
                return EXEC_ERROR_FORMAT;
            }

            // Определяем координаты "следующей" клетки, куда пойдёт камень

            int pushToX = pushFromX;
            int pushToY = pushFromY;

            if (strcmp(direction, "UP") == 0) pushToY++;
            else if (strcmp(direction, "DOWN") == 0) pushToY--;
            else if (strcmp(direction, "LEFT") == 0) pushToX++;
            else if (strcmp(direction, "RIGHT") == 0) pushToX--;


            pushToX = (pushToX + f->width) % f->width;
            pushToY = (pushToY + f->height) % f->height;


            char targetCell = f->grid[pushToY][pushToX];
            if (targetCell == '&' || targetCell == '^' || targetCell == '@') {
                // Камень не может двигаться в дерево, гору или другой камень
                return EXEC_SUCCESS;
            }

            // Если всё ок, перемещаем камень

            if (targetCell == '%') { // Яма
                // Камень падает в яму, яма засыпается
                f->grid[pushToY][pushToX] = '_';
                // Если яма была покрашена, цвет сохраняется
                if (f->paintGrid[pushToY][pushToX] != 0) {
                    f->grid[pushToY][pushToX] = f->paintGrid[pushToY][pushToX];
                }
            } else {
                // Камень просто перемещается в пустую или цветную клетку
                f->grid[pushToY][pushToX] = '@';
            }

            // Освобождаем старую клетку, где был камень

            if (f->paintGrid[pushFromY][pushFromX] != 0) {
                f->grid[pushFromY][pushFromX] = f->paintGrid[pushFromY][pushFromX];
            } else {
                f->grid[pushFromY][pushFromX] = '_';
            }

            return EXEC_SUCCESS;
        }
        return EXEC_ERROR_FORMAT;
    }


    else if (strcmp(cmd, "EXEC") == 0) {
        char filename[256];
        if (sscanf(line, "EXEC %255s", filename) == 1) {
            // Проверяем глубину вложенности
            if (state->execDepth >= MAX_EXEC_DEPTH) {
                printf("Error: Maximum EXEC nesting depth exceeded.\n");
                return EXEC_ERROR_FATAL; // или EXEC_ERROR_FORMAT
            }
            state->execDepth++; // Увеличиваем счётчик

            // Вызываем функцию для выполнения команд из файла
            int execResult = executeCommandsFromFile(f, d, filename, state);

            state->execDepth--; // Уменьшаем счётчик при выходе

            return execResult; // Возвращаем результат выполнения файла
        }
        return EXEC_ERROR_FORMAT; // Ошибка при извлечении имени файла
    }


    else if (strcmp(cmd, "LOAD") == 0) {
        // Если мы дошли до этого места, значит LOAD НЕ была первой командой
        // Это ошибка.
        printf("Error: LOAD command must be the first command in the file.\n");
        return EXEC_ERROR_FATAL; // Критическая ошибка
    }

    else if (strcmp(cmd, "UNDO") == 0) {
            // Проверяем, есть ли что откатывать
            if (state->history && state->history->top >= 0) {
                StateSnapshot* snapshot = popState(state->history);
                if (snapshot) {
                    // Восстанавливаем состояние из снимка
                    restoreSnapshot(f, d, snapshot);
                    // Освобождаем память снимка
                    freeSnapshot(snapshot);
                    // Возвращаем успех
                    return EXEC_SUCCESS;
                }
            }
            // Если история пуста или произошла ошибка при извлечении
            // Возвращаем успех, но ничего не делаем (или можно выводить предупреждение)
            return EXEC_SUCCESS; // Или EXEC_ERROR_FORMAT если хочется ошибку
        }

        else if (strcmp(cmd, "IF") == 0) {

            // Найдём "THEN"
            char* then_ptr = strstr(line, " THEN ");
            if (!then_ptr) {
                return EXEC_ERROR_FORMAT; // Нет "THEN"
            }

            // Разбиваем строку на "IF CELL x y IS <char>" и "<command>"

            char condition_part[256];
            char command_part[256];
            strncpy(condition_part, line, then_ptr - line);
            condition_part[then_ptr - line] = '\0';
            strcpy(command_part, then_ptr + 6);


            int x, y;
            char expected_char;
            if (sscanf(condition_part, "IF CELL %d %d IS %c", &x, &y, &expected_char) != 3) {
                return EXEC_ERROR_FORMAT;
            }

            // Проверяем границы координат
            if (x < 0 || x >= f->width || y < 0 || y >= f->height) {
                return EXEC_ERROR_FORMAT; // Координаты за пределами поля
            }

            // Получаем символ из поля
            char actual_char = f->grid[y][x];

            // Проверяем, совпадает ли символ

            if (actual_char == expected_char) {

                int if_result = execute_command_internal(f, d, command_part, state);
                return if_result; // Возвращаем результат выполнения команды в IF
            } else {
                // Условие не выполнено, команда игнорируется
                return EXEC_SUCCESS; // Или EXEC_SUCCESS, если просто пропускаем
            }
        }

        // Если команда не подошла ни под одну из известных, возвращаем ошибку
        return EXEC_ERROR_FORMAT;
    }
}
