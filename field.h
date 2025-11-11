#ifndef LAB1_FIELD_H
#define LAB1_FIELD_H

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

// Объявления функций
Field* createField(int width, int height);
void printField(Field* f);
void freeField(Field* f);

Field* loadFieldFromFile(const char* filename, Dino** dino);
#endif //LAB1_FIELD_H