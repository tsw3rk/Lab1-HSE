#ifndef LAB1_STATE_H
#define LAB1_STATE_H

#include "field.h" // Подключаем field.h, чтобы знать про Field
#include "dino.h"  // Подключаем dino.h, чтобы знать про Dino

// Структура для хранения снимка состояния
typedef struct {
    int width;
    int height;
    char grid[MAX_HEIGHT][MAX_WIDTH]; // MAX_HEIGHT, MAX_WIDTH берутся из field.h
    char paintGrid[MAX_HEIGHT][MAX_WIDTH];
    Dino dino; // Копия структуры Dino (x, y)
} StateSnapshot;

// Структура для хранения истории состояний (реализация через стек)
#define MAX_HISTORY_SIZE 100 // Максимальное количество сохранённых состояний

typedef struct {
    StateSnapshot* snapshots[MAX_HISTORY_SIZE];
    int top; // Индекс верхнего элемента стека
} StateHistory;

// Функции для работы со снимками
StateSnapshot* createSnapshot(Field* f, Dino* d);
void restoreSnapshot(Field* f, Dino* d, StateSnapshot* snapshot);
void freeSnapshot(StateSnapshot* snapshot);

// Функции для работы с историей
StateHistory* createStateHistory();
void pushState(StateHistory* history, StateSnapshot* snapshot);
StateSnapshot* popState(StateHistory* history);
void freeStateHistory(StateHistory* history);

#endif //LAB1_STATE_H