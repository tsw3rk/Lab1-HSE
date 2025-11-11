#include "field.h"
#include "dino.h" // Для Dino
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Вспомогательная функция для подсчёта размеров при загрузке ---
static int getFieldDimensions(FILE* file, int* width, int* height) {
    char tempLine[256];
    *height = 0;
    *width = -1;

    long pos = ftell(file);

    while (fgets(tempLine, sizeof(tempLine), file) != NULL) {
        int len = strlen(tempLine);
        if (len > 0 && tempLine[len - 1] == '\n') {
            len--;
        }
        if (len == 0) continue;

        if (*width == -1) {
            *width = len;
        } else if (len != *width) {
            printf("Error loading field: Inconsistent row lengths.\n");
            return 0;
        }
        (*height)++;
    }

    if (*width < 10 || *width > 100 || *height < 10 || *height > 100) {
        printf("Error loading field: Dimensions (%d x %d) out of range.\n", *width, *height);
        return 0;
    }

    fseek(file, pos, SEEK_SET);
    return 1;
}

// --- Создание поля ---
Field* createField(int width, int height) {
    if (width < 10 || width > 100 || height < 10 || height > 100) {
        return NULL;
    }

    Field* f = malloc(sizeof(Field));
    if (!f) return NULL;

    f->width = width;
    f->height = height;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            f->grid[i][j] = '_';
            f->paintGrid[i][j] = 0;
        }
    }

    return f;
}

// --- Печать поля (не используется напрямую в main, но может быть полезна) ---
void printField(Field* f) {
    if (!f) return;
    for (int i = 0; i < f->height; i++) {
        for (int j = 0; j < f->width; j++) {
            putchar(f->grid[i][j]);
        }
        putchar('\n');
    }
}

// --- Освобождение памяти поля ---
void freeField(Field* f) {
    if (f) {
        free(f);
    }
}

// --- Загрузка поля из файла ---
Field* loadFieldFromFile(const char* filename, Dino** dino) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening field file for loading");
        return NULL;
    }

    int width, height;
    if (!getFieldDimensions(file, &width, &height)) {
        fclose(file);
        return NULL;
    }

    Field* f = createField(width, height);
    if (!f) {
        fclose(file);
        return NULL;
    }

    int dinoX = -1, dinoY = -1;
    int row = 0;
    char line[256];

    while (fgets(line, sizeof(line), file) != NULL && row < height) {
        int len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[--len] = '\0';
        }
        if (len == 0) continue;

        if (len != width) {
            printf("Error: Unexpected row length while loading.\n");
            freeField(f);
            fclose(file);
            return NULL;
        }

        for (int col = 0; col < width && col < len; col++) {
            char c = line[col];
            f->grid[row][col] = c;

            if (c == '#') {
                if (dinoX != -1) {
                    printf("Error loading field: Multiple dinos found.\n");
                    freeField(f);
                    fclose(file);
                    return NULL;
                }
                dinoX = col;
                dinoY = row;
            }
            if (c >= 'a' && c <= 'z') {
                f->paintGrid[row][col] = c;
            }
        }
        row++;
    }

    fclose(file);

    if (dinoX == -1) {
        printf("Error loading field: Dino (#) not found in loaded field.\n");
        freeField(f);
        return NULL;
    }

    *dino = createDino(dinoX, dinoY);
    if (!(*dino)) {
        printf("Error creating dino struct after loading field.\n");
        freeField(f);
        return NULL;
    }

    return f;
}