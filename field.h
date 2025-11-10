#ifndef LAB1_FIELD_H
#define LAB1_FIELD_H


#include "field.h"
#include "dino.h"

#define MAX_WIDTH 100
#define MAX_HEIGHT 100
#define MAX_CELL_VALUE 'z'  // for painting

typedef struct {
    int width;
    int height;
    char grid[MAX_HEIGHT][MAX_WIDTH];  // символы на поле
    char paintGrid[MAX_HEIGHT][MAX_WIDTH];  // цвета клеток (a-z)
} Field;

Field* createField(int width, int height);
void printField(Field* f);
void freeField(Field* f);
Field* loadFieldFromFile(const char* filename, struct Dino** dino);

typedef struct {
    int width;
    int height;
    char grid[MAX_HEIGHT][MAX_WIDTH];
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

#endif //LAB1_FIELD_H