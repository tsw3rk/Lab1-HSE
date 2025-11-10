#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "commands.h"
#include "field.h"
#include "dino.h"
#include "utilities.h"



void parseOptions(int argc, char* argv[], ExecutionState* state) {
    state->noDisplay = 0;
    state->noSave = 0;
    state->intervalSeconds = 1; // По умолчанию 1 секунда

    for (int i = 3; i < argc; i++) { // Начинаем с 3-го аргумента (после ./movdino input output)
        if (strcmp(argv[i], "no-display") == 0) {
            state->noDisplay = 1;
        } else if (strcmp(argv[i], "no-save") == 0) {
            state->noSave = 1;
        } else if (strcmp(argv[i], "interval") == 0 && i + 1 < argc) {
            // Проверяем, является ли следующий аргумент числом
            char* endptr;
            long val = strtol(argv[i + 1], &endptr, 10);
            if (*endptr == '\0' && val >= 0 && val <= 10) { // Простая проверка диапазона
                state->intervalSeconds = (int)val;
                i++; // Пропускаем аргумент с числом
            } else {
                printf("Warning: Invalid interval value '%s'. Using default 1 second.\n", argv[i + 1]);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s input.txt output.txt [options]\n", argv[0]);
        printf("Options: interval N, no-display, no-save\n");
        return 1;
    }

    ExecutionState state = {0};
    parseOptions(argc, argv, &state); // Парсим опции

    FILE* inputFile = fopen(argv[1], "r");
    if (!inputFile) {
        perror("Error opening input file");
        return 1;
    }

    // Предположим, что f и d создаются в процессе выполнения команд SIZE и START
    Field* f = NULL;
    Dino* d = NULL;
    int result = 1;
    char line[256];

    if (fgets(line, sizeof(line), inputFile) != NULL) {
        state.lineNumber = 1; // Устанавливаем номер строки

        // Пропускаем комментарии в начале файла
        // Используем while, чтобы обработать несколько комментариев подряд

        while (strncmp(line, "//", 2) == 0) {
            if (fgets(line, sizeof(line), inputFile) == NULL) {
                // Файл закончился только комментариями
                printf("Error: Input file contains only comments or is empty.\n");
                result = 0;
                break; // Выходим из while, а затем из внешнего if
            }
            state.lineNumber++;
        }

        // Если цикл был прерван из-за ошибки (файл пуст после комментариев),
        // переменная result уже установлена в 0.
        if (result == 1) { // Только если мы не вышли по ошибке из цикла
            // Теперь line содержит первую "реальную" команду
            char firstCmd[20];
            if (sscanf(line, "%19s", firstCmd) == 1) {
                if (strcmp(firstCmd, "LOAD") == 0) {
                    char filename[256];
                    if (sscanf(line, "LOAD %255s", filename) == 1) {
                        // Вызываем функцию загрузки поля из файла
                        f = loadFieldFromFile(filename, &d); // d будет создана внутри loadFieldFromFile
                        if (f && d) {
                            state.sizeWasCalled = 1; // Поле загружено
                            state.startWasCalled = 1; // Динозавр загружен
                            // Продолжаем выполнение команд из основного файла inputFile
                        } else {
                            printf("Error: Failed to load field from '%s'. Terminating.\n", filename);
                            result = 0;
                        }
                    } else {
                        printf("Error: Invalid LOAD format at line %d. Terminating.\n", state.lineNumber);
                        result = 0;
                    }
                } else {
                    // Проверим, является ли она SIZE.
                    if (strcmp(firstCmd, "SIZE") != 0) {
                        printf("Error: First non-comment command must be SIZE or LOAD. Found '%s' at line %d. Terminating.\n", firstCmd, state.lineNumber);
                        result = 0;

                    } else {
                        // Обрабатываем SIZE из line
                        int w, h;
                        if (sscanf(line, "SIZE %d %d", &w, &h) == 2 && w >= 10 && w <= 100 && h >= 10 && h <= 100) {
                            if (f) { freeField(f); } // Если f уже создана, освобождаем
                            f = createField(w, h);
                            if (f) {
                                state.sizeWasCalled = 1;
                            } else {
                                printf("Error: Failed to create field at line %d. Terminating.\n", state.lineNumber);
                                result = 0;

                            }
                        } else {
                            printf("Error: Invalid SIZE format or dimensions at line %d. Terminating.\n", state.lineNumber);
                            result = 0;

                        }
                    }
                }
            } else {
                // Строка не содержит команды
                printf("Error: First non-comment line does not contain a command at line %d. Terminating.\n", state.lineNumber);
                result = 0;
            }
        }
    } else {
        // Файл пустой
        printf("Error: Input file is empty.\n");
        result = 0;
    }

    // Основной цикл чтения команд
    while (fgets(line, sizeof(line), inputFile) != NULL) {
        state.lineNumber++;

        // Проверяем, начинается ли строка с комментария
        if (strncmp(line, "//", 2) == 0) {
            continue; // Пропускаем комментарий
        }

        char cmd[20];
        if (sscanf(line, "%19s", cmd) == 1) {
            // Проверки последовательности команд (как в Части 4)
            if (strcmp(cmd, "SIZE") == 0) {
                if (state.sizeWasCalled) {
                    printf("Error: SIZE command repeated at line %d. Terminating.\n", state.lineNumber);
                    result = 0;
                    break;
                }
                int w, h;
                if (sscanf(line, "SIZE %d %d", &w, &h) == 2 && w >= 10 && w <= 100 && h >= 10 && h <= 100) {
                    if (f) { freeField(f); } // Если f уже создана, освобождаем
                    f = createField(w, h);
                    if (f) {
                        state.sizeWasCalled = 1;
                    } else {
                        printf("Error: Failed to create field at line %d. Terminating.\n", state.lineNumber);
                        result = 0;
                        break;
                    }
                } else {
                    printf("Error: Invalid SIZE format or dimensions at line %d. Terminating.\n", state.lineNumber);
                    result = 0;
                    break;
                }
            } else if (strcmp(cmd, "START") == 0) {
                if (!state.sizeWasCalled) {
                    printf("Error: START command before SIZE at line %d. Terminating.\n", state.lineNumber);
                    result = 0;
                    break;
                }
                if (state.startWasCalled) {
                    printf("Error: START command repeated at line %d. Terminating.\n", state.lineNumber);
                    result = 0;
                    break;
                }
                // Выполняем команду START, создаём d
                int x, y;
                if (sscanf(line, "START %d %d", &x, &y) == 2 && x >= 0 && x < f->width && y >= 0 && y < f->height) {
                    if (d) { freeDino(d); } // Если d уже создана, освобождаем
                    d = createDino(x, y);
                    if (d) {
                        f->grid[y][x] = '#'; // Отмечаем динозавра на поле
                        state.startWasCalled = 1;
                    } else {
                        printf("Error: Failed to create dino at line %d. Terminating.\n", state.lineNumber);
                        result = 0;
                        break;
                    }
                } else {
                    printf("Error: Invalid START format or coordinates out of bounds at line %d. Terminating.\n", state.lineNumber);
                    result = 0;
                    break;
                }
            } else {
                // Любая другая команда, требующая, чтобы SIZE и START были выполнены
                if (!state.sizeWasCalled) {
                    printf("Error: Command '%s' requires SIZE first, found at line %d. Terminating.\n", cmd, state.lineNumber);
                    result = 0;
                    break;
                }
                if (!state.startWasCalled) {
                    printf("Error: Command '%s' requires START first, found at line %d. Terminating.\n", cmd, state.lineNumber);
                    result = 0;
                    break;
                }

                // Вызываем executeСommand для остальных команд
                result = executeCommand(f, d, line);
                if (result == EXEC_ERROR_FORMAT) {
                    printf("Error: Unknown or invalid command '%s' at line %d. Terminating.\n", line, state.lineNumber);
                    break;
                } else if (result == EXEC_ERROR_FATAL) {
                    // Ошибка, вызванная внутри execute_command (например, наступление в яму)
                    // execute_command сам выводит сообщение
                    break; // Программа завершается
                }
                if (result == 1 && strcmp(cmd, "UNDO") != 0) { // Убедимся, что команда не была UNDO
                    StateSnapshot* currentSnapshot = createSnapshot(f, d);
                    if (currentSnapshot) {
                        pushState(state.history, currentSnapshot);
                    } else {
                        printf("Error: Failed to create state snapshot. Terminating.\n");
                        result = 0; // Устанавливаем ошибку
                        break;
                    }
                }
            }
        }

        if (result == 1 && !state.noDisplay) {
            clearScreen();
            printFieldToConsole(f);
            sleepSeconds(state.intervalSeconds);
        }
    }

    fclose(inputFile);

    if (result != 0 && !state.noSave && f != NULL) {
        FILE* outputFile = fopen(argv[2], "w");
        if (outputFile) {
            printFieldToFile(f, outputFile);
            fclose(outputFile);
        } else {
            perror("Error opening output file for writing");
            result = 0;
        }
    }

    if (result == 0) {
        return 1;
    }
}