#ifndef LAB1_COMMANDS_H
#define LAB1_COMMANDS_H
#define EXEC_SUCCESS 1      // Команда выполнена успешно
#define EXEC_ERROR_FORMAT 0 // Некорректный формат команды или аргументы
#define EXEC_ERROR_FATAL -1 // Критическая ошибка (например, наступление в яму)

typedef struct {
    int sizeWasCalled;  // 1 если SIZE была, 0 если нет
    int startWasCalled; // 1 если START была, 0 если нет
    int lineNumber;     // текущий номер строки

    // Новые поля для опций
    int noDisplay;
    int noSave;
    int intervalSeconds;

    // Новое поле для отслеживания вложенности EXEC
    int execDepth;

} ExecutionState;

#include "field.h"
#include "dino.h"

int executeCommand(Field* f, Dino* d, char* line);

#endif //LAB1_COMMANDS_H