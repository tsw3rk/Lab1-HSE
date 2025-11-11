#ifndef LAB1_DINO_H
#define LAB1_DINO_H

typedef struct {
    int x;
    int y;
}Dino;

Dino* createDino(int x, int y);
void freeDino(Dino* d);

#endif //LAB1_DINO_H