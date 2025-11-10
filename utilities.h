#ifndef LAB1_UTILITIES_H
#define LAB1_UTILITIES_H

// Функция для очистки консоли
void clearScreen();

// Функция для задержки в секундах
void sleepSeconds(int seconds);

// Функция для вывода поля в консоль
void printFieldToConsole(struct Field* f); // struct Field*, т.к. Field может быть определена в field.h

// Функция для вывода поля в файл
void printFieldToFile(struct Field* f, FILE* file);

#endif //LAB1_UTILITIES_H