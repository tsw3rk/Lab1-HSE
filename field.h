#ifndef LAB1_FIELD_H
#define LAB1_FIELD_H

#define MAX_WIDTH 100
#define MAX_HEIGHT 100
#define MAX_CELL_VALUE 'z'  // for painting

typedef struct {
    int width;
    int height;
    char grid[MAX_HEIGHT][MAX_WIDTH];  // символы на поле
    char paint_grid[MAX_HEIGHT][MAX_WIDTH];  // цвета клеток (a-z)
} Field;

Field* create_field(int width, int height);
void print_field(Field* f);
void free_field(Field* f);

#endif //LAB1_FIELD_H