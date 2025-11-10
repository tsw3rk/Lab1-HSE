#ifndef LAB1_COMMANDS_H
#define LAB1_COMMANDS_H
#define EXEC_SUCCESS 1      // Команда выполнена успешно
#define EXEC_ERROR_FORMAT 0 // Некорректный формат команды или аргументы
#define EXEC_ERROR_FATAL -1 // Критическая ошибка (например, наступление в яму)

#include <field.h>

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

#include "field.h"
#include "dino.h"

int executeCommandsFromFile(Field* f, Dino* d, const char* filename, ExecutionState* state); // Вспомогательная функция

int executeCommand(Field* f, Dino* d, char* line);

#endif //LAB1_COMMANDS_H