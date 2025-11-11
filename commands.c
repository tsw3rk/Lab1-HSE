#include "commands.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

// --- Вспомогательная функция для EXEC ---
int executeCommandsFromFile(Field* f, Dino* d, const char* filename, ExecutionState* state) {
    FILE* execFile = fopen(filename, "r");
    if (!execFile) {
        printf("Error: Could not open file '%s' for EXEC command.\n", filename);
        return EXEC_ERROR_FORMAT;
    }

    char execLine[256];
    int execResult = EXEC_SUCCESS;

    while (fgets(execLine, sizeof(execLine), execFile) != NULL) {
        if (strncmp(execLine, "//", 2) == 0) continue;

        execResult = executeCommand(f, d, execLine, state);
        if (execResult != EXEC_SUCCESS) {
            break;
        }
    }

    fclose(execFile);
    return execResult;
}

// --- Основная функция выполнения команд ---
int executeCommand(Field* f, Dino* d, char* line, ExecutionState* state) {
    char cmd[20];

    if (sscanf(line, "%19s", cmd) != 1) {
        return EXEC_SUCCESS; // Пустая строка
    }

    // --- UNDO ---
    if (strcmp(cmd, "UNDO") == 0) {
        if (state->history && state->history->top >= 0) {
            StateSnapshot* snapshot = popState(state->history);
            if (snapshot) {
                restoreSnapshot(f, d, snapshot);
                freeSnapshot(snapshot);
                return EXEC_SUCCESS;
            }
        }
        return EXEC_SUCCESS; // Нечего откатывать
    }

    // --- IF ---
    if (strcmp(cmd, "IF") == 0) {
        char* thenPtr = strstr(line, " THEN ");
        if (!thenPtr) return EXEC_ERROR_FORMAT;

        char conditionPart[256];
        char commandPart[256];
        strncpy(conditionPart, line, thenPtr - line);
        conditionPart[thenPtr - line] = '\0';
        strcpy(commandPart, thenPtr + 6);

        int x, y;
        char expectedChar;
        if (sscanf(conditionPart, "IF CELL %d %d IS %c", &x, &y, &expectedChar) != 3) {
            return EXEC_ERROR_FORMAT;
        }

        if (x < 0 || x >= f->width || y < 0 || y >= f->height) {
            return EXEC_ERROR_FORMAT;
        }

        char actualChar = f->grid[y][x];
        if (actualChar == expectedChar) {
            return executeCommand(f, d, commandPart, state); // Рекурсивный вызов для команды из IF
        }
        return EXEC_SUCCESS; // Условие не выполнено
    }

    // --- EXEC ---
    if (strcmp(cmd, "EXEC") == 0) {
        char filename[256];
        if (sscanf(line, "EXEC %255s", filename) == 1) {
            if (state->execDepth >= MAX_EXEC_DEPTH) {
                printf("Error: Maximum EXEC nesting depth exceeded.\n");
                return EXEC_ERROR_FATAL;
            }
            state->execDepth++;
            int execResult = executeCommandsFromFile(f, d, filename, state);
            state->execDepth--;
            return execResult;
        }
        return EXEC_ERROR_FORMAT;
    }

    // --- LOAD (должна быть обработана в main.c как первая команда) ---
    if (strcmp(cmd, "LOAD") == 0) {
        printf("Error: LOAD command must be the first command in the main input file.\n");
        return EXEC_ERROR_FATAL;
    }

    // --- SIZE ---
    if (strcmp(cmd, "SIZE") == 0) {
        // Обработка SIZE должна быть в main.c
        return EXEC_ERROR_FORMAT;
    }

    // --- START ---
    if (strcmp(cmd, "START") == 0) {
        // Обработка START должна быть в main.c
        return EXEC_ERROR_FORMAT;
    }

    // --- MOVE ---
    if (strcmp(cmd, "MOVE") == 0) {
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
            if (targetCell == '%') {
                printf("Error: Dino stepped into a pit at (%d, %d). Terminating.\n", newX, newY);
                return EXEC_ERROR_FATAL;
            } else if (targetCell == '^' || targetCell == '&' || targetCell == '@') {
                printf("Warning: Cannot move onto an obstacle (%c) at (%d, %d). Command ignored.\n", targetCell, newX, newY);
                return EXEC_SUCCESS;
            }

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

    // --- JUMP ---
    if (strcmp(cmd, "JUMP") == 0) {
        char direction[10];
        int n;
        if (sscanf(line, "JUMP %9s %d", direction, &n) == 2 && n > 0) {
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
                if (targetCell == '^' || targetCell == '&' || targetCell == '@') {
                    printf("Warning: Jump stopped before obstacle (%c) at (%d, %d). Dino stays at (%d, %d).\n", targetCell, nextX, nextY, currentX, currentY);
                    break;
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

    // --- PAINT ---
    if (strcmp(cmd, "PAINT") == 0) {
        char color;
        if (sscanf(line, "PAINT %c", &color) == 1 && color >= 'a' && color <= 'z') {
            f->paintGrid[d->y][d->x] = color;
            if (f->grid[d->y][d->x] != '#') {
                f->grid[d->y][d->x] = color;
            }
            return EXEC_SUCCESS;
        }
        return EXEC_ERROR_FORMAT;
    }

    // --- DIG ---
    if (strcmp(cmd, "DIG") == 0) {
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

    // --- MOUND ---
    if (strcmp(cmd, "MOUND") == 0) {
        char direction[10];
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

            if (f->grid[targetY][targetX] == '%') {
                f->grid[targetY][targetX] = '_';
            } else {
                f->grid[targetY][targetX] = '^';
            }
            return EXEC_SUCCESS;
        }
        return EXEC_ERROR_FORMAT;
    }

    // --- GROW ---
    if (strcmp(cmd, "GROW") == 0) {
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

            if (f->grid[targetY][targetX] == '_') {
                f->grid[targetY][targetX] = '&';
                return EXEC_SUCCESS;
            }
            return EXEC_ERROR_FORMAT; // Нельзя вырастить не в пустую
        }
        return EXEC_ERROR_FORMAT;
    }

    // --- CUT ---
    if (strcmp(cmd, "CUT") == 0) {
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

            if (f->grid[targetY][targetX] == '&') {
                if (f->paintGrid[targetY][targetX] != 0) {
                    f->grid[targetY][targetX] = f->paintGrid[targetY][targetX];
                } else {
                    f->grid[targetY][targetX] = '_';
                }
                return EXEC_SUCCESS;
            }
            return EXEC_ERROR_FORMAT; // Нечего срубать
        }
        return EXEC_ERROR_FORMAT;
    }

    // --- MAKE ---
    if (strcmp(cmd, "MAKE") == 0) {
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
                f->grid[targetY][targetX] = '@';
                return EXEC_SUCCESS;
            }
            return EXEC_ERROR_FORMAT; // Нельзя создать не в пустую
        }
        return EXEC_ERROR_FORMAT;
    }

    // --- PUSH ---
    if (strcmp(cmd, "PUSH") == 0) {
        char direction[10];
        if (sscanf(line, "PUSH %9s", direction) == 1) {
            int pushFromX = d->x;
            int pushFromY = d->y;
            if (strcmp(direction, "UP") == 0) pushFromY--;
            else if (strcmp(direction, "DOWN") == 0) pushFromY++;
            else if (strcmp(direction, "LEFT") == 0) pushFromX--;
            else if (strcmp(direction, "RIGHT") == 0) pushFromX++;
            else return EXEC_ERROR_FORMAT;

            pushFromX = (pushFromX + f->width) % f->width;
            pushFromY = (pushFromY + f->height) % f->height;

            if (f->grid[pushFromY][pushFromX] != '@') {
                return EXEC_ERROR_FORMAT; // Нет камня для толкания
            }

            int pushToX = pushFromX;
            int pushToY = pushFromY;
            if (strcmp(direction, "UP") == 0) pushToY++; // Противоположное DOWN
            else if (strcmp(direction, "DOWN") == 0) pushToY--;
            else if (strcmp(direction, "LEFT") == 0) pushToX++;
            else if (strcmp(direction, "RIGHT") == 0) pushToX--;

            pushToX = (pushToX + f->width) % f->width;
            pushToY = (pushToY + f->height) % f->height;

            char targetCell = f->grid[pushToY][pushToX];
            if (targetCell == '&' || targetCell == '^' || targetCell == '@') {
                return EXEC_SUCCESS; // Камень не сдвинулся
            }

            if (targetCell == '%') {
                f->grid[pushToY][pushToX] = '_';
                if (f->paintGrid[pushToY][pushToX] != 0) {
                    f->grid[pushToY][pushToX] = f->paintGrid[pushToY][pushToX];
                }
            } else {
                f->grid[pushToY][pushToX] = '@';
            }

            if (f->paintGrid[pushFromY][pushFromX] != 0) {
                f->grid[pushFromY][pushFromX] = f->paintGrid[pushFromY][pushFromX];
            } else {
                f->grid[pushFromY][pushFromX] = '_';
            }
            return EXEC_SUCCESS;
        }
        return EXEC_ERROR_FORMAT;
    }

    // --- Неизвестная команда ---
    return EXEC_ERROR_FORMAT;
}