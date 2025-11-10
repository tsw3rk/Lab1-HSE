#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"
#include "field.h"
#include "dino.h"

// Структура для хранения состояния выполнения
typedef struct {
    int sizeWasCalled;  // 1 если SIZE была, 0 если нет
    int startWasCalled; // 1 если START была, 0 если нет
    int lineNumber;     // текущий номер строки
} ExecutionState;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s input.txt output.txt [options]\n", argv[0]);
        return 1;
    }

    FILE* inputFile = fopen(argv[1], "r");
    if (!inputFile) {
        perror("Error opening input file");
        return 1;
    }

    ExecutionState state = {0}; // Инициализируем структуру

    char line[256]; // Буфер для строки
    int result = 1;
        while (fgets(line, sizeof(line), inputFile) != NULL) {
        state.lineNumber++;

        // Проверяем, начинается ли строка с комментария
        if (strncmp(line, "//", 2) == 0) {
            continue; // Пропускаем комментарий
        }

        // Проверяем, есть ли команда (не пустая строка или только пробелы)
        char cmd[20];
        if (sscanf(line, "%19s", cmd) == 1) {
            // Это начало проверок
            if (strcmp(cmd, "SIZE") == 0) {
                if (state.sizeWasCalled) {
                    printf("Error: SIZE command repeated at line %d. Terminating.\n", state.lineNumber);
                    result = 0; // Ошибка
                    break;
                }
                state.sizeWasCalled = 1;
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
                state.startWasCalled = 1;
            } else {
                // Любая другая команда, требующая, чтобы SIZE и START были выполнены
                if (!state.sizeWasCalled) {
                    printf("Error: Command '%s' requires SIZE first, found at line %d. Terminating.\n", cmd, state.lineNumber);
                    result = 0; // Ошибка
                    break;
                }
                if (!state.startWasCalled) {
                    printf("Error: Command '%s' requires START first, found at line %d. Terminating.\n", cmd, state.lineNumber);
                    result = 0; // Ошибка
                    break;
                }
            }

            // Создаём структуры Field и Dino (или получаем из LOAD)
            Field* f = NULL; // Предположим, что f создаётся в SIZE или LOAD
            Dino* d = NULL;  // Предположим, что d создаётся в START

        }
    }

    fclose(inputFile);

    if (result == 0) {
        return 1; // Ошибка
    }

    return 0;
}