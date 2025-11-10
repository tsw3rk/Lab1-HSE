#ifndef LAB1_DINO_H
#define LAB1_DINO_H

typedef struct {
    int x;
    int y;
}Dino;

Dino* createDino(int x, int y);
int moveDino(Dino* d, int dx, int dy, int width, int height);
void freeDino(Dino* d);


#endif //LAB1_DINO_H