#ifndef LAB1_COMMANDS_H
#define LAB1_COMMANDS_H

#include "field.h"
#include "dino.h"
#include "state.h"
#include <stdio.h>


#define EXEC_SUCCESS 1
#define EXEC_ERROR_FORMAT 0
#define EXEC_ERROR_FATAL -1

// Определяем максимальную глубину вложенности EXEC
#define MAX_EXEC_DEPTH 10

// Структура для хранения состояния выполнения и опций
typedef struct {
    int sizeWasCalled;
    int startWasCalled;
    int lineNumber;

    // Новые поля для опций
    int noDisplay;
    int noSave;
    int intervalSeconds;

    // Новое поле для отслеживания вложенности EXEC
    int execDepth;

    // Новое поле для хранения истории состояний
    StateHistory* history; // Указатель на историю, создаётся в main.c
} ExecutionState;

// Объявление новой функции
int executeCommandsFromFile(Field* f, Dino* d, const char* filename, ExecutionState* state);

// Обновлённая сигнатура execute_command
int executeCommand(Field* f, Dino* d, char* line, ExecutionState* state);

#endif //LAB1_COMMANDS_H