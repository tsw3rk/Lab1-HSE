#ifndef LAB1_COMMANDS_H
#define LAB1_COMMANDS_H
#define EXEC_SUCCESS 1      // Команда выполнена успешно
#define EXEC_ERROR_FORMAT 0 // Некорректный формат команды или аргументы
#define EXEC_ERROR_FATAL -1 // Критическая ошибка (например, наступление в яму)

#include "field.h"
#include "dino.h"

int executeCommand(Field* f, Dino* d, char* line);

#endif //LAB1_COMMANDS_H